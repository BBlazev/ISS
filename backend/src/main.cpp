#include <crow.h>
#include <print>

#include <algorithm>

#include "config.hpp"
#include "db.hpp"
#include "import_route.hpp"

#include "crow/app.h"

int main() {

  Config cnf = Config::load(".env");

  std::println("{}", cnf.directus_token);

  Database db("db.db");
  db.init_schema();

  crow::SimpleApp app;
  CROW_ROUTE(app, "/health")([] { return "ok\n"; });
  register_import_route(app, db);

  app.port(8080).multithreaded().run();

  return 0;
}
