from pathlib import Path
p=Path('outputs/Resource-Panel/README.md');s=p.read_text().replace('matching day/night paintings, drifting petals and moving mist','matching day/night paintings, 36 drifting petals, a gently swaying foreground sakura branch, subtle water ripples, slow cloud drift and daytime mist').replace('from 40% to 8%','from 60% to 12%');p.write_text(s)
