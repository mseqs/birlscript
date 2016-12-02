#include "stdafx.h"

#include "birlscript.h"
#include "Keywords.h"

#include <algorithm> 
#include <functional> 
#include <cctype>
#include <locale>

namespace birlscript
{

	namespace _string_trim
	{
		// trim from start (in place)
		void ltrim(std::string &s) {
			s.erase(s.begin(), std::find_if(s.begin(), s.end(),
				std::not1(std::ptr_fun<int, int>(std::isspace))));
		}

		// trim from end (in place)
		void rtrim(std::string &s) {
			s.erase(std::find_if(s.rbegin(), s.rend(),
				std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
		}

		// trim from both ends (in place)
		void trim(std::string &s) {
			ltrim(s);
			rtrim(s);
		}
	}

	// trim from both ends (copying)
	std::string _trim_str(std::string s) {
		_string_trim::trim(s);
		return s;
	}

	// trim from both ends (reference)
	void _trim_str_ref(std::string& s)
	{
		_string_trim::trim(s);
	}

	LineBuffer _load_file(const std::string& filename)
	{
		auto file = std::ifstream(filename.c_str());
		if(!file.is_open())
		{
			throw BirlscriptException("Erro abrindo o arquivo \"" + filename + "\".");
		}
		LineBuffer buffer;

		// Skip multiple allocations by creating the object outside of the loop
		std::string last_line;

		size_t line_counter = 1;

		while(!file.eof())
		{
			std::getline(file, last_line);
			Line line(last_line, line_counter++);
			if(line.content().empty())
			{
				// Nothing in this line
				continue;
			}
			buffer.push_back(line);
		}

		return buffer;
	}

	LineBuffer _load_source(const std::string& source)
	{
		std::string last_line;
		size_t line_counter = 1;

		LineBuffer buffer;

		for(auto c : source)
		{
			if(c == '\n')
			{
				Line line(last_line, line_counter++);
				last_line.clear();
				if(line.content().empty())
				{
					continue;
				}
				buffer.push_back(line);
			} else
			{
				last_line.push_back(c);
			}
		}

		return buffer;
	}

	namespace
	{
		/// What a line represents to the parser
		enum LineKind
		{
			LINE_FUNCTION_DECLARATION,
			LINE_FUNCTION_DECLARATION_END,
			LINE_STRUCTURE_DECLARATION,
			LINE_STRUCTURE_DECLARATION_END,
			LINE_GLOBAL_DEFINITION,
			LINE_OTHER,
		};

		LineKind GetLineKind(const Line& line)
		{
			using namespace keywords;

			auto& content = line.content();

			// Try to guess by looking at the first word
			auto space = content.find(' ');
			auto& word = space != std::string::npos ? content.substr(0, space) : content;

			auto keyword = ParseKeyword(word);

			auto kind = LINE_OTHER;

			if(keyword == Keyword::GLOBAL_CONST_DEF)
			{
				kind = LINE_GLOBAL_DEFINITION;
			}
			else if(keyword == Keyword::FUNCTION_DEF)
			{
				kind = LINE_FUNCTION_DECLARATION;
			}
			else if(keyword == Keyword::STRUCTURE_DEF)
			{
				kind = LINE_STRUCTURE_DECLARATION;
			}
			else if(keyword == Keyword::STRUCTURE_DEF_END)
			{
				kind = LINE_STRUCTURE_DECLARATION_END;
			}
			else if(keyword == Keyword::FUNCTION_DEF_END)
			{
				kind = LINE_FUNCTION_DECLARATION_END;
			}

			return kind;
		}

		/// Removes comments from the line
		void RemoveComments(std::string& line)
		{
			if (line.empty()) return;

			for(auto i = 0; i < line.length(); i++)
			{
				auto c = line[i];

				if(c == '#')
				{
					line = line.substr(0, i);
					return;
				}
			}
		}
	}

	/// Parses a buffer of lines and populate the environment
	void Birlscript::_parse_buffer(const LineBuffer& buffer)
	{
		if (buffer.empty()) return;
		// Splits in different buffers, one for functions, one for structs and one for the globals
		LineBuffer _globals;
		std::vector<LineBuffer> _functions, _structs; // Multiple lines for definitions
		// The first function is always the global environment, which receives the global name

		/// Where the parser is actually
		enum ParseScope
		{
			/// Global environment. Can find global actions or constant global declarations
			SCOPE_GLOBAL,
			/// Function environment. Only actions and a block terminator are allowed
			SCOPE_FUNCTION,
			/// Structure environmente. Only parameters and a block terminator are allowed
			SCOPE_STRUCTURE,
		};

		auto scope = SCOPE_GLOBAL;

		for(auto line : buffer)
		{
			// Remove comments and trim spaces
			RemoveComments(line._content);
			_trim_str_ref(line._content);

			auto& content = line.content();
			auto  line_number = line.number();

			if (content.empty()) continue;

			// Get which kind of line we got
			auto kind = GetLineKind(line);

			// Check for it's validity considering the scope
			if (kind == LINE_STRUCTURE_DECLARATION)
			{
				if(scope == SCOPE_GLOBAL)
				{
					// Set the scope
					scope = SCOPE_STRUCTURE;

					// Setup the buffer to receive the structure's parameters
					LineBuffer buf;
					buf.push_back(line);
					_structs.push_back(buf);
				}
				if(scope == SCOPE_FUNCTION)
				{
					throw BirlscriptException("Não permitida declaração de estrutura dentro de uma função", line_number);
				}
				if(scope == SCOPE_STRUCTURE)
				{
					throw BirlscriptException("Não permitida declaração de estrutura dentro de uma estrutura", line_number);
				}
			}
			else if(kind == LINE_STRUCTURE_DECLARATION_END)
			{
				if (scope == SCOPE_GLOBAL)
				{
					throw BirlscriptException("Final da declaração de uma estrutura fora da declaração de uma.", line_number);
				}
				if (scope == SCOPE_FUNCTION)
				{
					throw BirlscriptException("Final da declaração de uma estrutura no meio da declaração de uma função", line_number);
				}
				if (scope == SCOPE_STRUCTURE)
				{
					// End structure declaration
					scope = SCOPE_GLOBAL;
				}
			}
			else if (kind == LINE_FUNCTION_DECLARATION)
			{
				if (scope == SCOPE_GLOBAL)
				{
					scope = SCOPE_FUNCTION;
					
					LineBuffer buf;
					buf.push_back(line);
					_functions.push_back(buf);
				}
				if (scope == SCOPE_FUNCTION)
				{
					throw BirlscriptException("Não permitida declaração de uma função dentro de uma função", line_number);
				}
				if (scope == SCOPE_STRUCTURE)
				{
					throw BirlscriptException("Não permitida declaração de uma função dentro de uma estrutura", line_number);
				}
			}
			else if (kind == LINE_FUNCTION_DECLARATION_END)
			{
				if (scope == SCOPE_GLOBAL)
				{
					throw BirlscriptException("Final da declaração de uma função fora da declaração de uma.", line_number);
				}
				if (scope == SCOPE_FUNCTION)
				{
					// End function declaration
					scope = SCOPE_GLOBAL;
				}
				else if (scope == SCOPE_STRUCTURE)
				{
					throw BirlscriptException("Final da declaração de uma função no meio da declaração de uma estrutura", line_number);
				}
			}
			else if (kind == LINE_GLOBAL_DEFINITION)
			{
				if (scope == SCOPE_GLOBAL)
				{
					_globals.push_back(line);
				}
				else if (scope == SCOPE_FUNCTION)
				{
					throw BirlscriptException("Não permitida a declaração de um global dentro de uma função", line_number);
				}
				else if (scope == SCOPE_STRUCTURE)
				{
					throw BirlscriptException("Não permitida declaração de um global dentro de uma estrutura", line_number);
				}
			}
			else if (kind == LINE_OTHER)
			{
				if (scope == SCOPE_GLOBAL)
				{
					/// Add action to the global function
					_functions[0].push_back(line);
				}
				else if (scope == SCOPE_STRUCTURE)
				{
					/// Add (probably is a) parameter to the last structure declaration
					_structs[_structs.size() - 1].push_back(line);
				}
			}
		}

		// Setup threads to parse the split buffers

		auto function_parser = std::async(_parse_functions, _functions);
		auto struct_parser = std::async(_parse_structs, _structs);
		auto global_parser = std::async(_parse_globals, _globals);

		auto parsed_functions = function_parser.get();
		auto parsed_structs = struct_parser.get();
		auto parsed_globals = global_parser.get();

		_function_definitions.insert(_function_definitions.end(), parsed_functions.begin(), parsed_functions.end());
		_struct_definitions.insert(_struct_definitions.end(), parsed_structs.begin(), parsed_structs.end());
		_global_definitions.insert(_global_definitions.end(), parsed_globals.begin(), parsed_globals.end());
	}

	void Birlscript::ParseFile(const std::string& filename)
	{
		auto contents = _load_file(filename);
		_parse_buffer(contents);
	}

	void Birlscript::ParseSource(const std::string& source)
	{
		auto contents = _load_source(source);
		_parse_buffer(contents);
	}


	Birlscript Birlscript::FromOptions(const Options& opts)
	{
		Birlscript tmp;

		for(auto file : opts.files())
		{
			tmp.ParseFile(file);
		}

		for(auto source : opts.sources())
		{
			tmp.ParseSource(source);
		}

		// Parse flags



		// Return object

		return tmp;
	}

}