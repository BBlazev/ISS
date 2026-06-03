#pragma once
#include <crow.h>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>

#include "db.hpp"
#include "file_entity.hpp"

#include "crow/app.h"

using nlohmann::json;

void register_import_route(crow::SimpleApp &app, Database &db);
