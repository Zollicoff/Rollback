"""Bundle only the exact verified ROM, instructions, checksums and receipts."""
import hashlib
import json
import zipfile
from pathlib import Path

version='0.4.0-campaign'
rom=Path('build/rollback.gbc').read_bytes()
digest=hashlib.sha256(rom).hexdigest()
report=json.loads(Path('build/verification.json').read_text())
if report['sha256']!=digest or {r['mission'] for r in report.get('missions',[])}!=set(range(1,55)) or not all(r['status']=='PASS' for r in report['missions']) or report.get('secret',{}).get('status')!='PASS':
    raise RuntimeError('Run make test against this exact ROM before packaging')
files={
 'rollback.gbc':rom,
 'rollback.rom':rom,
 'PLAY-ON-MAC.md':Path('docs/release-guide.md').read_bytes(),
 'MAC-CARTRIDGE-SETUP.md':Path('docs/mac-cartridge-setup.md').read_bytes(),
 'CAMPAIGN-ADAPTATION.md':Path('docs/campaign-adaptation.md').read_bytes(),
 'CHANGELOG.md':Path('CHANGELOG.md').read_bytes(),
 'THIRD_PARTY_NOTICES.md':Path('THIRD_PARTY_NOTICES.md').read_bytes(),
 'LICENSES/GBDK_LIBRARY.txt':Path('LICENSES/GBDK_LIBRARY.txt').read_bytes(),
 'verification.json':Path('build/verification.json').read_bytes(),
}
for name in ('campaign-title','checkpoint','rewind','mission-01-pass','mission-26-pass','mission-54-pass','world-northwest','world-northeast','world-southeast','world-southwest','world-return','world-diagonal','waypoint-protect-and-enemy','waypoint-rescue','waypoint-sabotage','secret-ending','clean-helmet'):
    files['screenshots/'+name+'.png']=Path('build/screenshots/'+name+'.png').read_bytes()
files['SHA256SUMS']=''.join(f'{hashlib.sha256(content).hexdigest()}  {name}\n' for name,content in sorted(files.items())).encode()
dist=Path('dist');dist.mkdir(exist_ok=True)
output=dist/f'rollback-{version}.zip'
with zipfile.ZipFile(output,'w',compression=zipfile.ZIP_DEFLATED) as archive:
    for name,content in sorted(files.items()):
        info=zipfile.ZipInfo(name,date_time=(2026,10,1,0,0,0))
        info.compress_type=zipfile.ZIP_DEFLATED;info.external_attr=0o100644<<16
        archive.writestr(info,content)
zip_hash=hashlib.sha256(output.read_bytes()).hexdigest()
(dist/'SHA256SUMS').write_text(f'{zip_hash}  {output.name}\n')
print(json.dumps({'bundle':str(output),'bundle_sha256':zip_hash,'rom_sha256':digest,'rom_bytes':len(rom)},indent=2))
