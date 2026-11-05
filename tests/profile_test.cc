// Copyright (C) 2026 GizmoData LLC. Apache-2.0.
#include "driver_profile.h"
#include "chargewire_connection.h"
#include <gtest/gtest.h>
namespace driver::chargewire_odbc {
TEST(HardchargerProfile, BrandingAndDefaults) {
  EXPECT_STREQ(GetDriverProfile().name, "Hardcharger ODBC Driver");
  auto o = ChargewireConnection::BuildConnectOptions({});
  EXPECT_EQ(o.port, 31337); EXPECT_TRUE(o.tls.enabled);
  EXPECT_EQ(o.user_agent.find("hardcharger-odbc/"), 0);
}
TEST(HardchargerProfile, UriDefaultsTo31337AndTls) {
  auto o = ChargewireConnection::BuildConnectOptions({{"uri", "hardcharger://db.example.com"}});
  EXPECT_EQ(o.host, "db.example.com"); EXPECT_EQ(o.port, 31337); EXPECT_TRUE(o.tls.enabled);
  EXPECT_EQ(o.base_path, "/chargewire");
}
TEST(HardchargerProfile, PlainUriKeepsHardchargerPort) {
  auto o = ChargewireConnection::BuildConnectOptions({{"uri", "hardcharger://localhost?tls=false"}});
  EXPECT_EQ(o.port, 31337); EXPECT_FALSE(o.tls.enabled);
}
TEST(HardchargerProfile, ExplicitPortPathAndIpv6) {
  auto o = ChargewireConnection::BuildConnectOptions({{"uri", "hardcharger://[::1]:31409/custom/chargewire?tls=false"}});
  EXPECT_EQ(o.host, "::1"); EXPECT_EQ(o.port, 31409); EXPECT_EQ(o.base_path, "/custom/chargewire");
}
TEST(HardchargerProfile, GenericUrisRetainGenericDefaults) {
  EXPECT_EQ(ChargewireConnection::BuildConnectOptions({{"uri", "chargewire://db.example.com"}}).port, 443);
  EXPECT_EQ(ChargewireConnection::BuildConnectOptions({{"uri", "http://db.example.com"}}).port, 80);
}
TEST(HardchargerProfile, GizmoDataCloudUriDefaultsTo443) {
  for (auto uri : {"hardcharger://gizmodata.com", "hardcharger://GizmoData.COM",
                   "hardcharger://my-cluster.us-east-1.aws.gizmodata.com",
                   "hardcharger://MY-CLUSTER.GizmoData.Com.", "hardcharger://gizmodata.com.",
                   "hardcharger://c1.gizmodata.com/custom/chargewire",
                   "hardcharger://c1.gizmodata.com?tls=false"}) {
    SCOPED_TRACE(uri);
    EXPECT_EQ(ChargewireConnection::BuildConnectOptions({{"uri", uri}}).port, 443);
  }
}
TEST(HardchargerProfile, GizmoDataCloudUriKeepsExplicitPort) {
  for (auto [uri, port] : {std::pair{"hardcharger://c1.gizmodata.com:31337", 31337},
                           std::pair{"hardcharger://gizmodata.com:31337", 31337},
                           std::pair{"hardcharger://c1.gizmodata.com:8443", 8443}}) {
    SCOPED_TRACE(uri);
    EXPECT_EQ(ChargewireConnection::BuildConnectOptions({{"uri", uri}}).port, port);
  }
  // An explicit port property overrides the URI's default, for a cloud host too.
  EXPECT_EQ(ChargewireConnection::BuildConnectOptions(
      {{"uri", "hardcharger://c1.gizmodata.com"}, {"port", "31337"}}).port, 31337);
}
TEST(HardchargerProfile, LookAlikeUriHostsKeep31337) {
  for (auto uri : {"hardcharger://evilgizmodata.com", "hardcharger://gizmodata.com.example.org",
                   "hardcharger://c1.gizmodata.com.example.org", "hardcharger://gizmodata.co",
                   "hardcharger://gizmodata.com-evil.net", "hardcharger://xgizmodata.com.",
                   "hardcharger://localhost", "hardcharger://db.example.com"}) {
    SCOPED_TRACE(uri);
    EXPECT_EQ(ChargewireConnection::BuildConnectOptions({{"uri", uri}}).port, 31337);
  }
}
// A DSN or connection string's host without a port: the same rule as the URI.
TEST(HardchargerProfile, GizmoDataCloudHostPropertyDefaultsTo443) {
  for (auto host : {"c1.gizmodata.com", "gizmodata.com", "GizmoData.COM", "C1.GIZMODATA.COM.",
                    "my-cluster.us-east-1.aws.gizmodata.com"}) {
    SCOPED_TRACE(host);
    EXPECT_EQ(ChargewireConnection::BuildConnectOptions({{"host", host}}).port, 443);
    EXPECT_EQ(ChargewireConnection::BuildConnectOptions({{"host", host}, {"useEncryption", "false"}}).port, 443);
  }
}
TEST(HardchargerProfile, GizmoDataCloudHostPropertyKeepsExplicitPort) {
  EXPECT_EQ(ChargewireConnection::BuildConnectOptions({{"host", "c1.gizmodata.com"}, {"port", "31337"}}).port, 31337);
  EXPECT_EQ(ChargewireConnection::BuildConnectOptions({{"host", "gizmodata.com"}, {"port", "8443"}}).port, 8443);
}
TEST(HardchargerProfile, LookAlikeHostPropertiesKeep31337) {
  for (auto host : {"evilgizmodata.com", "gizmodata.com.example.org", "c1.gizmodata.com.example.org",
                    "gizmodata.co", "gizmodata.com-evil.net", "xgizmodata.com.", "localhost",
                    "db.example.com", "127.0.0.1"}) {
    SCOPED_TRACE(host);
    EXPECT_EQ(ChargewireConnection::BuildConnectOptions({{"host", host}}).port, 31337);
  }
  EXPECT_EQ(ChargewireConnection::BuildConnectOptions({}).port, 31337);  // host defaults to localhost
}
TEST(HardchargerProfile, RefusesAlternativeSchemesAndInvalidPorts) {
  for (auto uri : {"hardcharger+http://host", "hardcharger://host:0", "hardcharger://host:", "hardcharger://user:secret@host"})
    EXPECT_THROW(ChargewireConnection::BuildConnectOptions({{"uri", uri}}), odbcabstraction::DriverException);
}
}
