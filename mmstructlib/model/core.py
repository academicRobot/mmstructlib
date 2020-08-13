# vim: set fileencoding=utf-8
"""
Data model representing a structure.
The most general heirarchy is:
    Structure
        └Entity
            └Molecule
                └Monomer
                    └Atom
Molecule is roughly equivelent to a chain in common PDB parlance
More completely, the heirachy is:
    Structure
        └Model
            └Entity
                ├Molecule
                    └Monomer
                        └Atom
                └Molecule
                    └Atom

In this view, entity is a type of molecule
"""

#from functools import total_ordering
import numpy as np
from mmstructlib.util.double_linked_hierarchy import Parent, Child

BACKBONE_HEAVY_ATOM_NAMES = set(("CA", "N", "C", "O"))

def get_by_attrib(lst, attr, val):
    """
    Need to add error checking
    """
    return [item for item in lst if getattr(item, attr) == val]

def get_by_attrib_single(lst, attr, val):
    """
    Need to add error checking
    """
    lst = get_by_attrib(lst, attr, val)
    if len(lst) > 1:
        raise AttributeError("Multiple results found: " + str(lst))
    return lst[0] if lst else None

class Structure(Parent):
    """
    Contains information from a PDB mmcif file

    Instances of this class are lists of Models, with additional attributes.

    Attributes:
        id (str): The PDB ID of the structure
        type (str): The experimental method used to determine the structure
        resolution (float, optional): The relolution of the structure, None if
            not applicable
        unitcell (UnitCell, optional): A class containing unit cell details,
            None if not applicable
        assemblies (list(Assembly), optional): A list of assembly objects
            describing different arrangements of unit cell contents.  Empyt list
            if not applicable
    """
    def __init__(self, structure_id, structure_type, resolution):
        """
        """
        super(Structure, self).__init__()
        self.id = structure_id
        self.type = structure_type
        self.resolution = float(resolution) if resolution else None
        self.unitcell = None
        self.assemblies = []

    def __str__(self):
        return self.id

    def __repr__(self):
        return "Structure({0}, {1}, {2})".format(self.id, self.type, self.resolution)

    def model_by_id(self, model_id):
        """
        """
        return get_by_attrib_single(self, 'id', int(model_id))

    def is_xray(self):
        """
        """
        return self.type == "X-RAY DIFFRACTION"

    def is_nmr(self):
        """
        """
        return self.type == "SOLUTION NMR"

    @property
    def models(self):
        """
        """
        return self

    @property
    def entities(self):
        return [
            e
            for mod in self
            for e in mod
        ]

    @property
    def molecules(self):
        return [
            mol
            for mod in self
            for e in mod
            for mol in e
        ]

    @property
    def polymers(self):
        return [
            mol
            for mod in self
            for e in mod
            for mol in e
            if mol.is_polymer()
        ]

    @property
    def proteins(self):
        return [
            mol
            for mod in self
            for e in mod
            for mol in e
            if mol.is_protein()
        ]

    @property
    def monomers(self):
        return [
            mon
            for mod in self
            for e in mod
            for mol in e
            for mon in mol
        ]

    @property
    def atoms(self):
        return [
            a
            for mod in self
            for e in mod
            for mol in e
            for mon in mol
            for a in mon
        ]

class Model(Child, Parent):
    """
    """
    def __init__(self, model_id):
        """
        """
        super(Model, self).__init__()
        self.id = int(model_id)

    def delete(self):
        """
        """
        self.structure.remove(self)

    def __str__(self):
        return str(self.id)

    def __repr__(self):
        return "Model({0})".format(self.id)

    def __eq__(self, other):
        """
        Overrides list equality, only the same object compares as equal
        """
        return id(self) == id(other)

    def __hash__(self):
        return id(self)

    @property
    def structure(self):
        return self.parent

    @structure.setter
    def structure(self, value):
        self.parent = value

    def entity_by_id(self, entity_id):
        """
        """
        return get_by_attrib_single(self, 'id', int(entity_id))

    def molecule_by_id(self, molecule_id):
        return get_by_attrib(self.molecules, 'id', molecule_id)

    def polymer_by_id(self, poly_id):
        return get_by_attrib_single(self.polymers, 'id', poly_id)

    def molecule_by_pdb_id(self, pdb_id):
        return get_by_attrib(self.molecules, 'pdb_id', pdb_id)

    def polymer_by_pdb_id(self, pdb_id):
        return get_by_attrib_single(self.polymers, 'pdb_id', pdb_id)

    @property
    def entities(self):
        return self

    @property
    def molecules(self):
        return [ mol for e in self for mol in e ]

    @property
    def polymers(self):
        return [ mol for e in self for mol in e if mol.is_polymer() ]

    @property
    def proteins(self):
        return [ mol for e in self for mol in e if mol.is_protein() ]

    @property
    def monomers(self):
        return [ mon for e in self for mol in e for mon in mol ]

    @property
    def atoms(self):
        return [ a for e in self for mol in e for mon in mol for a in mon ]

class Entity(Child, Parent):
    """
    """
    def __init__(self, entity_id, primary_type, secondary_type, name):
        super(Entity, self).__init__()
        self.id = int(entity_id)
        self.primary_type = primary_type
        self.secondary_type = secondary_type
        self.name = name

    def delete(self):
        """
        """
        self.model.remove(self)

    def __str__(self):
        return "{0}:{1}".format(self.id, self.name)

    def __repr__(self):
        return "Entity({0}, {1}, {2}, {3})".format(self.id, self.primary_type, self.secondary_type, self.name)

    def __eq__(self, other):
        """
        Overrides list equality, only the same object compares as equal
        """
        return id(self) == id(other)

    def __hash__(self):
        return id(self)

    @property
    def model(self):
        return self.parent

    @model.setter
    def model(self, value):
        self.parent = value

    @property
    def structure(self):
        return self.model.structure if self.model else None

    def is_water(self):
        return self.primary_type == "water"

    def is_non_polymer(self):
        return self.primary_type == "non-polymer"

    def is_polymer(self):
        return self.primary_type == "polymer"

    def is_branch(self):
        return self.primary_type == "branch"

    def is_protein(self):
        return self.secondary_type == "polypeptide(L)"

    def is_dna(self):
        return self.secondary_type == "polydeoxyribonucleotide"

    def is_rna(self):
        return self.secondary_type == "polyribonucleotide"

    def is_nucleic_acid(self):
        return self.secondary_type == "polydeoxyribonucleotide/polyribonucleotide hybrid" or self.is_dna() or self.is_rna()

    def molecules_by_id(self, molecule_id):
        """
        Returns a list of molecules with the given id
        """
        return get_by_attrib(self.models, 'id', molecule_id)

    @property
    def molecules(self):
        return self

    @property
    def monomers(self):
        return [ mon for mol in self for mon in mol ]

    @property
    def atoms(self):
        return [ a for mol in self for mon in mol for a in mon ]

class Molecule(Child, Parent):
    """
    """
    def __init__(self, molecule_id, pdb_id):
        """
        """
        super(Molecule, self).__init__()
        self.id = molecule_id
        self.pdb_id = pdb_id

    def delete(self):
        """
        """
        self.entity.remove(self)

    def __str__(self):
        return self.id

    def __repr__(self):
        return "Molecule({0}, {1})".format(self.id, repr(self.entity))

    def __eq__(self, other):
        """
        Overrides list equality, only the same object compares as equal
        """
        return id(self) == id(other)

    def __hash__(self):
        return id(self)

    @property
    def entity(self):
        return self.parent

    @entity.setter
    def entity(self, value):
        self.parent = value

    @property
    def model(self):
        return self.entity.model if self.entity else None

    @property
    def structure(self):
        return self.model.structure if self.model else None

    def is_water(self):
        return self.entity.is_water()

    def is_polymer(self):
        return self.entity.is_polymer()

    def is_branch(self):
        return self.entity.is_branch()

    def is_nonpolymer(self):
        return self.entity.is_nonpolymer()

    def is_protein(self):
        return self.entity.is_protein()

    def is_nucleic_acid(self):
        return self.entity.is_nucleic_acid()

    def monomer_by_id(self, molecule_id):
        """
        Returns a monomer with the given id
        """
        return get_by_attrib_single(self.models, 'id', molecule_id)

    @property
    def monomers(self):
        return self

    @property
    def atoms(self):
        return [ a for mon in self for a in mon ]


class Polymer(Molecule):
    """
    """
    def __init__(self, molecule_id, pdb_id, seq_res, atom_res):
        """
        """
        super(Polymer, self).__init__(molecule_id, pdb_id)

        seq_res_i = []
        i = 0
        for s, a in zip(seq_res, atom_res):
            if a:
                index = i
                i += 1
            else:
                index = None
            seq_res_i.append((s, index))
        self.seq_res = seq_res_i
        
    def __repr__(self):
        return "Polymer({0}, {1})".format(self.id, repr(self.entity))

class Monomer(Child, Parent):
    """
    Representation of a collection of atoms.
    """
    def __init__(self, monomer_id, monomer_type):
        """
        """
        super(Monomer, self).__init__()
        self.id = monomer_id
        self.type = monomer_type

    def __str__(self):
        return "{0}.{1}".format(self.id, self.type)

    def __repr__(self):
        return "Monomer({0}, {1})".format(
            self.id, self.type
        )

    def __eq__(self, other):
        """
        Overrides list equality, only the same object compares as equal
        """
        return id(self) == id(other)

    def __hash__(self):
        return id(self)

    def delete(self):
        """
        """
        self.molecule.remove(self)

    @property
    def molecule(self):
        return self.parent

    @molecule.setter
    def molecule(self, value):
        self.parent = value

    @property
    def entity(self):
        return self.molecule.entity if self.molecule else None

    @property
    def model(self):
        return self.entity.model if self.entity else None

    @property
    def structure(self):
        return self.model.structure if self.model else None

    def is_residue(self):
        return False

    def is_amino_acid(self):
        return False

    def is_nucleotide(self):
        return False

    def atom_by_name(self, name):
        """
        Returns a atom with the given name
        """
        return get_by_attrib_single(self, 'name', name)

    @property
    def atoms(self):
        return self;

#@total_ordering
class Residue(Monomer):
    def __eq__(self, other):
        return self.molecule == other.molecule and self.id == other.id
    def __lt__(self, other):
        return self.molecule == other.molecule and self.id < other.id
    def __le__(self, other):
        return self.molecule == other.molecule and self.id <= other.id
    def __gt__(self, other):
        return self.molecule == other.molecule and self.id > other.id
    def __ge__(self, other):
        return self.molecule == other.molecule and self.id >= other.id
    def __hash__(self):
        return id(self)
    def is_residue(self):
        return True
    def is_amino_acid(self):
        return self.molecule.entity.is_protein()
    def is_nucleotide(self):
        return self.molecule.entity.is_nucleotide()
    def backbone_atoms(self):
        return [a for a in self if a.name in BACKBONE_HEAVY_ATOM_NAMES]
    def side_chain_atoms(self):
        return [a for a in self if a.name not in BACKBONE_HEAVY_ATOM_NAMES]

class Atom(Child):
    """
    Atom representation.

    Contains name, coordinate, b-factors, occupancy, and links to parent
    objects: residue/molecule and element
    """

    def __init__(self, element, name, coordinates, bfactor, occupancy):
        """
        Atom ctor.

        @param element: element
        @type element: string

        @param name: the atom name in context of the monomer
        @type name: string

        @param coordinates: x,y,z atom coordinates
        @type coordinates: iterable with 3 componets, each convertible to float

        @param bfactor: B-factor measure of coordinate uncertainty
        @type bfactor: convertible to float

        @param occupancy: the fractional occupancy of a resuide, range 0.0 to 1.0
        @type occupancy: convertible to float

        """
        super(Atom, self).__init__()
        self.element = element
        self.name = name
        self.coordinates = np.array(list(map(float, coordinates)))
        self.bfactor = float(bfactor)
        self.occupancy = float(occupancy)

    def delete(self):
        """
        Atom dtor.

        Deletes atoms by removing it from parents.
        """
        self.monomer.remove(self)

    @property
    def monomer(self):
        return self.parent

    @monomer.setter
    def monomer(self, value):
        self.parent = value

    @property
    def molecule(self):
        return self.monomer.molecule if self.monomer else None

    @property
    def entity(self):
        return self.molecule.entity if self.molecule else None

    @property
    def model(self):
        return self.entity.model if self.entity else None

    @property
    def structure(self):
        return self.model.structure if self.model else None

    def __str__(self):
        return self.name

    def __repr__(self):
        return "Atom({0})".format(", ".join(list(map(repr,[self.element, self.monomer, self.name, self.coordinates, self.bfactor, self.occupancy]))))

    def __hash__(self):
        return id(self)

    def is_multi_occ(self):
        """
        Returns true if this atom has multiple possitions with fractional
        occupancy, false otherwise.
        """
        return False

class MultiOccAtom(Child,Parent):
    """
    Multiple occupancy atom representation.

    Presents the same interface as an Atom, defaulting to the highest occupancy
    atom.  Also presents methods to access alternative atoms and set the
    default atom.

    These are defined as atoms with the same name and same monomer index. Alt
    location ids are not always provided, but are included here if provided.
    """

    def __init__(self, element, name):
        """
        """
        super(MultiOccAtom, self).__init__()
        self.element = element
        self.name = name

        self.alt_locs = []
        self.default = 0

    def __eq__(self, other):
        """
        Overrides list equality, only the same object compares as equal
        """
        return id(self) == id(other)

    def __hash__(self):
        return id(self)

    def delete(self):
        """
        Atom dtor.

        Deletes atoms by removing it from parents.
        """
        self.monomer.remove(self)

    @property
    def monomer(self):
        return self.parent

    @monomer.setter
    def monomer(self, value):
        self.parent = value

    @property
    def molecule(self):
        return self.monomer.molecule if self.monomer else None

    @property
    def entity(self):
        return self.molecule.entity if self.molecule else None

    @property
    def model(self):
        return self.entity.model if self.entity else None

    @property
    def structure(self):
        return self.model.structure if self.model else None

    def add_atom(self, coordinates, bfactor, occupancy, alt_loc):
        atom = Atom(self.element, self.name, coordinates, bfactor, occupancy)
        self.append(atom)
        if atom.occupancy > self[0].occupancy:
            self.default = len(self)-1
        self.alt_locs.append(alt_loc)

    def __getattr__(self,name):
        """
        Forwards attribute access to default atom.
        """
        return getattr(self[self.default], name)

#    def __setattr__(self,name,value):
#        """
#        Forwards attribute access to default atom.
#        """
#        return setattr(self.atoms[self.default], name, value)

#    def __delattr__(self,name):
#        """
#        Forwards attribute access to default atom.
#        """
#        return delattr(self.atoms[self.default], name)

    def __str__(self):
        return str(self.atom[self.default])

    def __repr__(self):
        return "MultiOccAtom({0},{1})".format(repr(self.atoms), repr(self.alt_locs))

    def is_multi_occ(self):
        """
        Returns true if this atom has multiple possitions with fractional
        occupancy, false otherwise.
        """
        return True

    def get_num_occ(self):
        return len(self)

    def set_default_atom(self, default):
        self.default = default


#@total_ordering
class MonomerID(object):
    """
    Encapsulates residue index and insertion code.
    """
    __slots__ = ["_index", "_pdb_index", "_ic"]
    def __init__(self, index, pdb_index, ic=None):
        """
        """
        self._index = int(index) if index is not None else None
        self._pdb_index = int(pdb_index)
        self._ic = str(ic) if ic else None

    def __str__(self):
        return str(self._pdb_index) + (self._ic if self._ic else "") if self else ""

    def __repr__(self):
        return "MonomerID({0}, {1}, {2})".format(self._index, self._pdb_index, self._ic)

    def __eq__(self, other):
        """
        Equality only when both index and ic are the same
        """
        return self._index == other._index and self._ic == other._ic

    def __ne__(self, other):
        """
        total_ordering should supply this, but doesn't, why?
        """
        return not self.__eq__(other)

    def __gt__(self, other):
        """
        Greater then based on numerical order for index then lexical
        for ic
        """
        return self._index > other._index or (self._index == other._index and self._ic > other._ic)

    def __hash__(self):
        return hash( (self._index, self._ic) )

    def __lt__(self, other):
        return not (self.__eq__(other) or self.__gt__(other))

    def __ge__(self, other):
        return self.__eq__(other) or self.__gt__(other)

    def __le__(self, other):
        return not self.__gt__(other)

    def __nonzero__(self):
        return self._index is not None

    @property
    def index(self):
        return self._index

    @property
    def ic(self):
        return self._ic
