#include "stdafx.h"

#include "birlscript.h"
#include "Keywords.h"

namespace birlscript
{

	Parameter Parameter::parse(const Line& l)
	{
		auto& s = l.content();
		auto line_number = l.number();
		std::string id, tp;

		// A parameter declaration is as follows:
		// identifier : type

		auto middle = s.find(':');

		if(middle == std::string::npos)
		{
			throw BirlscriptException("Erro na declaração, esperado um parametro (id : tipo)", line_number);
		}

		id = s.substr(0, middle);
		tp = s.substr(middle + 1);

		_trim_str_ref(id);
		_trim_str_ref(tp);

		return Parameter(id, tp);
	}

	namespace
	{

		Action::ActionKind _act_get_kind(const Line& s)
		{
			/// Get keyword
			auto resulted_kw = keywords::ParseKeyword(s.content());

			Action::ActionKind kind;

			using keywords::Keyword;

			switch(resulted_kw)
			{
			case Keyword::ACTION_PRINT:
				kind = Action::ACT_PRINT;
				break;
			case Keyword::ACTION_PRINTLN:
				kind = Action::ACT_PRINTLN;
				break;
			case Keyword::ACTION_ASSIGN:
				kind = Action::ACT_ASSIGN;
				break;
			case Keyword::ACTION_DECLARE:
				kind = Action::ACT_DECLARE;
				break;
			case Keyword::ACTION_DECLARE_WV:
				kind = Action::ACT_DECLARE_WV;
				break;
			case Keyword::ACTION_CALL:
				kind = Action::ACT_CALL;
				break;
			case Keyword::ACTION_COMPARE:
				kind = Action::ACT_COMP;
				break;
			case Keyword::ACTION_COMPARE_EQ:
				kind = Action::ACT_COMP_EQ;
				break;
			case Keyword::ACTION_COMPARE_NEQ:
				kind = Action::ACT_COMP_NEQ;
				break;
			case Keyword::ACTION_COMPARE_LESS:
				kind = Action::ACT_COMP_LESS;
				break;
			case Keyword::ACTION_COMPARE_LESSEQ:
				kind = Action::ACT_COMP_LESSEQ;
				break;
			case Keyword::ACTION_COMPARE_MORE:
				kind = Action::ACT_COMP_MORE;
				break;
			case Keyword::ACTION_COMPARE_MOREEQ:
				kind = Action::ACT_COMP_MOREEQ;
				break;
			case Keyword::ACTION_QUIT:
				kind = Action::ACT_QUIT;
				break;
			case Keyword::ACTION_RETURN:
				kind = Action::ACT_RETURN;
				break;
			case Keyword::ACTION_INPUT:
				kind = Action::ACT_INPUT;
				break;
			case Keyword::ACTION_INPUT_UPPER:
				kind = Action::ACT_INPUT_UPPER;
				break;
				
			default:
				/// Command invalid or not found
				throw BirlscriptException("O comando \"" + s.content() + "\" não foi encontrado.", s.number());
			}

			return kind;
		}

		/// Return the actions which are multiline
		bool _act_multiline(const Action::ActionKind& kind)
		{
			bool res;
			switch(kind)
			{
			case Action::ACT_COMP_EQ:
			case Action::ACT_COMP_LESS:
			case Action::ACT_COMP_LESSEQ:
			case Action::ACT_COMP_MORE:
			case Action::ACT_COMP_MOREEQ:
			case Action::ACT_COMP_NEQ:
				res = true;
				break;
			default:
				res = false;
				break;
			}
			return res;
		}

		std::vector<std::string> _act_parse_arguments(const Line& s)
		{
			std::vector<std::string> result;
			auto in_str = false, in_sym = false, in_par = false, last_escape = false;
			// Number of open parenthesis
			size_t open_par = 0;
			std::string last_arg;

			for(auto& c : s.content())
			{
				if(c == '\"')
				{
					if(in_str)
					{
						if(last_escape)
						{
							last_escape = false;
							last_arg += "\\\"";
						} else
						{
							in_str = false;
							last_arg += '\"';
						}
					} else
					{
						in_str = true;
						last_arg += '\"';
					}
				}
				else if(c == '\\')
				{
					if(in_str)
					{
						if(last_escape)
						{
							last_escape = false;
							last_arg += "\\";
						} else
						{
							last_escape = true;
						}
					} else
					{
						throw BirlscriptException("Erro: Caractere \"\\\" fora de uma string.", s.number());
					}
				}
				// Since is hard to know if a character is alpha considering UTF-8, put this condition in the else
				else if(c == '(')
				{
					if(in_sym)
					{
						open_par += 1;
						in_par = true; // No time for checks
						last_arg += c;
					}
				}
				else if(c == ')')
				{
					if(in_par)
					{
						open_par -= 1;
						if (open_par <= 0) in_par = false;
						last_arg += c;
					}
				}
				else if(c == ',')
				{
					if(!in_str && !in_par)
					{
						_trim_str_ref(last_arg);
						result.push_back(last_arg);
						last_arg.clear();
						in_sym = false; // If in a symbol, isn't anymore
					}
				}
				// If a space is found and the parser isn't inside a string, skip it
				else if(c == ' ' && !in_str) {}
				else
				{
					if(!in_str)
					{
						// Can be a symbol
						in_sym = true;
					}
					last_arg += c;
				}
			}

			if(!last_arg.empty())
			{
				result.push_back(std::move(last_arg));
			}

			return result;
		}

		Action _act_parse_oneliner(const Line& s)
		{
			// Actions follows the following rules
			// His structure is "KEYWORD(s) : ARGUMENTS"
			// Some use more than one line

			auto divider = s.content().find(':');
			std::string identifier;
			std::string arguments;
			std::vector<std::string> parsed_arguments;
			if (divider == std::string::npos)
			{
				identifier = s.content();
			}
			else
			{
				identifier = s.content().substr(0, divider);
				arguments = s.content().substr(divider);

				_trim_str_ref(identifier);
				_trim_str_ref(arguments);
			}
			auto kind = _act_get_kind(Line(identifier, s.number()));
			if (!arguments.empty())
			{
				// Set arguments to the last parsed action
				parsed_arguments = _act_parse_arguments(Line(arguments, s.number()));
			}

			return Action(kind, parsed_arguments);
		}

	}

	Action::~Action()
	{
		if(!_inner_actions.empty())
		{
			// Free the pointers
			for(auto* ptr : _inner_actions)
			{
				delete ptr;
			}
		}
	}

	std::vector<Action> Action::parse(const LineBuffer& s)
	{
		// An action is divided as follows:
		// KEYWORD: parameters...

		// Some actions use more than one line, like conditionals and loops

		std::vector<Action> res;

		auto is_in_action = false; // If is parsing a action with multiple lines

		for(const auto& line : s)
		{
			auto current = _act_parse_oneliner(line);

			if(!is_in_action)
			{
				if (_act_multiline(current._kind))
				{
					// Action consumes more than one line
					is_in_action = true;
					continue;
				}
				// Isn't multiline, push the parsed action to the vector
				res.push_back(current);
			} else
			{
				if(current._kind == ACT_BLOCK_END)
				{
					is_in_action = false;
					continue;
				}
				if (res.empty()) continue; // ??

				// If the current action isn't a block break, push the action to the current one's inner actions
				res.back()._inner_actions.push_back(new Action(current));
			}
		}

		return res;
	}

}
