import numpy as np

class GridHash:
    """
    Facilitate finding spacial neighbors.  Maps 3D coordinates to some arbitrary item.
    """
    def __init__(self, coord_item_pair_iter, max_distance):
        self.coord_sparse_mat = dict()
        self.max_coords = (-np.inf,)*3
        self.min_coords = (np.inf,)*3
        for coord, item in coord_item_pair_iter:
            i = np.array(coord / max_distance).astype(int)
            self.max_coords = np.max(np.vstack([self.max_coords, i]), axis=0)
            self.min_coords = np.min(np.vstack([self.min_coords, i]), axis=0)
            i = tuple(i)
            if i not in self.coord_sparse_mat:
                self.coord_sparse_mat[i] = []
            self.coord_sparse_mat[i].append( (coord, item) )
        self.max_coords = self.max_coords.astype(int)
        self.min_coords = self.min_coords.astype(int)
        self.max_distance = max_distance
        self.max_distance_sq = max_distance**2

    def neighbor_iter(self, coord):
        x = np.array(coord / self.max_distance).astype(int)
        pot_neighbors = (
            pair
            for di in range(-1, 2)
            for dj in range(-1, 2)
            for dk in range(-1, 2)
            for pair in self.coord_sparse_mat.get(
                (x[0] + di, x[1] + dj, x[2] + dk),
                []
            )
        )
        neighbors = [
            (icoord, item)
            for icoord, item in pot_neighbors
            if self._dist(icoord, coord)
        ]
        return neighbors

    def _dist(self, c1, c2):
        return np.sum(np.square(c1 - c2)) <= self.max_distance_sq

    def all_neighbors_iter(self):
        def _all_neighbors(lst1, lst2):
            return [
                (i1, i2)
                for i1 in lst1
                for i2 in lst2
                if id(i1) != id(i2) and #not the same object
                self._dist(i1[0], i2[0])
            ]

        return (
            pair
            for xi in range(self.min_coords[0], self.max_coords[0] + 1)
            for xj in range(self.min_coords[1], self.max_coords[1] + 1)
            for xk in range(self.min_coords[1], self.max_coords[1] + 1)
            for di in range(0, 2)
            for dj in range(0, 2)
            for dk in range(0, 2)
            for pair in _all_neighbors(
                self.coord_sparse_mat.get((xi, xj, xk), []),
                self.coord_sparse_mat.get((xi+di, xj+dj, xk+dk), [])
            )
        )

if __name__ == '__main__':
    import sys
    from mmstructlib.IO import load_cif_from_mirror
    if len(sys.argv) != 2:
        exit("usage: {} <struct id>".format(sys.argv[0]))

    struct = load_cif_from_mirror(sys.argv[1])

    gridhash = GridHash([(a.coordinates, a) for a in struct[0].atoms], 10)
    for (ac1, a1), (ac2, a2) in gridhash.all_neighbors_iter():
        print("{}.{}/{}/{}-{}.{}/{}/{}".format(
            a1.molecule.id, a1.monomer.id, a1.monomer.type, a1.name,
            a2.molecule.id, a2.monomer.id, a2.monomer.type, a2.name
        ))
