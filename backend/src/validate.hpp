#pragma once
#include <expected>

#include <string>
#include <vector>

std::expected<void, std::vector<std::string>>
validate_xml(const std::string &xml_content, const std::string &xsd_path);

std::expected<void, std::vector<std::string>>
validate_json(const std::string &json_content, const std::string &json_path);
