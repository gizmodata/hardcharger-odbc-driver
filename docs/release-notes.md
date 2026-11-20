**Hardcharger™ ODBC Driver 1.6.1**, built on Chargewire ODBC 2.5.1 and Chargewire C++ SDK 2.5.0.

- **Power BI and Excel's Navigator lists the tables in a catalog** ([#6](https://github.com/gizmodata/hardcharger-odbc-driver/issues/6)). Power Query discovers tables with `SQLTables("%", NULL, NULL, "TABLE,VIEW")`. Under ODBC 3 that catalog is a search pattern, but the driver sent `%` to the server as a catalog name, so it returned no tables. `%` now lists every catalog's tables.
- Catalog patterns follow the ODBC rules: `%` matches any run of characters, `_` matches one character, and `\` escapes either. Matching is case-sensitive. A name without wildcards, or with escaped ones (`my\_lake`), is looked up exactly.

Release gates run the shared core and Hardcharger wrapper suites on all four platforms over HTTP and verified TLS. Windows DLLs and MSIs are EV-signed, signature-verified and installed-driver tested before publication.
