#ifndef CHEAPESTROUT_PATHENCODER_HPP
#define CHEAPESTROUT_PATHENCODER_HPP

#include "./length_unit.hpp"

#include "lib/search.hpp"

#include <cstdio>

namespace cheapest_route
{
	class path_encoder
	{
	public:
		explicit path_encoder(std::string_view str);

		void operator()(FILE* f, path const& nodes, length_unit lu, scaling_factors factors) const
		{ encode(f, nodes, lu, factors); }

	private:
		using func = void (*)(FILE*, path const&, length_unit, scaling_factors);
		func encode;
	};
}

#endif
