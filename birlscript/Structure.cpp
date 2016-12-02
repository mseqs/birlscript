#include "stdafx.h"

#include "birlscript.h"

namespace birlscript
{

	namespace
	{
		std::string _structure_extract_identifier(const Line& s)
		{
			// Get where the identifier should start
			auto starting_point = strlen("MUTANTE") + 1;

			auto id = s.content().substr(starting_point);

			_trim_str_ref(id);

			return id;
		}

		std::vector<Parameter> _structure_extract_inner(const LineBuffer& s)
		{
			std::vector<Parameter> res;
			// The first line of the buffer is the declaration
			for(auto i = 1; i < s.size(); i++)
			{
				res.push_back(Parameter::parse(s[i]));
			}
			return res;
		}

		Structure _int_parse(const LineBuffer& s)
		{
			// Structure declaration is a keyword followed by a identifier
			// MUTANTE identifier
			// parameters...
			// BIRL
			auto identifier = _structure_extract_identifier(s[0]);
			auto params = _structure_extract_inner(s);

			return Structure(identifier, params);
		}
	}
	
	std::vector<Structure> _parse_structs(const std::vector<LineBuffer>& s)
	{
		std::vector<Structure> res;

		for(const auto& lines : s)
		{
			res.push_back(_int_parse(lines));
		}
		
		return res;
	}

}