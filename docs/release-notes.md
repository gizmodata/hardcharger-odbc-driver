**Hardcharger™ ODBC Driver 1.4.3**, on Chargewire ODBC 2.3.3 and Chargewire C++ 2.3.0.

- **Disconnecting never signs in.** A disconnect late in a token's life no longer signs in again just to close, so
  closing a connection doesn't start a stopped GizmoData Cloud cluster with auto-resume (or undo a person's Stop),
  and it no longer waits for a resuming server. Requests that do work renew the token as before.

The release gates test against released Hardcharger 2.7.0 on all four platforms over HTTP and verified TLS. Windows DLL and MSI are EV-signed and verified before publication.
