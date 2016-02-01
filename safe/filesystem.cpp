#include "filesystem.h"
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <sstream>
#include <fstream>

filesystem::filesystem()
{
}

filesystem::~filesystem()
{
    SecureZeroMemory(this->key_256, sizeof(this->key_256));
    this->clear_files();
}

void filesystem::clear_files()
{
    for(files_t::iterator it = this->files.begin(); it != this->files.end(); it++)
        SecureZeroMemory(it->first.data(), it->first.size());
    this->files.clear();
}

void filesystem::create_archive(const std::string& path, const char* password_160)
{
    this->clear_files();

    this->path = path;
    this->change_password(password_160);
}

void filesystem::load_archive(const std::string& path, const char* password_160)
{
    this->path = path;
    this->read_file(password_160);
}

void filesystem::save_archive()
{
    this->write_file();
}

void filesystem::change_password(const char* password_160)
{
    encryption e;
    e.create_salt(this->salt, salt_len);
    e.derive_key(password_160, pass_len, this->salt, this->key_256);
}

void filesystem::read_file(const char* password_160)
{
    std::ifstream in(this->path, std::ios_base::binary);
    std::stringstream blob;

    in.exceptions(std::ios_base::eofbit | std::ios_base::failbit | std::ios_base::badbit);
    // read
    in.seekg(-salt_len, std::ios_base::end);
    const size_t in_len = in.tellg();
    char salt[salt_len] = {0};
    in.read(salt, salt_len);
    this->salt.assign(salt, salt_len);
    in.seekg(0, std::ios_base::beg);

    // unencrypt
    encryption e;
    unsigned char iv[128 / 8];
    e.derive_key(password_160, pass_len, this->salt, this->key_256);
    const int plain_len = e.decrypt(in, in_len, this->key_256, iv, blob);

    // serialize
    // TODO: enable filestream exceptions & create new
    int cur_pos = plain_len - directory_size;
    this->clear_files();
    for(size_t i = 0; i < max_files; i++)
    {
        blob.seekg(cur_pos);

        // read meta
        size_t offset, size;
        char name[max_name + 1] = {0}, extensions[3];
        blob.read((char*)&offset, sizeof(offset));
        blob.read((char*)&size, sizeof(size));
        blob.read(name, max_name);
        blob.read(extensions, sizeof(extensions));
        cur_pos = blob.tellg();
        if(size == 0)
            break; // no more files

        // read file
        blob.seekg(-(directory_size + (int)offset), std::ios_base::end);
        this->files.push_back(files_t::value_type());
        files_t::reference last = this->files.back();
        last.first.resize(size);
        for(size_t j = 0; j < size; j++)
        {
            char c;
            blob.read(&c, 1);
            last.first[j] = c;
        }
        last.second = name;
    }

    // TODO: clear blob
}

void filesystem::write_file()
{
    std::stringstream blob;

    // salt: 12 bytes, pass: 20 bytes (total 32 bytes)
    encryption e;
    unsigned char iv[128 / 8];
    encryption::create_iv(iv);
    blob.write((char*)iv, sizeof(iv));

    // serialize
    for(files_t::const_iterator it = this->files.begin(); it != this->files.end(); it++)
        std::copy(it->first.begin(), it->first.end(), std::ostream_iterator<char>(blob));

    size_t index = 0;
    size_t offset = 0;
    for(files_t::reverse_iterator rit = this->files.rbegin(); 
        rit != this->files.rend() && index < max_files; 
        rit++, index++)
    {
        size_t size = rit->first.size();
        offset += size;

        blob.write((char*)&offset, sizeof(offset)); // offset
        blob.write((char*)&size, sizeof(size)); // size
        char name[max_name];
        strncpy(name, rit->second.c_str(), max_name);
        blob.write(name, max_name); // name
        char ext[3] = {0};
        blob.write(ext, sizeof(ext)); // extension
    }
    for(; index < max_files; index++)
    {
        char empty[file_size] = {0};
        blob.write(empty, file_size);
    }

    // encrypt&write
    std::ofstream file(this->path, std::ios_base::binary | std::ios_base::trunc);
    const int ciph_len = e.encrypt(blob, blob.tellp(), this->key_256, iv, file);
    file.write(this->salt.c_str(), this->salt.length());

    // TODO: clear blob
}

bool filesystem::add(std::istream& file, const std::string& name)
{
    if(name.length() > max_name || files.size() >= max_files || this->get(name) != this->files.end())
        return false;

    files.push_back(files_t::value_type());
    files_t::reference last = files.back();
    std::copy(
        std::istreambuf_iterator<char>(file),
        std::istreambuf_iterator<char>(),
        std::back_inserter(last.first));
    last.second = name;

    return true;
}

bool filesystem::add(const std::string& path, const std::string& name)
{
    std::ifstream in(path, std::ios_base::binary);
    return this->add(in, name);
}

bool filesystem::remove(const std::string& name)
{
    files_t::iterator it = this->get(name);
    if(it != this->files.end())
    {
        SecureZeroMemory(it->first.data(), it->first.size());
        this->files.erase(it);
        return true;
    }
    return false;
}

filesystem::files_t::iterator filesystem::get(const std::string& name)
{
    for(files_t::iterator it = this->files.begin(); it != this->files.end(); it++)
        if(it->second == name)
            return it;
    return this->files.end();
}