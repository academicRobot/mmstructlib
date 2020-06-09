#!/usr/bin/env python

from __future__ import print_function

from mmstructlib.cli import ArgParse
from mmstructlib.tools.preprocess import strip_water, strip_hydrogen
from mmstructlib.radii import add_radii
from mmstructlib.tools.summarize import from_atom

import sys

mirror = "/var/data/pdb"

def window_gen(half_window_size, model):
    for mol in model.molecules:
        if mol.is_protein():
            chain = [
                mol[index] if index is not None else None 
                for aa, index in mol.seq_res
            ]
        else:
            chain = mol.monomers
        l = len(chain)-1
        for i in range(l+1):
            if chain[i] is not None:
                s = max(0, i - half_window_size)
                e = min(l, i + half_window_size)
                yield chain[i], filter(lambda x: x is not None, chain[s:e+1])


argparse = ArgParse("Calculate residue-wise surface area", mirror_default_path=mirror)
argparse.struct_arg()
argparse.report_chain_arg(nargs='+')

argparse.add_argument("-a", action='store_true', help="Caluclate analytic surface area")
argparse.add_argument("-n", action='store_true', help="Caluclate numerical surface area [default]")
argparse.add_argument("--n_win", action='store_true', help="Caluclate numerical, windowed surface area")
argparse.add_argument("--n_w_win", action='store_true', help="Caluclate numerical, windowed surface area using weighting method")
argparse.add_argument("--half_window", nargs=1, metavar='length', type=int, help="Half window length for windowed calculations %(default)s", default=[5])
argparse.add_argument("--dots", nargs=1, metavar='dots', type=int, help="Number of mesh dots for numerical surface %(default)s", default=[362])
args = argparse()

struct = args.struct
chains = args.chain

half_window = args.half_window[0]
num_dots = args.dots[0]
print(half_window, num_dots)

strip_water(struct[0])
strip_hydrogen(struct[0])

add_radii(struct[0])

area_strs = []
if args.a:
    try:
        import mmstructlib.sas.alpha_shapes as alpha_shapes
    except:
        print("analytical sasa not supported in this mmsl installation", file=sys.stderr)
        sys.exit(1)
    area_strs.append('a_area')
    alpha_shapes.area_atoms(struct[0].atoms, out_attr='a_area')
    from_atom(struct[0].monomers, 'a_area')
if (args.n or args.n_win or args.n_w_win) or not (args.n or args.a or args.n_win or args.n_w_win):
    try:
        import mmstructlib.sas.nsc as nsc
    except:
        print("numerical sasa not supported in this mmsl installation", file=sys.stderr)
        sys.exit(1)
    if args.n or not (args.n or args.a or args.n_win or args.n_w_win):
        area_strs.append('n_area')
        nsc.area_atoms(struct[0].atoms, out_attr='n_area', num_dots=num_dots)
        from_atom(struct[0].monomers, 'n_area')
    if args.n_win:
        attr_str = 'n_win_area_{0}'.format(args.n_win)
        area_strs.append(attr_str)
        for c_res, window in window_gen(half_window, struct[0]):
            nsc.area_atoms(
                [a for r in window for a in r.atoms], 
                out_attr=attr_str, 
                num_dots=num_dots
            )
            from_atom([c_res], attr_str)
    if args.n_w_win:
        attr_str = 'n_w_win_area_{0}'.format(args.n_win)
        area_strs.append(attr_str)
        for mon in struct[0].monomers:
            mon.weight = 1.0
        nsc.atoms_area_weighted(
            struct[0], 
            half_window, 
            out_attr=attr_str, 
            num_dots=num_dots
        )
        from_atom(struct[0].monomers, attr_str)
        
print("chain\tid\ttype\t" + "\t".join(area_strs))
for chain in chains:
    for mon in chain:
#        for i, atom in enumerate(mon):
#            print(i, round(atom.n_area, 1), round(atom.a_area, 1))
        print("{0}\t{1}\t{2}\t{3}".format(
            chain.pdb_id, 
            mon.id, 
            mon.type, 
            "\t".join([ str(round(getattr(mon, area_str),3)) for area_str in area_strs])
        ))

