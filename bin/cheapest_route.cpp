//@	{"target":{"name":"cheapest_route.o"}}

#include "./cmdline.hpp"
#include "lib/search.hpp"

#include <filesystem>
#include <variant>

#include <cassert>

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

	private:
		std::variant<FILE*, file_handle> m_file;
	};
};

int main(int argc, char** argv) try
{
	cheapest_route::command_line cmdline{argc, argv};

	std::filesystem::path heighmap_path{cmdline["heightmap"]};
	auto const cost_function_path = get_if<std::filesystem::path>(cmdline, "cost_function");
	if(!cost_function_path.has_value())
	{
		fprintf(stderr, "(!) No cost_function set, using homogenous cost of 1\n");
	}

	auto output =
		get_or<cheapest_route::output_file>(get_if<std::filesystem::path>(cmdline, "output"),
			cheapest_route::output_file{cheapest_route::std_output_stream{stdout}});
#if 0
	int64_t const size = 1024;
	auto const valid_range =
		cheapest_route::make_interval<cheapest_route::boundary_type::inclusive,
			cheapest_route::boundary_type::exclusive>(0l, size);
	auto const rect = cheapest_route::rectangle{valid_range, valid_range}.dimensions();

	auto result = search(cheapest_route::from<int64_t>{size - 1, 2*size/3},
		cheapest_route::to<int64_t>{0, size/3}, rect);

	std::ranges::for_each(result, [](auto const& item){
		printf("%.8g %.8g %.8g\n", item.loc[0], item.loc[1], item.total_cost);
	});
#endif
}
catch(std::exception const& err)
{
	fprintf(stderr, "cheapest_route: %s\n", err.what());
	return -1;
}