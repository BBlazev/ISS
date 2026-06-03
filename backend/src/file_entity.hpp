#pragma once
#include <nlohmann/json.hpp>

#include <optional>
#include <string>
#include <vector>

struct FileEntity {

  std::string id;
  std::string filename_download;
  std::string title;
  std::string type;
  long long filesize = 0;
  std::optional<int> width;
  std::optional<int> height;
  std::optional<std::string> description;
  std::vector<std::string> tags;
  std::string uploaded_on;

  nlohmann::json to_json() const;
  static FileEntity from_json(const nlohmann::json &j);
};
