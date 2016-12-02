#include "stdafx.h"

#include "birlscript.h"

namespace birlscript
{
	BirlscriptException::BirlscriptException(std::string w) : runtime_error(w)
	{
		_message = w;
	}

	BirlscriptException::BirlscriptException(std::string msg, size_t line_number) : runtime_error("")
	{
		std::stringstream ss;
		ss << "Erro na linha " << line_number << ": " << msg;
		_message = ss.str();
	}

	const char* BirlscriptException::what() const
	{
		return _message.c_str();
	}

}