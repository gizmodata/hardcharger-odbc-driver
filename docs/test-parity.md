# Regression coverage

This wrapper pins published Chargewire ODBC 2.5.0 and compiles its entire implementation
and shared unit suite, plus Hardcharger URI/branding cases. See the
[shared coverage report](../third_party/chargewire-odbc/docs/test-parity.md).

The same inherited 15 C metadata/key/parameter, 9 C transaction and 6 pyodbc cases run
through this wrapper, together with the shared 25 Chargewire live cases, its
parameter-array, `SQLExecDirect` and no-driver-manager C suites, its compatibility suite, and its
session-loss suites (3 C and 5 pyodbc cases, against a server restarted with a 1 s
idle-in-transaction timeout). Wrapper live cases cover hardcharger://, query/session
instrumentation tags on the actual user SQL, apostrophes in tags, invalid tags,
alternative URI scheme errors, and `ResumeTimeout` through a stand-in for a resuming server
(waits then connects, `0` and an expired timeout give `HYT01`, invalid values `HY024`). Five more run against a Hardcharger released before the
protocol's rename when `HARDCHARGER_ODBC_TEST_OLD_SERVER` names one (docs/BUILDING.md):
connecting every way, discovery, prepared statements, metadata, session tags, a SQLSTATE
and a rollback. Read-only coverage verifies server confirmation, write refusal and
Hardcharger session tags on a session that starts read-only.

All four platforms run the suites against Hardcharger 2.13.0, from its published release
or verified artifacts of a successful main workflow while the release is pending. Python suites run
over HTTP and verified TLS. Windows additionally installs the signed MSI and repeats
Python suites using the installed signed DLL. Missing binaries, failures or skips in
the C fixtures, invalid signatures and installer failures block publication.
