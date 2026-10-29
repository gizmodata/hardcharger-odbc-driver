"""Thin-wrapper regressions, through the real ODBC driver manager and server."""
import json
import os
import sys
import time
import unittest
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

if __name__ == '__main__':
    unittest.main(verbosity=2)
