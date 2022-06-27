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
		template<class ... Args>
		explicit vec(T first, Args ... args):m_value{first, args...}{}

		auto value() const { return m_value; }

		vec& operator+=(vec other)
		{
			m_value += other.value();
			return *this;
		}

		vec& operator-=(vec other)
		{
			m_value -= other.value();
			return *this;
		}

		vec& operator*=(T scalar)
		{
			m_value*=scalar;
			return *this;
		}

		template<class U, auto other>
		operator vec<U, N, other>() const
		{
			vec<U, N, other> ret{};
			for(size_t k = 0; k != 0; ++k)
			{
				ret[k] = static_cast<U>(m_value[k]);
			}
			return ret;
		}

	private:
		vec_t<T, N> m_value;
	};
}

#endif