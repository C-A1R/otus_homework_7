#ifndef BSCANNER_H
#define BSCANNER_H

#include <set>
#include <string>
#include <filesystem>
#include <algorithm>

#include <iostream>

namespace fs = std::filesystem;

/**
 * @brief параметры
 */
struct BOptions
{
    std::set<fs::path> _include;
    std::set<fs::path> _exclude;
    bool _recursive {false};
    uintmax_t _file_size {1};
    std::string _mask;
    uintmax_t _block_size {5};
    std::string _alg_hash;
};

/**
 * @brief ищет файлы в соответствии с параметрами _opt
 */
class BScanner
{
    const BOptions _opt;
    std::set<fs::path> _scanned; ///< отсканированные папки
public:
    BScanner(const BOptions &opt) : _opt{opt} {}
    std::set<fs::path> getFilePaths()
    {
        std::set<fs::path> result;
        for (const auto &in : _opt._include)
        {
            if (!fs::exists(in))
            {
                std::cout << in.string() << " not exists!" << std::endl;
                continue;
            }
            findFilePaths(in, result);
        }
        return result;
    }
private:
    /**
     * @brief поиск файлов с фильтрацией по размеру и маске
     */
    void findFilePaths(const fs::path &path, std::set<fs::path> &result)
    {
        if (_opt._exclude.find(path) != _opt._exclude.end())
        {
            // папка в списке исключенных
            return;
        }

        for (auto iter = fs::directory_iterator(path); iter != fs::directory_iterator(); iter++)
        {
            fs::path _path = *iter;
            if (fs::is_directory(_path) && _opt._recursive)
            {
                if (_scanned.find(path) != _scanned.end())
                {
                    // папка уже отсканирована
                    continue;
                }
                findFilePaths(_path, result);
                _scanned.emplace(_path);
            }
            else if (!fs::is_directory(_path) && fs::file_size(_path) >= _opt._file_size)
            {
                std::string filename = _path.stem();
                std::transform(filename.begin(), filename.end(), filename.begin(), ::tolower);
                auto it = std::search(filename.begin(), filename.end(), _opt._mask.begin(), _opt._mask.end());
                if (it != filename.cend())
                {
                    result.emplace(_path);
                }
            }
        }
    }
};

#endif //BSCANNER_H