# mmstructlib
## Macromolecular structure library
A python 3 package for reading and working with PDB structures in mmCIF format.  Initially written to access the richer data in mmCIF files, relative to PDB format.  Expanded to include some structural analysis tools.  In development, interfaces are not gaurenteed, please fork me before use.
## Features
- Simple hierarchical datastructure that is universal for all molecule types
- All data objects in pure python for intuitive coding
- Much of the back end analysis and IO is in C/C++ for speed
- Integrated structural analysis including surface areas, atomic contacts, and assembly and lattice reconstruction
- Reads seqres/atomres alignment table for accessing sequences with missing density
## Primary data structure
Each data structure is a list of child data structures.  Parent structures have acessors to traverse and search the tree more succinctly. Below is the basic data structure, additional data concerning assemblies and associated opperators are available for xray structures.

- Structure
  - Model
    - Entity
      - Molecule or Polymer
        - Monomer (residues, or single monomer for small molecules)
          - Atom
          - MultiAtom (will function transparently as Atom, presenting first conformer)
        - Atom
## Installation
### Requirements
- python3
- cython (in principle should work without it)
- c/c++ compiler
- CGAL v4.14 (earlier minor versions may work, 4.7 does not work)
- gmp
###
```
$ python3 setup.py build
$ python3 setup.py test
$ python3 setup.py install
```
## Basic usage
### Using load functions
Load functions are useful for setting up batch jobs

Set the location of your local PDB mirror
```
$ export PDB_MIRROR="/home/cjoldfield/data/pdb/"
```
Then load directly from a PDB ID
```
from mmstructlib.IO import load_cif_from_mirror
struct = load_cif_from_mirror('1AUI')
```
### Using cli helpers
The cli module is useful for writing tools
```
from mmstructlib.cli import ArgParse

argparse = ArgParse("This is my analysis function")
argparse.struct_arg()       #Add a structure id arugment to the command line
argparse.report_chain_arg() #Add a chain id argument to the command line 
argparse.outfile_arg()      #Add an output file to the command line

#then the usual argparse things
args = argparse()

outfh = args.outfile

struct = args.struct #a structure object, not the id on the command line
chains = args.chain  #a Polymer object
```
## Command line tools
- mmsl_missing_density_fasta - Derives a fasta sequence file, with sequence encoded for defined density (upper case) and missing density (lower case)
- mmsl_nia_nma - Structural analysis of interface area and monomer area for complex structures
- mmsl_residue_disorder - Internal use only, depends on unreleased code
## Directory road map
- bin
- mmstructlib - main module
  - IO - mostly just I, some report formatting (output Chimera features)
  - model - core data structure
  - assembly - building assemblies from operators
  - crystal - tools getting operators from space groups
  - radii - assign radii to atoms, with reasonable defaults
  - tools - preprocessing and reduce methods
  - sas - solvent accessible surface calculations, analytical (alphashapes) or numeric(nsc)
  - math - not much here
  - util - internal use only
- src - C/C++ code for analysis or interface to external libraries
  - alpha_shapes
  - cif
  - nsc
  - sginfo_python
  - vendor
- test
##Known issues
- need to update tests for modifications to spacegroup and operator changes (expect these to fail)

