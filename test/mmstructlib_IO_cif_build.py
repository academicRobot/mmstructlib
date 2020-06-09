import unittest
from mmstructlib.IO.cif import parse_cif, build
from mmstructlib.model import Structure
from test.data import get_file_path

# TODO: need more checks on structure, perhaps using mock tables
class TestCifBuild(unittest.TestCase):
    def setUp(self):
        pass
    def tearDown(self):
        pass
    def test_run_build_structure1(self):
        with open(get_file_path("2OCJ.cif"), "rb") as fh:
            tables = parse_cif(fh)
        struct = build(tables)
        self.assertEqual(type(struct), Structure)
        self.assertEqual(struct.id, "2OCJ")
        self.assertTrue(struct.is_xray())
        self.assertFalse(struct.is_nmr())
        self.assertEqual(type(struct.resolution), float)
    def test_run_build_structure2(self):
        with open(get_file_path("1JSP.cif"), "rb") as fh:
            tables = parse_cif(fh)
        struct = build(tables)
        self.assertEqual(type(struct), Structure)
        self.assertEqual(struct.id, "1JSP")
        self.assertFalse(struct.is_xray())
        self.assertTrue(struct.is_nmr())
        self.assertEqual(struct.resolution, None)

if __name__ == '__main__':
    unittest.main()

