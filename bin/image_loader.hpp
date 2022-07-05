//@	{"dependencies_extra":[{"ref":"./image_loader.o", "rel":"implementation"}]}

#ifndef CHEAPESTROUTE_IMAGELOADER_HPP
#define CHEAPESTROUTE_IMAGELOADER_HPP

#include "lib/vec.hpp"

#include "pixel_store/image.hpp"
#include <filesystem>

namespace cheapest_route
{
	struct cost_values
	{
	public:
		constexpr cost_values():m_values{1.0f, 1.0f, 0.0f, 0.0f}
		{}

		constexpr float elevation() const
		{ return m_values[0]; }

		constexpr float friction() const
		{ return m_values[1]; }

		constexpr auto wind() const
		{
			return vec<double, 2, quantity_type::vector>{m_values[2], m_values[3]};
		}

		constexpr cost_values& operator*=(float scalar)
		{
			m_values *= scalar;
			return *this;
		}

		constexpr cost_values& operator+=(cost_values a)
		{
			m_values += a.m_values;
			return *this;
		}

	private:
		vec4f_t m_values;
	};

	constexpr inline cost_values operator*(float scalar, cost_values a)
	{
		a *= scalar;
		return a;
	}

	constexpr inline cost_values operator+(cost_values a, cost_values b)
	{
		a += b;
		return a;
	}

	using image_type = pixel_store::image<cost_values>;

	image_type load_image(std::filesystem::path const& filename);
}
#endif