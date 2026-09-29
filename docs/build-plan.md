# Rollback: Chromatic ROM build plan

Updated September 29, 2026. Zach authorized implementation and narrowed the MVP to Episode 1. The first native training prototype is built and emulator-verified.

## Confirmed direction

- Target the ModRetro Chromatic and the rewritable cartridge Zach received at DevDay.
- Build on Apple Silicon macOS and distribute portable ROM releases.
- The assistant owns code, assets, level authoring, computer-use operation, builds, and emulator verification. Zach supplies creative direction and physical-device interaction where it is not exposed to the computer.
- The earlier Phaser + TypeScript runtime is superseded. Its plan is preserved in [the September 27 archive](archive/phaser-build-plan-2026-09-27.md).
- Zach is remote on his MacBook Pro. Build on Mini1 and deliver downloadable ROM releases; cartridge connection, activation, and writing happen on the MacBook Pro.
- Keep the supplied Rollback story and mission framework as design context. Do not silently rewrite dialogue or remove missions to satisfy hardware constraints.
- Focus this MVP on Episode 1, missions 1–12. Episodes 2–4 are deferred.

## Current implementation

Version 0.1.0 provides twelve explicitly non-canonical training scenarios across the nine required mission types. They exercise flight, firing/aim lock, boost, collision/cover, enemy attacks, repair pickups, mission objectives, briefing/debrief, a three-phase boss, pause/restart, game-owned checkpoints, and resume passwords. Original constrained pixel art and synthesized audio are generated with the build. These scenarios establish the systems while the authored Episode 1 documents are pending; they are not substitutes for the canonical missions.

The built ROM declares CGB-only, MBC5, 64 KiB ROM, and zero external RAM. Its physical cartridge compatibility is still unverified. `make test` checks the actual executable in an emulator, including complete controller-input playthroughs of all twelve drills and midpoint recovery after death. The release report states its exact ROM hash.

## ROM target and toolchain

Chromatic runs Game Boy and Game Boy Color cartridges. The output must therefore contain native GB/GBC code and a valid cartridge header. GB/GBC ROMs are commonly named `.gb` or `.gbc`; a `.rom` filename alone does not identify or convert the executable format. Use the filename expected by the supplied cartridge-writing utility.

Use **Game Boy Color** as the initial hardware target. Original monochrome Game Boy support is outside this MVP.

| Component | Implementation / next option | Reason |
| --- | --- | --- |
| Game runtime | GBDK-2020 4.5.0 and C, implemented | Direct control over movement, enemies, projectile pools, memory, and rendering work per frame |
| Alternative authoring engine | GB Studio | Visual scene/dialogue editing and native ROM output; evaluate representative shooter behavior before adopting it |
| Mac emulator | PyBoy 2.7.0 for executable tests; SameBoy is an optional player | Play, inspect, and debug the actual ROM on Mini1 |
| Art | Tile/palette-constrained pixel sprites, backgrounds, and portraits | Fit the 160×144 display and GB/GBC graphics model |
| Audio | Chiptune music, compact sound effects, and text dialogue initially | Establish a feasible sound and storage budget before considering voice samples |
| Story authoring | Readable source data converted to compact banked ROM data at build time | Keep writing editable without requiring JSON parsing or a scripting runtime on the device |
| Progress | Four-digit resume passwords, implemented | Avoid depending on still-unknown cartridge save hardware |

GBDK was selected for the first implementation under Zach’s authorization to start building. The official ModRetro Codex plugin’s GB Studio workflow remains an alternative, but has not been evaluated against this prototype. Its cartridge-writing path is independent of the game runtime choice.

Sources: [Chromatic cartridge compatibility](https://support.modretro.com/en_us/welcome-to-chromatic-using-chromatic-rJGfWxdRx), [Chromatic display](https://modretro.com/blogs/blog/display-the-hard-way), [GBDK on macOS and ROM building](https://gbdk.org/docs/api/docs_getting_started.html), [GBDK banking and cartridge controllers](https://gbdk.org/docs/api/docs_rombanking_mbcs.html), [GB Studio scene types](https://www.gbstudio.dev/docs/project-editor/scenes/types/), [GB Studio ROM export](https://www.gbstudio.dev/docs/build/), [SameBoy](https://sameboy.github.io/).

## Revised milestones

1. **Identify the cartridge configuration.** Follow the now-located [official DevDay setup and verified Mac CLI instructions](mac-cartridge-setup.md), including developer activation on the MacBook Pro. Confirm the actual cartridge’s supported memory controller, ROM capacity, and persistent save storage. Do not infer these from “rewritable.” Select the ROM header and linker budget accordingly.
2. **Prove a tiny ROM.** Build a title screen plus movement and sound on Mini1. Verify it in an emulator, then write and read back the exact build using the supported cartridge workflow and verify boot/input/audio on Chromatic. Hardware connection and physical button/power actions may require Zach.
3. **Build a constrained combat arena.** One vehicle, one weapon, a small enemy/projectile pool, collisions, damage, and restart. Measure frame timing, memory usage, and sprite visibility under representative load before increasing density.
4. **Complete one short mission.** Compact portrait briefing, objective, combat, carefully timed radio text, checkpoint retry, debrief, and progress persistence appropriate to the cartridge.
5. **Establish asset budgets and authoring.** Repeatable pixel-art conversion, palettes, animation, music/SFX, map encoding, and dialogue layout. Measure actual compressed content size before committing to the full campaign layout.
6. **Integrate Episode 1.** Replace the temporary drills with the authored missions 1–12, dialogue, story events, portraits, and layouts. Check storage, text pacing, and real-hardware behavior in batches. Later episodes remain deferred.
7. **Deliver a cartridge-ready release.** Validate headers, ROM size/controller compatibility, checksums, cold boot, controls, sound, progression, and saves across power cycles. Provide the ROM, checksum, supported cartridge specification, and exact flashing instructions.

## Gameplay adaptations to evaluate

- D-pad flight/aim, A to fire and lock aim, B to boost or channel sabotage, Start to pause, and Select to toggle sound are implemented. Tune these through playtests.
- Readable compact text and expressive pixel portraits. Schedule radio exchanges around the action; briefings/debriefs carry long conversations. Flag timing conflicts without rewriting canonical dialogue.
- Deliberate enemy/projectile counts, tile reuse, and bounded effects. Establish budgets empirically.
- Game-owned checkpoint restoration, distinct from emulator save states. Do not claim real-cartridge rewind is working because an emulator can restore state.
- Optional browser sharing could later run the same ROM in an emulator; it would not restore Phaser as the game runtime.

## Current limits and next step

Only the supplied Handoff for Astra has been reviewed. The full Story Bible, Mission List, Episode 1 scripts, Bark Library, and On-Screen Text are still needed for faithful mission integration. A link to those documents was requested while implementation proceeded. No cartridge flash or hardware test has been performed.

The next steps are physical cartridge identification/validation on the MacBook Pro and canonical Episode 1 integration once its source documents are available. Preserve existing cartridge contents/save data before writing. See [Mac setup](mac-cartridge-setup.md) and [release instructions](release-guide.md).
