#include"stdafx.h"

#include "Display.h"
#include "birlscript.h"
#include "Options.h"

/// Entry point to the actual program
int32_t start_program(const Options& opts) {
	using namespace birlscript;
	using namespace Display;

	Displayer display;

	if(opts.empty())
	{
		display("Erro fatal: Nenhum arquivo ou fonte passado pra execução/compilação.");
		return -1;
	}

	try {
		auto virtual_machine = Birlscript::FromOptions(opts);
	} catch(const BirlscriptException& e)
	{
		display("Exceção Birlscript: \"");
		display(e.what());
		display("\"");
		return -1;
	}

    return 0;
}

int main(int argc, char **argv) {
    // Create a displayer
	Display::Displayer display;
    // Parse the command line parameters and
    Options options(argc, argv);
	auto return_value = start_program(options);
    getchar();
    return return_value;
}