"""
Functions for building a structure from a macromolecular crystallographic
information file (mmcif).  The cif.parser module provides parsed cif 'tabels'
that these functions use to build a structure model representation.

Primary user functions are parse_cif, build

"""

from mmstructlib.IO.cif_parser import parse as _cif_parser

def parse_cif(file_handle):
    """
    Returns processed tables from first block in a cif file
    """
    cif_file = _cif_parser(file_handle.read())
    #print "file", len(cif_file.blocks)
    return cif_file.blocks[0].tables

import mmstructlib.model as sm
from itertools import groupby
from operator import itemgetter
from copy import deepcopy

def build(tables):
    struct = build_structure(tables)
    if struct.is_xray():
        struct.unitcell = build_unit_cell(tables)
        struct.assemblies = build_assemblies(tables)

    asym_id_trans = get_pdb_asym_ids(tables)

    model_atoms = get_model_atoms(tables)
    for model, atom in model_atoms:
        struct.append(sm.Model(model))

    build_entities(tables, struct[0]) #adds entities to first model
    for model in struct[1:]: #duplicate entities across models
        for entity in struct[0]:
            entity_copy = deepcopy(entity)
            entity_copy.model = model

    seq_atom_res = get_seq_atom_res(tables)

    #relevant indices for monster loop
    atom = tables['atom_site']
    atom_entity_i = atom.index('label_entity_id')
    atom_molecule_i = atom.index('label_asym_id')
    atom_seq_i = atom.index('label_seq_id')
    atom_pdbseq_i = atom.index('auth_seq_id')
    atom_ic_i = atom.index('pdbx_PDB_ins_code')
    atom_mono_type_i = atom.index('label_comp_id')
    atom_element_i = atom.index('type_symbol')
    atom_name_i = atom.index('label_atom_id')
    atom_coords_i = [atom.index(s) for s in ['Cartn_x', 'Cartn_y', 'Cartn_z']]
    atom_bfactor_i = atom.index('B_iso_or_equiv')
    atom_occ_i = atom.index('occupancy')
    atom_altloc_i = atom.index('label_alt_id')

    #monster loop to build molecules->monomers->atoms->multi_occ, note that
    #  each subloop is over unique subsets of the superloop, so complexity of
    #  the entire thing is nest_depth * n (not n ^ nest_depth)
    for model_id, m_atoms in model_atoms:
        model = struct.model_by_id(model_id)
        em_groups = groupby(
            m_atoms,
            lambda x: (x[atom_entity_i], x[atom_molecule_i])
        )
        for (entity_id, molecule_id), em_atoms in em_groups:
            entity = model.entity_by_id(entity_id)
            if entity is None:
                raise RuntimeError("Model {0} missing entity {1}".format(model.id, entity_id))
            #not all polys have poly entries :(
            #if entity.is_polymer(): #one molecule per molecule_id
            is_poly_entry_poly =  entity.is_polymer() and molecule_id in seq_atom_res
            if is_poly_entry_poly:
                seq_res, atom_res = seq_atom_res[molecule_id]
                molecule = sm.Polymer(molecule_id,  asym_id_trans[molecule_id], seq_res, atom_res)
                entity.append(molecule)

            mono_groups = groupby(
                em_atoms,
                lambda x: (
                    sm.MonomerID(x[atom_seq_i], x[atom_pdbseq_i], x[atom_ic_i]),
                    x[atom_mono_type_i]
                )
            )
            for (mono_id, mono_type_id), mono_atoms in mono_groups:
                #if not entity.is_polymer(): #one molecule per monomer
                if not is_poly_entry_poly:
                    #occassionally waters do not have asym_id_trans entries, only
                    #  allow this case, crash for others
                    if molecule_id not in asym_id_trans and entity.is_water():
                        asym_id = molecule_id
                    else:
                        asym_id = asym_id_trans[molecule_id]
                    molecule = sm.Molecule(molecule_id, asym_id)
                    entity.append(molecule)
                    monomer = sm.Monomer(mono_id, mono_type_id)
                    molecule.append(monomer)
                else:
                    monomer = sm.Residue(mono_id, mono_type_id)
                    molecule.append(monomer)

                alt_loc_groups = groupby(
                    mono_atoms,
                    itemgetter(atom_name_i)
                )
                for atom_name, al_atoms in alt_loc_groups:
                    al_atoms = list(al_atoms)
                    row = al_atoms[0]
                    element = row[atom_element_i]
                    if len(al_atoms) == 1:
                        monomer.append(sm.Atom(
                            element,
                            atom_name,
                            (row[i] for i in atom_coords_i),
                            row[atom_bfactor_i],
                            row[atom_occ_i]
                        ))

                    elif entity.is_water():
                        for row in al_atoms:
                            monomer.append(sm.Atom(
                                element,
                                atom_name,
                                (row[i] for i in atom_coords_i),
                                row[atom_bfactor_i],
                                row[atom_occ_i]
                            ))
                    else:
                        mo_atom = sm.MultiOccAtom(element, atom_name)
                        monomer.append(mo_atom)
                        for row in al_atoms:
                            mo_atom.add_atom(
                                (row[i] for i in atom_coords_i),
                                row[atom_bfactor_i],
                                row[atom_occ_i],
                                row[atom_altloc_i]
                            )

    return struct

def build_structure(tables):
    """
    """
    exptl = tables['exptl']
    struct_id = exptl.field('entry_id')
    struct_type = exptl.field('method')
    res = tables['refine'].field('ls_d_res_high') if 'refine' in tables else None
    #res = tables['reflns'].field('d_resolution_high') if 'reflns' in tables else None
    return sm.Structure(struct_id, struct_type, res)

def build_unit_cell(tables):
    """
    """
    cell = tables['cell']
    dims = [ cell.field('length_'+l) for l in ('a', 'b', 'c') ]
    angles = [ cell.field('angle_'+l) for l in ('alpha', 'beta', 'gamma') ]
    sg = tables['symmetry'].field('space_group_name_H-M')
    sgn = tables['symmetry'].field('Int_Tables_number')
    return sm.xray.UnitCell(dims, angles, sg, sgn)

def parse_operator_list(string):
    """
    Quick, hoaky fix.  You can do better.
    """
    string = string.strip()

    #unify notation
    if not string.startswith("("):
        string = "(" + string
    if not string.endswith(")"):
        string = string + ")"

    op_grps = []
    for op_grp_str in [a[1:] for a in string.split(')')[:-1]]:
        ops = []
        for op_str in op_grp_str.split(","):
            op_range = op_str.split("-")
            if len(op_range) == 1:
                ops.append(op_range[0])
            else:
                op_range = list(map(int, op_range))
                ops.extend(map(str, range(op_range[0], op_range[1]+1)))
        op_grps.append(ops)
    return op_grps

def build_assemblies(tables):
    """
    Nearly all PDB structures have a struct_assembly table.  If not present, return an
    empty list.
    """
    ops = dict((op.id, op) for op in build_operators(tables))
    rets = dict()
    if 'pdbx_struct_assembly' in tables:
        assemblies = tables['pdbx_struct_assembly']
        a_id = assemblies.index('id')
        ids = [
            assemblies.index(x) if x in assemblies.cell_names else None
            for x in [
                'details', 'method_details',
                'oligomeric_details', 'oligomeric_count'
            ]
        ]
        assemblies_lu = dict([(r[a_id], r) for r in assemblies.cells])
        assemblies_gen = tables['pdbx_struct_assembly_gen']
        for i in range(len(assemblies_gen.cells)):
            assem_id = assemblies_gen.field('assembly_id', i)
            if assem_id in rets:  #assembly records may be broken into multiple rows
                assem = rets[assem_id]
            else:
                args = [assem_id]
                args.extend([assemblies_lu[assem_id][x] if x is not None else None for x in ids])
                assem = sm.xray.Assembly(*args)
            assym_ids = assemblies_gen.field('asym_id_list', i).split(",")
            for op_ids in parse_operator_list(assemblies_gen.field('oper_expression', i)):
                assem.add_operator([ops[op_id] for op_id in op_ids], assym_ids)
            rets[assem_id] = assem
    return list(rets.values())

def build_operators(tables):
    """
    """
    ret = []
    if 'pdbx_struct_oper_list' in tables:
        ops = tables['pdbx_struct_oper_list']
        ret = [
            sm.xray.AssemblyOperator(
                ops.field('id', i),
                ops.field('type', i),
                ops.field('name', i) if 'name' in ops.cell_names else None,
                ops.field('symmetry_operation', i) if 'symmetry_operation' in ops.cell_names else None,
                [ ops.field('vector[{0}]'.format(j), i) for j in (1,2,3) ],
                [ ops.field('matrix[{0}][{1}]'.format(j, k), i)
                    for j in (1,2,3) for k in (1,2,3) ]
            )
            for i in range(len(ops.cells))
        ]
    return ret

def get_model_atoms(tables):
    atom_site = tables['atom_site']
    model_index = atom_site.index('pdbx_PDB_model_num')
    return [
        (k, list(g))
        for k, g in groupby(atom_site.cells, itemgetter(model_index))
    ]

def get_seq_atom_res(tables):
    ret = dict()
    table_name = 'pdbx_poly_seq_scheme'
    if table_name in tables:
        table = tables[table_name]
        seq_asym_i = table.index('asym_id')
        seq_res_i = table.index('mon_id')
        atom_res_i = table.index('pdb_mon_id')
        chain_group = groupby(table.cells,itemgetter(seq_asym_i))
        for (chain, rows) in chain_group:
            rows = list(rows)
            ret[chain] = (
                list(map(itemgetter(seq_res_i), rows)),
                list(map(itemgetter(atom_res_i), rows))
            )
    return ret

def build_entities(tables, model):
    ret = []
    entity = tables['entity']

    poly = tables.get('entity_poly', None)
    if poly:
        entity_id_i = poly.index('entity_id')
        poly_type_i = poly.index('type')
        poly_entity_id_lu = dict([(r[entity_id_i], r) for r in poly.cells])
    else: 
        poly_entity_id_lu = dict()

#    nonpoly = tabels.get('pdbx_entity_nonpoly', None)
#    if nonpoly:
#       field_index_table(nonpoly, 'entity_id')


    for i in range(len(entity.cells)):
        entity_id = entity.field('id', i)
        entity_type = entity.field('type', i)
        entity_name = entity.field('pdbx_description', i)
        entity_type2 = None

        #note: some non-protein/nucleic acids polys not in table
        #if entity_type == 'polymer':
        if entity_id in poly_entity_id_lu:
            entity_type2 = poly_entity_id_lu[entity_id][poly_type_i]

        model.append(sm.Entity(entity_id, entity_type, entity_type2, entity_name))

def get_pdb_asym_ids(tables):
    ret = dict()
    table_names = ['pdbx_poly_seq_scheme', 'pdbx_nonpoly_scheme']
    for table_name in table_names:
        if table_name in tables:
            table = tables[table_name]
            seq_asym_i = table.index('asym_id')
            pdb_asym_i = table.index('pdb_strand_id')
            ret.update([(r[seq_asym_i], r[pdb_asym_i]) for r in table.cells])
    return ret


