#pragma once
#include "file_entity.hpp"
#include <string>
#include <vector>
#include <optional>
#include <sqlite3.h>

class Database
{
public:

    explicit Database(const std::string& path);
    ~Database();

    void init_schema();

private:
    struct sqlite3* db_ = nullptr;
};