def residue_attribute(monomers, name, attr=None):
    if attr is None:
        attr = name
    out_str = "attribute: {0}\nmatch mode: 1-to-1\nrecipient: residues\n".format(name)
    return out_str + "\n".join([
#        "\t#0.{0}:{1}.{2}\t{3}".format(
#            mon.model.id, 
        "\t:{0}.{1}\t{2}".format(
            mon.id, 
            mon.molecule.pdb_id, 
            getattr(mon, attr)
        )
        for mon in monomers
    ])
