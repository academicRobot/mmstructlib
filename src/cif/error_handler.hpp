#ifndef CIF_ERROR_HANDLER_HPP
#define CIF_ERROR_HANDLER_HPP

#include <string>
#include <vector>
#include <sstream>

namespace cif{

template <typename iterator_tp>
struct error_handler{
	typedef iterator_tp iterator_t;

	error_handler(void): errFlag(false) {}

	template <typename a1_t, typename a2_t, typename a3_t, typename a4_t>
	struct result { typedef void type; };

	template <typename lex_it_t, typename info_t>
	void operator()(const lex_it_t& startIter, const lex_it_t& posIter, 
	                std::string loc, const info_t& info)
//#if (defined(_MSC_VER) && _MSC_VER == 1700)
	const //workaround for vs 2012 std::bind bug
//#endif
	{
		stackLocs.push_back(loc);
		if(!errFlag){
			//startIt = startIter->matched().begin();
			//posIt = posIter->matched().begin();
			std::stringstream ss;
			ss << info;
			msg = ss.str();//.what();
			errFlag = true;
		}
	}

//#ifdef _MSC_VER
	mutable //workaround for vs 2012 std::bind bug
//#endif
	bool errFlag;

//#ifdef _MSC_VER
	mutable //workaround for vs 2012 std::bind bug
//#endif
	iterator_t startIt, posIt;

//#ifdef _MSC_VER
	mutable //workaround for vs 2012 std::bind bug
//#endif
	std::string msg;

//#ifdef _MSC_VER
	mutable //workaround for vs 2012 std::bind bug
//#endif
	std::vector<std::string> stackLocs;
};

}

#endif //CIF_ERROR_HANDLER_HPP
