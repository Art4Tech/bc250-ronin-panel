from pathlib import Path
p=Path('work/panel-test/sdkconfig');s=p.read_text()
settings={'CONFIG_ESP_HOSTED_MEMPOOL_PREFER_SPIRAM':'y','CONFIG_ESP_HOSTED_SDIO_TX_Q_SIZE':'8','CONFIG_ESP_HOSTED_SDIO_RX_Q_SIZE':'8','CONFIG_BT_ALLOCATION_FROM_SPIRAM_FIRST':'y','CONFIG_SPIRAM_MALLOC_ALWAYSINTERNAL':'1024','CONFIG_FREERTOS_HZ':'1000'}
s='\n'.join(l for l in s.splitlines() if l.split('=')[0] not in settings and not any(l=='# '+k+' is not set' for k in settings))+'\n'+'\n'.join(k+'='+v for k,v in settings.items())+'\n';p.write_text(s)
