# Rollback build plan

Updated September 27, 2026. Planning document; game implementation has not started.

## Confirmed direction

- Canonical repository: `/Volumes/zas-hot-22/Github/game-rollback`.
- Development machine: Mini1, the Mac used by this task.
- Engine and language: Phaser + TypeScript, selected by Zach.
- The assistant owns implementation, authoring-tool operation through computer use, asset preparation/integration, level construction, builds, and verification. Zach supplies creative direction and feedback. The plan does not assume additional artists, level designers, programmers, or audio staff.
- The existing story handoff describes 54 missions across four episodes. Its suggested sequence and schemas remain inputs for planning, not independently approved requirements.

## Proposed technical baseline

Start with a local browser game using Phaser, strict TypeScript, Vite, and npm. Use a compatible current stable Phaser release and pin exact dependency versions when scaffolding. The official Phaser Vite TypeScript template supports this combination, but framework, template, and optional visual-editor versions must be checked together before installation.

Use Phaser scenes for loading, menus, briefings, gameplay, debriefs, and HUD presentation. Keep mission definitions, story text, bark pools, and progression rules in validated data. Keep campaign progress distinct from the current mission checkpoint. Browser saves should be versioned, survive a normal reload, and offer export/import so players can back up progress independently of browser storage.

Initial delivery is a build served locally on Mini1 and opened in Chrome. Check Safari and Firefox before promising support. A shareable hosted build is a later deliverable; a hosting provider and public publication have not been selected. Native desktop packaging and Ubuntu testing remain optional later work.

Sources: [official Phaser TypeScript/Vite template](https://github.com/phaserjs/template-vite-ts), [installation and TypeScript support](https://docs.phaser.io/phaser/getting-started/installation), [Phaser Editor platform support](https://docs.phaser.io/phaser-editor/first-steps/download-and-install).

## Milestones and evidence

| Milestone | Work I own | Evidence before expanding scope |
| --- | --- | --- |
| 1. Prove the local workflow | Set up the project on Mini1; open a minimal scene; exercise browser input; assess Phaser Editor for visual authoring | An edit appears in the running game, browser interaction works, and a production build loads locally |
| 2. Find the combat feel | Player movement/aiming, shooting, collision, damage, enemies, camera, readable HUD, and restart using temporary assets | A short arena is enjoyable and readable; performance is measured under a representative load |
| 3. Complete one mission | Briefing, objective, waves, scripted radio, competing barks, death/checkpoint retry, debrief, and save/reload | The entire loop can be played through; retries restore state without duplicating important events |
| 4. Establish presentation | Produce visual directions, choose one with Zach, then create a consistent portrait/environment/sprite pipeline plus sound and music samples | One mission represents the intended appearance and sound, with repeatable asset production |
| 5. Build Episode 1 | Expand reusable mission components, then author Missions 1–12 against the supplied scripts | Episode 1 can be played in order with correct story state, varied objectives, and stable progression |
| 6. Produce the campaign | Build subsequent eras and missions in small batches; add codex, achievements, replay clues, menus, and credits as needed | Every mission is reachable, completable, retryable, and consistent with story gates |
| 7. Prepare release | Cross-browser checks, save recovery, controller checks, audio startup, loading, accessibility, performance, and packaging | A reproducible release candidate with documented supported browsers/devices and known limitations |

Milestones 2 and 4 can overlap enough to show an early style sample, but avoid mass-producing art before its scale, palette, camera, and animation rules are established. The first complete mission is a proposed three-to-five-minute experience, not the whole Episode 1 slice. Production dates should follow observed iteration and asset throughput.

## How I will work through computer use

For each feature, implement or author it, run the game, inspect the visible result, interact with its controls, record concrete failures, and revise. Use visual editors for tasks where scene or animation manipulation helps; use code and data tooling for repeatable systems and content. Confirm the actual editor workflow before making it a dependency. Phaser Editor is a candidate, not an installed or verified tool in this project.

Exercise gameplay through the connected Mac/Chrome computer-use surface. Automated input sequences and repeatable scenarios can verify state and regressions; they do not establish subjective combat quality. Zach's play feedback informs pacing and feel. Report exactly which browsers, devices, and gameplay paths were exercised.

The assistant will prepare and integrate the assets. Raster generation and authoring tools can contribute to portraits and environment concepts; sprites still need consistent scale, silhouettes, frames, transparency, and in-game review. Evaluate an available sound/music/voice workflow with a small sample before promising full production. Maintain source assets and an asset provenance/license record. Full voice acting and any paid service require a separate scope decision.

## First playable scope

- One compact arena, one player vehicle, one weapon, and a small enemy set.
- Keyboard/mouse controls first as a proposed starting point; choose movement and aiming feel in the arena. Preserve an input-action layer for remapping and later controller support.
- One objective, a brief opening exchange, one radio exchange, bark priority, and a debrief.
- One checkpoint, a death/retry loop, pause/restart, and persistent progress with reload verification.
- A coherent temporary visual treatment, readable text, basic effects, and a small sound set.

Use a clearly labeled test arena and placeholder dialogue until the real mission scripts are available. Do not invent canonical story events or rewrite supplied dialogue to hide a gameplay conflict.

## Content rules carried from the handoff

Treat checkpoint restoration as the described rewind mechanism; continuous reverse-time simulation and multiplayer have not been requested. Keep stage directions separate from spoken text and clue tags separate from visible text. Scripted radio exchanges outrank incidental barks. Encode mission-dependent roster, Gus voice/stage, Mae routing, catchphrases, and tally states in data with stable IDs. Scope checkpoint state so retries do not accidentally unlock campaign content or repeat one-time consequences.

## Inputs and decisions still needed

The full Story Bible, Mission List, Episode 1–4 scripts, Bark Library, and On-Screen Text are not yet available in this task. A document link or local folder path will allow the assistant to retrieve them. This blocks faithful mission-content integration, not the temporary combat arena.

Visual style will be chosen from concrete samples; use polished 2D gameplay and illustrated portraits as a proposal, not an approved style. Full versus selective voice, sound/music production method and budget, secret ending inclusion, touch support, controller release scope, supported browsers, and release hosting remain open. Single-player is the working assumption from the handoff.

## Immediate next implementation target

Prove the Mini1 edit/run/inspect/input/build loop, then deliver the small combat arena for feedback. This plan records the direction and does not itself start that implementation.
