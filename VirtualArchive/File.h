#pragma once
#include "Resource.h"
#include <string>

class File : public Resource {
private:
    std::string extension;
    long long size;

public:
    File(const std::string& name, const std::string& ext, long long sz = 0);

    std::string getExtension() const;
    long long getSize() const override;
    void setSize(long long sz);

    void print(int depth = 0) const override;
    std::unique_ptr<Resource> clone() const override;

    static bool validateName(const std::string& name);
};