# Playing Rollback v0.3.0 on your Mac

This is a playable **training prototype** for the Episode 1 systems. Its twelve drills are temporary gameplay scenarios. The completed Episode 1 story has not yet been supplied for integration.

1. Download and unzip the ZIP from [Rollback releases](https://github.com/Zollicoff/Rollback/releases).
2. Open `rollback-e1-training.gbc` in a Game Boy Color emulator, such as [SameBoy](https://sameboy.github.io/), or use the ModRetro Chromatic Codex plugin’s ROM preview if available. Choose Game Boy Color mode. The companion `.rom` is an identical copy, not a different format.
3. Select **Enter Simulator**, choose a drill, and press A through the briefing and objective to launch.

## Controls

D-pad flies and aims; A fires; holding A locks your aim while you move. Release A before changing firing direction. B boosts, except when held beside a sabotage relay. Start pauses. Select toggles sound.

Boost takes a short time to recharge; the HUD shows `B OK` when it is ready. Green repair cells restore two hull points. On death, A restores the current checkpoint, B restarts the whole sortie, and Start returns to drill selection. The pause menu’s Take a Mulligan restarts the whole sortie.

## Exploring the world

Each map is now **1,600 × 1,440 pixels**: ten times the original width and ten times its height. Fly in any direction to explore. The camera follows outside a central resting band, leaving 56 pixels ahead horizontally and 40 pixels between the ship and either edge of the playable area vertically. Reversing direction lets the ship cross the resting band before the camera follows back. The camera stops at the four world boundaries.

The bottom-right readout is **column/row**, each from 01 to 10. The two direction indicators point horizontally and vertically toward the nearest remaining mission target; `=` means aligned on that axis. In rescue drills, `HOME` points back to the blue pad in the northwest after the third pod is collected. The transport crosses the map in both directions; the heavy boss waits in the southeast. Death checkpoints restore both camera axes and your world position along with the mission state. Travel drills have longer timers to suit the expanded area.

## Keeping progress

Write down the **resume code** shown in the drill menu and on the completion screen. At the title screen, select **Resume Code**, change each digit with up/down, move between digits with left/right, then press A.

Progress uses passwords in this build, so it survives a power cycle without depending on cartridge save hardware. It does not resume mid-mission after power-off.

## Writing the cartridge

Use the accompanying **MAC-CARTRIDGE-SETUP.md** and the [official DevDay guide](https://support.modretro.com/en_us/chromatic-devday-edition-quickstart-guid-By1iOlcMg). Developer activation must be configured on the MacBook Pro that is connected to the Chromatic.

The ROM header declares **Game Boy Color only, MBC5, 64 KiB ROM, zero external RAM**. First identify the actual cartridge and confirm the writer supports it. Back up any contents/save that need preserving. The physical DevDay cartridge has not been tested with this build yet.

In Terminal, from the unzipped folder:

```sh
shasum -a 256 -c SHA256SUMS
chromatic-cli detect-cart --all
```

Once the cart is confirmed and you intend to overwrite it, use `chromatic-cli write-homebrew rollback-e1-training.gbc --expect-sha256` followed by the ROM’s hash from SHA256SUMS. Keep the writer’s confirmation prompt. After it succeeds, verify a cold boot, movement, shooting, audio, and password-based progress on the handheld.

## Included verification

The report in `verification.json` corresponds to this exact ROM hash. All twelve drills have been completed in the emulator using controller input. Checkpoint restoration, pause timing, audio/mute, passwords, and both cartridge checksums were also checked. This establishes emulator behavior; hardware confirmation is still needed.
