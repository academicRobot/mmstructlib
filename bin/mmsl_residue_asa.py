#!/usr/bin/env python

import sys
import getopt
from mmstructlib.IO import load_cif_from_mirror, load_cif_from_file
from mmstructlib.radii import add_radii
from mmstructlib.sas import add_area
from mmstructlib.tools.summarize import sum_from_atom

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

#remove water
print "water", len([mol for mol in struct.molecules() if mol.is_water()])
for mol in struct.molecules():
    print mol.id, mol.entity.primary_type, mol.entity.secondary_type, len(mol.atoms())
    if mol.is_water():
        mol.delete()
print "water", len([mol for mol in struct.molecules() if mol.is_water()])

#remove hydrogen
for atom in struct.atoms():
    if atom.element == "H":
        atom.delete()

add_radii(struct)
add_area(struct.atoms(), dots=1000)
sum_from_atom(struct.monomers(), 'area')

print("chain\tid\ttype\tarea")
for chain in struct.molecules():
    if not chains or chain.id in chains:
        for mon in chain:
            print("{0}\t{1}\t{2}\t{3}".format(chain.id, mon.id, mon.type, mon.area))

