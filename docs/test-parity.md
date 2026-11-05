# Regression coverage

This wrapper pins the generic Chargewire ODBC driver and compiles its entire implementation
and 212 unit cases, plus six Hardcharger URI/branding cases (218 total). The GizmoSQL
baseline has 174. See the [shared coverage report](../third_party/chargewire-odbc/docs/test-parity.md).

The same inherited 15 C metadata/key/parameter, 9 C transaction and 6 pyodbc cases run
through this wrapper, together with the shared 25 Chargewire live cases, its
parameter-array, `SQLExecDirect` and no-driver-manager C suites, its compatibility suite, and its
session-loss suites (3 C and 5 pyodbc cases, against a server restarted with a 1 s
idle-in-transaction timeout). Nine wrapper live cases cover hardcharger://, query/session
instrumentation tags on the actual user SQL, apostrophes in tags, invalid tags,
alternative URI scheme errors, and `ResumeTimeout` through a stand-in for a resuming server
(waits then connects, `0` and an expired timeout give `HYT01`, invalid values `HY024`). Five more run against a Hardcharger released before the
protocol's rename when `HARDCHARGER_ODBC_TEST_OLD_SERVER` names one (docs/BUILDING.md):
connecting every way, discovery, prepared statements, metadata, session tags, a SQLSTATE
and a rollback.

All four platforms run the suites against released Hardcharger 2.7.0. Python suites run
over HTTP and verified TLS. Windows additionally installs the signed MSI and repeats
Python suites using the installed signed DLL. Missing binaries, failures or skips in
the C fixtures, invalid signatures and installer failures block publication.
