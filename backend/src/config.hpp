#pragma once
#include <string>

struct Config {
    std::string directus_url;
    std::string directus_token;
    int http_port = 8080;
    bool use_custom_api = false;
    std::string jwt_secret = "ISS key";

    static Config load(const std::string& env_path = ".env");
};
