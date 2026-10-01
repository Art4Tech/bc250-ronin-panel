import asyncio
from bleak import BleakScanner, BleakClient
async def main():
    device=await BleakScanner.find_device_by_filter(lambda d,a: 'Nova 2 Lite' in (a.local_name or d.name or ''),timeout=25)
    if not device:
        print('Controller not found; put it back into pairing mode.');return
    print('Connecting to',device.name,flush=True)
    async with BleakClient(device,timeout=20) as client:
        for service in client.services:
            print('SERVICE',service.uuid,flush=True)
            if service.uuid.startswith('00001812'):
                for ch in service.characteristics:
                    print('HID',ch.uuid,ch.properties,flush=True)
                    if ch.uuid.startswith('00002a4b'):
                        try:print('REPORT_MAP',bytes(await client.read_gatt_char(ch)).hex(),flush=True)
                        except Exception as e:print('Map read:',type(e).__name__,str(e),flush=True)
        print('Probe complete; disconnecting without a permanent pairing request.',flush=True)
asyncio.run(main())
