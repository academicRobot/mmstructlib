backbone_atom_names = set(("CA", "N", "C", "O"))
side_chain_atom_names = {
    'GLY' : set(),
    'ALA' : set(('CB',)),
    'VAL' : set(('CB', 'CG1', 'CG2')),
    'LEU' : set(('CB', 'CG', 'CD1', 'CD2')),
    'ILE' : set(('CB', 'CG1', 'CG2', 'CD1')),
    'PRO' : set(('CB', 'CG', 'CD')),
    'MET' : set(('CB', 'CG', 'SD', 'CE')),
    'PHE' : set(('CB', 'CG', 'CD1', 'CD2', 'CE1', 'CE2', 'CZ')),
    'TYR' : set(('CB', 'CG', 'CD1', 'CD2', 'CE1', 'CE2', 'CZ', 'OH')),
    'TRP' : set(('CB', 'CG', 'CD1', 'NE1', 'CD2', 'CE2', 'CE3', 'CZ3', 'CZ2', 'CH2', 'CEH2')),
    'SER' : set(('CB', 'OG')),
    'THR' : set(('CB', 'OG1', 'CG2')),
    'ASN' : set(('CB', 'CG', 'OD1', 'ND2')),
    'GLN' : set(('CB', 'CG', 'CD', 'OE1', 'NE2')),
    'CYS' : set(('CB', 'SG')),
    'CSS' : set(('CB', 'SG')),
    'HIS' : set(('CB', 'CG', 'ND1', 'CD2', 'CE1', 'NE2')),
    'GLU' : set(('CB', 'CG', 'CD', 'OE1', 'OE2')),
    'ASP' : set(('CB', 'CG', 'OD1', 'OD2')),
    'ARG' : set(('CB', 'CG', 'CD', 'NE', 'CZ', 'NH1', 'NH2')),
    'LYS' : set(('CB', 'CG', 'CD', 'CE', 'NZ'))
}
standard_atom_names = { 
    name : atoms.union(backbone_atom_names) 
    for name, atoms in side_chain_atom_names.items()
}

def has_all_atoms(residue, atoms):
    return atoms.issubset([a.name for a in residue])

def has_complete_side_chain(residue):
    return has_all_atoms(residue, side_chain_atom_names[residue.type])

def has_complete_backbone(residue):
    return has_all_atoms(residue, backbone_atom_names)

def has_all_standard_atoms(residue):
    return has_all_atoms(residue, standard_atom_names)

def non_backbone_atoms(residue):
    return [a for a in residue if a.name not in backbone_atom_names]
    
def non_standard_atoms(residue):
    return [a for a in residue if a.name not in standard_atom_names[residue.type]]
    
def standardize_residue_side_chain(residue):
    """
    Modify residue to contain only standard atoms and modify to smaller 
    residue if any standard atoms are missing.
    
    In the case of missing atoms, residue is reduced to alanine if a 
    beta carbon is present, or glycine otherwise.  The type attribute
    is modified to reflect this change.
    
    Always adds the attribute org_type to the residue, which contains the
    value of the original type attribute.
    """
    residue.org_type = residue.type
    if residue.type in standard_atom_names:
        for a in non_standard_atoms(residue):
            residue.remove(a)
    if residue.type not in side_chain_atom_names or not has_complete_side_chain(residue):
        beta_carbon = residue.atom_by_name('CB')
        for a in non_backbone_atoms(residue):
            if a != beta_carbon:
                residue.remove(a)
        if beta_carbon is not None:
            residue.type = 'ALA'
        else:
            residue.type = 'GLY'
