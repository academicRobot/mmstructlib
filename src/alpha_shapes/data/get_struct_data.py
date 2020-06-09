#!/usr/bin/env python

from __future__ import print_function

from mmstructlib.cli import ArgParse
from mmstructlib.tools.preprocess import strip_water, strip_hydrogen
from mmstructlib.radii import add_radii
from mmstructlib.sas.alpha_shape_weighted_area import assign_indices

import sys

mirror = "/var/data/pdb"

if __name__ == "__main__":
    argparse = ArgParse("Create index/coordinate/radii file", mirror_default_path=mirror)
    argparse.struct_arg()
    argparse.report_chain_arg("chain", metavar="chain")
    args = argparse()

    struct = args.struct
    chain = args.chain[0]

    #assign an id number to each atom
    for i, a in enumerate(chain.atoms):
        a.number = i

    strip_water(struct[0])
    strip_hydrogen(struct[0])
    add_radii(struct[0])
    assign_indices([chain])

    for a in chain.atoms:
        print("{0}\t{1}\t{2}\t{3}\t{4}".format(
            a.monomer.as_index,
            a.coordinates[0],
            a.coordinates[1],
            a.coordinates[2],
            a.radius + 1.4
        ))

