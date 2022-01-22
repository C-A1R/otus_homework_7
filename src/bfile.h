#ifndef BFILE_H
#define BFILE_H

#include <filesystem>
#include <iostream>

/**
 * @brief RAII оболочка для файла
 */
class BFile 
{
    std::FILE *m_file_handle {nullptr};
    uintmax_t fileSize {0};

public:
    BFile(const fs::path &path) : m_file_handle{std::fopen(path.string().data(), "rb")} 
    {
        if (!m_file_handle)
        {
            std::cerr << "file not open: " << path.string() << std::endl;
            return;
        }
        //найдем количество символов в файле:
        fseek (m_file_handle , 0 , SEEK_END);
        fileSize = ftell (m_file_handle);
        rewind (m_file_handle);
    }
    ~BFile() 
    {
        if (!m_file_handle)
        {
            return;
        }
        std::fclose(m_file_handle);
    }
    uintmax_t read(char *buffer, uintmax_t sizeToRead)
    {
        if (!m_file_handle)
        {
            return 0;
        }
        return fread(buffer, 1, sizeToRead, m_file_handle);
    }
    uintmax_t getFileSize() const
    {
        return fileSize;
    }
    bool opened() const
    {
        return m_file_handle != nullptr;
    }

    // Копирование и присваивание не реализовано.  Предотвратим их использование.
    BFile(const BFile &) = delete;
    BFile & operator=(const BFile &) = delete;
    BFile(BFile &&) = delete;
    BFile & operator=(BFile &&) = delete;
};

#endif //BFILE_H 
