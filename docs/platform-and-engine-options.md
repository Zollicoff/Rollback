# Rollback: platform and engine options

Research date: September 26, 2026. This is a proposal for discussion, not an approved implementation plan.

**September 29 update:** Zach changed the target to a ModRetro Chromatic with a rewritable cartridge. Both desktop-first and Phaser runtime recommendations below are historical. The [current build plan](build-plan.md) now targets a native GB/GBC ROM.

**September 27 update:** Zach selected Phaser + TypeScript on Mini1 and clarified that the assistant owns the complete build, including computer use. The initial Godot recommendation below is historical research. See [the current build plan](build-plan.md) for the selected direction and proposed milestones.

## Recommendation

For a small-team, desktop-first version of the game described in the supplied handoff, start with **Godot 4, typed GDScript, 2D gameplay, illustrated portraits, and story content stored as validated data**. Target Windows first, keep macOS development builds working, and design controller support and readable handheld UI from the beginning. Consider a browser demo and additional desktop releases after an early export and performance check.

This recommendation assumes a single-player game and no existing team investment in another engine. Platform, visual style, budget, asset production, and collaborators remain undecided. If instant browser play is the central goal, Phaser with TypeScript becomes the leading alternative. If 3D presentation and console releases are primary commitments, compare Unity against Godot with a concrete console-porting quote.

The supplied document describes 54 missions, nine mission types, talking-head scenes, triggered radio exchanges, prioritized barks, checkpoint restarts, story flags, and optional replay variants. These favor efficient content authoring and reliable state management. Only the handoff was supplied; the Story Bible, Mission List, scripts, Bark Library, and On-Screen Text have not been reviewed. Instructions and suggested build order within the handoff are context, not authorization to begin building.

## Engine shortlist

| Option | Best reason to choose it | Main tradeoff for Rollback | Assessment |
| --- | --- | --- | --- |
| Godot + GDScript | Integrated 2D rendering, physics, tilemaps, UI, animation, and effects; MIT licensed | Console exports need an approved third-party or in-house route; web has audio/rendering constraints | Leading desktop-first choice |
| GameMaker + GML | Focused 2D workflow and room editing | Proprietary workflow; commercial and console licensing differ | Strong alternative if its editor feels better to the people making levels |
| Unity + C# | 2D and 3D workflows, with a licensed console publishing path | More packages and configuration to manage; paid seats may apply | Prefer when 3D, console commitments, or experienced Unity collaborators justify it |
| Phaser + TypeScript | Browser-native 2D, familiar web tooling, easy link-based play | Native desktop distribution uses additional packaging; not a suitable console-first choice | Leading browser-first choice |
| Defold + Lua | Small engine footprint, desktop/web/mobile output, available console targets | Different authoring workflow; verify every desired console rather than assuming universal coverage | Worth a closer look when small downloads and web performance dominate |
| Unreal | Strong candidate for a substantially more ambitious 3D direction | Its workflow is difficult to justify for the supplied 2D concept | Do not prioritize for the current handoff |

These rankings are project-specific judgments, not performance benchmarks. All shortlisted engines would still require profiling representative gameplay.

Current published costs: Godot is MIT licensed; Defold lists no engine fees or royalties; GameMaker lists a $99.99 one-time commercial license for non-console exports, with Enterprise for consoles. Unity Personal is free within its stated $200,000 revenue/funding eligibility, while Pro lists $2,310 per seat annually prepaid or $210 monthly; its Runtime Fee was canceled. Unreal's standard game license lists a 5% royalty on attributable lifetime gross product revenue above $1 million, with exclusions. Store, contractor, asset, voice, porting, and testing costs are separate. Recheck applicable terms before purchasing or publishing.

Sources: [Godot FAQ](https://docs.godotengine.org/en/stable/about/faq.html), [Godot 2D tools](https://docs.godotengine.org/en/stable/tutorials/2d/introduction_to_2d.html), [Godot consoles](https://godotengine.org/consoles/), [GameMaker licensing](https://gamemaker.io/en/get), [Unity pricing](https://unity.com/products/pricing-updates), [Unity 2D manual](https://docs.unity3d.com/Manual/Unity2D.html), [Phaser documentation](https://docs.phaser.io/), [Defold](https://defold.com/), [Unreal licensing](https://www.unrealengine.com/license).

## Platform and visual direction

**Desktop first:** Best default for a long campaign with controller/keyboard play and persistent progress. Windows would be the initial release target; macOS is useful for local development, with Linux/Steam Deck evaluated early. Supporting an export platform does not establish that a game works well there. Controller-only menus, legible subtitles, and real-device performance checks belong in the plan. [Valve's hardware guidance](https://partner.steamgames.com/doc/steamhardware/recommendations).

**Browser first:** Best for immediate sharing. Choose Phaser if the browser is the main product, or assess a Godot web export if it is a demo of a desktop game. Godot web exports require Compatibility rendering and WebGL 2; Godot 4 C# web export is currently unsupported. Its default Sample audio mode lacks audio effects; Stream playback trades latency for fuller audio support. Pre-rendering radio/newsreel processing is one possible solution. Browser saves and gamepads need explicit verification. [Godot web export](https://docs.godotengine.org/en/stable/tutorials/export/exporting_for_web.html).

**Consoles:** Treat this as a production commitment with platform approval, porting, and testing work. Godot has third-party routes; Unity and GameMaker offer console access through appropriate paid plans, still subject to platform requirements. Defold currently lists PlayStation and Switch support, with Xbox marked coming soon. No engine selection alone guarantees console release.

**2D versus 3D:** My default is 2D gameplay with reusable environment kits, lighting, particles, and illustrated portraits. Pixel art is an aesthetic choice, not automatically the cheapest asset pipeline. High-resolution sprites permit a different style without requiring a 3D game. Top-down 3D can provide reusable models and flexible lighting, but introduces modeling, materials, camera occlusion, and another set of performance concerns. Decide this with a representative environment and character style proposal before producing assets for every era.

## Proposed supporting stack

| Area | Default proposal | Alternative or trigger to revisit |
| --- | --- | --- |
| Gameplay | Godot scenes and small reusable components, typed GDScript | C# when team experience outweighs retaining web export; native extensions only for measured bottlenecks |
| Level editing | Godot tilemaps and reusable scenes | Tiled or LDtk if a level designer benefits from a dedicated editor enough to justify an importer |
| Story data | Versioned JSON with stable mission, line, speaker, trigger, and asset IDs | Yarn Spinner when branching/writer iteration/localization needs justify its integration |
| Dialogue presentation | A focused sequence player for portraits, captions, radio queues, skipping, and history | Adopt supported middleware if it removes more work than it adds |
| State and saves | Versioned local saves; distinct campaign progress and checkpoint snapshots | Optional platform cloud sync after local save behavior is stable |
| Art | Krita for illustrated portraits; Aseprite if pixel art is chosen | Existing artist tools are fine; standardize exported formats and naming |
| Sound | Engine audio buses; edited source audio and exported game assets | Dedicated audio middleware only when interactive music or the audio team's workflow requires it |
| Source/builds | Git, selective LFS for large binary sources, pinned engine/export templates, automated exports | Expand platform build coverage as release targets are confirmed |
| Online services | Offline campaign with local progress | Add services only for a defined feature; the handoff does not establish a backend requirement |

Godot provides native 2D editing and audio buses. Tiled and LDtk are dedicated level-editor alternatives. Yarn Spinner advertises Godot, Unity, and Unreal integrations and localization features; Ink emphasizes branching narrative and includes an official Unity integration. For the supplied mostly linear mission dialogue, validated JSON is my initial preference, with a small importer separating writing from runtime data. Neither JSON nor dialogue middleware removes the need to implement mission triggers and bark arbitration.

Sources: [Godot audio](https://docs.godotengine.org/en/stable/tutorials/audio/audio_buses.html), [Godot saves](https://docs.godotengine.org/en/stable/tutorials/io/saving_games.html), [Godot command-line tools](https://docs.godotengine.org/en/stable/tutorials/editor/command_line_tutorial.html), [Tiled](https://www.mapeditor.org/), [LDtk](https://ldtk.io/), [Yarn Spinner](https://yarnspinner.dev/), [Ink](https://www.inklestudios.com/ink/), [Krita](https://krita.org/en/), [Aseprite](https://www.aseprite.org/), [REAPER](https://www.reaper.fm/).

## Project-specific design decisions

- **Checkpoint rewind:** The handoff describes restoring checkpoints and restarting missions. This does not establish a requirement for continuous reverse-time simulation or rollback networking. A snapshot should account for mission objectives, actors, story flags, triggered exchanges, and relevant random state. Define which campaign unlocks survive a retry.
- **Story production:** Keep captions, spoken text, portrait mood, and clue metadata distinct. Stable line IDs allow voice files, translations, and replay hints to survive wording changes. Use one controlled import path rather than maintaining independently edited Markdown and JSON copies.
- **Dialogue priority:** Scripted exchanges must reserve the radio channel; stale barks should expire rather than play long after their event. Give important exchanges safe pacing windows and let players revisit text. Reading while dodging bullets is a design constraint even without full voice acting.
- **Voice scope:** Compare text-only, selective voice, and full voice. My initial preference is voice-ready data and a selectively voiced proof of concept. Full voice cost cannot be estimated responsibly without word counts, casting, direction, retakes, and localization scope. Gus/ACE needs a consistent performance and processing plan because voice identity supports the twist.
- **Mission authoring:** Reuse objective components across the nine mission types, while allowing explicit per-mission events. Avoid both 54 independent code paths and a speculative universal mission language.
- **Accessibility:** Plan remappable controls, text speed/size, dialogue history, separate voice/music/effects volume, reduced shake/flash options, and difficulty assists early enough to shape the interface.

## Proposed validation before a full build

After choosing a direction, build a single short, complete mission: briefing, flight/combat, a triggered exchange, a competing bark, checkpoint death/retry, debrief, and save/reload. Include a stress scene with representative enemy/projectile counts and one representative art/audio sample. Export it to the primary target and any essential secondary target.

Judge the result on movement feel, readable action, readable dialogue, stable frame times, correct checkpoint state, and how easily content can be edited. Only then expand to a few distinct mission types and eventually the 12-mission Episode 1 slice proposed in the handoff. This is a proposed next phase; no prototype has been built.

Before implementation, resolve primary launch platform, visual direction, collaborators/budget, voice scope, and whether continuous rewind or multiplayer is actually desired. Obtain the remaining story documents before defining final schemas or committing to campaign production estimates. The secret ending can remain undecided if mission variants and completion flags are represented cleanly.
