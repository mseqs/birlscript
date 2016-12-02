// Copyright (c) 2016 Rafael Rodrigues Nakano. All rights reserved.
// See LICENSE for licensing details

#pragma once

namespace Display {
	/// Display stuff to the standard out
	class Displayer {
	public:
		Displayer();

		/// Displays stuff
		void operator() (std::string) const;
	};
}
