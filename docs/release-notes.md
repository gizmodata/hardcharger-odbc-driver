**Hardcharger™ ODBC Driver 1.2.0**, on [Chargewire ODBC](https://github.com/gizmodata/chargewire-odbc) 2.1.0, for Hardcharger 2.7.0.

- A session Hardcharger ended because its transaction sat idle past
  `idle_in_transaction_session_timeout` fails the statement with SQLSTATE `40000` and a
  message naming the reason (`idle_in_transaction`); the driver never runs the statement
  again on a new session.
- A connection whose session the server ended or no longer has reports
  `SQL_ATTR_CONNECTION_DEAD`, so connection pools discard it; a closed or killed session
  reports `08S01`.
- Nested schemas (`a.b`) pass through the catalog functions by their path.

See CHANGELOG.md for details and README.md for installation and connection strings. GizmoData
EV-signed Windows x64 DLL and MSI; macOS 15+ ARM64 and Linux x64/ARM64 libraries (glibc 2.39
or later). Built and tested against released Hardcharger 2.7.0, using Chargewire 2.1.0.
