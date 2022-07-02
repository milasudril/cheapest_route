#ifndef CHEAPESTROUTE_RECTANGLE_HPP
#define CHEAPESTROUTE_RECTANGLE_HPP

#include "./interval.hpp"
#include "./vec.hpp"

namespace cheapest_route
{
	template<class T, boundary_type left, boundary_type right, boundary_type top, boundary_type bottom>
	struct rectangle
	{
		constexpr auto width() const
		{ return horz_interval.max - horz_interval.min;}

		constexpr auto height() const
		{ return vert_interval.max - vert_interval.min; }

		constexpr auto min() const
		{ return vec<T, 2>{horz_interval.min, vert_interval.min}; }

		constexpr auto max() const
		{ return vec<T, 2>{horz_interval.max, vert_interval.max}; }


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
}

#endif