#pragma once
#include <string>
#include <memory>
#include <fstream>
#include "Directory.h"
#include "File.h"

class Serializer {
private:
    static const uint32_t MAGIC_NUMBER = 0x56415243; // "VARC" â hex

    static void writeString(std::ofstream& out, const std::string& str);
    static std::string readString(std::ifstream& in);

    static void writeDirectory(std::ofstream& out, Directory* dir);
    static void writeFile(std::ofstream& out, File* fileObj);

    static std::unique_ptr<Resource> readResource(std::ifstream& in);

public:
    static void save(const std::string& filename, Directory* root);
    static std::unique_ptr<Directory> load(const std::string& filename);

    static bool checkMagicNumber(std::ifstream& in);
};