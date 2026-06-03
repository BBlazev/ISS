
#include "db.hpp"
#include <openssl/sha.h>
#include <stdexcept>
#include <iomanip>
#include <sstream>
#include <nlohmann/json.hpp>

Database::Database(const std::string& path) {

    if (sqlite3_open(path.c_str(), &db_) != SQLITE_OK) {
        std::string err = sqlite3_errmsg(db_);
        sqlite3_close(db_);
        throw std::runtime_error("SQLite open: " + err);
    }
}

Database::~Database() { if (db_) sqlite3_close(db_); }

void Database::init_schema() {

    const char* sql = R"SQL(
        CREATE TABLE IF NOT EXISTS files (
          id TEXT PRIMARY KEY,
          filename_download TEXT NOT NULL,
          title TEXT NOT NULL,
          type TEXT NOT NULL,
          filesize INTEGER NOT NULL,
          width INTEGER,
          height INTEGER,
          description TEXT,
          tags_json TEXT,
          uploaded_on TEXT NOT NULL
        );
        CREATE TABLE IF NOT EXISTS users (
          id INTEGER PRIMARY KEY AUTOINCREMENT,
          username TEXT UNIQUE NOT NULL,
          password_hash TEXT NOT NULL,
          role TEXT NOT NULL
        );
    )SQL";
    char* err = nullptr;
    if (sqlite3_exec(db_, sql, nullptr, nullptr, &err) != SQLITE_OK) {
        std::string msg = err ? err : "unknown";
        sqlite3_free(err);
        throw std::runtime_error("init_schema: " + msg);
    }
}