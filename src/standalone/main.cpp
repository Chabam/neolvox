#include <pdal/PointView.hpp>
#include <iostream>

constexpr auto s_invalid_args_msg = "Error invalid amount of arguments supplied";
constexpr auto s_usage_msg = "Usage: lvox <LASFILE>";

int main(int argc, char* argv[])
{
    if (argc == 2)
    {
        std::cout << s_invalid_args_msg << "\n" << s_usage_msg << std::endl;
    }

    std::string_view las_filepath{argv[1]};
}
