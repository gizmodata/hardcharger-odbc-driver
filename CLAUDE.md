# Hardcharger ODBC wrapper

Keep this repository thin. ODBC implementation, Warp SQL transport, common suites and
release automation belong in the pinned third_party/warp-sql-odbc repository. This
wrapper owns Hardcharger URI translation, branding, session initialization and tests.
Always run the shared suites and wrapper suites. Do not publish Windows DLLs/MSIs
without the existing GizmoData EV signature, timestamp and installed-driver live tests.
Publish the generic core release before the corresponding product wrapper release.
