#ifndef LEXER_FILE_ITERATOR_HPP
#define LEXOR_FILE_ITERATOR_HPP

#include <string>
#include <istream>
#include <boost/iterator/iterator_facade.hpp>

namespace cif{

namespace detail{

struct lex_entry{ std::string name, type, value; };

}

class lexer_file_iterator :
	public boost::iterator_facade<
		lexer_file_iterator,
		const detail::lex_entry,
		boost::forward_traversal_tag
	>
{
	public:
		lexer_file_iterator(void);
		explicit lexer_file_iterator(std::istream& is);

	private:
		friend class boost::iterator_core_access;

		void increment(void);

		bool equal(const lexer_file_iterator& other) const;

		const detail::lex_entry& dereference() const;

		std::istream* isPtr;
		detail::lex_entry entry;
		size_t i;
};

}

#endif // LEXER_FILE_ITERATOR_HPP

