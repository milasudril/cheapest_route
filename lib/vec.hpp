#ifndef CHEAPESTROUTE_VEC_HPP
#define CHEAPESTROUTE_VEC_HPP

#include "./value_array.hpp"

#include <type_traits>
#include <cstdint>
#include <cstddef>
#include <string>
#include <stdexcept>

namespace cheapest_route
{
	template<class T, size_t N>
	requires(std::is_arithmetic_v<T> && N != 0)
	using vec_t [[gnu::vector_size(N*sizeof(T))]] = T;

	using vec2i_t = vec_t<int64_t, 2>;
	using vec2f_t = vec_t<double, 2>;
	using vec4i_t = vec_t<int32_t, 4>;
	using vec4f_t = vec_t<float, 4>;

    template<class U, class T>
    constexpr auto vector_cast(vec_t<T, 2> x)
    {return vec_t<U, 2>{static_cast<U>(x[0]),static_cast<U>(x[1])}; }

    template<class U, class T, int = 0>
    constexpr auto vector_cast(vec_t<T, 4> x)
    {return vec_t<U, 4>{static_cast<U>(x[0]),static_cast<U>(x[1]), static_cast<U>(x[2]), static_cast<U>(x[3])}; }

	template<class T, size_t N, auto tag = 0>
	class vec
	{
	public:
        vec() = default;

        constexpr explicit vec(vec_t<T, N> val): m_value{val}{}

		template<class ... Args>
        requires(sizeof...(Args) == N - 1)
		constexpr explicit vec(T first, Args ... args):m_value{first, args...}{}

		explicit vec(std::string_view src):m_value{}
		{
			static_value_array<T, N> values{src};

			for(size_t k = 0; k != N; ++k)
			{ m_value[k] = values[k]; }
		}

		constexpr auto value() const { return m_value; }

		constexpr vec& operator+=(vec other)
		{
			m_value += other.value();
			return *this;
		}

		constexpr vec& operator-=(vec other)
		{
			m_value -= other.value();
			return *this;
		}

		constexpr vec& operator*=(T scalar)
		{
			m_value*=scalar;
			return *this;
		}

		template<class U, auto other>
		explicit constexpr operator vec<U, N, other>() const
        { return vec<U, N, other>(vector_cast<U, T>(m_value)); }

        constexpr T operator[](size_t k) const { return m_value[k]; }

        constexpr T& operator[](size_t k)  { return m_value[k]; }

        constexpr auto operator-() const { return vec{-m_value}; }

	private:
		vec_t<T, N> m_value;
	};

	template<class T, size_t N, auto tag>
	requires std::is_floating_point_v<T>
	constexpr auto operator/(vec<T, N, tag> val, T scalar)
	{
		return vec<T, N, tag>{val.value()/scalar};
	}

	template<class T, size_t N, auto tag>
	constexpr auto operator*(T scalar, vec<T, N, tag> val)
	{
		val*=scalar;
		return val;
	}

	template<class T, size_t N, auto tag>
	constexpr auto operator*(vec<T, N, tag> val, T scalar)
	{
		val*=scalar;
		return val;
	}

	template<class T, size_t N, auto tag>
	constexpr auto operator+(vec<T, N, tag> a, vec<T, N, tag> b)
	{
		a+=b;
		return a;
	}

	template<class T, size_t N, auto tag>
	constexpr auto operator-(vec<T, N, tag> a, vec<T, N, tag> b)
	{
		a-=b;
		return a;
	}

	template<class T, size_t N, auto tag>
	constexpr vec<T, N, tag> elem_abs(vec<T, N, tag> a)
	{
		auto const tmp = a.value();
		return vec<T, N, tag>{(tmp < vec_t<T, N>{}) ? -tmp : tmp};
	}

	template<class T, size_t N, auto tag>
	constexpr auto dot(vec<T, N, tag> a, vec<T, N, tag> b)
	{
		T sum{};
		auto const tmp = a.value() * b.value();
		for(size_t k = 0; k!= N; ++k)
		{
			sum += tmp[k];
		}
		return sum;
	}

	static_assert(dot(vec<int, 2, 0>{1, 0}, vec<int, 2, 0>{0, 1}) == 0);

	template<class T, size_t N, auto tag>
	constexpr auto length_squared(vec<T, N, tag> a)
	{
		return dot(a ,a);
	}

	template<class U, class T, size_t N, auto tag>
	requires(std::is_integral_v<T>)
	constexpr auto scale_to_float(U factor, vec<T, N, tag> a)
	{
		return static_cast<vec<U, N, tag>>(a)/factor;
	}

	template<class T, size_t N, auto tag>
	auto to_string(vec<T, N, tag> a)
	{
		static_value_array<T, N> values{};
		for(size_t k = 0; k != N; ++k)
		{
			values[k] = a[k];
		}

		return to_string(values);
	}
}

#endif