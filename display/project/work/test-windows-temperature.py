import sys,unittest,time
sys.path.insert(0,'outputs/Resource-Panel')
from windows_temperature import select_cpu_temperature, select_http_temperature, WindowsTemperature
def sensor(parent,name,value):return dict(Parent=parent,Name=name,Value=value,SensorType='Temperature')
class TemperatureTests(unittest.TestCase):
 def test_http_formatted_raw_values(self):
  for text,expected in [('65.0 °C',65),('149.0 °F',65),('65,5 °C',65.5),('N/A',None)]:
   self.assertEqual(select_http_temperature({'Type':'Temperature','SensorId':'/intelcpu/0/temperature/0','Text':'CPU Package','RawValue':text}),expected)
 def test_distance_to_limit_is_not_cpu_temperature(self):
  self.assertIsNone(select_cpu_temperature([sensor('/intelcpu/0','Core Distance to TjMax',40)]))
 def test_http_uses_raw_celsius_not_formatted_fahrenheit(self):
  tree={'Children':[{'Type':'Temperature','SensorId':'/intelcpu/0/temperature/0','Text':'CPU Package','RawValue':60,'Value':'140 F'},{'Type':'Temperature','SensorId':'/nvme/0/temperature/0','Text':'SSD','RawValue':90}]}
  self.assertEqual(select_http_temperature(tree),60)
 def test_package_preferred_over_core_and_gpu(self):
  self.assertEqual(select_cpu_temperature([sensor('/intelcpu/0','CPU Package',62),sensor('/intelcpu/0','Core #1',71),sensor('/gpu-nvidia/0','GPU Core',90)]),62)
 def test_amd_and_disk_separation(self):
  self.assertEqual(select_cpu_temperature([sensor('/amdcpu/0','Core (Tctl/Tdie)',57.25),sensor('/nvme/0','Temperature',99)]),57.2)
 def test_rejects_invalid_or_unidentified_readings(self):
  self.assertIsNone(select_cpu_temperature([sensor('/intelcpu/0','CPU Package',None),sensor('/intelcpu/0','Core',float('nan')),sensor('/intelcpu/0','Core',999),sensor('/nvme/0','CPU Package',50)]))
 def test_stale_reading_is_not_displayed(self):
  r=WindowsTemperature();r.started=True;r.value=65;r.updated=time.monotonic()-30;self.assertIsNone(r.read())
unittest.main()
