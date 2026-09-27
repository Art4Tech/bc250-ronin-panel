from html.parser import HTMLParser
from pathlib import Path
from collections import Counter
import re

p = Path(r'<PROJECT_ROOT>\outputs\BC250-Omarchy-Runbook.html')
class Check(HTMLParser):
    void = {'meta','link','br','hr','img','input','source','wbr','area','base','col','embed','param','track'}
    def __init__(self):
        super().__init__(convert_charrefs=True)
        self.stack=[]; self.ids=[]; self.hrefs=[]; self.assets=[]; self.sections=0
    def handle_starttag(self, tag, attrs):
        a=dict(attrs)
        if tag not in self.void: self.stack.append(tag)
        if 'id' in a: self.ids.append(a['id'])
        if 'href' in a: self.hrefs.append(a['href'])
        if 'src' in a: self.assets.append(a['src'])
        if tag=='section': self.sections+=1
    def handle_endtag(self, tag):
        assert self.stack and self.stack[-1]==tag, (tag,self.stack[-5:])
        self.stack.pop()

s=p.read_text(encoding='utf-8')
c=Check();c.feed(s);c.close()
assert not c.stack,c.stack
assert all(n==1 for n in Counter(c.ids).values()), 'Duplicate IDs'
assert all(h[1:] in c.ids for h in c.hrefs if h.startswith('#')), 'Broken navigation'
assert not c.assets, 'Unexpected external runtime assets'
assert '<script' not in s.lower(), 'Unexpected scripts'
assert '\ufffd' not in s, 'Invalid encoding'
assert 'B0FHW5DZ20' in s and 'B0FHW5DZ29' in s and '1515' in s
assert c.sections==11, c.sections
print(f'PASS: balanced HTML; {c.sections} sections; all local anchors resolve; no external runtime assets; UTF-8 intact; {p.stat().st_size:,} bytes.')
