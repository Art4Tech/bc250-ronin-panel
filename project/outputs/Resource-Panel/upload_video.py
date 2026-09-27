"""Transfer prepared RV01 video clips through the panel's USB serial port."""
import argparse,base64,hashlib,json,struct,time,zlib
from pathlib import Path
import serial

def request(port,message,timeout=4):
    payload=json.dumps(message,separators=(',',':')).encode()
    # Fill the receiver's 128-byte reads so a partial read does not wait for its timeout.
    payload+=b' '*((-len(payload)-1)%128)+b'\n'
    if len(payload)>2048:raise ValueError('Packet exceeds firmware line buffer')
    port.write(payload)
    deadline=time.monotonic()+timeout
    while time.monotonic()<deadline:
        line=port.readline()
        marker=line.find(b'@ASSET ')
        if marker>=0:
            try:return json.loads(line[marker+7:])
            except ValueError:pass
    raise TimeoutError(f"No panel acknowledgement for {message['asset']}")

def validate(path):
    raw=path.read_bytes()
    if path.name in ('nature2.pcm','day3.pcm','nite3.pcm','sleep3.pcm','wake3.pcm'):
        if not raw or len(raw)%4 or len(raw)>8000000:raise ValueError('Invalid stereo 16-bit PCM')
        return raw
    if len(raw)<16:raise ValueError('File too short')
    magic,w,h,fps,reserved,count=struct.unpack_from('<4sHHHHI',raw)
    if magic!=b'RV01' or (w,h)!=(800,480) or not 1<=fps<=30 or not 1<=count<=9000:raise ValueError('Invalid video header')
    pos=16
    for _ in range(count):
        if pos+4>len(raw):raise ValueError('Missing frame')
        length=struct.unpack_from('<I',raw,pos)[0];pos+=4
        if not 0<length<=262144 or pos+length>len(raw) or raw[pos:pos+2]!=b'\xff\xd8':raise ValueError('Invalid JPEG frame')
        pos+=length
    if pos!=len(raw):raise ValueError('Unexpected trailing data')
    return raw

def main():
    ap=argparse.ArgumentParser();ap.add_argument('--port',default='COM10');ap.add_argument('--chunk-size',type=int,choices=(256,1024),default=256);ap.add_argument('--info',action='store_true');ap.add_argument('files',nargs='*',type=Path);args=ap.parse_args()
    clips=[]
    for path in args.files:
        if path.name not in ('day.rvj','night.rvj','day2.rvj','nite2.rvj','sleep2.rvj','wake2.rvj','nature2.pcm','day3.rvj','nite3.rvj','sleep3.rvj','wake3.rvj','day3.pcm','nite3.pcm','sleep3.pcm','wake3.pcm'):ap.error('Unsupported panel asset name')
        clips.append((path.name,validate(path)))
    port=serial.Serial();port.port=args.port;port.baudrate=115200;port.timeout=.3;port.write_timeout=4;port.dtr=False;port.rts=False;port.open()
    accelerated=False;in_progress=False
    try:
        reply=request(port,{'asset':'info'});print(reply,flush=True)
        if args.info:return
        if reply['status']!='ready':raise RuntimeError('Panel is not ready: '+reply['status'])
        reply=request(port,{'asset':'baud','rate':460800})
        if reply['status']!='ready':raise RuntimeError(reply)
        port.baudrate=460800;accelerated=True;time.sleep(.1)
        for name,raw in clips:
            reply=request(port,{'asset':'begin','name':name,'size':len(raw),'sha256':hashlib.sha256(raw).hexdigest()})
            if reply['status']=='exists':raise RuntimeError(name+' or its temporary file already exists; nothing overwritten')
            if reply['status']!='ready':raise RuntimeError(reply)
            in_progress=True;last_progress=-1
            for offset in range(0,len(raw),args.chunk_size):
                block=raw[offset:offset+args.chunk_size];message={'asset':'chunk','offset':offset,'data':base64.b64encode(block).decode(),'crc32':zlib.crc32(block)}
                for attempt in range(10):
                    try:
                        reply=request(port,message)
                        if reply['status'] in ('ok','offset') and reply['offset']==offset+len(block):break
                        if reply['status'] in ('invalid','checksum_retry','offset') and reply['offset']==offset and attempt<9:
                            time.sleep(.1);continue
                        raise RuntimeError(reply)
                    except TimeoutError:
                        if attempt==9:raise
                progress=int((offset+len(block))*20/len(raw))
                if progress!=last_progress:print(f'{name}: {progress*5}%',flush=True);last_progress=progress
            reply=request(port,{'asset':'finish'},timeout=10)
            if reply['status']!='complete':raise RuntimeError(reply)
            in_progress=False;print(name+': SHA-256 verified and committed',flush=True)
    finally:
        try:
            if in_progress:request(port,{'asset':'abort'})
            if accelerated:request(port,{'asset':'baud','rate':115200});port.baudrate=115200
        finally:port.close()

if __name__=='__main__':main()
