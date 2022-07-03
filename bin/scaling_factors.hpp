#ifndef CHEAPESTROUTE_SCALINGFACTORS_HPP
#define CHEAPESTROUTE_SCALINGFACTORS_HPP

#include "lib/vec.hpp"

namespace cheapest_route
{
	class scaling_factors
	{
	public:
		constexpr explicit scaling_factors(float x, float y, float z):m_values{x, y, z, 0.0f}{}

		explicit scaling_factors(std::string_view str):m_values{}
		{
			static_value_array<float, 3> values{str};

			for(size_t k = 0; k != 3; ++k)
			{ m_values[k] = values[k]; }
		}

		constexpr auto values() const { return m_values; }

		constexpr auto x() const { return m_values[0]; }

		constexpr auto y() const { return m_values[1]; }

		constexpr auto z() const { return m_values[2]; }

	private:
		vec4f_t m_values;
	};
}

#endif