#include "stdafx.h"

#include "birlscript.h"

namespace birlscript
{

	namespace
	{
		struct _function_header_info
		{
			std::string ident;
			std::vector<Parameter> args;
		};

		std::vector<Parameter> _function_parse_params(const Line& l, size_t starting_point)
		{
			std::vector<Parameter> res;

			// Just one parameter?
			if (l.content().find(',', starting_point) != std::string::npos)
			{
				std::string last_param;

				for(const auto& c : l.content())
				{
					if(c == ',')
					{
						const Line line(last_param, l.number());
						res.push_back(Parameter::parse(line));
						last_param.clear();
					} else
					{
						last_param += c;
					}
				}
			} else
			{
				const Line line(l.content().substr(starting_point), l.number());
				res.push_back(Parameter::parse(line));
			}

			return res;
		}

		_function_header_info _function_parse_header(const Line& l)
		{
			// A function header looks like this:
			// JAULA identifier
			// or
			// JAULA identifier (param_list...)

			// Where the decl really starts
			auto starting_point = strlen("JAULA") + 1;
			// If the decl has a argument list
			auto args_start = l.content().find('(');
			auto has_args = args_start != std::string::npos;

			auto ident = l.content().substr(starting_point, args_start);

			_trim_str_ref(ident);
			
			if (has_args)
			{
				return { ident, _function_parse_params(l, args_start) };
			}
			return { ident, std::vector<Parameter>() };
		}

		Function _int_parse(const LineBuffer& b)
		{
			auto header_info = _function_parse_header(b[0]);

			std::vector<Line> acts(b.begin() + 1, b.end());

			auto actions = Action::parse(acts);

			return Function(header_info.ident, actions, header_info.args);
		}
	}

	std::vector<Function> _parse_functions(const std::vector<LineBuffer>& src)
	{

		std::vector<Function> res;

		for(const auto& lines : src)
		{
			res.push_back(_int_parse(lines));
		}
		
		return res;
	}

}