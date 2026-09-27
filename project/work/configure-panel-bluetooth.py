from pathlib import Path
import subprocess
p=Path('work/panel-test/main/idf_component.yml');s=p.read_text();s+='  espressif/esp_hosted: "2.12.12"\n  espressif/esp_wifi_remote: "0.16.3"\n';p.write_text(s)
p=Path('work/panel-test/sdkconfig');s=p.read_text()
source=subprocess.check_output(['git','-c','safe.directory=<PROJECT_ROOT>/work/elecrow-official','-C','work/elecrow-official','show','HEAD:example/V1.0/idf-code/Lesson17-Wi-Fi_function/ESP32_P4-softAP/sdkconfig'],text=True)
settings=[l for l in source.splitlines() if l.startswith('CONFIG_ESP_HOSTED_')]
settings += ['CONFIG_BT_ENABLED=y','CONFIG_BT_CONTROLLER_DISABLED=y','CONFIG_BT_BLUEDROID_ENABLED=y','CONFIG_BT_BLE_42_FEATURES_SUPPORTED=y','CONFIG_BT_BLE_50_FEATURES_SUPPORTED=y','CONFIG_BT_GATTC_ENABLE=y','CONFIG_BT_BLE_SMP_ENABLE=y','CONFIG_ESP_WIFI_REMOTE_ENABLED=y','CONFIG_SLAVE_IDF_TARGET_ESP32C6=y']
keys={l.split('=')[0] for l in settings}
s='\n'.join(l for l in s.splitlines() if l.split('=')[0] not in keys and not any(l=='# '+k+' is not set' for k in keys))+'\n'+'\n'.join(settings)+'\n'
p.write_text(s)
