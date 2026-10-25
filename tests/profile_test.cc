// Copyright (C) 2026 GizmoData LLC. Apache-2.0.
#include "driver_profile.h"
#include "warpsql_connection.h"
#include <gtest/gtest.h>
namespace driver::warpsql_odbc {
TEST(HardchargerProfile, BrandingAndDefaults) {
  EXPECT_STREQ(GetDriverProfile().name, "Hardcharger ODBC Driver");
  auto o = WarpSqlConnection::BuildConnectOptions({});
  EXPECT_EQ(o.port, 31337); EXPECT_TRUE(o.tls.enabled);
  EXPECT_EQ(o.user_agent.find("hardcharger-odbc/"), 0);
}
TEST(HardchargerProfile, UriDefaultsTo31337AndTls) {
  auto o = WarpSqlConnection::BuildConnectOptions({{"uri", "hardcharger://db.example.com"}});
  EXPECT_EQ(o.host, "db.example.com"); EXPECT_EQ(o.port, 31337); EXPECT_TRUE(o.tls.enabled);
}
TEST(HardchargerProfile, PlainUriKeepsHardchargerPort) {
  auto o = WarpSqlConnection::BuildConnectOptions({{"uri", "hardcharger://localhost?tls=false"}});
  EXPECT_EQ(o.port, 31337); EXPECT_FALSE(o.tls.enabled);
}
TEST(HardchargerProfile, ExplicitPortPathAndIpv6) {
  auto o = WarpSqlConnection::BuildConnectOptions({{"uri", "hardcharger://[::1]:31409/custom/warp?tls=false"}});
  EXPECT_EQ(o.host, "::1"); EXPECT_EQ(o.port, 31409); EXPECT_EQ(o.base_path, "/custom/warp");
}
TEST(HardchargerProfile, GenericUrisRetainGenericDefaults) {
  EXPECT_EQ(WarpSqlConnection::BuildConnectOptions({{"uri", "warp://db.example.com"}}).port, 443);
  EXPECT_EQ(WarpSqlConnection::BuildConnectOptions({{"uri", "http://db.example.com"}}).port, 80);
}
TEST(HardchargerProfile, RefusesAlternativeSchemesAndInvalidPorts) {
  for (auto uri : {"hardcharger+http://host", "hardcharger://host:0", "hardcharger://host:", "hardcharger://user:secret@host"})
    EXPECT_THROW(WarpSqlConnection::BuildConnectOptions({{"uri", uri}}), odbcabstraction::DriverException);
}
}
