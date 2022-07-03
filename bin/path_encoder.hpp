//@	{"dependencies_extra":[{"ref":"./path_encoder.o", "rel":"implementation"}]}

#ifndef CHEAPESTROUT_PATHENCODER_HPP
#define CHEAPESTROUT_PATHENCODER_HPP

#include "./length_unit.hpp"
#include "./scaling_factors.hpp"

#include "lib/search.hpp"

#include <cstdio>

namespace cheapest_route
{
	class path_encoder
	{
	public:
		explicit path_encoder(std::string_view str);

		void operator()(FILE* f, length_unit lu, scaling_factors factors, search_domain domain, path const& nodes) const
		{ encode(f, lu, factors, domain, nodes); }

	private:
		using func = void (*)(FILE* f, length_unit lu, scaling_factors factors, search_domain domain, path const& nodes);
		func encode;
	};
}

#endif
