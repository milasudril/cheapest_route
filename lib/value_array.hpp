#ifndef CHEAPESTROUTE_VALUEARRAY_HPP
#define CHEAPESTROUTE_VALUEARRAY_HPP

#include <cstddef>
#include <string>
#include <string_view>
#include <array>
#include <span>
#include <stdexcept>

namespace cheapest_route
{
	template<class T, size_t N>
	class static_value_array
	{
	public:
		constexpr static_value_array() = default;

		explicit static_value_array(std::string_view val_str);

		constexpr T& operator[](size_t n) { return m_values[n]; }

		constexpr T const& operator[](size_t n) const { return m_values[n]; }

	private:
		std::array<T, N> m_values;
	};

	template<class T, size_t N>
	static_value_array<T, N>::static_value_array(std::string_view)
	{}

	template<class T, size_t N>
	std::string to_string(static_value_array<T, N> const& values)
	{
		using std::to_string;
		std::string ret{"("};
		for(size_t k = 0; k != N; ++k)
		{
			ret.append(to_string(values[k])).append(k != N - 1 ? ", " : ")");
		}
		return ret;
	}
}

#endif