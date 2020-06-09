#include "lexer_file_iterator.hpp"
#include "lexer_file_keywords.hpp"
#include <stdexcept>

void checkString(const std::string& e, const std::string& g){
	if(e != g){
		throw std::logic_error(
			"lexer_file_iterator: Expected '"+e+"' got '"+g+"'"
		);
	}
}

namespace cif{

lexer_file_iterator::lexer_file_iterator(void) : isPtr(NULL), i(0) {}

lexer_file_iterator::lexer_file_iterator(std::istream& is) : isPtr(&is), i(0) {
	std::string line;
	std::getline(*isPtr, line); 
	checkString(cif::LEXER_TAG_STR, line);
	increment();
}

void lexer_file_iterator::increment(void){ 
	std::string line;

	//tag
	std::getline(*isPtr, entry.name);

	//type
	std::getline(*isPtr, line); 
	checkString(cif::LEXER_TYPE_STR, line);
	std::getline(*isPtr, entry.type);

	//value
	std::getline(*isPtr, line); 
	checkString(cif::LEXER_VALUE_STR, line);
	std::getline(*isPtr, entry.value);

	while(!isPtr->eof()){
		std::getline(*isPtr, line);
		if(line == cif::LEXER_TAG_STR){
			break;
		}
		entry.value += "\n";
		entry.value += line;
	}

	if(isPtr->eof()){
		isPtr = NULL;
	}
	++i;
}

bool lexer_file_iterator::equal(const lexer_file_iterator& other) const{
	return 
		isPtr && !other.isPtr ?  
			isPtr->eof() : 
			!isPtr && other.isPtr ?
				other.isPtr->eof() :
				isPtr && other.isPtr ?
					isPtr == other.isPtr && i == other.i :
					true;
}

const detail::lex_entry& lexer_file_iterator::dereference() const { 
	return entry; 
}

}

