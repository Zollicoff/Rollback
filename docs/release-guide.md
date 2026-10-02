# Playing the Rollback campaign on your Mac

1. Download and unzip the campaign ZIP from [Rollback releases](https://github.com/Zollicoff/Rollback/releases).
2. Open `rollback.gbc` in a Game Boy Color emulator. Choose CGB mode. The companion `rollback.rom` is an identical copy.
3. Select **Campaign**, choose the available mission, and advance the briefing with A. Radio pauses flight while you read; release A, then press it to advance.

## Controls and navigation

D-pad flies and aims. A fires; holding it locks aim while moving. Release A to change firing direction. B boosts, or channels sabotage beside an amber target. Start pauses, Select toggles sound. Results and debriefs require fresh Start presses.

The bottom-right grid reads column/row, each 01–10. Red arrows identify attack targets and threats, green identifies protection, cyan identifies rescue/navigation, and amber identifies sabotage. Escorts wait when you fall behind. In rescue missions, return to the northwest pad when the HUD says HOME. During radar sweeps, wait within sixteen pixels of cover.

The weapon indicator beside boost readiness shows `1` for the standard gun, `S` for Spread Shot, `L` for Tachyon Lance, and `M` for Seeker Swarm. The archive contains item descriptions and enemy entries. Four difficulties are available in Settings.

On defeat, release the controls briefly. Start restores the most recent checkpoint, B restarts the mission, and Select returns to mission selection. The pause menu's Take a Mulligan also restores the checkpoint. Checkpoints raise hull to at least three before capturing the world; retry totals remain separate.

## Progress and endings

Write down the six-digit **Resume Code** displayed in mission selection and results. Enter it from the title menu using up/down for digits, left/right for the cursor, and A to submit. It restores unlocked missions, difficulty, main completion, and Loop 2. It does not restore a mid-mission checkpoint or session statistics after power-off.

Complete the 54-mission main campaign to unlock Loop 2. Its last boss uses B to talk during the encounter. Mission 54-B is a noncombat epilogue. The full four-episode screenplay is included as readable text, with portraits and sound effects rather than recorded voice acting.

## Cartridge

Use **MAC-CARTRIDGE-SETUP.md** in the bundle or the repository's [setup guide](mac-cartridge-setup.md). Developer activation belongs on the MacBook Pro connected to the Chromatic.

This release declares **Game Boy Color only, MBC5, 256 KiB ROM, zero external RAM**. Detect the cartridge and confirm support before overwriting it. Preserve any existing contents or save data you need.

From the unzipped folder:

```sh
shasum -a 256 -c SHA256SUMS
chromatic-cli detect-cart --all
```

After confirming the intended cart, use `chromatic-cli write-homebrew rollback.gbc --expect-sha256` followed by the ROM hash in SHA256SUMS. Keep the writer's confirmation prompt. Verify a cold boot, movement, shooting, audio, and resume-code recovery on the handheld.

## Verification

`verification.json` identifies the exact tested ROM and the emulator checks. No game RAM writes or injected victories are used. Physical cartridge boot, controller, audio, and power-cycle verification remain outstanding.
