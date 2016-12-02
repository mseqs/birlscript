// Copyright (c) 2016 Rafael Rodrigues Nakano. All rights reserved.
// See LICENSE for licensing details

#pragma once

// Defines an interface to the parser discover if a string is a keyword and which it is

namespace birlscript
{
	namespace keywords
	{

		const char * const GLOBAL_ENVIRONMENT_IDENTIFIER = "__birl_GLOBAL";

		enum class Keyword
		{
			/// Invalid keyword
			NONE = -1,
			/// Definition of a global constant
			GLOBAL_CONST_DEF = 0,
			/// Definition of a variable global
			GLOBAL_VAR_DEF,
			/// Function declaration
			FUNCTION_DEF,
			/// End of function declaration
			FUNCTION_DEF_END,
			/// Structure declaration
			STRUCTURE_DEF,
			/// End of structure declaration
			STRUCTURE_DEF_END,
			/// End of block
			BLOCK_END,

			// Actions

			ACTION_PRINT,
			ACTION_PRINTLN,
			ACTION_ASSIGN,
			ACTION_DECLARE,
			ACTION_DECLARE_WV,
			ACTION_CALL,
			ACTION_COMPARE,
			ACTION_COMPARE_EQ,
			ACTION_COMPARE_NEQ,
			ACTION_COMPARE_LESS,
			ACTION_COMPARE_LESSEQ,
			ACTION_COMPARE_MORE,
			ACTION_COMPARE_MOREEQ,
			ACTION_QUIT,
			ACTION_RETURN,
			ACTION_INPUT,
			ACTION_INPUT_UPPER,

		};

		Keyword ParseKeyword(const std::string &src);

	}
}