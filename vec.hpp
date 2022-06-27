#ifndef CHEAPESTROUTE_VEC_HPP
#define CHEAPESTROUTE_VEC_HPP

#include <type_traits>
#include <cstdint>
#include <cstddef>

namespace cheapest_route
{
	template<class T, size_t N>
	requires(std::is_arithmetic_v<T>)
	using vec_t [[gnu::vector_size(N*sizeof(T))]] = T;

	using vec2i_t = vec_t<int64_t, 2>;
	using vec2f_t = vec_t<double, 2>;

	template<class T, size_t N, auto tag>
	class vec
	{
	public:
        vec() = default;

        constexpr explicit vec(vec_t<T, N> val): m_value{val}{}

		template<class ... Args>
        requires(sizeof...(Args) == N - 1)
		constexpr explicit vec(T first, Args ... args):m_value{first, args...}{}

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
		constexpr operator vec<U, N, other>() const
		{
			vec_t<U, N> ret;
			for(size_t k = 0; k != N; ++k)
			{
				ret[k] = static_cast<U>(m_value[k]);
			}
			return vec<U, N, other>{ret};
		}

        constexpr T operator[](size_t k) const { return m_value[k]; }

	private:
		vec_t<T, N> m_value;
	};

	template<class T, size_t N, auto tag>
	requires std::is_floating_point_v<T>
	auto operator/(vec<T, N, tag> val, T scalar)
	{
		return vec<T, N, tag>{val.value()/scalar};
	}

	template<class T, size_t N, auto tag>
	auto operator*(T scalar, vec<T, N, tag> val)
	{
		val*=scalar;
		return val;
	}

	template<class T, size_t N, auto tag>
	auto operator*(vec<T, N, tag> val, T scalar)
	{
		val*=scalar;
		return val;
	}

	template<class T, size_t N, auto tag>
	auto operator+(vec<T, N, tag> a, vec<T, N, tag> b)
	{
		a+=b;
		return a;
	}

	template<class T, size_t N, auto tag>
	auto operator-(vec<T, N, tag> a, vec<T, N, tag> b)
	{
		a-=b;
		return a;
	}

	template<class T, size_t N, auto tag>
	auto dot(vec<T, N, tag> a, vec<T, N, tag> b)
	{
		T sum{};
		a*=b;
		for(size_t k = 0; k!= N; ++k)
		{
			sum += a[k];
		}
		return sum;
	}

	template<class T, size_t N, auto tag>
	auto length_squared(vec<T, N, tag> a)
	{
		return dot(a ,a);
	}


}

#endif