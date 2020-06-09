import numpy as np
from mmstructlib.math.operation import Operator

def to_unit_cell(dimensions, angles):
    """
    Returns a transform from cartesian to unit cell coordinates.
    """
    a, b, c = _basis_vectors(dimensions, angles)
    det = np.linalg.det(np.array([a, b, c]))
    rows = []
    for pos in range(3):
        unit = [0.0] * 3
        unit[pos] = 1.0
        unit = np.array(unit)
        arr = np.array([
            np.linalg.det(np.array([unit,b,c])),
            np.linalg.det(np.array([a,unit,c])),
            np.linalg.det(np.array([a,b,unit]))
        ])
        arr /= det
        rows.append(arr)
    return Operator(transform=np.transpose(np.array(rows)))


def to_cartesian(dimensions, angles):
    """
    Returns a transform from unit cell to cartesian coordinates.
    """
    return Operator(transform=np.transpose(np.array(_basis_vectors(dimensions, angles))))

def _basis_vectors(dimensions, angles):
    """
    Calculates basis vectors for transforms

    @param dimensions: list of three floats
    @type element: equivelent of [float, float, float]

    @param angles: list of three floats, in radians
    @type element: equivelent of [float, float, float]
    """
    a, b, c = dimensions
    alpha, beta, gamma = angles
    a_basis = np.array([a, 0.0, 0.0])
    b_basis = np.array([np.cos(gamma)*b, np.sin(gamma)*b, 0.0])
    c_basis = np.array([
        c * np.cos(beta),
        c * (np.cos(alpha) - np.cos(beta) * np.cos(gamma)) / np.sin(gamma),
        c * np.sqrt(
            1
            - np.power(np.cos(alpha),2.0)
            - np.power(np.cos(beta),2.0)
            - np.power(np.cos(gamma),2.0)
            + 2*np.cos(alpha)*np.cos(beta)*np.cos(gamma)
        ) / np.sin(gamma)
    ])
    return a_basis, b_basis, c_basis




