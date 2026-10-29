# Building

Follow the commands in README.md. Clone with --recurse-submodules: all ODBC code and
common tests come from third_party/chargewire-odbc, pinned to an exact release commit.
The root CMake file supplies only the product profile, library name and extra tests.

See [the shared build instructions](../third_party/chargewire-odbc/docs/BUILDING.md)
for dependencies. Output is build/Release/lib/libhardcharger-odbc.{so,dylib} or
build/Release/lib/hardcharger-odbc.dll. For local live tests on another port, pass
--port 31409 to the shared scripts/test-live.py; an occupied port is refused.
