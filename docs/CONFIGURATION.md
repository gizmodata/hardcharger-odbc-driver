# Configuration

See the [README connection options](../README.md#connection-options) for the complete
supported list, DSN setup, TLS and authentication examples. The default port is 31337;
TLS is enabled by default. ODBC connection-string and DSN property names are case
insensitive. This driver speaks Warp SQL, so a GizmoSQL Flight SQL endpoint is not a
compatible server. No Flight SQL or gRPC connection options are required.

Use `uri=hardcharger://host[:port]` (port 31337, verified TLS by default). Add `?tls=false` for HTTP. Generic warp://, https:// and http:// URIs pass through with their 443/80 defaults. Explicit ODBC properties override URI values.
