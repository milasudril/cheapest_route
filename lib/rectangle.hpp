#ifndef CHEAPESTROUTE_RECTANGLE_HPP
#define CHEAPESTROUTE_RECTANGLE_HPP

#include "./interval.hpp"
#include "./vec.hpp"

namespace cheapest_route
{
	template<class T, boundary_type left, boundary_type right, boundary_type top, boundary_type bottom>
	struct dimensions_2d
	{
		constexpr dimensions_2d(vec<T, 2> dim):m_dim{dim}{}

		constexpr dimensions_2d() = default;

		constexpr dimensions_2d(T width, T height):m_dim{width, height}{}

		constexpr auto width() const
		{ return m_dim[0];}

		constexpr auto height() const
		{ return m_dim[1]; }

		constexpr auto min() const
		{ return vec<T, 2>{static_cast<T>(0), static_cast<T>(0)}; }

		constexpr auto max() const
		{ return m_dim; }

		vec<T, 2> m_dim;
	};

	template<class T, boundary_type left, boundary_type right, boundary_type top, boundary_type bottom>
	constexpr auto operator*(T scale, dimensions_2d<T, left, right, top, bottom> dim)
	{
		dim.m_dim *= scale;
		return dim;
	}

	template<class T, boundary_type left, boundary_type right, boundary_type top, boundary_type bottom>
	constexpr auto operator-(dimensions_2d<T, left, right, top, bottom> dim, vec<T, 2> adjustment)
	{
		dim.m_dim -= adjustment;
		return dim;
	}

	struct origin_at_zero{};

	template<class T, boundary_type left, boundary_type right, boundary_type top, boundary_type bottom>
	struct rectangle
	{
		constexpr rectangle() = default;

		constexpr explicit rectangle(interval<T, left, right> i1, interval<T, top, bottom> i2):
			horz_interval{i1},
			vert_interval{i2}
		{}

		constexpr explicit rectangle(dimensions_2d<T, left, right, top, bottom> dim, origin_at_zero):
			horz_interval{static_cast<T>(0), dim.width()},
			vert_interval{static_cast<T>(0), dim.height()}
		{}

		constexpr auto width() const
		{ return horz_interval.max - horz_interval.min;}

		constexpr auto height() const
		{ return vert_interval.max - vert_interval.min; }

		constexpr auto min() const
		{ return vec<T, 2>{horz_interval.min, vert_interval.min}; }

		constexpr auto max() const
		{ return vec<T, 2>{horz_interval.max, vert_interval.max}; }

		constexpr auto dimensions() const
		{
			return dimensions_2d<T, left, right, top, bottom>{max() - min()};
		}

		interval<T, left, right> horz_interval;
		interval<T, top, bottom> vert_interval;
	};

	template<class T, boundary_type left, boundary_type right, boundary_type top, boundary_type bottom>
	constexpr auto inside(vec<T, 2> v, rectangle<T, left, right, top, bottom> const& r)
	{
		return inside(v[0], r.horz_interval) && inside(v[1], r.vert_interval);
	}

	template<class T, boundary_type left, boundary_type right, boundary_type top, boundary_type bottom>
	constexpr auto outside(vec<T, 2> v, rectangle<T, left, right, top, bottom> const& r)
	{
		return !inside(v, r);
	}

	template<class T, boundary_type left, boundary_type right, boundary_type top, boundary_type bottom>
	constexpr auto inside(vec<T, 2> v, dimensions_2d<T, left, right, top, bottom> dim)
	{
		return inside(v, rectangle{dim, origin_at_zero{}});
	}

	template<class T, boundary_type left, boundary_type right, boundary_type top, boundary_type bottom>
	constexpr auto outside(vec<T, 2> v, dimensions_2d<T, left, right, top, bottom> dim)
	{
		return !inside(v, dim);
	}
}

#endif