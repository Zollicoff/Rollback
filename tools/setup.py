"""Install pinned, project-local tools from their official distributions."""
import hashlib
import io
import platform
import subprocess
import tarfile
import urllib.request
import venv
from pathlib import Path

root=Path(__file__).resolve().parents[1]
releases={
 ('Darwin','arm64'):('gbdk-macos-arm64.tar.gz','289ee60e46c5a2785a21e35533f84a5131ed4a063b21b0dbdedc9a10af15bf78'),
 ('Darwin','x86_64'):('gbdk-macos.tar.gz','1aa549d12032d8f6509d11923bb28b1a453098f42597feb378e9a42541f8fd89'),
 ('Linux','x86_64'):('gbdk-linux64.tar.gz','d7857a5f6d135ee4c249043ca26aad9f2ec8ab5d4106d97720d404114f42605c'),
 ('Linux','aarch64'):('gbdk-linux-arm64.tar.gz','31eb2235f0fdb60163d0b1e9574a022098d6069cd56606a1daca4478a46e0439')}
archive,digest=releases[(platform.system(),platform.machine())]
tools=root/'.tools'; tools.mkdir(exist_ok=True)
if not (tools/'gbdk/bin/lcc').exists():
    data=urllib.request.urlopen('https://github.com/gbdk-2020/gbdk-2020/releases/download/4.5.0/'+archive).read()
    if hashlib.sha256(data).hexdigest()!=digest: raise RuntimeError('GBDK download checksum mismatch')
    with tarfile.open(fileobj=io.BytesIO(data),mode='r:gz') as package: package.extractall(tools,filter='data')
if not (tools/'venv/bin/python').exists(): venv.create(tools/'venv',with_pip=True)
subprocess.run([str(tools/'venv/bin/python'),'-m','pip','install','pyboy==2.7.0','Pillow==11.3.0','numpy==2.5.3','PySDL2==0.9.17','pysdl2-dll==2.32.10'],check=True)
print('Ready: run make, make play, or make test.')
