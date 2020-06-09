#!/usr/bin/env python

import sys
import getopt
from mmstructlib.IO import load_cif_from_mirror, load_cif_from_file
from mmstructlib.tools.seq import convert_three_to_one_extended as c321

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
chains = [ 
    m for m in struct[0].molecules
        if m.is_protein() and (not chains or m.pdb_id in chains) 
]

for chain in chains:
    seq = [ c321(s) if a is not None else c321(s).lower() for s, a in chain.seq_res ]
    print(">{0}_{1}".format(struct.id, chain.pdb_id))
    print("".join(seq))


