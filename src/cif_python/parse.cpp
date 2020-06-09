#include <common.hpp>

void parse(const std::string& str, parser_t::file_t& fileObj){
	std::string::const_iterator it = str.begin(), endIt = str.end();

	lexer_t lexer;
	lexer.init_dfa(true);
	err_t errHandle;
	parser_t parser(lexer, errHandle);
	if(!tokenize_and_parse(it, endIt, lexer, parser, fileObj)){
		std::stringstream sio;
		sio
			<< "**parse failed** " << std::endl
			<< 
				util::join(
					strs_rev_it_t(errHandle.stackLocs.end()),
					strs_rev_it_t(errHandle.stackLocs.begin()),
					" -> "
				)
			<< ":" << errHandle.msg << std::endl
			<< cif::parse_err_msg(it, str.begin(), endIt);
		throw std::runtime_error(sio.str());
	}
}

void export_parse()
{
	def("parse", parse);
}

