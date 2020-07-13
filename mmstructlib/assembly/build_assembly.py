import numpy as np
import copy
from mmstructlib.model.core import Structure, Model
from mmstructlib.model.xray import Assembly
from mmstructlib.assembly import apply_operation

def build_assembly(struct, assembly):
    """
    Returns a copy of the atom_container with atom positions moved by
    operation.
    """
    if not isinstance(struct, Structure):
        raise ValueError("first argument must be a Structure")
    if not isinstance(assembly, Assembly):
        raise ValueError("second argument must be an Assembly")
    assert len(struct.models) == 1
    ass_struct = copy.copy(struct)
    ass_struct[:] = []
    for op_exp, assyms in assembly.operation_assyms:
        for op in op_exp:
            model = copy.deepcopy(struct.models[0])
            for molecule in model.molecules:
                if molecule.id not in assyms:
                    molecule.entity = None
            apply_operation.atoms(model.atoms, op) 
            ass_struct.models.append(model)
    return ass_struct

