from mmstructlib.math.operation import Operator
import numpy as np

class UnitCell(object):
    """
    Contains unit cell parameters.  Also, generates transforms between
    cartesian and unit cell coordinates.

    @param dimensions: unit cell dimensions [a, b, c]
    @type dimensions: iterable of floats or convertible to floats

    @param dimensions: unit cell angles [alpha, beta, gamma]
    @type dimensions: in degrees, iterable of floats or convertible to floats

    @param space_group: space group symbol
    @type space_group: string

    """
    def __init__(self, dimensions, angles, space_group_name, space_group_number):
        """
        """
        self.dimensions = np.array(list(map(float,dimensions)))
        self.angles = np.radians(np.array(list(map(float,angles))))
        self.space_group = space_group_name
        self.space_group_number = space_group_number

class AssemblyOperator(Operator):
    """
    Describes a symmetry operation.

    Additionally, contains metadata about the operation.

    Attributes:
        id (str): unique id of the operation
        type (str): description of the physical basis of the operation
        name (str): condensed operation description
        symmetry (str): mathmatical description of the operation
    """
    def __init__(self, op_id, op_type, op_name, op_sym, translate, transform):
        super(AssemblyOperator, self).__init__(translate, transform)
        self.id = op_id
        self.type = op_type
        self.name = op_name
        self.symmetry = op_sym

class Assembly(object):
    """
    Contains information about biological assemblies.

    X-ray structure files contain the assymetric unit.  Assembly entries
    describe the transformations to (portions of) the assymetric unti that
    create the biologically relevant assemblies.

    Attributes:
        assembly_id (str): unique id of assembly
        details (str, optional): description of how the assemby was determined
        method (str, optional): method used to determine the assembly
        oligomer (str): cardinality of the complex (text)
        oligomer_count (int): cardinality of the complex (int)
        operation_assyms (list(list(AssemblyOperator), assym_id)): the operator expression,
            assembly id pairs
        assym_ids (set): set of assym ids (not pdb ids) included in the assembly.
    """
    def __init__(self, assembly_id, details, method, oligomer, oligomer_count):
        """
        For use by cif builder.
        """
        self.id = assembly_id
        self.details = details
        self.method = method
        self.oligomer = oligomer
        self.oligomer_count = int(oligomer_count) if oligomer_count is not None else None
        self.operation_assyms = []

    def add_operator(self, op_expr, asym_id_lst):
        """
        For use by cif builder.
        """
        #for asym_id in asym_id_lst:
        self.operation_assyms.append( [op_expr, asym_id_lst] )

    @property
    def assym_ids(self):
        return set([aid for ops, aids in self.operation_assyms for aid in aids])
