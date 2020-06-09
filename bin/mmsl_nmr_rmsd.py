#!/usr/bin/env python

import sys
import getopt
from mmstructlib.IO import load_cif_from_mirror, load_cif_from_file
from mmstructlib.tools.rmsd import nmr_struct_pos_rmsd

mirror = "/var/data/pdb"

optlst, args = getopt.getopt(sys.argv[1:], 'c:fm:')
optdict = dict(optlst)

if len(args) != 1:
    print("usage: {0} [opt] <struct>".format(sys.argv[0]))
    print("\n".join([
        "options:",
        "	-c <chains>    limit return to these chains (comma seperated)",
        "	-f             interpret <struct> as file name",
        "	-m             set pdb mirror directory, default: {0}".format(mirror)
    ]))
    sys.exit(1)

chains = optdict.get('-c', None)
if chains:
    chains = chains.split(",")
mirror = optdict.get('-m', mirror)

if '-f' in optdict:
    loader = load_cif_from_file
else:
    loader = lambda x: load_cif_from_mirror(x, mirror)

struct = loader(args[0])
nmr_struct_pos_rmsd(struct)

print("chain\tid\ttype\trmsd")
for chain in struct[0].molecules:
    if chain.is_protein() and (not chains or chain.pdb_id in chains):
        for mon in chain:
            print("{0}\t{1}\t{2}\t{3}".format(chain.id, mon.id, mon.type, mon.rmsd))

