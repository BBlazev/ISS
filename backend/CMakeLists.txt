include(FetchContent)


FetchContent_Declare(
    Crow
    GIT_REPOSITORY https://github.com/CrowCpp/Crow.git
    GIT_TAG v1.2.1.2
)
set(CROW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(CROW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(Crow)


FetchContent_Declare(
    json_schema_validator
    GIT_REPOSITORY https://github.com/pboettch/json-schema-validator.git
    GIT_TAG 2.3.0
)
set(JSON_VALIDATOR_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(JSON_VALIDATOR_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(json_schema_validator)

FetchContent_Declare(
    pugixml
    GIT_REPOSITORY https://github.com/zeux/pugixml.git
    GIT_TAG v1.14
)
FetchContent_MakeAvailable(pugixml)

find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBXML2  REQUIRED IMPORTED_TARGET libxml-2.0)
pkg_check_modules(SQLITE3  REQUIRED IMPORTED_TARGET sqlite3)
pkg_check_modules(LIBCURL  REQUIRED IMPORTED_TARGET libcurl)
pkg_check_modules(PROTOBUF REQUIRED IMPORTED_TARGET protobuf)
pkg_check_modules(GRPCPP   REQUIRED IMPORTED_TARGET grpc++)
pkg_check_modules(OPENSSL  REQUIRED IMPORTED_TARGET openssl)
pkg_check_modules(LIBJWT   REQUIRED IMPORTED_TARGET libjwt)


set(SOAP_GEN_DIR ${CMAKE_CURRENT_SOURCE_DIR}/soap_gen)
set(SOAP_GEN_SOURCES
    ${SOAP_GEN_DIR}/soapC.cpp
    ${SOAP_GEN_DIR}/soapFileSearchService.cpp
)

set(GRPC_GEN_DIR ${CMAKE_CURRENT_SOURCE_DIR}/grpc_gen)
set(GRPC_GEN_SOURCES
    # ${GRPC_GEN_DIR}/weather.pb.cc
    # ${GRPC_GEN_DIR}/weather.grpc.pb.cc
)


file(GLOB BACKEND_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

add_executable(backend
    ${BACKEND_SOURCES}
    ${SOAP_GEN_SOURCES}
    ${GRPC_GEN_SOURCES}
)

target_include_directories(backend PRIVATE
    src
    ${SOAP_GEN_DIR}
    ${GRPC_GEN_DIR}
    /usr/include/gsoap
)

target_link_libraries(backend PRIVATE
    Crow::Crow
    nlohmann_json_schema_validator
    PkgConfig::LIBXML2
    PkgConfig::SQLITE3
    PkgConfig::LIBCURL
    PkgConfig::PROTOBUF
    PkgConfig::GRPCPP
    PkgConfig::OPENSSL
    PkgConfig::LIBJWT
    pugixml
    gsoap++
)

target_compile_definitions(backend PRIVATE
    SCHEMAS_DIR="${CMAKE_SOURCE_DIR}/schemas"
    DB_PATH="${CMAKE_SOURCE_DIR}/interop.db"
)
