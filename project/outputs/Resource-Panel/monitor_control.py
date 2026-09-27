"""Start/stop this panel's monitor without changing Windows startup settings."""
import argparse, json, os, subprocess, sys, time
from pathlib import Path
import psutil

ROOT = Path(__file__).resolve().parent
SCRIPT = str(ROOT / 'resource_panel.py')
RECORD = ROOT / 'running.json'

def matches(process):
    return SCRIPT.lower() in [arg.lower() for arg in process.cmdline()]

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('action', choices=['Start', 'Stop'])
    parser.add_argument('--port', default='COM10')
    args = parser.parse_args()
    process = None
    if RECORD.exists():
        record = json.loads(RECORD.read_text())
        try:
            candidate = psutil.Process(record['pid'])
            if abs(candidate.create_time()-record['created']) < 0.01 and matches(candidate):
                process = candidate
        except psutil.NoSuchProcess:
            pass
        # AccessDenied deliberately leaves the record intact.
    if process:
        if args.action == 'Start':
            print('Panel monitoring is already running.'); return
        children = process.children(recursive=True)
        for child in children:
            try:
                if matches(child): child.terminate()
            except psutil.NoSuchProcess:
                pass
        try: process.terminate()
        except psutil.NoSuchProcess: pass
        psutil.wait_procs([process]+children, timeout=5)
    if RECORD.exists(): RECORD.unlink()
    if args.action == 'Stop':
        print('Panel monitoring stopped.'); return
    with (ROOT/'monitor.log').open('ab') as out, (ROOT/'monitor-errors.log').open('ab') as err:
        flags = subprocess.CREATE_NO_WINDOW if os.name == 'nt' else 0
        child = subprocess.Popen([sys.executable, SCRIPT, '--port', args.port, '--panel-log', str(ROOT/'panel-runtime.log')],
                                 stdin=subprocess.DEVNULL, stdout=out, stderr=err,
                                 creationflags=flags, close_fds=True)
    p = psutil.Process(child.pid)
    RECORD.write_text(json.dumps({'pid':p.pid,'created':p.create_time()}))
    time.sleep(1.5)
    if child.poll() is not None:
        RECORD.unlink(missing_ok=True)
        raise RuntimeError('Monitor exited; see monitor-errors.log')
    print(f'Panel monitoring continues on {args.port} until stopped explicitly.')

if __name__ == '__main__': main()
