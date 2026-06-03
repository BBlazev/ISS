#include <asio/placeholders.hpp>

#include "import_route.hpp"

#include "db.hpp"
#include "validate.hpp"

#include "crow/app.h"
#include "crow/http_response.h"
#include "crow/multipart.h"

static crow::response
error_response(int code, const std::vector<std::string> &xml_errs,
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
  CROW_ROUTE(app, "/api/import")
      .methods("POST"_method)([&db](const crow::request &req) {
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

        auto xml_result =
            validate_xml(xml_content, std::string(SCHEMAS_DIR) + "/file.xsd");
        if (!xml_result)
          return error_response(400, xml_result.error(), {},
                                "XML validation failed");

        json output;
        output["status"] = "ok";
        output["id"] = 1;
        output["entity"] = "entity_1";

        crow::response r(201, output.dump());
        r.add_header("Content-Type", "application/json");
        return r;
      });
}
