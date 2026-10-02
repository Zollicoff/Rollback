# Native campaign adaptation

The nine files in `story/source/` are the supplied screenplay, unchanged. Their import hashes are recorded in `story/source-sha256.json`. They supersede the earlier single-tab handoff for story scope. The target remains Game Boy Color / ModRetro Chromatic, built on Mini1 with GBDK 2020.

The campaign contains all 54 main missions and the optional second-loop 53-B / 54-B ending. The original spoken words, speaker, mood, stage directions, and event order are compiled from Markdown. Typography becomes uppercase ASCII for the cartridge font. Italic stage directions become caption cards; `(clue)` remains hidden metadata. Replayed clues receive an amber highlight. Radio pauses flight while the player reads. No recorded voice assets were supplied: this version uses text, portraits, synthesized music, and sound effects.

## Story and play

`data/campaign.json` defines native mission parameters. `tools/campaign.py` binds every supplied radio trigger to a gameplay event and rejects unbound triggers. Briefings and debriefs are multi-page scenes. The game does not silently shorten the dialogue to fit one screen.

- All arenas retain the 1,600 × 1,440 world and camera padding on both axes. Ten terrain themes cover Kestrel, the Channel, the river, Cold War airfields, the server farm, the Cult, Newsreel Empire, occupied valley, Quiet One, and Day Zero.
- Missions use connected flight corridors and cover. Defense waves, transport routes, raid sites, rescue cages, pursuit targets, radar sweeps, and boss phases have world positions.
- Mission 7 is an escape from an invulnerable Auditor; Mission 51 is a race to the core. Their success condition is reaching the route's end.
- Mission 9 is the authored 30-second test jump. Duplicate-ship encounters and the Mission 23 weapon glitches are hazards.
- Mission 11 has seven carrier sites and one recovered core. Mission 13 requires clearing four bandits before rescuing Dotty.
- Missions 17 and 31 pursue multiple boats / blimps. Mission 20 has a 12-second radar cycle and requires nearby cover during a sweep.
- Mission 21's launch and Mission 43's upload advance the campaign as scripted setbacks. They do not strand the player on a defeat screen.
- Mission 26 introduces the Auditor while the Seeder is still alive; both must be defeated. A checkpoint after the Seeder falls preserves that victory on a retry. Other bosses use three damage phases; their dialogue distinguishes destruction, retreat, and escape.
- Mission 35's targets are unarmed machines. Mission 36 requires opening cages and returning their occupants home.
- Mission 48 starts a 90-second upload after holding B at the relay; the relay must survive.
- Loop 2 replaces Mission 53's dialogue and uses B to talk through its phases. Mission 54-B is a noncombat epilogue.

Narrative minute counts are preserved in dialogue. Most long story waits are compressed into playable waves or shorter survival clocks, rather than requiring twenty minutes of repeated enemies. Mission 9's 30 seconds, the radar's 12-second cycle, and Rule Zero's 90-second upload are literal game-clock timings.

Gus's combat barks begin in Mission 4, after his installation in Mission 3's debrief; Teddy supplies retries through Mission 3. This resolves the draft bark library's Mission 3 start against the actual introduction scene without moving or rewriting that scene.

Bark pools avoid the last three lines when enough lines exist. Smaller story-gated pools retain as much history as their size permits, so a two-line pool alternates and a single-line enemy reaction can recur after its cooldown.

## Progress and recovery

A six-digit code restores unlocked missions, difficulty, completion status, and loop. It does not require cartridge SRAM. Session statistics, codex encounters, and skill achievements remain in memory until power-off; the code is not a full save-state. Old four-digit training codes belong to the earlier prototype.

Checkpoints capture position, camera, enemies, projectiles, objectives, story-event flags, power-ups, mission time, and RNG. A checkpoint raises hull to a minimum of three before capturing the state. Retry totals stay outside the snapshot. Death requires a fresh intentional input after a short neutral interval. Start rewinds, B restarts the mission, and Select returns to the mission menu. Firing never skips results or debriefs.

The eight supplied pickups have native effects: repair, spread bolts, piercing shots, homing shots, temporary shield, combat slowdown, an automatic extra rewind, and a collectible core. The weapon HUD shows `1`, `S`, `L`, or `M`. Achievements and item descriptions are in the archive; enemy entries remain hidden until encountered or their introducing mission has been cleared.

## Build and verification

The content compiler creates banked C data, and GBDK's bank packer places code, art, and text in a 256 KiB MBC5 cartridge. Data readers temporarily switch banks while executing in fixed bank zero. Sprite pixels use CGB VRAM bank 1 so the expanded roster cannot overwrite background lettering.

Verification uses the actual compiled cartridge, public controller input, LCD output, audio, and OAM. Story assertions read the supplied Markdown independently of the compiler. No test writes game RAM, patches the ROM, or injects a victory. `build/verification.json` records the exact tested hash and hardware-testing status.
