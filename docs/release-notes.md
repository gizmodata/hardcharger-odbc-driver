**Hardcharger™ ODBC Driver 1.3.0**, on [Chargewire ODBC](https://github.com/gizmodata/chargewire-odbc) 2.2.0, for Hardcharger 2.7.0.

- **Connecting to a Hardcharger server that is resuming**, such as a GizmoData Cloud cluster
  stopped while idle: the driver waits while it starts and sends the same request again, the
  sign-in included, for up to the new `ResumeTimeout` (seconds, default 600, `0` turns it off),
  then fails with `HYT01`.
- `SQLExecDirect` runs `SET hardcharger.*` and `KILL SESSION`, which can't be prepared; with
  bound parameters it now sends them.
- A statement without rows no longer reports the previous statement's result columns or reads
  its result; `SQLFetch`/`SQLGetData`/`SQLCloseCursor` without a result set report `24000`.

See CHANGELOG.md for details and README.md for installation and connection strings. GizmoData
EV-signed Windows x64 DLL and MSI; macOS 15+ ARM64 and Linux x64/ARM64 libraries (glibc 2.39
or later). Built and tested against released Hardcharger 2.7.0, using Chargewire 2.2.0.
