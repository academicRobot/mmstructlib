def strip_water(obj):
    if hasattr(obj, "entities"):
        for entity in obj.entities:
            if entity.is_water():
                entity.model = None
    else:
        raise ValueError("Can't process object type {0}".format(type(obj)))

def strip_hydrogen(obj):
    if hasattr(obj, "atoms"):
        for atom in obj.atoms:
            if atom.element == "H":
                atom.monomer = None
    else:
        raise ValueError("Can't process object type {0}".format(type(obj)))

def strip_small_molecule(obj, num_atom_thresh):
    if hasattr(obj, "molecules"):
        entities = set()
        for mol in obj.molecules:
            if len(mol.atoms) <= num_atom_thresh:
                entities.update(mol.entity)
                mol.entity = None
        for entity in entities:
            if len(entity) == 0:
                entity.model = None
    else:
        raise ValueError("Can't process object type {0}".format(type(obj)))
