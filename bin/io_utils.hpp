#ifndef CHEAPESTROUTE_IO_UTILS_HPP
#define CHEAPESTROUTE_IO_UTILS_HPP

#include <filesystem>
#include <variant>

namespace cheapest_route
{
	struct std_output_stream
	{
		FILE* file;
	};

	struct file_deleter
	{
		void operator()(FILE* f) const
		{
			if(f != nullptr)
			{ fclose(f); }
		}
	};

	using file_handle = std::unique_ptr<FILE, file_deleter>;

	class output_file
	{
	public:
		explicit output_file(std::filesystem::path const& path):
			m_file{file_handle{fopen(path.c_str(), "wb")}}
		{}

		explicit output_file(std_output_stream stream):m_file{stream.file}
		{}

		bool is_owner() const
		{ return m_file.index() == 1; }

		auto get() const
		{
			return std::visit([]<class T>(T const& item){
				if constexpr(std::is_same_v<T, FILE*>)
				{ return item; }
				else
				{ return item.get(); }
			}, m_file);
		}

	private:
		std::variant<FILE*, file_handle> m_file;
	};
};

#endif