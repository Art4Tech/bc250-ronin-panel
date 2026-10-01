import argparse, json, math, os, platform, time
from pathlib import Path
import psutil
import serial
from serial.tools import list_ports


def cpu_temperature():
    getter = getattr(psutil, 'sensors_temperatures', None)
    if getter:
        try:
            sensors = getter()
            for group in ('coretemp', 'k10temp', 'cpu_thermal', 'cpu-thermal', 'zenpower'):
                values = [s.current for s in sensors.get(group, []) if s.current is not None and math.isfinite(s.current)]
                if values:
                    return round(max(values), 1)
        except (OSError, RuntimeError):
            pass
    if platform.system() == 'Linux':
        for zone in Path('/sys/class/thermal').glob('thermal_zone*'):
            try:
                if zone.joinpath('type').read_text().strip() in ('cpu-thermal', 'cpu_thermal', 'x86_pkg_temp'):
                    return round(int(zone.joinpath('temp').read_text()) / 1000, 1)
            except (OSError, ValueError):
                pass
    if platform.system() == 'Windows':
        from windows_temperature import reader
        return reader.read()
    return None


def sample(disk):
    memory = psutil.virtual_memory()
    return {'v': 1, 'host': platform.node()[:40], 'os': platform.system(),
            'cpu': round(psutil.cpu_percent(interval=None), 1),
            'ram': round(memory.percent, 1), 'used': round((memory.total-memory.available)/2**30, 1),
            'total': round(memory.total/2**30, 1), 'disk': round(psutil.disk_usage(disk).percent, 1),
            'temp': cpu_temperature()}


def main():
    parser = argparse.ArgumentParser(description='USB resource dashboard for the Elecrow P4 panel')
    parser.add_argument('--port', help='COM10 on Windows; /dev/serial/by-id/... on Linux')
    parser.add_argument('--list-ports', action='store_true')
    parser.add_argument('--once', action='store_true', help='Print one sample without opening USB')
    parser.add_argument('--disk', default=os.path.abspath(os.sep), help='Filesystem to monitor; default system root')
    parser.add_argument('--seconds', type=float, default=0, help='Stop after this many seconds; default runs until Ctrl+C')
    parser.add_argument('--panel-log', type=Path, help='Optional local diagnostic log from the display (limited to 2 MB)')
    args = parser.parse_args()
    if args.list_ports:
        for port in list_ports.comports():
            print(f'{port.device}: {port.description}')
        return
    if not args.port and not args.once:
        parser.error('Specify --port or use --list-ports')
    psutil.cpu_percent(interval=None)
    time.sleep(1)
    if args.once:
        print(json.dumps(sample(args.disk), allow_nan=False))
        return
    end = time.monotonic()+args.seconds if args.seconds else float('inf')
    connection = None
    last_sent = None
    try:
        while time.monotonic() < end:
            try:
                if connection is None:
                    connection = serial.Serial()
                    connection.port=args.port
                    connection.baudrate=115200
                    connection.timeout=0
                    connection.write_timeout=2
                    connection.dtr=False
                    connection.rts=False
                    connection.open()
                    last_sent = None
                    print(f'Connected to {args.port}', flush=True)
                frame = sample(args.disk)
                now = time.time()
                frame['wake'] = last_sent is None or now-last_sent > 5
                payload = json.dumps(frame, separators=(',', ':'), ensure_ascii=True, allow_nan=False).encode()+b'\n'
                if len(payload)>511:
                    raise ValueError('Telemetry packet exceeds panel limit')
                connection.write(payload)
                last_sent = now
                # Drain the panel log to prevent the host receive buffer from filling.
                incoming = connection.read(connection.in_waiting)
                if incoming and args.panel_log:
                    mode = 'wb' if args.panel_log.exists() and args.panel_log.stat().st_size > 2_000_000 else 'ab'
                    with args.panel_log.open(mode) as log:
                        log.write(incoming)
                time.sleep(1)
            except (serial.SerialException, OSError) as error:
                print(f'USB unavailable; retrying: {error}', flush=True)
                if connection:
                    connection.close()
                connection=None
                time.sleep(2)
    except KeyboardInterrupt:
        pass
    finally:
        if connection:
            connection.close()

if __name__=='__main__':
    main()
