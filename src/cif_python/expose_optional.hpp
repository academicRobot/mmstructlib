/* Taken from:
   http://mail.python.org/pipermail/cplusplus-sig/2004-September/007756.html
*/

#include <boost/python/object.hpp>

namespace python_wrap{

template<typename T>
inline void set (boost::optional<T>& o, T x) {
  o = x;
}  

template<typename T>
inline boost::python::object get(boost::optional<T>& o) {
  return o ? boost::python::object(o.get()) : boost::python::object() ;
}

template<typename T>
static void exposeOptional (const char* name) {
  class_<boost::optional<T> >(name)
    .def ("set", &set<T>)
    .def ("get", &get<T>)
    .def (!(self))
    ;
}

}

