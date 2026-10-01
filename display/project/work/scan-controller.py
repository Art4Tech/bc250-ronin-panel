import asyncio,json
from bleak import BleakScanner
async def main():
 found={}
 def detected(device,adv):
  name=adv.local_name or device.name or ''
  if any(x in name.lower() for x in ('gamesir','nova','wireless controller','duoshok','xbox')):
   found[device.address]={'name':name,'address':device.address,'services':adv.service_uuids}
 print('Scanning controller BLE advertisements for 25 seconds...',flush=True)
 async with BleakScanner(detection_callback=detected):
  await asyncio.sleep(25)
 print(json.dumps(list(found.values()),indent=2),flush=True)
 if not found:print('No matching BLE advertisement found. This does not prove the controller is Classic-only.')
asyncio.run(main())
