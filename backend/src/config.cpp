#include "config.hpp"
#include <fstream>
#include <sstream>

static std::string trim(const std::string& s) 
{
    size_t a = s.find_first_not_of(" \t\r\n");
    size_t b = s.find_last_not_of(" \t\r\n");
    if (a == std::string::npos) return "";
    return s.substr(a, b - a + 1);
}

Config Config::load(const std::string& env_path) 
{
    Config c;
    std::ifstream f(env_path);
    if (!f) return c;

    std::string line;
    while (std::getline(f, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#') continue;
        auto eq = line.find('=');
        if (eq == std::string::npos) continue;
        std::string key = trim(line.substr(0, eq));
        std::string val = trim(line.substr(eq + 1));

        if      (key == "DIRECTUS_URL")    c.directus_url = val;
        else if (key == "DIRECTUS_TOKEN")  c.directus_token = val;
        else if (key == "HTTP_PORT")       c.http_port = std::stoi(val);
        else if (key == "USE_CUSTOM_API")  c.use_custom_api = (val == "true" || val == "1");
        else if (key == "JWT_SECRET")      c.jwt_secret = val;
    }
    return c;
}
