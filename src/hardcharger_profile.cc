// Copyright (C) 2026 GizmoData LLC. Apache-2.0.
#include "driver_profile.h"
#include <boost/url.hpp>
#include <boost/algorithm/string.hpp>

namespace driver::chargewire_odbc {
namespace {
// gizmodata.com or a name under it, in any case, with an optional trailing dot. The same rule
// as the Hardcharger CLI's IsGizmoDataCloudHost: GizmoData Cloud clusters answer on 443.
bool IsGizmoDataCloudHost(std::string host) {
  boost::algorithm::to_lower(host);
  if (!host.empty() && host.back() == '.') host.pop_back();
  constexpr std::string_view kDomain = "gizmodata.com";
  if (host == kDomain) return true;
  return host.size() > kDomain.size() + 1 && boost::algorithm::ends_with(host, kDomain) &&
         host[host.size() - kDomain.size() - 1] == '.';
}
int HardchargerDefaultPort(const std::string& host) { return IsGizmoDataCloudHost(host) ? 443 : 31337; }
}
const DriverProfile& GetDriverProfile() {
  static const DriverProfile profile{"Hardcharger ODBC Driver", "Hardcharger",
      "hardcharger-odbc/" CHARGEWIRE_ODBC_VERSION, "hardcharger-odbc.ini", 31337, &HardchargerDefaultPort};
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
  uri.set_scheme("chargewire");
  if (!uri.has_port()) uri.set_port(std::to_string(HardchargerDefaultPort(std::string(uri.host()))));
  return uri.buffer();
}
void InitializeProductSession(ChargewireClient& client, const ConnectionProperties& properties,
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
