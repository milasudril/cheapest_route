//@	{"target":{"name":"./length_unit.o"}}

#include "./length_unit.hpp"

#include <stdexcept>

cheapest_route::length_unit::length_unit(std::string_view str)
{
	if(str == "m")
	{ m_factor = 1.0; }
	else
	if(str == "dm")
	{ m_factor = 0.1; }
	else
	if(str == "cm")
	{ m_factor = 0.01; }
	else
	if(str == "mm")
	{ m_factor = 0.001; }
	else
	if(str == "in")
	{ m_factor = 0.0254; }
	else
	if(str == "svg")
	{ m_factor = 0.0254/90.0; }
	else
	{
		throw std::runtime_error{"Unsupported length unit"};
	}
}
