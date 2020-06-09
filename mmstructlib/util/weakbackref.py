import weakref
import copy

_DEFAULT_FUNC = lambda: None

class WeakBackRef(object):
    """
    Wrapper for weakref that will pickle and deepcopy in an intuitive way, kinda.  
    Pickling in an internal node will cause the entire structure to be pickled,
    but ancestors and siblings will be immediately garbage collected after
    unpickling (safe but inefficient).  Deepcopy will update the pointer to
    new parent node, unless there is no new parent, in which case it is set
    to None.
    """
    def __init__(self, obj):
        self._weakref = weakref.ref(obj) if obj is not None else _DEFAULT_FUNC
    def __deepcopy__(self, memo):
        return type(self)(memo.get(id(self._weakref()), None))
    def __reduce__(self):
        return (StrongWeakRef, self._weakref())
    def __call__(self):
        return self._weakref() 

