from mmstructlib.radii.load_radii import load_default

def add_radii(obj, radii=None, out_attr="radius"):
    """
    """
    if not radii:
        radii = load_default()

    if hasattr(obj, "monomers"):
        monomers = obj.monomers
    elif hasattr(obj, "models"):
        monomers = [ mon for mod in obj.models for mon in mod.monomers ]
    else:
        raise ValueError("Can't get monomers for object type {0}".format(type(obj)))

    monomer_radii = radii.monomer_radii
    aa_common_radii = radii.aa_common_radii
    element_radii = radii.element_radii
    default_radius = radii.default
    empty_dict = dict()
    for monomer in monomers:
        if monomer.is_residue():
            common_radii = radii.aa_common_radii
        else:
            common_radii = empty_dict
        radii_dict = monomer_radii.get(monomer.type, empty_dict)
        for atom in monomer:
            setattr(
                atom, 
                out_attr, 
                radii_dict.get(
                    atom.name,
                    common_radii.get(
                        atom.name,
                        element_radii.get(
                            atom.element,
                            radii.default
                        )
                    )
                )
            )

