# Changes

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
