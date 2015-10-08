#pragma once

#include "encryption.h"
#include <iostream>
#include <string>
#include <vector>
#include <utility>

// TODO: drop extension field
// TODO: secure memory
class filesystem
{
private:
    static const int salt_len = 12;
    static const int max_files = 128;
    static const int max_name = 16;
    static const int file_size = sizeof(size_t) * 2 + max_name + 3;
    static const int directory_size = max_files * file_size;

    struct file
    {
        size_t offset;
        size_t size;
        char name[max_name], extension[3];
    };
    struct directory
    {
        file files[max_files];
    };
public:
    typedef std::vector<std::pair<std::vector<char>, std::string>> files_t;
private:
    std::string path;
    std::string salt;
    unsigned char key_256[256 / 8];
    files_t files;

    void read_file(const char* password_160);
    void write_file();
public:
    explicit filesystem(const std::string& path, const char* password_160);
    ~filesystem();
    
    bool add(std::istream& file, const std::string& name);
    bool add(const std::string& path, const std::string& name);
    bool remove(const std::string& name);
    files_t::iterator get(const std::string& name);
    const files_t& get_all() const {return this->files;}
};