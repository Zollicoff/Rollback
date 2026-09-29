"""Bundle only the exact verified ROM, instructions, checksums and receipts."""
import hashlib
import json
import zipfile
from pathlib import Path

version='0.3.1-waypoints'
rom=Path('build/rollback-e1-training.gbc').read_bytes()
digest=hashlib.sha256(rom).hexdigest()
report=json.loads(Path('build/verification.json').read_text())
if report['sha256']!=digest or not all(r['status']=='PASS' for r in report['scenarios']):
    raise RuntimeError('Run make test against this exact ROM before packaging')
files={
 'rollback-e1-training.gbc':rom,
 'rollback-e1-training.rom':rom,
 'PLAY-ON-MAC.md':Path('docs/release-guide.md').read_bytes(),
 'MAC-CARTRIDGE-SETUP.md':Path('docs/mac-cartridge-setup.md').read_bytes(),
 'CHANGELOG.md':Path('CHANGELOG.md').read_bytes(),
 'THIRD_PARTY_NOTICES.md':Path('THIRD_PARTY_NOTICES.md').read_bytes(),
 'LICENSES/GBDK_LIBRARY.txt':Path('LICENSES/GBDK_LIBRARY.txt').read_bytes(),
 'verification.json':Path('build/verification.json').read_bytes(),
}
for name in ('title','gameplay','checkpoint','rewind','drill-12-pass','world-northwest','world-northeast','world-southeast','world-southwest','world-return','world-diagonal','waypoint-protect-and-enemy','waypoint-rescue','waypoint-sabotage'):
    files['screenshots/'+name+'.png']=Path('build/screenshots/'+name+'.png').read_bytes()
files['SHA256SUMS']=''.join(f'{hashlib.sha256(content).hexdigest()}  {name}\n' for name,content in sorted(files.items())).encode()
dist=Path('dist');dist.mkdir(exist_ok=True)
output=dist/f'rollback-{version}.zip'
with zipfile.ZipFile(output,'w',compression=zipfile.ZIP_DEFLATED) as archive:
    for name,content in sorted(files.items()):
        info=zipfile.ZipInfo(name,date_time=(2026,9,29,0,0,0))
        info.compress_type=zipfile.ZIP_DEFLATED;info.external_attr=0o100644<<16
        archive.writestr(info,content)
zip_hash=hashlib.sha256(output.read_bytes()).hexdigest()
(dist/'SHA256SUMS').write_text(f'{zip_hash}  {output.name}\n')
print(json.dumps({'bundle':str(output),'bundle_sha256':zip_hash,'rom_sha256':digest,'rom_bytes':len(rom)},indent=2))
