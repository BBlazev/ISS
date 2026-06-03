find_package(SQLite3 REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(nlohmann_json REQUIRED)

find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBXML2 REQUIRED IMPORTED_TARGET libxml-2.0)

include(FetchContent)

FetchContent_Declare(
    Crow
    GIT_REPOSITORY https://github.com/CrowCpp/Crow.git
    GIT_TAG v1.2.0
)
FetchContent_MakeAvailable(Crow)

FetchContent_Declare(
    json_schema_validator
    GIT_REPOSITORY https://github.com/pboettch/json-schema-validator.git
    GIT_TAG 2.3.0
)
set(JSON_VALIDATOR_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(JSON_VALIDATOR_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(json_schema_validator)

file(GLOB BACKEND_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
add_executable(backend ${BACKEND_SOURCES})
target_include_directories(backend PRIVATE src)
target_link_libraries(backend PRIVATE
    SQLite::SQLite3
    OpenSSL::Crypto
    nlohmann_json::nlohmann_json
    Crow::Crow
    PkgConfig::LIBXML2
    nlohmann_json_schema_validator
)

target_compile_definitions(backend PRIVATE
    SCHEMAS_DIR="${CMAKE_SOURCE_DIR}/schemas"
    DB_PATH="${CMAKE_SOURCE_DIR}/interop.db"
)
