#include "file_entity.hpp"

using nlohmann::json;

json FileEntity::to_json() const {
    json j;
    j["id"] = id;
    j["filename_download"] = filename_download;
    j["title"] = title;
    j["type"] = type;
    j["filesize"] = filesize;
    if (width) j["width"] = *width;
    if (height) j["height"] = *height;
    if (description) j["description"] = *description;
    j["tags"] = tags;
    j["uploaded_on"] = uploaded_on;
    return j;
}

FileEntity FileEntity::from_json(const json& j) {
    FileEntity f;
    f.id = j.at("id").get<std::string>();
    f.filename_download = j.at("filename_download").get<std::string>();
    f.title = j.at("title").get<std::string>();
    f.type = j.at("type").get<std::string>();
    f.filesize = j.at("filesize").get<long long>();
    if (j.contains("width") && !j["width"].is_null())
        f.width = j["width"].get<int>();
    if (j.contains("height") && !j["height"].is_null())
        f.height = j["height"].get<int>();
    if (j.contains("description") && !j["description"].is_null())
        f.description = j["description"].get<std::string>();
    if (j.contains("tags"))
        f.tags = j["tags"].get<std::vector<std::string>>();
    f.uploaded_on = j.at("uploaded_on").get<std::string>();
    return f;
}
