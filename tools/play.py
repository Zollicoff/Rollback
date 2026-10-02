"""Play the built native ROM in a desktop emulator."""
import sys
from pyboy import PyBoy

rom=sys.argv[1] if len(sys.argv)>1 else 'build/rollback.gbc'
emulator=PyBoy(rom,window='SDL2',scale=4,cgb=True)
try:
    while emulator.tick(): pass
finally:
    emulator.stop()
