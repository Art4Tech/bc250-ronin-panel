import serial, time
s = serial.Serial()
s.port = 'COM10'
s.baudrate = 115200
s.timeout = 0.5
s.dtr = False
s.rts = False
s.open()
end = time.monotonic() + 10
with open('work/panel-art-diagnostic.log', 'ab') as out:
    while time.monotonic() < end:
        data = s.read(4096)
        if data:
            out.write(data)
            out.flush()
s.close()

