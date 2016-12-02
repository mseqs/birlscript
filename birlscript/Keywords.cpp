#include "stdafx.h"

#include "Keywords.h"

namespace birlscript
{
	namespace keywords
	{

		/// A function that takes a string and compares to the specific related keywords, returning true if matches
		using KeywordQuery = bool(*)(const std::string&);

		namespace
		{
			bool match(const std::string& src, const char* slice)
			{
				return src.compare(slice) == 0;
			}
		}

		bool _global_const_def(const std::string& s)
		{
			return match(s, "IBIRAPUERA");
		}

		bool _global_var_def(const std::string& s)
		{
			return match(s, "SAI");
		}

		bool _function_def(const std::string& s)
		{
			return match(s, "JAULA");
		}

		bool _function_def_end(const std::string& s)
		{
			return match(s, "SAINDO");
		}

		bool _structure_def(const std::string& s)
		{
			return match(s, "MUTANTE");
		}

		bool _structure_def_end(const std::string& s)
		{
			return match(s, "FORA");
		}

		// Actions

		bool _block_end(const std::string& s)
		{
			return match(s, "SAI FORA");
		}

		bool _action_print(const std::string& s)
		{
			return match(s, "CE QUER VER");
		}

		bool _action_println(const std::string& s)
		{
			return match(s, "CE QUER VER ISSO");
		}

		bool _action_assign(const std::string& s)
		{
			return match(s, "BORA");
		}

		bool _action_declare(const std::string& s)
		{
			return match(s, "VEM");
		}

		bool _action_declare_wv(const std::string& s)
		{
			return match(s, "VEM, CUMPADE");
		}

		bool _action_call(const std::string& s)
		{
			return match(s, "E HORA DO") || match(s, "É HORA DO");
		}

		bool _action_compare(const std::string& s)
		{
			return match(s, "E ELE QUE A GENTE QUER") || match(s, "É ELE QUE A GENTE QUER");
		}

		bool _action_compare_eq(const std::string& s)
		{
			return match(s, "E ELE MEMO") || match(s, "É ELE MEMO");
		}

		bool _action_compare_neq(const std::string& s)
		{
			return match(s, "NUM E ELE") || match(s, "NUM É ELE");
		}

		bool _action_compare_less(const std::string& s)
		{
			return match(s, "MENOR, CUMPADE");
		}

		bool _action_compare_lesseq(const std::string& s)
		{
			return match(s, "MENOR OU E MEMO") || match(s, "MENOR OU É MEMO");
		}

		bool _action_compare_more(const std::string& s)
		{
			return match(s, "MAIOR, CUMPADE");
		}
		
		bool _action_compare_moreeq(const std::string& s)
		{
			return match(s, "MAIOR OU E MEMO") || match(s, "MAIOR OU É MEMO");
		}

		bool _action_quit(const std::string& s)
		{
			return match(s, "NUM VAI DA NAO") || match(s, "NUM VAI DA NÃO");
		}

		bool _action_return(const std::string& s)
		{
			return match(s, "BIRL");
		}

		bool _action_input(const std::string& s)
		{
			return match(s, "BORA, CUMPADE");
		}

		bool _action_input_upper(const std::string& s)
		{
			return match(s, "BORA, CUMPADE!!!");
		}

		std::tuple<KeywordQuery, Keyword> _queries[] =
		{
			{_global_const_def, Keyword::GLOBAL_CONST_DEF},
			{_global_var_def, Keyword::GLOBAL_VAR_DEF},
			{_function_def, Keyword::FUNCTION_DEF},
			{_function_def_end, Keyword::FUNCTION_DEF_END},
			{_structure_def, Keyword::STRUCTURE_DEF},
			{_structure_def_end, Keyword::STRUCTURE_DEF_END},

			// Actions

			{ _block_end, Keyword::BLOCK_END },
			{ _action_print, Keyword::ACTION_PRINT},
			{ _action_println, Keyword::ACTION_PRINTLN},
			{ _action_assign, Keyword::ACTION_ASSIGN},
			{ _action_declare, Keyword::ACTION_DECLARE},
			{ _action_declare_wv, Keyword::ACTION_DECLARE_WV},
			{ _action_call, Keyword::ACTION_CALL},
			{ _action_compare, Keyword::ACTION_COMPARE},
			{ _action_compare_eq, Keyword::ACTION_COMPARE_EQ},
			{ _action_compare_neq, Keyword::ACTION_COMPARE_NEQ},
			{ _action_compare_less, Keyword::ACTION_COMPARE_LESS},
			{ _action_compare_lesseq, Keyword::ACTION_COMPARE_LESSEQ},
			{ _action_compare_more, Keyword::ACTION_COMPARE_MORE},
			{ _action_compare_moreeq, Keyword::ACTION_COMPARE_MOREEQ},
			{ _action_quit, Keyword::ACTION_QUIT},
			{ _action_return, Keyword::ACTION_RETURN},
			{ _action_input, Keyword::ACTION_INPUT},
			{ _action_input_upper, Keyword::ACTION_INPUT_UPPER},
		};

		constexpr size_t _num_queries()
		{
			return sizeof _queries / sizeof * _queries;
		}

		Keyword ParseKeyword(const std::string& s)
		{
			for(auto i = 0; i < _num_queries(); i++)
			{
				auto& query = _queries[i];
				if(std::get<0>(query)(s))
				{
					return std::get<1>(query);
				}
			}

			return Keyword::NONE;
		}
	}
}