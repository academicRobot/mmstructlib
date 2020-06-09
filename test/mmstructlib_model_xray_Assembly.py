import unittest
import numpy as np
from mmstructlib.model.xray import Assembly

class MockAssemblyOperator(object):
    def __init__(self, id):
        self.id = id

class TestModelXrayAssembly(unittest.TestCase):
    def setUp(self):
        try:
            self.assem = Assembly(
                "1",
                "author_and_software_defined_assembly", 
                "PISA",
                "dimeric",
                "2"
            )
        except:
            self.fail()
    def tearDown(self):
        pass
    def test_id(self):
        self.assertEqual(self.assem.id, '1')
    def test_details(self):
        self.assertEqual(self.assem.details, 'author_and_software_defined_assembly')
    def test_method(self):
        self.assertEqual(self.assem.method, 'PISA')
    def test_oligomer(self):
        self.assertEqual(self.assem.oligomer, 'dimeric')
    def test_oligomer_count(self):
        self.assertEqual(self.assem.oligomer_count, 2)
    def test_add_operator(self):
        self.assem.add_operator(MockAssemblyOperator(1), ["A", "B"])
        self.assem.add_operator(MockAssemblyOperator(2), ["C"])
        self.assertEqual(self.assem.operations[0][0].id, 1)
        self.assertEqual(self.assem.operations[0][1], ["A", "B"])
        self.assertEqual(self.assem.operations[1][0].id, 2)
        self.assertEqual(self.assem.operations[1][1], ["C"])

if __name__ == '__main__':
    unittest.main()

