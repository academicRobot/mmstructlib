#include <boost/optional.hpp>

template <typename T>
inline bool check_optional(const boost::optional<T>& opt){
	return bool(opt);
}

