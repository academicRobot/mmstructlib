import unittest
from mmstructlib.IO.cif import parse_cif, get_model_atoms
from test.data import get_file_path

def flattenCells(cells):
    lst = []
    for row in cells:
        lst.extend(row)
    return lst

class TestCifGetModelAtoms(unittest.TestCase):
    "CIF parser"
    def setUp(self):
        pass
    def tearDown(self):
        pass
    def test_run_get_model_atoms1(self):
        with open(get_file_path("2OCJ.cif"), "rb") as fh:
            tables = parse_cif(fh)
        lst = get_model_atoms(tables)
        self.assertEqual(type(lst), list)
        self.assertEqual(len(lst), 1)
        self.assertEqual([k for k, g in lst], ["1"])
        self.assertEqual(len(lst[0][1]), 6485)
    def test_run_get_model_atoms2(self):
        with open(get_file_path("1JSP.cif"), "rb") as fh:
            tables = parse_cif(fh)
        lst = get_model_atoms(tables)
        self.assertEqual(type(lst), list)
        self.assertEqual(len(lst), 20)
        self.assertEqual([k for k, g in lst], list(map(str, range(1,21))))
        for k, g in lst:
            self.assertEqual(len(g), 2375)

if __name__ == '__main__':
    unittest.main()

