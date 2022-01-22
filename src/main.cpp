
#include "bayan.h"

#include <boost/program_options.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/tokenizer.hpp>

namespace po = boost::program_options;

int main (int argc, char *argv[])
{
    po::options_description desc;
    desc.add_options()
        ("help,h", "This screen")
        ("include,i", po::value<std::string>(), "Included dirrectories for scan (maybe several) separated by ','")
        ("exclude,e", po::value<std::string>(), "Excluded dirrectories (maybe several) separated by ','")
        ("recursive,r", po::value<int>()->default_value(0), "Values:\n"
                                                            "  0: \tscan only included directories\n"
                                                            "  1: \trecursive scan")
        ("file_size,s", po::value<uintmax_t>()->default_value(1), "Minimal size of files to scan (bytes)")
        ("mask,m", po::value<std::string>(), "Mask of filename to scan (case-independent)")
        ("block_size,b", po::value<uintmax_t>()->default_value(5), "Size of block reading files")
        ("alg_hash,a", po::value<std::string>(), "Hash algorythm. Values:\n"
                                                 "  \"\" \t no hash\n"
                                                 "  \"md5\"\n"
                                                 "  \"sha1\"")
        
        ;

    po::variables_map vm;
    BOptions opt;
    try 
    {
        po::store(po::parse_command_line(argc, argv, desc), vm);
        if (vm.count("help"))
        {
            std::cout << desc << std::endl;
            return 0;
        }
        if (!vm.count("include"))
        {
            // папки не заданы, выходим
            std::cerr << "included directories not set" << std::endl;
            return 0;
        }

        auto pathReader = [](const std::string &var, std::set<fs::path> &paths) -> void
        {
            boost::tokenizer<boost::char_separator<char>> tok{var, boost::char_separator<char>{","}};
            for (auto &&path : tok)
            {
                paths.emplace(fs::current_path() / std::move(path));
            }
        };
        
        for (const auto &var : vm)
        {
            if (var.first == "include")
            {
                pathReader(var.second.as<std::string>(), opt._include);
            }
            else if (var.first == "exclude")
            {
                pathReader(var.second.as<std::string>(), opt._exclude);
            }
            else if (var.first == "recursive")
            {
                opt._recursive = var.second.as<int>() == 1;
            }
            else if (var.first == "file_size")
            {
                opt._file_size = var.second.as<uintmax_t>();
            }
            else if (var.first == "mask")
            {
                opt._mask = var.second.as<std::string>();
                std::transform(opt._mask.begin(), opt._mask.end(), opt._mask.begin(), ::tolower);
            }
            else if (var.first == "block_size")
            {
                opt._block_size = var.second.as<uintmax_t>();
            }
            else if (var.first == "alg_hash")
            {
                opt._alg_hash = var.second.as<std::string>();
            }
        }
    }
    catch (const std::exception &e) 
    {
        std::cerr << e.what() << std::endl;
        return 0;
    }

    Bayan bayan{opt};
    bayan.scan();
    bayan.printResult();
  
    return 0;
}
