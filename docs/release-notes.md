**Hardcharger™ ODBC Driver 1.6.0**, built on Chargewire ODBC 2.5.0 and Chargewire C++ SDK 2.5.0.

- `ReadOnly=true` opens a server-confirmed read-only session. `SQL_ATTR_ACCESS_MODE=SQL_MODE_READ_ONLY` can also restrict a connection; the restriction is one-way.
- Unsupported or ignored read-only requests fail instead of silently opening a writable connection. Read-only mode requires Hardcharger 2.13.0 or newer; ordinary connections retain existing older-server compatibility.
- Hardcharger session settings and tags are applied correctly when a session starts read-only. Read-write connections remain available by default.

Release gates run the shared core and Hardcharger wrapper suites on all four platforms over HTTP and verified TLS. They use Hardcharger 2.13.0 from its published release, or verified artifacts from a successful main workflow while that release is pending. Windows DLLs and MSIs are EV-signed, signature-verified and installed-driver tested before publication.
