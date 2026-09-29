# Verification boundary

The primary behavior owner is the compiled ROM running on an emulated Game Boy Color. Tests send controller inputs and observe the LCD, hardware sprites, and audio. There are no test-only exports, game-state writes, invulnerability cheats, injected mission completions, or calls into private C helpers.

| Contract | Credible failure | Evidence |
| --- | --- | --- |
| Bootable cartridge binary | Incorrect CGB/mapper/size fields or corrupted checksums | Independent header and checksum calculations |
| Playable controls and audio | Broken joypad edges, projectile rendering, or sound registers | Visible movement/projectiles and sampled audio/mute output |
| Paused mission clock | Wall time accumulated during pause | Compare displayed timer around four seconds of paused emulation |
| Progress recovery | Invalid code accepted or valid progress tied to volatile RAM | Invalid-code UI and valid code entered after a fresh emulator boot |
| Game-owned checkpoints | Death restarts the whole mission or keeps the death-time clock | Reach a distant midpoint through input, die, invoke the in-game mulligan, inspect restored timer, world position, camera and objectives |
| Mission objectives | An objective cannot be completed, ends incorrectly, or fails to transition | Controller-input completion of each of twelve scenarios, including all nine types |
| Scrolling world | 8-bit coordinate wrap, edge-triggered camera, moving HUD, stale streamed terrain, offscreen objects reappearing | Button-driven travel through all ten sectors and back, dead-zone reversals, per-scanline scroll observation, far-end pause/resume and sprite clipping |
| Release identity | Package contains a build different from the tested ROM | Packaging refuses a mismatched verification hash; ZIP records per-file hashes |

`rom_harness.py` uses the font’s ASCII tile mapping to read visible text and Game Boy OAM to observe vehicles. `playthrough.py` navigates from the displayed terrain and sprites. Its twelve scenarios are the single primary owner of objective-completion coverage; the shorter checks cover distinct control, audio, persistence, and lifecycle failures.

The tests establish emulator behavior. They do not establish cartridge electrical/controller compatibility, display readability on the physical Chromatic, subjective balance, or fidelity to scripts that have not been supplied. Desktop UI automation could not bind the SDL window on Mini1, so the current playtest evidence is the emulator API and captured framebuffers, not a claimed manual GUI playthrough.

The scrolling regression was also run against the v0.1.0 ROM and failed at the expected missing behavior: “Camera must follow before the right edge.” Its checks observe hardware scroll values and the visible sector counter, without reading private C coordinates.
