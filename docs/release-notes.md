**Hardcharger™ ODBC Driver 1.5.0**, on Chargewire ODBC 2.4.0 and Chargewire C++ 2.4.1.

- **Connecting to a stopped GizmoData Cloud cluster doesn't start it.** With auto-resume on, the driver connects
  without signing in; connection checks, `SQLGetInfo`, autocommit changes and disconnects send nothing, and the
  first statement signs in (starting the cluster), with `queryTag`/`sessionTag` applied just before it. BI tools
  and connection pools no longer wake a stopped cluster; a query still does.
- A connection whose token was refused for good reports itself dead, so pools replace it.
- `SQLCancel` and closing a partly read result never sign in.

The release gates test against released Hardcharger 2.7.0 on all four platforms over HTTP and verified TLS. Windows DLL and MSI are EV-signed and verified before publication.
