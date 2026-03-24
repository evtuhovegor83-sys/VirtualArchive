#pragma once
#include <string>
#include <memory>
#include "Date.h"

class Resource {
protected:
    std::string name;
    Date creationDate;

public:
    Resource(const std::string& name);
    virtual ~Resource() = default;

    std::string getName() const;
    Date getCreationDate() const;
    void setName(const std::string& newName);

    virtual long long getSize() const = 0;
    virtual void print(int depth = 0) const = 0;
    virtual std::unique_ptr<Resource> clone() const = 0;
};