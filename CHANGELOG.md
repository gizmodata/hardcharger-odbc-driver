# Changelog

## [Unreleased]

## [1.5.0] - 2026-11-07

On Chargewire ODBC 2.4.0 (Chargewire C++ 2.4.1).

### Added

- **Connecting to a stopped GizmoData Cloud cluster doesn't start it** (Chargewire §11.2, lazy connect). When the
  cluster is stopped with auto-resume on, `SQLDriverConnect` returns without signing in; connection checks
  (`SQL_ATTR_CONNECTION_DEAD`, which driver-manager pools use), `SQLGetInfo`, autocommit changes, `SQLEndTran` and
  `SQLDisconnect` send nothing. The first statement or catalog function signs in, which starts the cluster, and the
  `queryTag`/`sessionTag` settings go on the session just before it. BI tools and pools that open and check
  connections no longer wake a stopped cluster; a query still does.

### Fixed

- A connection whose token was refused for good (`token_revoked`, `credentials_changed`, `principal_dropped`)
  reports itself dead, so a pool replaces it instead of reusing it.
- `SQLCancel` on a running statement and closing a partly read result never sign in (2.4.0 covered disconnects).

## [1.4.3] - 2026-11-06

### Fixed

- **Disconnecting never signs in.** On Chargewire ODBC 2.3.3: an `SQLDisconnect` (or freeing a statement) late
  in a token's life used to sign in again just to close, which started a stopped GizmoData Cloud cluster with
  auto-resume for nobody, or undid a person's Stop, and could wait for the resume. Disconnects now use the token the
  connection already has and wait at most a few seconds; requests that do work renew the token as before.

## [1.4.2] - 2026-11-06

### Changed

- Build on Chargewire ODBC 2.3.2 and Chargewire C++ 2.3.0 for reusable HTTP connections and efficient Arrow stream reads. Hardcharger URI defaults, Cloud port selection and the ODBC API are unchanged.

## [1.4.1] - 2026-11-05

### Fixed

- **A fully qualified host name connects over TLS** (Chargewire ODBC 2.3.1, on the Chargewire
  C++ client 2.2.2). A host written with a final dot, such as `my-cluster.gizmodata.com.` in a
  `hardcharger://` URI or the `host` property, failed every connection with "the TLS handshake
  ... failed: certificate verify failed", because the dotted name was sent as the TLS server
  name and checked against the certificate. The name without the final dot is used for both.
  1.4.0 already chose port 443 for such a GizmoData Cloud host.

### Added

- `scripts/smoke-cloud-host.py` checks a driver against the public `gizmodata.com` host: port
  443 for `gizmodata.com` and `gizmodata.com.` (URI and `host` property), verified TLS, and an
  explicit port winning. The new *Smoke-test a published release* workflow runs it on the
  published Linux x64 and macOS ARM64 packages after each release (or by hand for a tag).

## [1.4.0] - 2026-11-05

### Changed

- **GizmoData Cloud hosts default to port 443.** A host without a port whose name is
  `gizmodata.com` or ends in `.gizmodata.com` (any case, a trailing dot allowed; matched on the
  domain boundary, so `evilgizmodata.com` and `gizmodata.com.example.org` don't count) connects
  on 443, where GizmoData Cloud clusters answer. This applies to a `hardcharger://` URI and to
  the `host` property of a DSN or connection string. An explicit port always wins, and every
  other host keeps 31337. It is the same rule as the Hardcharger CLI, web UI and the other
  Hardcharger drivers.
- Built on Chargewire ODBC 2.3.0, whose driver profile can choose the default port per host.

## [1.3.0] - 2026-11-05

### Added

- **Connecting to a Hardcharger server that is resuming** (Chargewire ODBC 2.2.0). A server
  that was stopped and is starting again, such as a GizmoData Cloud cluster stopped while idle,
  answers `server_resuming` until it serves; the driver waits the time the server asks for and
  sends the same request again, the sign-in included. The new `ResumeTimeout` property (DSN or
  connection string, seconds, default `600`, `0` turns the retry off) bounds the wait; after it
  the connection fails with SQLSTATE `HYT01`. `SQL_ATTR_LOGIN_TIMEOUT` bounds the TCP connection
  and TLS handshake only, so a sign-in a resuming server holds is still answered.

### Fixed

- `SQLExecDirect` runs Hardcharger's `SET hardcharger.*` and `KILL SESSION`, which can't be
  prepared (chargewire-odbc#3): without bound parameters it runs the statement directly
  instead of preparing it first. With bound parameters it now sends them (it ignored them).
- A statement without rows no longer reports the previous statement's result columns or
  reads its result, and `SQLFetch`/`SQLGetData`/`SQLCloseCursor` without a result set report
  `24000` (a driver loaded without a driver manager crashed; `SQLCloseCursor` said `28000`).

### Changed

- Built on Chargewire ODBC 2.2.0, and tested against Hardcharger 2.7.0. Wrapper tests connect
  through a resuming stand-in server (`ResumeTimeout` waits, `0` and an expired timeout give
  `HYT01`, invalid values `HY024`).

## [1.2.0] - 2026-11-04

### Changed

- Built on Chargewire ODBC 2.1.0 (Chargewire 2.1.0), and tested against Hardcharger 2.7.0.

### Fixed

- A session Hardcharger 2.7.0 ended because its transaction sat idle past
  `idle_in_transaction_session_timeout` (`session_ended`, reason `idle_in_transaction`) now
  fails the statement with SQLSTATE `40000` (transaction rollback) and a message that names the
  reason and says the transaction was rolled back; it reported `28000` before. The driver
  never replaces a lost session or runs a statement again, so nothing commits on its own
  after the transaction was rolled back.
- A connection whose session the server no longer has (closed, evicted after idling, the
  server restarted) or ended now reports `SQL_ATTR_CONNECTION_DEAD`, so connection pools
  discard it; closed and killed sessions report `08S01` (they reported `HY000` and `28000`).
  Rolling back the transaction the server already rolled back succeeds.

## [1.1.0] - 2026-10-29

### Changed

- Built on Chargewire ODBC 2.0.0.
- The driver now uses the Chargewire protocol and the Chargewire ODBC driver and client
  library. Its registered name, library name, `hardcharger://` URIs and DSNs are
  unchanged, and connection URIs and mount paths written for 1.0 keep working. It still
  works with Hardcharger servers released before the rename: on its first request it
  finds out which spelling of the protocol the server speaks and uses that one.
- A `chargewire://` URI is now the generic alternative to `hardcharger://`, and the
  default `basePath` is `/chargewire`.

### Fixed

- pyodbc `fast_executemany` no longer crashes the process, and a column-wise parameter
  array inserts every row instead of only the first (the shared driver's parameter-array
  fix).

## [1.0.1] - 2026-10-25

The first published release. The v1.0.0 candidate was stopped by its signing gate before publication.

- Initialize the pinned generic core in the Windows signing job so the shared scripts and live suites are available.
- Pin signing automation separately from the released core implementation.
- Publish the verified Homebrew formula and document macOS 15+ and Linux glibc 2.39+ requirements.

- Thin wrapper around the released generic Chargewire ODBC driver; add hardcharger:// URIs, port 31337, branding and query/session settings.

### Added
- Initial Hardcharger ODBC driver with native Chargewire over HTTP/HTTPS and Arrow IPC streaming.
- Windows x64 signed DLL and MSI, macOS arm64 and Linux amd64/arm64 libraries.
- Password, bearer token, browser OAuth and client-certificate authentication.
- Prepared parameters, catalog/schema/table/column/type metadata, primary and foreign keys,
  transactional commit/rollback, cancellation and server-side statement timeouts.
- The GizmoSQL ODBC regression suites adapted to Hardcharger, plus native Chargewire regressions.
- Hardcharger branding matching the Hardcharger ADBC repository.

### Fixed
- Wide column and descriptor names now report character counts instead of byte counts,
  preventing garbage names and UTF-16 decoding errors in pyodbc.
- Bound NULL and empty parameters remain bound even when their data pointer is null.
- Parameters in untyped expressions such as `SELECT ?` now derive their Arrow types from ODBC
  bindings; binary parameters support embedded NUL bytes in both typed and untyped statements.
- Cancellation no longer waits behind the statement mutex held by a blocked query/fetch, and
  it leaves the executing thread in control of result buffers and diagnostics.
- Cached prepared statements now start a new transaction after commit or rollback when
  autocommit is off. Previously, reusing a pyodbc prepared handle could commit its next
  write immediately because only preparing a new handle re-established manual transactions.
- Legacy diagnostic iteration now terminates after the last record, preventing driver managers
  from looping when a connection fails.
- ODBC trace logs no longer write connection strings containing passwords or bearer tokens.
- Connection strings now handle multiple braced values, embedded semicolons and escaped closing braces.
- Query/session tags are validated as JSON objects and recorded in Hardcharger instrumentation.
- UTF-16 conversion now handles supplementary characters such as emoji in parameters and results.

The ODBC compatibility layer is derived from GizmoSQL ODBC 1.1.3. Its history and attribution
are retained in Git and NOTICE; the Hardcharger first published release is 1.0.1.
