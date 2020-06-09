from mmstructlib.sas.nsc import DoubleArray, nsc_atom_sas, nsc_total_sas, nsc_total_volume

def run_nsc(atoms, probe, dots, func):
    """
    """
    num = len(atoms)

    coords = DoubleArray(num*3)
    radii = DoubleArray(num)
    for i, a in enumerate(atoms):
        atom_index = i*3
        coords[atom_index] = a.coordinates[0]
        coords[atom_index+1] = a.coordinates[1]
        coords[atom_index+2] = a.coordinates[2]
        radii[i] = a.radius + probe

    result = DoubleArray()
    if func(coords, radii, dots, result):
        raise RuntimeError("Error running nsc")

    return result

def add_area(atoms, probe = 1.4, dots = 1000):
    """
    """
    result = run_nsc(atoms, probe, dots, nsc_atom_sas)
    for i, a in enumerate(atoms):
        a.area = result[i]

def total_area(atoms, probe = 1.4, dots = 1000):
    """
    """
    return run_nsc(atoms, probe, dots, nsc_total_sas)[0]

def total_volume(atoms, probe = 1.4, dots = 1000):
    """
    """
    return run_nsc(atoms, probe, dots, nsc_total_volume)[0]

