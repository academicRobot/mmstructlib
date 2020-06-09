import unittest
from mmstructlib.IO.cif_parser import parse
from test.data import get_file_contents

def flattenCells(cells):
    lst = []
    for row in cells:
        lst.extend(row)
    return lst

class TestCifParser(unittest.TestCase):
    "CIF parser"
    def setUp(self):
        pass
    def tearDown(self):
        pass
    def check_table(self, table, name, names, cells):
        self.assertEqual(table.name, name)
        self.assertEqual(len(table.cell_names), len(names))
        self.assertEqual(table.cell_names, names)
        self.assertEqual(len(table.cells), len(cells))
        self.assertEqual(flattenCells(table.cells), flattenCells(cells))
    def test_parse(self):
        "Parse valid, basic cif and verify"
        cif_file = parse(get_file_contents("cif_test_good.cif"))
        self.assertEqual(len(cif_file.blocks), 1)
        block = cif_file.blocks[0]
        self.assertEqual(block.name, "test_data")
        self.assertEqual(len(block.tables), 4)
        t1, t2, t3, t4 = block.tables #uses __getitem__ to unpack TableVector
        self.check_table(t1, "single_single", ["f1"], [["A"]])
        self.check_table(t2, "single_multi", ["f1", "f2", "f3"], [["B", "C", "D"]])
        self.check_table(t3, "multi_single", ["f1"], [["E"], ["F"], ["G"]])
        self.check_table(t4, "multi_multi", ["f1", "f2", "f3"], [["H", "I", "J"], [None, "K", "L"], ["M", "N", "O"]])
    def test_parse_table_lookup(self):
        "Parse valid, basic cif and verify"
        cif_file = parse(get_file_contents("cif_test_good.cif"))
        self.assertEqual(len(cif_file.blocks), 1)
        block = cif_file.blocks[0]
        self.assertEqual(block.name, "test_data")
        self.assertEqual(len(block.tables), 4)
        t = block.tables
        self.check_table(t['single_single'], "single_single", ["f1"], [["A"]])
        self.check_table(t['single_multi'], "single_multi", ["f1", "f2", "f3"], [["B", "C", "D"]])
        self.check_table(t['multi_single'], "multi_single", ["f1"], [["E"], ["F"], ["G"]])
        self.check_table(t['multi_multi'], "multi_multi", ["f1", "f2", "f3"], [["H", "I", "J"], [None, "K", "L"], ["M", "N", "O"]])
    def test_bad_parse_missing_data(self):
        "Test missing field in single row table"
        self.assertRaises(RuntimeError,  parse, get_file_contents("cif_test_bad1.cif"))
    def test_bad_parse_block_keyword(self):
        "Test bad block keyword"
        self.assertRaises(RuntimeError,  parse, get_file_contents("cif_test_bad2.cif"))

if __name__ == '__main__':
    unittest.main()

