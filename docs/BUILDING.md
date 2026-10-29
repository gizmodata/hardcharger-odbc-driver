# Building

Follow the commands in README.md. Clone with --recurse-submodules: all ODBC code and
common tests come from third_party/chargewire-odbc, pinned to an exact release commit.
The root CMake file supplies only the product profile, library name and extra tests.

See [the shared build instructions](../third_party/chargewire-odbc/docs/BUILDING.md)
for dependencies. Output is build/Release/lib/libhardcharger-odbc.{so,dylib} or
build/Release/lib/hardcharger-odbc.dll. For local live tests on another port, pass
--port 31409 to the shared scripts/test-live.py; an occupied port is refused.

To also run the wrapper suite's old-server cases, start a Hardcharger released before the
protocol's rename over plain HTTP (for example `docker run -p 127.0.0.1:31410:31337 -e
HARDCHARGER_ADMIN_PASSWORD=... ghcr.io/gizmodata/hardcharger:2.2.0`) and set
`HARDCHARGER_ODBC_TEST_OLD_SERVER=127.0.0.1:31410` and
`HARDCHARGER_ODBC_TEST_OLD_SERVER_PASSWORD` (`HARDCHARGER_ODBC_TEST_OLD_SERVER_USER`
defaults to `hardcharger`) for scripts/test-live.py. Once set, those cases fail rather
than skip if the driver cannot work with that server.
