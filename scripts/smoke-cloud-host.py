#!/usr/bin/env python3
"""Smoke-test a built or released driver against the public gizmodata.com host (needs internet).

gizmodata.com is a GizmoData Cloud host but not a Chargewire server, so a correct driver gets
past TCP and verified TLS on port 443 and is then answered with an HTTP error page. A wrong port,
or a TLS failure (for example with the fully qualified name "gizmodata.com."), fails the check.
"""
import argparse
import sys

import pyodbc

p = argparse.ArgumentParser()
p.add_argument('--driver', required=True, help='Driver library path (or {Driver Name} on Windows)')
a = p.parse_args()

# Placeholder credentials: gizmodata.com is not a Chargewire server and never signs anyone in.
AUTH = 'uid=odbc-smoke;pwd=not-a-password;ResumeTimeout=0;'
REACHES_443 = ['host=gizmodata.com', 'host=gizmodata.com.', 'host=GizmoData.COM.',
               'uri=hardcharger://gizmodata.com', 'uri=hardcharger://gizmodata.com.']
failures = 0


def attempt(options):
    try:
        pyodbc.connect(f'Driver={a.driver};{options};{AUTH}', timeout=10).close()
    except pyodbc.Error as e:
        return e.args[0], str(e.args[1] if len(e.args) > 1 else e)
    return None, 'connected'


for options in REACHES_443:
    state, message = attempt(options)
    # Past TLS on 443: the site answers with an HTML page, not a transport error (08001/08S01).
    ok = state not in (None, '08001', '08S01') and '<html' in message.lower()
    failures += not ok
    print(f'{"ok  " if ok else "FAIL"} {options}: {state} {message[:120]!r}', flush=True)

# An explicit port wins: 31337 is closed on gizmodata.com, and the error names that port.
state, message = attempt('host=gizmodata.com;port=31337')
ok = state == '08S01' and 'gizmodata.com:31337' in message
failures += not ok
print(f'{"ok  " if ok else "FAIL"} host=gizmodata.com;port=31337: {state} {message[:120]!r}', flush=True)

print(f'{failures} failure(s)')
sys.exit(1 if failures else 0)
