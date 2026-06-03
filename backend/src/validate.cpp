#include <cstdarg>
#include <cstdio>
#include <expected>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/valid.h>
#include <libxml/xmlschemas.h>
#include <nlohmann/json-schema.hpp>
#include <nlohmann/json_fwd.hpp>

#include <fstream>
#include <regex>
#include <vector>

#include "validate.hpp"

using nlohmann::json;
using nlohmann::json_schema::json_validator;

class ErrorHandler : public nlohmann::json_schema::basic_error_handler {
  public:
    std::vector<std::string> errors;
    void error(const nlohmann::json::json_pointer &ptr, const json &instance,
               const std::string &message) override {
        nlohmann::json_schema::basic_error_handler::error(ptr, instance, message);
        std::string where = ptr.to_string();
        if (where.empty())
            where = "/";
        errors.push_back(where + ": " + message);
    }
};

namespace {

void collect_error(void *userData, xmlErrorPtr err) {
    auto *errors = static_cast<std::vector<std::string> *>(userData);

    std::string msg = err->message ? err->message : "unknown error";
    while (!msg.empty() && (msg.back() == '\n' || msg.back() == '\r'))
        msg.pop_back();

    if (err->line > 0)
        errors->push_back("line " + std::to_string(err->line) + ": " + msg);
    else
        errors->push_back(msg);
}
} // namespace

void format_check(const std::string &format, const std::string &value) {
    if (format == "date-time") {
        static const std::regex re(
            R"(^\d{4}-\d{2}-\d{2}T\d{2}:\d{2}:\d{2}(\.\d+)?(Z|[+-]\d{2}:\d{2})$)");
        if (!std::regex_match(value, re)) {
            throw std::invalid_argument("is not a valid RFC 3339 date-time");
        }
    } else if (format == "date") {
        static const std::regex re(R"(^\d{4}-\d{2}-\d{2}$)");
        if (!std::regex_match(value, re)) {
            throw std::invalid_argument("is not a valid date");
        }
    } else if (format == "uri") {
        static const std::regex re(R"(^[a-zA-Z][a-zA-Z0-9+.-]*:.+)");
        if (!std::regex_match(value, re)) {
            throw std::invalid_argument("is not a valid URI");
        }
    }
}

std::expected<void, std::vector<std::string>> validate_xml(const std::string &xml_content,
                                                           const std::string &xsd_path) {
    std::vector<std::string> errors;

    xmlLineNumbersDefault(1);

    xmlSchemaParserCtxtPtr ctxt = xmlSchemaNewParserCtxt(xsd_path.c_str());

    if (!ctxt)
        return std::unexpected(std::vector<std::string>{"Couldnt create schema parser"});

    xmlSchemaSetParserStructuredErrors(ctxt, collect_error, &errors);

    xmlSchemaPtr schema = xmlSchemaParse(ctxt);
    xmlSchemaFreeParserCtxt(ctxt);

    if (!schema) {
        if (errors.empty())
            errors.push_back("Failed to parse XSD schema: " + xsd_path);
        return std::unexpected(std::move(errors));
    }

    xmlDocPtr doc =
        xmlReadMemory(xml_content.data(), (int)xml_content.size(), "xml.xml", nullptr, 0);

    if (!doc) {
        xmlSchemaFree(schema);
        return std::unexpected(std::vector<std::string>{"bad XML"});
    }

    xmlSchemaValidCtxtPtr vctxt = xmlSchemaNewValidCtxt(schema);
    xmlSchemaSetValidStructuredErrors(vctxt, collect_error, &errors);

    int ret = xmlSchemaValidateDoc(vctxt, doc);

    xmlSchemaFreeValidCtxt(vctxt);
    xmlFreeDoc(doc);
    xmlSchemaFree(schema);

    if (ret == 0 && errors.empty())
        return {};

    if (errors.empty())
        errors.push_back("XML failed schema validation");

    return std::unexpected(std::move(errors));
}

std::expected<void, std::vector<std::string>> validate_json(const std::string &json_content,
                                                            const std::string &json_path) {

    std::vector<std::string> errors;

    std::ifstream file(json_path);
    if (!file) {
        errors.push_back("Failed to open json schema");
        return std::unexpected(std::move(errors));
    }

    json schema_json;
    try {
        file >> schema_json;
    } catch (const std::exception &e) {
        errors.push_back("Json schema parsing error ");
        return std::unexpected(std::move(errors));
    }

    nlohmann::json_schema::json_validator validator(nullptr, format_check);
    try {
        validator.set_root_schema(schema_json);
    } catch (const std::exception &e) {
        errors.push_back("Json schema loading error");
        return std::unexpected(std::move(errors));
    }

    json instance;
    try {
        instance = json::parse(json_content);
    } catch (const std::exception &e) {
        errors.push_back(std::string("JSON parsing error: ") + e.what());
        return std::unexpected(std::move(errors));
    }

    ErrorHandler handler;
    validator.validate(instance, handler);
    errors = std::move(handler.errors);
    if (errors.empty())
        return {};
    return std::unexpected(std::move(errors));
}
