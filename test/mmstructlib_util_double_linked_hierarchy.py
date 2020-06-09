import unittest
from mmstructlib.util.double_linked_hierarchy import Parent, Child

class TestUtilDoubleLinkedHierarchy(unittest.TestCase):
    def setUp(self):
        pass
    def tearDown(self):
        pass
    def test_append(self):
        parent = Parent()
        child = Child()
        parent.append(child)
        self.assertEqual(parent[0], child)
        self.assertEqual(id(parent), id(child.parent))
    def test_extend(self):
        parent = Parent()
        children = [ Child() for i in range(5) ]
        parent.extend(children)
        self.assertEqual(list(parent), children)
        self.assertEqual(5, len(parent))
        self.assertEqual([id(parent)]*5, [ id(c.parent) for c in children ])
    def test_insert(self):
        parent = Parent()
        children = [ Child() for i in range(5) ]
        parent.extend(children)
        child = Child()
        parent.insert(1, child)
        self.assertEqual(parent[1], child)
        self.assertEqual(id(parent), id(child.parent))
    def test_delitem(self):
        parent = Parent()
        children = [ Child() for i in range(5) ]
        parent.extend(children)
        del parent[1]
        self.assertEqual(4, len(parent))
        self.assertEqual(None, children[1].parent)
        self.assertEqual(list(parent), children[:1]+children[2:])
    def test_delslice(self):
        parent = Parent()
        children = [ Child() for i in range(5) ]
        parent.extend(children)
        del parent[1:3]
        self.assertEqual(3, len(parent))
        self.assertEqual(None, children[1].parent)
        self.assertEqual(None, children[2].parent)
        self.assertEqual(list(parent), children[:1]+children[3:])
    def test_setslice(self):
        parent = Parent()
        children = [ Child() for i in range(5) ]
        parent.extend(children)
        child = Child()
        parent[1:3] = [child]
        self.assertEqual(4, len(parent))
        self.assertEqual(None, children[1].parent)
        self.assertEqual(None, children[2].parent)
        self.assertEqual(id(parent), id(child.parent))
        self.assertEqual(list(parent), children[:1]+[child]+children[3:])
    def test_remove(self):
        parent = Parent()
        children = [ Child() for i in range(5) ]
        parent.extend(children)
        parent.remove(children[1])
        self.assertEqual(4, len(parent))
        self.assertEqual(None, children[1].parent)
        self.assertEqual(list(parent), children[:1]+children[2:])
    def test_pop(self):
        parent = Parent()
        children = [ Child() for i in range(5) ]
        parent.extend(children)
        child = parent.pop(1)
        self.assertEqual(4, len(parent))
        self.assertEqual(None, child.parent)
        self.assertEqual(list(parent), children[:1]+children[2:])
    def test_parent(self):
        parent = Parent()
        child = Child()
        child.parent = parent
        self.assertEqual(parent[0], child)
        self.assertEqual(id(parent), id(child.parent))

if __name__ == '__main__':
    unittest.main()

