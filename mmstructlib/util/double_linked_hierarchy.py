"""
Implements a double linked hierarchy.  Parent methods over-ride list manipulation
methods inorder to keep child-to-parent pointers consistent.  The child's member
'parent' points to the parent list.  Setting this member will remove the child
from its current list and place it at the end of the new one.
"""

from mmstructlib.util.weakbackref import WeakBackRef
from copy import deepcopy

class Parent(list):
    """
    Note: builtins don't call super().__init__(), so must this class must be put
    last in MRO
    """
    def __init__(self, *args, **kwargs):
        super(Parent, self).__init__(*args, **kwargs)

    def append(self, val):
        val._set_parent(self)
        list.append(self, val)

    def extend(self, seq):
        for item in seq:
            item._set_parent(self)
        list.extend(self, seq)

    def insert(self, i, val):
        val._set_parent(self)
        list.insert(self, i, val)

    def __delitem__(self, i):
        if isinstance(i, slice):
            for child in self[i]:
                child._weakref = WeakBackRef(None)
        else:
            self[i]._weakref = WeakBackRef(None)
        list.__delitem__(self, i)

    # !!! __delslice__ does not exist in python 3
    # python calls __delitem__ with a slice object
    #def __delslice__(self, i, j):
    #    for index in range(i,min(j,len(self))):
    #        self[index]._weakref = WeakBackRef(None)
    #    list.__delslice__(self, i, j)

    # !!! __setslice__ does not exist in python 3
    # python calls __setitem__ with a slice object
    #def __setslice__(self, i, j, seq):
    #    for index in range(i,min(j,len(self))):
    #        self[index]._weakref = WeakBackRef(None)
    #    for item in seq:
    #        item._set_parent(self)
    #    list.__setslice__(self, i, j, seq)

    def __setitem__(self, i, val):
        if isinstance(i, slice):
            for child in self[i]:
                child._weakref = WeakBackRef(None)
            list.__setitem__(self, i, val)
            for item in val:
                item._set_parent(self)
        else:
            self[i]._weakref = WeakBackRef(None)
            list.__setitem(self, i, val)
            val._set_parent(self)

    def __copy__(self):
        """
        Shallow copies include no children.
        """
        clss = type(self)
        new = clss.__new__(clss)
        new.__dict__.update(self.__dict__)
        return new

    def __deepcopy__(self, memo={}):
        """
        Deep copies include deepcopied children.
        """
        #print("parent deep copy {}".format(type(self)))
        clss = type(self)
        new = clss.__new__(clss)
        memo[id(self)] = new
        new.__dict__ = deepcopy(self.__dict__, memo)
        for i in self:
            i = deepcopy(i, memo)
            i._deepcopy_set_parent(new)
            list.append(new, i)
        return new

    def remove(self, val):
        del self[self.index(val)]

    def pop(self, i = -1):
        val = self[i]
        del self[i]
        return val

class Child(object):
    def __init__(self, *args, **kwargs):
        super(Child, self).__init__(*args, **kwargs)
        self._weakref = WeakBackRef(None)

    def _set_parent(self, parent):
        if self._weakref() is not None:
            self.parent.remove(self)
        self._weakref = WeakBackRef(parent)

    def __copy__(self):
        """
        Shallow copies have no parents
        """
        #make sure that Parent.__deepcopy__ is called first
        if isinstance(self, Parent):
            new = Parent.__copy__(self)
        else:
            clss = type(self)
            new = clss.__new__(clss)
            new.__dict__.update(self.__dict__)
        new._weakref = WeakBackRef(None)
        return new

    def __deepcopy__(self, memo={}):
        """
        Deepcopies have no parents, unless the parent sets it
        """
#        print("child deep copy {} {} {} {}".format(type(self), id(self), self, len(memo)))
        #make sure that Parent.__deepcopy__ is called first
        if isinstance(self, Parent):
#            print("\tgo to P")
            new = Parent.__deepcopy__(self, memo)
        else:
#            print("\tdo C")
            clss = type(self)
            new = clss.__new__(clss)
            memo[id(self)] = new
#            print("\tdo dict")
            new.__dict__ = deepcopy(self.__dict__, memo)
#            print("\tdone")

        #if deepcopy was called from parent, it will reset _weakref correctly
        #   after copy.  If called on this object directly, _weakref should be
        #   None, so set it as fall through
        new._weakref = WeakBackRef(None)
#        print("\tret new")
        return new

    def _deepcopy_set_parent(self, parent):
        """
        Don't remove from parent, I'm not actually there
        """
        self._weakref = WeakBackRef(parent)

    @property
    def parent(self):
        return self._weakref()

    @parent.setter
    def parent(self, new_parent):
        if new_parent is None:
            self._set_parent(None)
        else:
            new_parent.append(self)
