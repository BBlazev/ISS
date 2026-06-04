
#include <nlohmann/json.hpp>
#include <openssl/sha.h>

#include <iomanip>
#include <sstream>
#include <stdexcept>

#include "db.hpp"

Database::Database(const std::string &path) {

    if (sqlite3_open(path.c_str(), &db_) != SQLITE_OK) {
        std::string err = sqlite3_errmsg(db_);
        sqlite3_close(db_);
        throw std::runtime_error("SQLite open: " + err);
    }
}

Database::~Database() {
    if (db_)
        sqlite3_close(db_);
}

void Database::init_schema() {

    const char *sql = R"SQL(
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
    char *err = nullptr;
    if (sqlite3_exec(db_, sql, nullptr, nullptr, &err) != SQLITE_OK) {
        std::string msg = err ? err : "unknown";
        sqlite3_free(err);
        throw std::runtime_error("init_schema: " + msg);
    }
}

void Database::upsert_file(const FileEntity &f) {
    const char *sql = "INSERT INTO files (id, filename_download, title, type, filesize, "
                      "                   width, height, description, tags_json, uploaded_on) "
                      "VALUES (?,?,?,?,?,?,?,?,?,?) "
                      "ON CONFLICT(id) DO UPDATE SET "
                      "  filename_download=excluded.filename_download, "
                      "  title=excluded.title, "
                      "  type=excluded.type, "
                      "  filesize=excluded.filesize, "
                      "  width=excluded.width, "
                      "  height=excluded.height, "
                      "  description=excluded.description, "
                      "  tags_json=excluded.tags_json, "
                      "  uploaded_on=excluded.uploaded_on;";

    sqlite3_stmt *stmt = nullptr;
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK)
        throw std::runtime_error(std::string("prepare: ") + sqlite3_errmsg(db_));

    sqlite3_bind_text(stmt, 1, f.id.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, f.filename_download.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, f.title.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, f.type.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int64(stmt, 5, f.filesize);

    if (f.width)
        sqlite3_bind_int(stmt, 6, *f.width);
    else
        sqlite3_bind_null(stmt, 6);
    if (f.height)
        sqlite3_bind_int(stmt, 7, *f.height);
    else
        sqlite3_bind_null(stmt, 7);
    if (f.description)
        sqlite3_bind_text(stmt, 8, f.description->c_str(), -1, SQLITE_TRANSIENT);
    else
        sqlite3_bind_null(stmt, 8);

    nlohmann::json tj = f.tags;
    std::string tags_str = tj.dump();
    sqlite3_bind_text(stmt, 9, tags_str.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 10, f.uploaded_on.c_str(), -1, SQLITE_TRANSIENT);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    if (rc != SQLITE_DONE)
        throw std::runtime_error(std::string("step: ") + sqlite3_errmsg(db_));
}

