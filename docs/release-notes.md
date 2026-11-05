**Hardcharger™ ODBC Driver 1.4.0**, on [Chargewire ODBC](https://github.com/gizmodata/chargewire-odbc) 2.3.0, for Hardcharger 2.7.0.

- **GizmoData Cloud hosts default to port 443.** A `hardcharger://` URI, or a DSN or connection
  string `host`, given without a port connects on 443 when the host is `gizmodata.com` or a name
  under it (any case, trailing dot allowed), where GizmoData Cloud clusters answer. An explicit
  port always wins; every other host keeps 31337.

See CHANGELOG.md for details and README.md for installation and connection strings. GizmoData
EV-signed Windows x64 DLL and MSI; macOS 15+ ARM64 and Linux x64/ARM64 libraries (glibc 2.39
or later). Built and tested against released Hardcharger 2.7.0, using Chargewire 2.2.0.
