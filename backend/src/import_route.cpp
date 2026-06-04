#include <asio/placeholders.hpp>

#include "import_route.hpp"

#include "db.hpp"
#include "file_entity.hpp"
#include "validate.hpp"

#include "crow/app.h"
#include "crow/http_response.h"
#include "crow/multipart.h"

static crow::response error_response(int code, const std::vector<std::string> &xml_errs,
                                     const std::vector<std::string> &json_errs,
                                     const std::string &reason = "validation_failed") {
    json output;
    output["status"] = "error";
    output["reason"] = reason;
    output["xml_errors"] = xml_errs;
    output["json_errors"] = json_errs;

    crow::response r(code, output.dump(2));
    r.add_header("Content-Type", "application/json");
    return r;
}

void register_import_route(crow::SimpleApp &app, Database &db) {
    CROW_ROUTE(app, "/api/import").methods("POST"_method)([&db](const crow::request &req) {
        crow::multipart::message msg(req);

        if (msg.parts.empty() || msg.parts.size() < 2)
            return error_response(400, {}, {}, "expected xml and json files");

        std::string xml_content, json_content;

        for (const auto &part : msg.part_map) {
            const std::string &name = part.first;
            const std::string &body = part.second.body;

            if (name == "xml")
                xml_content = body;
            if (name == "json")
                json_content = body;
        }

        if (xml_content.empty() || json_content.empty())
            return error_response(400, {}, {}, "files are empty");

        auto xml_result = validate_xml(xml_content, std::string(SCHEMAS_DIR) + "/file.xsd");
        if (!xml_result)
            return error_response(400, xml_result.error(), {}, "XML validation failed");

        auto json_result =
            validate_json(json_content, std::string(SCHEMAS_DIR) + "/file.schema.json");
        if (!json_result)
            return error_response(400, {}, json_result.error(), "JSON validation failed");

        json json_doc;
        json_doc = json::parse(json_content);

        FileEntity entity;
        try {
            entity = FileEntity::from_json(json_doc);
            db.upsert_file(entity);
        } catch (const std::exception &e) {
            return error_response(400, {}, {}, "Entity mapping or upserting failed");
        }

        json output;
        output["status"] = "ok";
        output["id"] = entity.id;
        output["entity"] = entity.to_json();

        crow::response r(201, output.dump());
        r.add_header("Content-Type", "application/json");
        return r;
    });
}
