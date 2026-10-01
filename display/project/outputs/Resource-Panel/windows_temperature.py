"""Optional, background-only Libre Hardware Monitor WMI reader."""
import json
import math
import os
import subprocess
import threading
import time
import urllib.request
import re

def http_celsius(value):
    if isinstance(value, str):
        match = re.fullmatch(r'\s*([+-]?\d+(?:[.,]\d+)?)\s*(?:°\s*)?([CF])\s*',value,re.IGNORECASE)
        if not match:return None
        number=float(match[1].replace(',','.'))
        return (number-32)*5/9 if match[2].upper()=='F' else number
    return value

def select_http_temperature(tree):
    rows=[]
    def walk(node):
        if not isinstance(node,dict):return
        if node.get('Type')=='Temperature':
            raw=node.get('RawValue')
            if raw is None:raw=node.get('Value')
            rows.append({'Identifier':node.get('SensorId',''),'Name':node.get('Text',''),'Value':http_celsius(raw),'SensorType':'Temperature'})
        for child in node.get('Children',[]):walk(child)
    walk(tree)
    return select_cpu_temperature(rows)

def read_local_http():
    # Do not send local sensor requests through system HTTP proxies.
    opener=urllib.request.build_opener(urllib.request.ProxyHandler({}))
    with opener.open('http://127.0.0.1:8085/data.json',timeout=2) as response:
        data=response.read(2_000_001)
        if len(data)>2_000_000:raise ValueError('Oversized sensor response')
    return select_http_temperature(json.loads(data))

def select_cpu_temperature(rows):
    if isinstance(rows, dict):
        rows = [rows]
    candidates = []
    for row in rows or []:
        if not isinstance(row, dict):
            continue
        parent = str(row.get('Parent', '')).lower()
        identifier = str(row.get('Identifier', '')).lower()
        if not any('/' + cpu + '/' in parent + ' ' + identifier for cpu in ('intelcpu', 'amdcpu')):
            continue
        if row.get('SensorType') != 'Temperature' or isinstance(row.get('Value'), bool):
            continue
        try:
            value = float(row['Value'])
        except (KeyError, ValueError, TypeError):
            continue
        if not math.isfinite(value) or not -40 <= value <= 150:
            continue
        name = str(row.get('Name', '')).lower()
        if 'distance' in name or 'tjmax' in name or 'critical' in name:
            continue
        preferred = 'package' in name or 'tctl' in name or 'tdie' in name
        candidates.append((preferred, value))
    if not candidates:
        return None
    preferred = [value for priority, value in candidates if priority]
    return round(max(preferred or [value for _, value in candidates]), 1)

class WindowsTemperature:
    def __init__(self):
        self.value = None
        self.updated = 0
        self.started = False
        self.lock = threading.Lock()

    def read(self):
        with self.lock:
            if not self.started:
                self.started = True
                threading.Thread(target=self._poll, daemon=True).start()
            return self.value if time.monotonic() - self.updated < 20 else None

    def _poll(self):
        powershell = os.path.join(os.environ.get('SystemRoot', r'C:\Windows'), 'System32', 'WindowsPowerShell', 'v1.0', 'powershell.exe')
        script = "Get-CimInstance -Namespace root/LibreHardwareMonitor -ClassName Sensor -Filter \"SensorType='Temperature'\" -ErrorAction Stop | Select-Object Name,Identifier,Parent,SensorType,Value | ConvertTo-Json -Compress"
        while True:
            value = None
            try:
                value=read_local_http()
            except (OSError,ValueError):
                pass
            try:
                if value is None:
                    result = subprocess.run([powershell, '-NoProfile', '-NonInteractive', '-Command', script], capture_output=True, text=True, timeout=5, creationflags=subprocess.CREATE_NO_WINDOW)
                    if result.returncode == 0 and result.stdout.strip():
                        value = select_cpu_temperature(json.loads(result.stdout))
            except (OSError, subprocess.TimeoutExpired, ValueError):
                pass
            with self.lock:
                self.value, self.updated = value, time.monotonic()
            time.sleep(5 if value is not None else 15)

reader = WindowsTemperature()
