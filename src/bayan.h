#ifndef BAYAN_H
#define BAYAN_H

#include "bscanner.h"
#include "hash_processor.h"
#include "bfile.h"

#include <iostream>
#include <fstream>


/**
 * @brief Сканирует дирректории и выводит результат
 */
class Bayan
{
    const BOptions _opt;
    BScanner _scanner;
    std::set<fs::path> _filePaths;
    std::unique_ptr<IHashProcessor> _hashProcessor;

    std::unordered_multimap<std::string, fs::path> _result;

public:
    Bayan(const BOptions &opt) : _opt{opt}, _scanner{_opt}
    {
        if (_opt._alg_hash == "md5")
        {
            _hashProcessor = std::make_unique<MD5Processor>();
        }
        else if (_opt._alg_hash == "sha1")
        {
            _hashProcessor = std::make_unique<SHA1Processor>();
        }
        else 
        {
            _hashProcessor = std::make_unique<EmptyProcessor>();
        }
    }
    void scan()
    {
        _filePaths = _scanner.getFilePaths();
        compareFiles();
    }
    void printResult() const
    {
        std::set<std::string> unique_keys;
        for (auto i = _result.cbegin(); i != _result.cend(); ++i)
        {
            unique_keys.emplace(i->first);
        }

        for (const auto &key : unique_keys)
        {
            auto iters = _result.equal_range(key);
            for (; iters.first != iters.second; ++iters.first)
            {
                std::cout << iters.first->second << std::endl;
            }
            std::cout << std::endl;
        }
    }

private:
    void compareFiles()
    {
        if (_filePaths.size() < 2)
        {
            std::cerr << "there is no files to compare" << std::endl;
            return;
        }

        std::set<std::pair<std::string, fs::path> > temp;
        for (auto i = _filePaths.begin(); i != std::prev(_filePaths.end(), 1); ++i)
        {
            for (auto j = std::next(i, 1); j != _filePaths.end(); ++j)
            {
                auto hash = compareByBlock(*i, *j);
                if (hash == "-1")
                {
                    continue;
                }
                temp.emplace(std::make_pair(hash, *i));
                temp.emplace(std::make_pair(hash, *j));
            }
        }
        for (const auto &p : temp)
        {
            _result.emplace(std::move(p));
        }
    }

    /**
     * @brief поблочное сравнение файлов
     */
    std::string compareByBlock(const fs::path &first, const fs::path &second)
    {
        BFile file_first(first.string().data());
        BFile file_second(second.string().data());
        if (!file_first.opened() || !file_second.opened())
        {
            return "-1";
        }
        if (file_first.getFileSize() != file_second.getFileSize())
        {
            return "-1";
        }

        std::string allBlocks;

        uintmax_t readedBytes = 0;//прочитанные байты всего
        uintmax_t sizeToRead = 0;//кол-во байт для чтения в каждой итерации
        char *buffer = (char*) malloc(sizeof(char) * _opt._block_size);

        auto _getNextBlock =[&](BFile &file) -> std::string
        {
            auto bytes = file.read(buffer, sizeToRead);
            if (bytes < _opt._block_size)
            {
                //если прочитано меньше, чем размер блока, остаток заполним нулями
                std::fill(buffer + bytes, buffer + _opt._block_size, '\0');
            }
            buffer[_opt._block_size] = '\0';
            return std::string(buffer); 
        };

        auto fSize = file_first.getFileSize();//кол-во символов в файлах
        while(readedBytes < fSize)
        {
            auto rest = fSize - readedBytes;// остаток
            rest > _opt._block_size ? sizeToRead = _opt._block_size
                                    : sizeToRead = rest;

            auto block_first = _getNextBlock(file_first);
            auto block_second = _getNextBlock(file_second);
            if (_hashProcessor->encrypt(block_first) != _hashProcessor->encrypt(block_second))
            {
                return "-1";
            }
            readedBytes += sizeToRead;
            allBlocks += block_first;
        }
        free(buffer);
        return _hashProcessor->encrypt(allBlocks);
    }
};

#endif //BAYAN_H