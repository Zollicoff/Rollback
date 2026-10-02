"""Recompile banked objects before packing so no previous bank assignment sticks."""
from pathlib import Path
import subprocess,sys,hashlib
lcc,rom=sys.argv[1:]
obj=Path('build/obj');obj.mkdir(exist_ok=True)
sources=sorted(Path('src').glob('*.c'))+[Path('build/assets.c')]+sorted(Path('build/campaign').glob('*.c'))+sorted(Path('build/portraits').glob('*.c'))
objects=[]
for source in sources:
    target=obj/(str(source).replace('/','_')+'.o');objects.append(str(target))
    digest=hashlib.sha256(source.read_bytes()+b''.join(p.read_bytes() for p in sorted(Path('src').glob('*.h')))+Path('build/assets.h').read_bytes()).hexdigest()
    stamp=target.with_suffix('.sha256')
    if not target.exists() or not stamp.exists() or stamp.read_text()!=digest:
        subprocess.run([lcc,'-Isrc','-Ibuild','-c','-o',str(target),str(source)],check=True)
        stamp.write_text(digest)
subprocess.run([lcc,'-autobank','-Wb-ext=.rel','-Wm-yC','-Wm-yt0x19','-Wm-yoA','-Wm-ynROLLBACK','-Wl-m','-Wl-j','-o',rom,*objects],check=True)
print(f'Built {rom}: {Path(rom).stat().st_size} bytes')
