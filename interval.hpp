#ifndef CHEAPESTROUTE_INTERVAL_HPP
#define CHEAPESTROUTE_INTERVAL_HPP

#include <type_traits>

namespace cheapest_route
{
	enum class boundary_type:int{inclusive, exclusive};

	template<class T, boundary_type left, boundary_type right>
	requires(std::is_arithmetic_v<T>)
	struct interval
	{
		using value_type = T;

		T begin;
		T end;
	};

	template<boundary_type left, boundary_type right, class T>
	constexpr auto make_interval(T a, T b)
	{
		return interval<T, left, right>{a, b};
	}

	template<class T>
	constexpr bool inside(T x, interval<T, boundary_type::inclusive, boundary_type::inclusive> i)
	{
		return x >= i.begin && x <= i.end;
	}

	template<class T>
	constexpr bool inside(T x, interval<T, boundary_type::inclusive, boundary_type::exclusive> i)
	{
		return x >= i.begin && x < i.end;
	}

	template<class T>
	constexpr bool inside(T x, interval<T, boundary_type::exclusive, boundary_type::inclusive> i)
	{
		return x > i.begin && x <= i.end;
	}

	template<class T>
	constexpr bool inside(T x, interval<T, boundary_type::exclusive, boundary_type::exclusive> i)
	{
		return x > i.begin && x < i.end;
	}

	template<class T, boundary_type left, boundary_type right>
	constexpr auto outside(T x, interval<T, left, right> i)
	{
		return !inside(x, i);
	}
}

#endif