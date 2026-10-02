# Rollback build direction

Updated October 1, 2026. Zach expanded scope from the Episode 1 MVP to the full original game and supplied all nine story documents.

The target remains native Game Boy Color / ModRetro Chromatic. Build on Mini1, distribute a downloadable ROM for the remote MacBook Pro, and use the identified cartridge workflow there. The GitHub repository is public at `Zollicoff/Rollback`; the existing local project folder and memory identity remain unchanged.

The campaign now implements all 54 missions across four episodes, the main ending, and an unlocked second-loop ending. See [campaign adaptation](campaign-adaptation.md) for story preservation, platform choices, gameplay rules, and progress behavior. Source documents remain unchanged in `story/source/`.

The runtime uses GBDK 4.5.0, banked C data, native tiles/portraits, and synthesized audio. The ROM is CGB-only MBC5 with no external RAM. Six-digit passwords avoid assuming a cartridge save-memory configuration. The supplied story uses paginated text on the handheld; no recorded dialogue was supplied.

`make test` verifies the cartridge through controller input and hardware observations. The exact tested artifact and outcomes are recorded in `build/verification.json` and included with the download. Human play feedback, physical cartridge validation, and further audiovisual polish remain follow-up work.

Earlier plans are preserved: [Phaser proposal](archive/phaser-build-plan-2026-09-27.md), [Chromatic Episode 1 MVP](archive/chromatic-mvp-plan-2026-09-29.md). Existing releases remain downloadable.
