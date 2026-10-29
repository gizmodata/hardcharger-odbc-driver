"""Thin-wrapper regressions, through the real ODBC driver manager and server."""
import json
import os
import sys
import time
import unittest

import requests
sys.path.insert(0, os.path.join(os.environ['ODBC_SUITE_ROOT'], 'integration_tests'))
from test_chargewire import AUTH, BASE, CA, DRIVER, HOST, PORT, TLS, connect, pyodbc

class HardchargerTests(unittest.TestCase):
    def test_hardcharger_uri(self):
        uri = f'hardcharger://{HOST}:{PORT}' + ('?tls=false' if TLS != 'true' else '')
        options = f'Driver={DRIVER};uri={uri};' + AUTH
        if CA:
            options += f'trustedCerts={CA};'
        with pyodbc.connect(options, autocommit=True, timeout=5) as c:
            self.assertEqual(c.getinfo(pyodbc.SQL_DRIVER_NAME), 'Hardcharger ODBC Driver')
            self.assertEqual(c.execute('SELECT CAST(? AS VARCHAR)', 'hardcharger-uri').fetchval(), 'hardcharger-uri')

    def test_query_and_session_tags(self):
        with connect('queryTag={{"app":"odbc-regression"}}};sessionTag={{"app":"odbc-regression"}}};') as c:
            self.assertEqual(c.execute('SELECT 55 AS tagged_odbc_statement').fetchval(), 55)
            session = c.execute('SELECT CAST(HARDCHARGER_CURRENT_SESSION() AS VARCHAR)').fetchval()
            deadline = time.monotonic() + 5
            while time.monotonic() < deadline:
                session_tag = c.execute("SELECT session_tag->>'app' FROM _hardcharger_instr.sessions WHERE session_id = ?", session).fetchval()
                query_tags = c.execute("SELECT count(*) FROM _hardcharger_instr.sql_statements WHERE query_tag->>'app' = 'odbc-regression' AND sql_text = 'SELECT 55 AS tagged_odbc_statement'").fetchval()
                if session_tag == 'odbc-regression' and query_tags > 0:
                    break
                time.sleep(.1)
            self.assertEqual(session_tag, 'odbc-regression')
            self.assertGreater(query_tags, 0)

    def test_apostrophe_in_tags(self):
        with connect('queryTag={{"app":"O\'Reilly"}}};sessionTag={{"app":"O\'Reilly"}}};') as c:
            for name in ('hardcharger.session_tag', 'hardcharger.query_tag'):
                value = c.execute("SELECT CAST(value AS VARCHAR) FROM hardcharger_settings() WHERE name = ?", name).fetchval()
                self.assertEqual(json.loads(value)['app'], "O'Reilly")

    def test_invalid_session_tag(self):
        with self.assertRaises(pyodbc.Error):
            connect('sessionTag=[];')

    def test_alternative_scheme_has_actionable_error(self):
        with self.assertRaises(pyodbc.Error) as error:
            pyodbc.connect(f'Driver={DRIVER};uri=hardcharger+http://{HOST}:{PORT};' + AUTH, timeout=3)
        self.assertIn('?tls=false', str(error.exception))


# A Hardcharger released before the protocol's rename, over plain HTTP, as host:port. When it
# is configured these cases must pass: the driver falls back to that server's protocol.
OLD_SERVER = os.environ.get('HARDCHARGER_ODBC_TEST_OLD_SERVER', '')


@unittest.skipUnless(OLD_SERVER, 'HARDCHARGER_ODBC_TEST_OLD_SERVER is not set')
class OldServerTests(unittest.TestCase):
    host, _, port = OLD_SERVER.rpartition(':')
    auth = (f"uid={os.environ.get('HARDCHARGER_ODBC_TEST_OLD_SERVER_USER', 'hardcharger')};"
            f"pwd={os.environ.get('HARDCHARGER_ODBC_TEST_OLD_SERVER_PASSWORD', '')};")

    def connect(self, options=None, *, autocommit=True):
        options = options or f'uri=hardcharger://{self.host}:{self.port}?tls=false;'
        return pyodbc.connect(f'Driver={DRIVER};' + options + self.auth, autocommit=autocommit, timeout=5)

    def test_server_is_from_before_the_rename(self):
        # Otherwise these cases would not exercise the fallback at all.
        self.assertEqual(requests.get(url=f'http://{self.host}:{self.port}/chargewire', timeout=5).status_code, 404)

    def test_connect_every_way(self):
        for options in (None, f'host={self.host};port={self.port};useEncryption=false;',
                        f'uri=chargewire://{self.host}:{self.port}?tls=false;', f'uri=http://{self.host}:{self.port};'):
            with self.subTest(options=options), self.connect(options) as c:
                self.assertEqual(c.getinfo(pyodbc.SQL_DBMS_NAME), 'Hardcharger')
                # The server's own discovery document was read: it names the server's version.
                self.assertIn('DuckDB', c.getinfo(pyodbc.SQL_DBMS_VER))
                self.assertEqual(c.execute('SELECT 42').fetchval(), 42)

    def test_prepared_statements(self):
        table = f'odbc_old_server_{os.getpid()}_{time.monotonic_ns()}'
        with self.connect() as c:
            cur = c.cursor()
            for i in range(3):
                self.assertEqual(tuple(cur.execute('SELECT CAST(? AS VARCHAR), CAST(? AS BIGINT) + 1', '東京', i).fetchone()),
                                 ('東京', i + 1))
            c.execute(f'CREATE TABLE {table} (id INTEGER, label VARCHAR)')
            try:
                cur.executemany(f'INSERT INTO {table} VALUES (?, ?)', [(i, f'row {i}') for i in range(500)])
                self.assertEqual(tuple(c.execute(f'SELECT count(*), sum(id), max(label) FROM {table}').fetchone()),
                                 (500, 124750, 'row 99'))
                self.assertEqual([r.column_name for r in c.cursor().columns(table=table)], ['id', 'label'])
            finally:
                c.execute(f'DROP TABLE {table}')
            self.assertEqual(sum(r[0] for r in c.execute('SELECT i FROM range(100000) t(i)')), 4999950000)

    def test_session_tags_and_errors(self):
        with self.connect(f'uri=hardcharger://{self.host}:{self.port}?tls=false;'
                          'queryTag={{"app":"odbc-old-server"}}};sessionTag={{"app":"odbc-old-server"}}};') as c:
            value = c.execute("SELECT CAST(value AS VARCHAR) FROM hardcharger_settings() WHERE name = 'hardcharger.session_tag'").fetchval()
            self.assertEqual(json.loads(value)['app'], 'odbc-old-server')
            with self.assertRaises(pyodbc.Error) as error:
                c.execute('SELECT * FROM no_such_table_odbc')
            self.assertEqual(error.exception.args[0], '42P01')
            self.assertEqual(c.execute('SELECT 7').fetchval(), 7)

    def test_transaction_rollback(self):
        table = f'odbc_old_txn_{os.getpid()}_{time.monotonic_ns()}'
        with self.connect(autocommit=False) as c:
            c.execute(f'CREATE TABLE {table} (i INTEGER)')
            c.commit()
            c.execute(f'INSERT INTO {table} VALUES (1)')
            c.rollback()
            self.assertEqual(c.execute(f'SELECT count(*) FROM {table}').fetchval(), 0)
            c.execute(f'DROP TABLE {table}')
            c.commit()

if __name__ == '__main__':
    unittest.main(verbosity=2)
