#include "stdafx.h"

#include "Options.h"

Options::Options(int argc, char** argv)
{
	this->parse(argc, argv);
}

void Options::parse_arg(std::string arg)
{
	if (arg.empty()) {
		return;
	}
	std::string key, value;
	auto equal_sign = arg.find('=');
	if (equal_sign != std::string::npos) {
		key = arg.substr(0, equal_sign);
		value = arg.substr(equal_sign + 1);
	}
	else {
		key = std::move(arg);
	}

	// Filter the key
	if (key.compare("-a") == 0 || key.compare("--ajuda-o-maluco") == 0) display_help();
	else if (key.compare("-v") == 0 || key.compare("--versao") == 0) display_version();
	else if (key.compare("-s") == 0 || key.compare("--source") == 0) {
		if (!value.empty())
			_sources.push_back(value);
	}
	else {
		// Did not match anything, check if it doesn't start with a dash
		if (key[0] == '-') {
			// Is a option
			display("Opção não compreendida: " + key);
		}
		else {
			// Is a file
			_files.push_back(key);
		}
	}
}

void Options::display_help() const
{
	display_version();
	std::string help_txt = "Tá querendo ajuda, cumpade?\nSeguinte:\n";
	help_txt += "birlscript [Opções] [Arquivos]\n";
	help_txt += "[Opções]\n"
		"\t-a ou --ajuda-o-maluco:\t\tImprime essa mensagem de ajuda\n"
		"\t-v ou --versao:\t\t\tImprime somente a versão do programa\n"
		"\t-s=CODIGO ou --source=CODIGO:\tExecuta o código passado\n";
	help_txt += "[Arquivos]\n\tArquivos passados pra serem executados ou compilados\n";
	display(help_txt);
}

void Options::display_version() const
{
	auto text = "BIRLSCRIPT versão " + BIRLSCRIPT_VERSION + "\nCopyright (r) 2016 Rafael Rodrigues Nakano.\n";
	display(text);
}

void Options::parse(int argc, char** argv)
{
	if (argc > 1) {
		for (auto i = 1; i < argc; i++) {
			std::string current(argv[i]);
			parse_arg(current);
		}
	}
}

bool Options::empty() const
{
	return this->_files.empty() && this->_sources.empty();
}

