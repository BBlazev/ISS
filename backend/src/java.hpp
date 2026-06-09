#include <cstdio>
#include <cstdlib>
#include <sys/wait.h>

#include <array>
#include <string>
#include <type_traits>

static const std::string JAR_PATH = "validator/target/validator.jar";
static const std::string XSD_PATH = "schemas/files.xsd";

struct Result {
    int code;
    std::string output;
};
Result validate_java(std::string xmlPath) {
    Result result;
    std::string cmd = "java -jar " + JAR_PATH + " " + xmlPath + " " + XSD_PATH + " 2>&1";

    FILE *p = popen(cmd.c_str(), "r");
    if (!p)
        return {-1, "{\"valid\":false,\"errors\":[],\"error\":\"failed to start java\"}"};
    std::array<char, 256> buffer;

    while (fgets(buffer.data(), buffer.size(), p) != nullptr) {
        result.output += buffer.data();
    }
    int status = pclose(p);
    if (status == -1)
        result.code = -1;
    else
        result.code = WEXITSTATUS(status);

    return result;
}
