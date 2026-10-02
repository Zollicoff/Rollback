# Changes

## 0.4.0-campaign — October 1, 2026

- Integrate all nine supplied story documents: 54 missions, four episodes, complete dialogue and story events, the main ending, and unlocked Loop 2.
- Add paginated dialogue, character portraits/moods, stage-direction captions, replay clue highlighting, and story-aware combat barks.
- Add ten terrain themes, the full enemy roster, eight pickup effects, four difficulties, an archive, and session achievements.
- Implement escape and pursuit variants, radar, duplicate ships, rescue cages, seven-site carrier raid, scripted failed interceptions, the dual-boss roof fight, peaceful targets, and Rule Zero upload defense.
- Preserve two-axis worlds, padded camera movement, fixed HUD, colored waypoints, and complete checkpoint restoration. Separate movement cadence from rendering duration.
- Use six-digit progress codes and a banked 256 KiB cartridge; retain the no-SRAM requirement. Put sprites in their own CGB video-memory bank.
- Verify authored briefings/debriefs against source Markdown through the actual ROM display. Package only the exact tested binary.
- Protect the first radio page from firing during its transition, and checkpoint between the two opponents in Mission 26.


## 0.3.1-waypoints — local build

- Add eight-direction objective and nearest-enemy arrows, clamped inside the playfield and colored by purpose.
- Require fresh Start presses for results, debriefs, completion, and checkpoint recovery; A no longer skips end screens.
- Reserve two sprite slots for navigation and display at most two concurrent impact effects.
- Add a color legend to the field manual.

## 0.3.0-world — September 29, 2026

- Expand the vertical dimension too: worlds are now 1,600 × 1,440 pixels, ten times the original width and height.
- Follow the ship on both axes, including diagonals, with horizontal and vertical padding and clamping at all four boundaries.
- Stream both rows and columns while keeping the HUD in its own fixed bottom panel.
- Distribute terrain and objectives across a 10-by-10 grid; show both target directions and the current column/row.
- Restore both camera axes on pause/resume and death checkpoints. Extend travel timers for the larger area.


## 0.2.0-scrolling — September 29, 2026

- Expand every world from one screen to ten screens wide (1,600 pixels).
- Add a following camera with a central resting band and 56-pixel margins before either screen edge.
- Stream terrain as you explore; keep the HUD fixed and clip distant sprites before conversion to hardware coordinates.
- Spread pods, relays, convoy waypoints, navigation gates, and the boss across the world. Add sector numbers and target/home direction arrows; allow more time for travel objectives.
- Restore world position and camera on death checkpoints and pause/resume.
- Complete all twelve drills through emulator controller input. Verify full-width travel and return, both boundaries, register wrap, camera reversal, fixed HUD scanlines, distant-object clipping, and a checkpoint beyond the first screen.

The repository is now public at `Zollicoff/Rollback`. This remains a training prototype; authored Episode 1 story integration and physical Chromatic validation are pending.

## 0.1.0-training — September 29, 2026

First playable native Game Boy Color build. Adds the flight/combat loop, nine mission types, twelve temporary training scenarios, briefings/debriefs, aim lock, boost, cover, repair pickups, a three-phase boss, pause/restart, full-state checkpoints, progress passwords, original pixel art, and synthesized audio.

Includes a reproducible local toolchain, editable scenario data, executable-ROM tests, and a checksum-bearing download bundle for the remote MacBook Pro.

Scope: Episode 1 systems prototype. Canonical missions 1–12 and character scripts are awaiting the source documents. Physical Chromatic cartridge validation remains outstanding. Episodes 2–4 are deferred.
