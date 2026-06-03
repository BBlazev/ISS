#include <cstdarg>
#include <cstdio>
#include <expected>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/valid.h>
#include <libxml/xmlschemas.h>

#include <vector>

#include "validate.hpp"

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

std::expected<void, std::vector<std::string>>
validate_xml(const std::string &xml_content, const std::string &xsd_path) {
  std::vector<std::string> errors;

  xmlLineNumbersDefault(1);

  xmlSchemaParserCtxtPtr ctxt = xmlSchemaNewParserCtxt(xsd_path.c_str());

  if (!ctxt)
    return std::unexpected(
        std::vector<std::string>{"Couldnt create schema parser"});

  xmlSchemaSetParserStructuredErrors(ctxt, collect_error, &errors);

  xmlSchemaPtr schema = xmlSchemaParse(ctxt);
  xmlSchemaFreeParserCtxt(ctxt);

  if (!schema) {
    if (errors.empty())
      errors.push_back("Failed to parse XSD schema: " + xsd_path);
    return std::unexpected(std::move(errors));
  }

  xmlDocPtr doc = xmlReadMemory(xml_content.data(), (int)xml_content.size(),
                                "xml.xml", nullptr, 0);

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
