from mmstructlib.crystal.sginfo import sgTransforms
from mmstructlib.math.operation import Operator
from mmstructlib.model import Structure
from mmstructlib.crystal.unit_cell import to_unit_cell, to_cartesian
from mmstructlib.assembly import apply_operation
from mmstructlib.tools.coordinates import mean_position
from copy import copy, deepcopy
import math
import numpy as np

def construct_space_group_operators(space_group_str):
    """
    """
    ops = sgTransforms(space_group_str.encode('utf8'))
    return [
        Operator(vector, matrix)
        for (matrix, vector) in ops
    ]

def euclid_to_unit_cell(structure):

    # transform to unit cell coordinates
    uc_trans = to_unit_cell(structure.unitcell.dimensions, structure.unitcell.angles)
    apply_operation.atoms(structure.atoms, uc_trans)

def fill_unit_cell(structure):
    """
    Assumes assymetric unit in first and only model in the structure. Each assym 
    copy in a sepereate, subsequent models.
    """
    assert len(structure.models) == 1

    # apply unit cell symmetry operations to asymmetric unit
    sg_ops = construct_space_group_operators(structure.unitcell.space_group_number)
    for op in sg_ops[1:]:
        structure.models.append(deepcopy(structure.models[0]))
        apply_operation.atoms(structure.models[-1].atoms, op)
    apply_operation.atoms(structure.models[0].atoms, sg_ops[0])
    
    # normalize unit cell my molecule (pack unit cell)
    #for m in [mol for mod in structure.models for mol in mod.molecules]:
    for m in structure.models:
        m_atoms = m.atoms
        uc_loc = list(map(math.floor,mean_position(m_atoms)))
        op = Operator(translate=[-x for x in uc_loc])
        #print(uc_loc, op.translate)
        apply_operation.atoms(m_atoms, op)


def unit_cell_translation(structure, unit_cells):
    # make unitcell translations
    uc_models = list(structure.models)
    for unit_cell in unit_cells[1:]:
        #print("UC {}".format(unit_cell))
        op = Operator(translate=unit_cell)
        new_models = deepcopy(uc_models)
        structures.models.extend(new_models)
        apply_operation.atoms([a for mod in new_models for a in mod.atoms], op)
    apply_operation.atoms(
        [a for mod in uc_models for a in mod.atoms],
        Operator(translate=unit_cells[0])
    )

def unit_cell_to_euclid(structure):
    #bring back to cartesian coordinates
    cart_trans = to_cartesian(structure.unitcell.dimensions, structure.unitcell.angles)
    apply_operation.atoms(structure.atoms, cart_trans)

def generate_lattice(structure):
    """
    Transforms structure into a crystal lattice where contents of primary unit
    cell are surrounded by copies.  Will frequently be larger than 3x3x3 due to 
    translations that maintain molecular bonds.  Will contain all contacts with 
    primary unit cell.
    """
    assert structure.is_xray()

    euclid_to_unit_cell(structure)
    fill_unit_cell(structure)

    #set assym and cell labels
    for i, model in enumerate(structure):
        model.assym_id = i
        model.uc_id = (0, 0, 0)

    #get all occupied cells
    occ_cells = np.unique(
        np.array(
            np.floor(np.vstack([a.coordinates.flatten() for a in structure.atoms])),
            dtype=int
        ),
        axis=0
    )
    #print(occ_cells)

    #fill neighboring cells
    basic_grid = np.array([
        (i, j, k)  
        for i in (-1, 0, 1) 
        for j in (-1, 0, 1) 
        for k in (-1, 0, 1)
    ])
    grid = np.unique(
        [ c+g for c in occ_cells for g in basic_grid],
        axis=0
    )
    assert np.array([0., 0., 0.]) in grid

    #pop out (0,0,0), leave first untranslated
    grid = grid[np.any(grid != np.array([0.,0.,0.]), axis=1)]

    #unit cell models
    og_models = list(structure.models)

    #create lattice
    for cell_trans in grid:
        #print(cell_trans)
        op = Operator(translate=cell_trans)
        cell_id = tuple(np.array(cell_trans, dtype=int))
        for og_model in og_models:
            #print("\t{}".format(og_model.assym_id))
            structure.models.append(deepcopy(og_model))
            apply_operation.atoms(structure.models[-1].atoms, op)
            structure.models[-1].uc_id = cell_id

    unit_cell_to_euclid(structure)

def bounding_box(atoms):
    coord_mat = np.vstack([a.coordinates for a in atoms])
    return np.min(coord_mat, axis=0), np.max(coord_mat, axis=0)

def generate_minimal_lattice(structure, margin):
    """
    Transforms structure into a crystal lattice where contents of primary 
    assymetric unit are surrounded by copies.  Will contain all contacts with 
    primary assymetric unit.
    """
    assert structure.is_xray()

    euclid_to_unit_cell(structure)
    fill_unit_cell(structure)

    #calculate bounding box
    uc_margin = margin / structure.unitcell.dimensions
    #print('uc_margin', uc_margin)

    assym_bb_mins, assym_bb_maxs = bounding_box(structure[0].atoms)
    #print('assym_bb', assym_bb_mins, assym_bb_maxs)
    assym_bb_mins -= uc_margin
    assym_bb_maxs += uc_margin
    #print('assym_bb', assym_bb_mins, assym_bb_maxs)

    for model in list(structure.models):
        bb_mins, bb_maxs = bounding_box(model.atoms)
        #print('bb', bb_mins, bb_maxs)
        min_cs, max_cs = np.zeros(3, dtype=int), np.zeros(3, dtype=int)
        for d in range(3):
            while assym_bb_mins[d] < bb_maxs[d] + min_cs[d]:
                min_cs[d] -= 1
            while assym_bb_maxs[d] > bb_mins[d] + max_cs[d]:
                max_cs[d] += 1
            min_cs[d] += 1
            max_cs[d] -= 1
        for i in range(min_cs[0], max_cs[0]+1):
            for j in range(min_cs[1], max_cs[1]+1):
                for k in range(min_cs[2], max_cs[2]+1):
                    c = (i, j, k)
                    if c == (0, 0, 0):
                        continue #don't duplicate original
                    #check that all dimensions overlap
                    overlap = all([
                        assym_bb_mins[d] < bb_maxs[d] + c[d] and assym_bb_maxs[d] > bb_mins[d] + c[d]
                        for d in range(3)
                    ])
                    if not overlap:
                        continue
                    op = Operator(translate=np.array(c, dtype=int))
                    structure.models.append(deepcopy(model))
                    apply_operation.atoms(structure.models[-1].atoms, op)
                    structure.models[-1].uc_id = c
                    #print('\t', c)

    unit_cell_to_euclid(structure)
