// Copyright (c) 2016 Rafael Rodrigues Nakano. All rights reserved.
// See LICENSE for licensing details

#pragma once

#include "Options.h"

namespace birlscript
{

	std::string _trim_str(std::string s);
	void _trim_str_ref(std::string& s);

	class BirlscriptException : public std::runtime_error
	{
		std::string _message;

	public:
		explicit BirlscriptException(std::string w = "");
		explicit BirlscriptException(std::string msg, size_t line_number);

		const char * what() const override;
	};

	/// Represents a line, which contains a content and a line number
	struct Line
	{
		friend class Birlscript;
	private:
		std::string _content;
		size_t _line_number;

	public:
		explicit Line(std::string content = "", size_t line_number = 0) : _content(content), _line_number(line_number) {}

		const std::string& content() const { return _content; }
		size_t number() const { return _line_number; }
	};

	/// A buffer which holds lines
	using LineBuffer = std::vector<Line>;

	/// Global definition
	struct Global
	{
	private:
		std::string _identifier;
		std::string _value;
		bool _is_const = false;

	public:
		Global(std::string id, std::string val, bool is_const = false) : _identifier(id), _value(val), _is_const(is_const) {}

		const std::string& identifier() const { return _identifier; }
		const std::string& value() const { return _value; }
		bool is_const() const { return _is_const; }
	};

	/// An action, such as variable declaration, function call...
	struct Action
	{
		enum ActionKind
		{
			ACT_INVALID,
			ACT_BLOCK_END,

			ACT_PRINT,
			ACT_PRINTLN,
			ACT_ASSIGN,
			ACT_DECLARE,
			ACT_DECLARE_WV,
			ACT_CALL,
			ACT_COMP,
			ACT_COMP_EQ,
			ACT_COMP_NEQ,
			ACT_COMP_LESS,
			ACT_COMP_LESSEQ,
			ACT_COMP_MORE,
			ACT_COMP_MOREEQ,
			ACT_QUIT,
			ACT_RETURN,
			ACT_INPUT,
			ACT_INPUT_UPPER,
		};

	private:
		ActionKind _kind = ACT_INVALID;
		/// Some actions carry arguments
		std::vector<std::string> _arguments;
		/// Some actions carry other actions
		std::vector<Action*> _inner_actions;

	public:

		~Action();

		Action() {}

		Action(ActionKind kind, std::vector<std::string> arguments, std::vector<Action*> inner = std::vector<Action*>())
			: _kind(kind), _arguments(arguments), _inner_actions(inner) {}

		static std::vector<Action> parse(const LineBuffer& source);
	};

	/// Named parameter which contains a type, useful for function and structure declarations
	struct Parameter
	{
	private:
		std::string _identifier, _type;

	public:
		Parameter(std::string id, std::string tp) : _identifier(id), _type(tp) {}

		const std::string& identifier() const { return _identifier; }
		const std::string& type() const { return _type; }

		static Parameter parse(const Line& l);
	};

	/// Function declaration object
	struct Function
	{
	private:
		std::string _identifier;
		std::vector<Action> _actions;
		std::vector<Parameter> _args;

	public:
		Function(std::string identifier, std::vector<Action> actions, std::vector<Parameter> args = std::vector<Parameter>()) : _identifier(identifier), _actions(actions), _args(args) {}

		const std::string& identifier() const { return _identifier; }
		const std::vector<Action>& actions() const { return _actions; }
		const std::vector<Parameter>& arguments() const { return _args; }
	};

	/// Structure object (user-defined)
	struct Structure
	{
	private:
		std::string _identifier;
		std::vector<Parameter> _members;

	public:
		Structure(std::string id, std::vector<Parameter> inner) : _identifier(id), _members(inner) {}

		const std::string& identifier() const { return _identifier; }
		const std::vector<Parameter>& members() const { return _members; }
	};

	class Birlscript
	{
		/// Functions defined in the source files. The first is always the global environment (combined if passed more than one file/source)
		std::vector<Function> _function_definitions;
		/// Structures defined
		std::vector<Structure> _struct_definitions;
		/// Globals defined in the sources passed
		std::vector<Global> _global_definitions;

		void _parse_buffer(const LineBuffer&);

	public:

		/// Parse an file
		void ParseFile(const std::string& filename);
		/// Parses code inputted through a string
		void ParseSource(const std::string& source);

		/// Parses files, sources and set flags defined on the Options class
		static Birlscript FromOptions(const Options& opts);

	};

	/// Parse a buffer of functions
	std::vector<Function> _parse_functions(const std::vector<LineBuffer>&);

	/// Parse a buffer of structures
	std::vector<Structure> _parse_structs(const std::vector<LineBuffer>&);

	/// Parse a buffer of globals
	std::vector<Global> _parse_globals(const LineBuffer&);

}
