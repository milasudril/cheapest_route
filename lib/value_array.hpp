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

	template<class T>
	inline void throw_when_equal(T a, T b, std::string&& message)
	{
		if(a == b)
		{
			throw std::runtime_error{std::move(message)};
		}
	}

	template<class T>
	inline void throw_when_not_equal(T a, T b, std::string&& message)
	{
		if(a != b)
		{
			throw std::runtime_error{std::move(message)};
		}
	}

	template<class T>
	T from_string(std::string const& val)
	{
		return T{val};
	}


	template<>
	inline long from_string<long>(std::string const& val)
	{
		return std::stol(val);
	}

	template<class T, size_t N>
	static_value_array<T, N>::static_value_array(std::string_view val_str)
	{
		size_t k = 0;

		auto ptr = std::begin(val_str);
		enum class state:int{start, pre_value, value, after_value};
		state current_state = state::start;
		std::string buffer;

		while(true)
		{
			throw_when_equal(ptr, std::end(val_str), "Premature end of value array");
			auto ch_in = *ptr;
			++ptr;
			switch(current_state)
			{
				case state::start:
					if(ch_in >= '\0' && ch_in <= ' ')
					{ break;}

					if(ch_in != '(')
					{ throw std::runtime_error{"Expected ( in beginning of value array"}; }

					current_state = state::pre_value;
					break;

				case state::pre_value:
					if(ch_in >= '\0' && ch_in <= ' ')
					{ break;}

					buffer += ch_in;
					current_state = state::value;
					break;

				case state::value:
					switch(ch_in)
					{
						case ',':
							throw_when_equal(k, N, "Too long value array");
							m_values[k] = from_string<T>(buffer);
							buffer.clear();
							++k;
							current_state = state::after_value;
							break;

						case ')':
							throw_when_equal(k, N, "Too long value array");
							m_values[k] = from_string<T>(buffer);
							buffer.clear();
							++k;
							throw_when_not_equal(k, N, "Missing values in value array");
							return;

						default:
							buffer += ch_in;
							break;
					}
					break;

				case state::after_value:
					if(ch_in >= '\0' && ch_in <= ' ')
					{ break;}

					buffer += ch_in;
					current_state = state::value;
					break;
			}
		}

	}

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