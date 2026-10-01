import importlib.util, unittest
from unittest.mock import patch
from types import SimpleNamespace
spec=importlib.util.spec_from_file_location('panel','outputs/Resource-Panel/resource_panel.py')
m=importlib.util.module_from_spec(spec);spec.loader.exec_module(m)
class Sensors(unittest.TestCase):
 def test_disk_is_not_cpu(self):
  with patch.object(m.psutil,'sensors_temperatures',return_value={'nvme':[SimpleNamespace(current=51)]},create=True),patch.object(m.platform,'system',return_value='Windows'):
   self.assertIsNone(m.cpu_temperature())
 def test_invalid_sensor_ignored(self):
  with patch.object(m.psutil,'sensors_temperatures',return_value={'coretemp':[SimpleNamespace(current=float('nan')),SimpleNamespace(current=62),SimpleNamespace(current=65)]},create=True):
   self.assertEqual(m.cpu_temperature(),65)
 def test_sensor_error_not_fatal(self):
  with patch.object(m.psutil,'sensors_temperatures',side_effect=OSError(),create=True),patch.object(m.platform,'system',return_value='Windows'):
   self.assertIsNone(m.cpu_temperature())
unittest.main()
