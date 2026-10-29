The first **Hardcharger™ ODBC driver**, a thin wrapper of the generic [Chargewire ODBC driver](https://github.com/gizmodata/chargewire-odbc). Adds **hardcharger://** URIs (port 31337) and Hardcharger branding/settings, using **Chargewire over HTTP/HTTPS** and
Apache Arrow IPC streaming. Based on the GizmoSQL driver's ODBC compatibility layer,
with its conversion, metadata, parameter and transaction suites retained for Hardcharger.

- Windows x64: **GizmoData EV-signed DLL and MSI**, verified Authenticode signatures and timestamp.
- macOS Apple Silicon and Linux x64/ARM64 native libraries.
- Prepared parameters; catalog, schema, table, column, type, primary-key and foreign-key metadata.
- Commit/rollback, cancellation, statement timeouts, password, token, browser and certificate sign-in.
- TLS enabled by default; use `useEncryption=false` explicitly for a plaintext development server.

See README.md for installation and connection strings. Built and tested against released
Hardcharger 2.0.0, using Chargewire 1.0.1. Linux libraries are built on Ubuntu 24.04.
