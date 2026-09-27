from pathlib import Path
import qrcode, zxingcpp
url='https://github.com/LibreHardwareMonitor/LibreHardwareMonitor/releases/latest'
qr=qrcode.QRCode(error_correction=qrcode.constants.ERROR_CORRECT_M,box_size=5,border=4)
qr.add_data(url);qr.make(fit=True)
im=qr.make_image(fill_color='black',back_color='white').convert('RGB')
assert zxingcpp.read_barcode(im).text==url
out=Path('outputs/Resource-Panel');im.save(out/'windows-setup-qr.png')
raw=b''.join((0xffff if r else 0).to_bytes(2,'little') for r,g,b in im.getdata())
Path('work/panel-test/main/setup_qr.rgb565').write_bytes(raw)
Path('work/panel-test/main/include/setup_qr_size.h').write_text(f'#define SETUP_QR_SIZE {im.width}\n')
print('QR verified:',url,im.size)
