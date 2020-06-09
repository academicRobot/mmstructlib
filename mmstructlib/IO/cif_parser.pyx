#distutils: language = c++

from libcpp.vector cimport vector
from libcpp.map cimport map as cppmap
from libcpp cimport bool as cppbool
from libcpp.string cimport string
from cpython.buffer cimport PyObject_CheckBuffer, PyObject_GetBuffer, PyBuffer_Release, PyBUF_SIMPLE
from cython.operator cimport dereference as deref

ctypedef const char* const_char_ptr

cdef extern from "boost/optional.hpp" namespace "boost":
    cdef cppclass optional[T]:
        T& get()

cdef extern from "cif/check_optional.hpp":
#    cppbool check_optional_string "check_optional<std::string>" (const optional[string]& opt)
    cppbool check_optional[T](const optional[T]& opt)

cdef extern from "cif/table.hpp" namespace "cif":
    cdef cppclass table[T]:
        T name
        vector[T] cellNames
        vector[optional[T]] cells

cdef extern from "cif/save.hpp" namespace "cif":
    cdef cppclass save[T]:
        T tag
        optional[string] field
        vector[table[string]] tables

cdef extern from "cif/block.hpp" namespace "cif":
    cdef cppclass block[T]:
        T name
        vector[table[string]] tables
        vector[save[string]] saves

cdef extern from "cif/file.hpp" namespace "cif":
    cdef cppclass file[T]:
        vector[block[string]] blocks

cdef extern from "cif/parse.hpp":
    void parseString[T](T it, T endIt, file[string]& fileObj) except +
    #void parseString_const_char_ptr "parseString<const char*>"(const_char_ptr it, const_char_ptr endIt, file[string]& fileObj) except +

cdef object processOptional(const optional[string]& optstr):
    return (bytes(optstr.get())).decode('UTF-8') if check_optional(optstr) else None

ctypedef optional[string]* OptStrPtr

cdef class _Row:
    cdef object filePyobj #keep a pointer to file to keep it alive
    cdef OptStrPtr row_ptr
    cdef size_t num_col
    def __init__(self):
        raise RuntimeError("Do not construct directly")
    def __getitem__(self, size_t i):
        if not 0 <= i < self.num_col:
            raise IndexError("Row index out of range")
        return processOptional(deref(self.row_ptr+i))
    def __len__(self):
        return self.num_col
cdef object Row(object filePyobj, size_t num_col, OptStrPtr row_ptr):
    cdef _Row obj = _Row.__new__(_Row)
    obj.filePyobj = filePyobj
    obj.row_ptr = row_ptr
    obj.num_col = num_col
    return obj

cdef class _CellsIterator:
    cdef object filePyobj #keep a pointer to file to keep it alive
    cdef OptStrPtr ptr
    cdef OptStrPtr end_ptr
    cdef size_t num_col
    def __init__(self):
        raise RuntimeError("Do not construct directly")
    def __next__(self):
        if self.ptr >= self.end_ptr:
            raise StopIteration
        next_row = Row(self.filePyobj, self.num_col, self.ptr)
        self.ptr += self.num_col
        return next_row
cdef object CellsIterator(object filePyobj, size_t num_col, OptStrPtr ptr, OptStrPtr end_ptr):
    cdef _CellsIterator obj = _CellsIterator.__new__(_CellsIterator)
    obj.filePyobj = filePyobj
    obj.ptr = ptr
    obj.end_ptr = end_ptr
    obj.num_col = num_col
    return obj

cdef class _Cells:
    cdef object filePyobj #keep a pointer to file to keep it alive
    cdef OptStrPtr cell_ptr
    cdef size_t num_col
    cdef size_t num_row
    def __init__(self):
        raise RuntimeError("Do not construct directly")
    def __getitem__(self, size_t i):
        if not 0 <= i < self.num_row:
            raise IndexError("Cells row index out of range")
        return Row(self.filePyobj, self.num_col, self.cell_ptr + self.num_col*i)
    def __len__(self):
        return self.num_row
    def __iter__(self):
        return CellsIterator(self.filePyobj, self.num_col, self.cell_ptr, self.cell_ptr+self.num_row*self.num_col)
cdef Cells(object filePyobj, size_t num_row, size_t num_col, OptStrPtr cell_ptr):
    cdef _Cells obj = _Cells.__new__(_Cells)
    obj.filePyobj = filePyobj
    obj.cell_ptr = cell_ptr
    obj.num_row = num_row
    obj.num_col = num_col
    return obj

cdef class _Table:
    cdef object filePyobj #keep a pointer to file to keep it alive
    cdef table[string]* thisptr
    cdef cppmap[string, size_t] column_index
#    cdef object column_index
    cdef size_t num_col
    cdef size_t num_row
    def __init__(self):
        raise RuntimeError("Do not construct directly")
    def index(self, object name):
        return self.column_index[name.encode('UTF-8')]
    def field(self, object name, size_t row = 0):
        return processOptional(
            self.thisptr.cells[
                row * self.num_col + self.column_index[name.encode('UTF-8')]
            ]
        )
    property name:
        def __get__(self):
            return self.thisptr.name.decode('UTF-8')
    property cell_names:
        def __get__(self):
            return list(map(lambda x: x.decode('UTF-8'), list(self.thisptr.cellNames)))
    property cells:
        def __get__(self):
            return Cells(self.filePyobj, self.num_row, self.num_col, &(self.thisptr.cells[0]))
cdef object Table(object filePyobj, table[string]* tablePtr):
    cdef size_t i
    cdef size_t size
    cdef _Table obj = _Table.__new__(_Table)
    obj.filePyobj = filePyobj
    obj.thisptr = tablePtr
    obj.num_col = tablePtr.cellNames.size()
 #   obj.column_index = dict()
    for i in range(obj.num_col):
        obj.column_index[tablePtr.cellNames[i]] = i
    size = tablePtr.cells.size()
    obj.num_row = size / obj.num_col
    if size != obj.num_row * obj.num_col:
        raise RuntimeError("Table data size not a multiple of columns")
    return obj

cdef class _TableVector:
    cdef object filePyobj #keep a pointer to file to keep it alive
    cdef vector[table[string]]* thisptr
    cdef cppmap[string, size_t] table_index
    def __init__(self):
        raise RuntimeError("Do not construct directly")
    def __getitem__(self, object table_id):
        if isinstance(table_id, str):
            val = self.get(table_id)
            if val is None:
                raise IndexError("TableVector: name '{}' not found".format(table_id))
        elif isinstance(table_id, int):
            if not 0 <= table_id < self.thisptr.size():
                raise IndexError("TableVector row index out of range")
            val = Table(self.filePyobj, &(deref(self.thisptr)[table_id]))
        else:
            raise IndexError("TableVector getitem takes int or str")
        return val
    def get(self, object key_str, object default = None):
        cdef string key = key_str.encode('UTF-8')
        cdef cppmap[string, size_t].iterator res = self.table_index.find(key)
        if res == self.table_index.end():
            return default
        return Table(self.filePyobj, &(deref(self.thisptr)[deref(res).second]))
    def __contains__(self, object key_str):
        cdef string key = key_str.encode('UTF-8')
        cdef cppmap[string, size_t].iterator res = self.table_index.find(key)
        return  res != self.table_index.end()
    def __len__(self):
        return self.thisptr.size()
cdef object TableVector(object filePyobj, vector[table[string]]* ptr):
    cdef size_t i
    cdef _TableVector obj = _TableVector.__new__(_TableVector)
    obj.filePyobj = filePyobj
    obj.thisptr = ptr
    for i in range(ptr.size()):
        obj.table_index[ptr.at(i).name] = i
    return obj

cdef class _Save:
    cdef object filePyobj #keep a pointer to file to keep it alive
    cdef save[string]* thisptr
    def __init__(self):
        raise RuntimeError("Do not construct directly")
    property tag:
        def __get__(self):
            return self.thisptr.tag.decode('UTF-8')
    property field:
        def __get__(self):
            return processOptional(self.thisptr.field)
    property tables:
        def __get__(self):
            return TableVector(self.fileObj, &(self.thisptr.tables))
cdef object Save(object filePyobj, save[string]* savePtr):
    cdef _Save obj = _Save.__new__(_Save)
    obj.filePyobj = filePyobj
    obj.thisptr = savePtr
    return obj

cdef class _SaveVector:
    cdef object filePyobj #keep a pointer to file to keep it alive
    cdef vector[save[string]]* thisptr
    def __init__(self):
        raise RuntimeError("Do not construct directly")
    def __getitem__(self, size_t i):
        if not 0 <= i < self.thisptr.size():
            raise IndexError("SaveVector row index out of range")
        return Save(self.filePyobj, &(deref(self.thisptr)[i]))
    def __len__(self):
        return self.thisptr.size()
cdef SaveVector(object filePyobj, vector[save[string]]* ptr):
    cdef _SaveVector obj = _SaveVector.__new__(_SaveVector)
    obj.filePyobj = filePyobj
    obj.thisptr = ptr
    return obj

cdef class _Block:
    cdef object filePyobj #keep a pointer to file to keep it alive
    cdef block[string]* thisptr
    def __init__(self):
        raise RuntimeError("Do not construct directly")
    property name:
        def __get__(self):
            return deref(self.thisptr).name.decode('UTF-8')
    property tables:
        def __get__(self):
            return TableVector(self.filePyobj, &(self.thisptr.tables))
    property saves:
        def __get__(self):
            return SaveVector(self.filePyobj, &(self.thisptr.saves))
cdef Block(object filePyobj, block[string]* blockPtr):
    cdef _Block obj = _Block.__new__(_Block)
    obj.filePyobj = filePyobj
    obj.thisptr = blockPtr
    return obj

cdef class _BlockVector:
    cdef object filePyobj #keep a pointer to file to keep it alive
    cdef vector[block[string]]* thisptr
    def __init__(self):
        raise RuntimeError("Do not construct directly")
    def __getitem__(self, size_t i):
        if not 0 <= i < self.thisptr.size():
            raise IndexError("BlockVector index out of range")
        return Block(self.filePyobj, &(deref(self.thisptr)[i]))
    def __len__(self):
        return self.thisptr.size()
cdef object BlockVector(object filePyobj, vector[block[string]]* ptr):
    cdef _BlockVector obj = _BlockVector.__new__(_BlockVector)
    obj.filePyobj = filePyobj
    obj.thisptr = ptr
    return obj

cdef class File:
    cdef file[string] fileObj
    property blocks:
        def __get__(self):
            return BlockVector(self, &self.fileObj.blocks)

def parse(object pyobj):
    cdef Py_buffer objbuff
    cdef const_char_ptr ptr
    #not working as expected in py3, do I actually care if its a string or bytes?
    #if not isinstance(pyobj, basestring):
    #    raise RuntimeError("object is not a string")
    if not PyObject_CheckBuffer(pyobj):
        raise RuntimeError("argument does not support buffers")
    if PyObject_GetBuffer(pyobj, &objbuff, PyBUF_SIMPLE):
        raise RuntimeError("Inconsistent buffer object")
    ptr = <const_char_ptr>objbuff.buf
    fileObj = File()
    parseString(ptr, ptr+objbuff.len, fileObj.fileObj)
    PyBuffer_Release(&objbuff)
    return fileObj



