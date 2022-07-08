//@	{"dependencies_extra":[{"ref":"./length_unit.o", "rel":"implementation"}]}

#ifndef CHEAPESTROUT_LENGTHUNIT_HPP
#define CHEAPESTROUT_LENGTHUNIT_HPP

#include "lib/search.hpp"

namespace cheapest_route
{
	class length_unit
	{
	public:
		explicit length_unit(std::string_view str);

		double factor() const
		{ return m_factor; }

		char const* name() const { return m_name.c_str(); }

	private:
		double m_factor;
		std::string m_name;
	};
}

#endif
