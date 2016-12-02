// Copyright (c) 2016 Rafael Rodrigues Nakano. All rights reserved.
// See LICENSE for licensing details

#include "stdafx.h"

#include "Display.h"

namespace Display {
	Displayer::Displayer() {
#if defined(_WIN32)
		_setmode(_fileno(stdout), _O_U16TEXT);
#endif
	}

	void Displayer::operator()(std::string content) const {
#if defined(_WIN32)
		std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
		auto wstr = converter.from_bytes(content.c_str());
		std::wcout << wstr;
#else
		std::cout << content;
#endif
	}
}