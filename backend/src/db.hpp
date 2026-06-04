#pragma once
#include <sqlite3.h>

#include <optional>
#include <string>
#include <vector>

#include "file_entity.hpp"

class Database {
  public:
    explicit Database(const std::string &path);
    ~Database();

    void init_schema();
    void upsert_file(const FileEntity &f);

  private:
    struct sqlite3 *db_ = nullptr;
};
