// Copyright (c) 2016 Rafael Rodrigues Nakano. All rights reserved.
// See LICENSE for licensing details

#pragma once
#include "Display.h"

const static std::string BIRLSCRIPT_VERSION = "2.0-alpha";

/// Holds information passed through the command line and handle them
class Options {
private:
	/// Files and raw string sources can be passed to be executed
	std::vector<std::string> _files, _sources;

	/// Displayer
	Display::Displayer display;

	void display_version() const;

	void display_help() const;

	void parse_arg(std::string arg);

public:

	Options(int argc, char** argv);

	const std::vector<std::string>& files() const { return _files; }
	const std::vector<std::string>& sources() const { return _sources; }

	/// If no files or sources were passed
	bool empty() const;

	void parse(int argc, char **argv);
};