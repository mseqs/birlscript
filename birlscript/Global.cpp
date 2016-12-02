#include "stdafx.h"

#include "birlscript.h"

namespace birlscript
{

	namespace
	{
		bool __starts_with(const std::string& src, const std::string& slice)
		{
			return src.compare(0, slice.size(), slice) == 0;
		}

		bool _global_is_const(const Line& s)
		{
			// If the global is const, it starts with IBIRAPUERA
			// if the global is variable, it starts with SAI DE CASA

			return __starts_with(s.content(), "IBIRAPUERA");
		}

		struct _global_meta_info
		{
			std::string identifier, value;
		};

		_global_meta_info _global_get_info(const Line& l, bool is_const)
		{
			std::string ident, val;

			size_t starting_point;

			if(is_const)
			{
				// The starting point in the string is where the keyword ends
				starting_point = strlen("IBIRAPUERA") + 1;
			} else
			{
				starting_point = strlen("SAI DE CASA") + 1;
			}

			/// Find the first space after the declaration keyword
			auto ident_separator = l.content().find(' ', starting_point);

			// What's between the keyword and the value is the identifier
			ident = l.content().substr(starting_point, ident_separator);

			// Everything after the identifier is the value
			val = l.content().substr(ident_separator + 1);

			_trim_str_ref(ident);
			_trim_str_ref(val);

			return { ident, val };
		}

		/// Parse a single line contaning a global declaration
		Global _int_parse(const Line& s)
		{
			auto is_const = _global_is_const(s);
			auto meta_inf = _global_get_info(s, is_const);
			auto identifier = meta_inf.identifier, value = meta_inf.value;

			return Global(identifier, value, is_const);
		}
	}

	std::vector<Global> _parse_globals(const LineBuffer& src)
	{

		std::vector<Global> res;

		for(const auto& line : src)
		{
			res.push_back(_int_parse(line));
		}

		return res;
	}

}