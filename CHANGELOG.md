# Changelog

## [Unreleased]

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
