// Copyright (C) 2026 GizmoData LLC. Apache-2.0.
#include "driver_profile.h"
#include <boost/url.hpp>
#include <boost/algorithm/string.hpp>

namespace driver::warpsql_odbc {
const DriverProfile& GetDriverProfile() {
  static const DriverProfile profile{"Hardcharger ODBC Driver", "Hardcharger",
      "hardcharger-odbc/" WARP_ODBC_VERSION, "hardcharger-odbc.ini", 31337};
  return profile;
}
std::string TranslateProductUri(const std::string& raw) {
  auto parsed = boost::urls::parse_uri(raw);
  if (!parsed) throw odbcabstraction::DriverException("Invalid connection URI", "08001");
  auto scheme = boost::algorithm::to_lower_copy(std::string(parsed->scheme()));
  if (scheme.rfind("hardcharger+", 0) == 0)
    throw odbcabstraction::DriverException("Use hardcharger://host?tls=false for plain HTTP", "08001");
  if (scheme != "hardcharger") return raw;
  boost::urls::url uri(*parsed);
  uri.set_scheme("warp");
  if (!uri.has_port()) uri.set_port("31337");
  return uri.buffer();
}
void InitializeProductSession(WarpSqlClient& client, const ConnectionProperties& properties,
                              const CallOptions& options) {
  for (const auto& [property, setting] : {std::pair{"queryTag", "hardcharger.query_tag"},
                                       std::pair{"sessionTag", "hardcharger.session_tag"}}) {
    auto it = properties.find(property);
    if (it == properties.end() || it->second.empty()) continue;
    std::string literal = "'";
    for (char c : it->second) { literal += c; if (c == '\'') literal += c; }
    literal += "'";
    auto status = client.ExecuteUpdate(options, std::string("SET SESSION ") + setting + " = " + literal).status();
    if (!status.ok()) throw odbcabstraction::DriverException(status.ToString(), "HY000");
  }
}
}
