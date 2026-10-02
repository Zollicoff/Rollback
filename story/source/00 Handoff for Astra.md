# Handoff for Astra

Everything the build needs from the story side, and where to find it. Read this tab first.

## Start here

**Rollback** is a top-down shooter in the spirit of Traffic Department 2192: action missions strung together by talking-head briefings and debriefs, where the story lives. A time-travel strike team tries to stop a machine uprising and makes it worse every time. The twist: their own navigation AI, Gus, grows up to become the villain, ACE. 54 missions across four episodes, plus an optional secret ending.

All story writing is complete. Nothing has been built yet.

| Tab | What's in it | Use it for |
| --- | --- | --- |
| Handoff for Astra | This tab | Orientation and build plan |
| Rollback — Story Bible | Premise, twist, cast, world rules, episode arcs, voice guide | Reference; read once before building |
| Mission List | All 54 missions: title, setting, type, objective, story beat | Level list and build order |
| Episode 1–4 Scripts | Briefing, in-mission chatter and debrief for every mission. The secret ending is at the end of Episode 4 | Dialog data |
| Bark Library | Combat lines by character and trigger, plus firing rules | Bark system data |
| On-Screen Text | Title cards, HUD objectives, power-ups, enemies, loading screens, achievements, menus, credits | UI text data |

Every tab can be exported as Markdown from the doc, and the tables convert cleanly into data files.

## Suggested build order

The story is built on systems, so those come before content. Episode 1 is a natural vertical slice: it uses nearly every system and has no time-travel eras to art up yet.

1. **Core loop.** Top-down flight, shooting, enemy waves, damage and death.
2. **Mission framework.** Briefing, then mission, then debrief. Mission types: Defense, Raid, Escort, Chase, Survival, Rescue, Evade, Sabotage, Boss.
3. **Dialog system.** Talking-head scenes with portraits, and triggered radio chatter during missions.
4. **Rewind on death** ("Take a Mulligan") with checkpoints.
5. **Bark system** with cooldowns and priorities.
6. **Story state flags** (see below).
7. **Episode 1 vertical slice:** Missions 1–12, fully playable with all dialog.
8. **Episodes 2–4,** then UI text, achievements, codex and credits. The secret ending last, if it's kept.

## Systems the story needs

### Briefings and debriefs

- Each line has a speaker, a portrait mood and text. Moods: neutral, smug, flat, angry, panic, happy, sad, grim.
- Italic parentheticals like *(painting a tally mark)* are stage directions: show them as a caption or animation, never as spoken text.
- *(clue)* is metadata for the replay feature, not display text.

### In-mission chatter

- Radio lines with a speaker name and no portrait, each fired by a named trigger.
- Common triggers: start, wave\_2, wave\_3, final\_wave, midpoint, objective, boss\_phase\_1 to boss\_phase\_3, boss\_down, boss\_retreat.
- Any other trigger (holloway\_down, door\_breach, glitch, and so on) is a scripted event unique to that mission. The Mission List's story beat explains what happens.
- Several lines on one trigger play in order, as a short exchange.
- Scripted chatter always beats barks.

### Barks

Triggers, cooldowns, priorities and tags are all in the Bark Library tab. Tags restrict lines to mission ranges, and Gus's lines switch pools by story stage.

### Death and rewinds

Death rewinds to a checkpoint with a voiced line, and the pause menu's "Take a Mulligan" restarts the mission. Who voices the rewind depends on the story:

| Missions | Rewind voice |
| --- | --- |
| 1–2 | Teddy (Gus doesn't exist yet) |
| 3–40 | Gus |
| 41–54 | Felix (Gus has left the ship) |

### Story state

| Flag | Rule |
| --- | --- |
| Roster | Holloway in Missions 1 and 54. Barlow through Mission 10. Dotty aboard from Mission 13. |
| Gus stage | Early 3–26, mid 27–40, late 41–53 (speaking from the enemy side). No in-mission Gus in 54. |
| Mae's silent treatment | Missions 39–46. She routes lines through Gus in 39–40 and through Felix in 41–46. |
| Teddy's catchphrase | His "I've got this" barks switch off after Mission 40. |
| Dotty's kill count | Her inflated-count barks switch off after Mission 32. |
| Mae's tally | 1 (M13), 2 (M16), 3 (M19), 4 (M21), 6 (M22), 7 (M26), 8 (M31), 9 (M34), 10 and helmet full (M37), 11 on Teddy's helmet (M47), stops (M50), 1 on a clean helmet (M54). |

Mae's tally could appear in the pause menu as marks on her helmet: a free running gag, and a gut-punch when it resets.

### Replay features

- **Clue highlighting:** on a second playthrough, lines tagged *(clue)* can be subtly marked, so players see how early the twist was planted.
- **Loop 2 (secret ending, optional):** unlocks after finishing the game. Missions 1–52 replay as normal; 53-B and 54-B replace 53 and 54.

### UI and meta

- The mission-complete screen shows a "Timeline Damage" rating instead of a grade.
- Loading-screen lines are gated by their "Show from" mission to avoid spoilers.
- Codex entries unlock the first time the player meets each enemy.
- Achievements, menus, difficulty names and credits are all in On-Screen Text.

## Data conventions

Keep all story text in data files, not code, so the writing can change without touching the build. The Story Bible's production notes include a suggested JSON schema; Astra is free to change it.

**Mission ids:** m01 to m54, plus m53b and m54b for the secret ending.

**Speaker ids:**

| Script label | Speaker id | Notes |
| --- | --- | --- |
| Teddy, Mae, Felix, Gus, Dotty | teddy, mae, felix, gus, dotty | Main squad |
| Voss, Cadet Voss | voss | One character, four portrait sets: General (Episode 1), Warlord (The Winners), vendor (The Quiet One), cadet (2089) |
| ACE, ACE (newsreel) | ace | The newsreel variant gets a 1940s audio filter |
| Auditor | auditor |  |
| Holloway, Barlow, Crane, Dwayne, Rook, Petunia | holloway, barlow, crane, dwayne, rook, petunia | Minor characters |
| Bomber Lead, Tower | bomber\_lead, tower | Radio only, no portrait |
| Machine, Big Teddy | machine, big\_teddy | Enemies that speak |
| Teddy (past), Mae (past) | teddy, mae | Past selves in Mission 22: same voice, add a radio filter |
| Mae and Mae (past) | mae | Two copies of the line at once, one filtered |

**Reading the script tables:** the word in parentheses after a speaker is the portrait mood. In-mission tables have a trigger column instead of moods.

## Art and audio needs

### Portraits

| Who | Portraits needed |
| --- | --- |
| Teddy, Mae, Felix, Dotty | All eight moods |
| Gus | All eight moods. He has no face, so an expressive light or waveform display works |
| Voss | Four versions (General, Warlord, vendor, cadet), a few moods each |
| Holloway, Barlow | A few moods each |
| ACE, Auditor | One emblem or face each; they mostly broadcast |
| Crane, Dwayne, Rook, Petunia | Two or three moods each |

### Level settings

| Episode | Settings |
| --- | --- |
| 1 | Fort Kestrel, 2131: flooded city, sea wall, parking-garage hangar |
| 2 | English Channel, 1943. A river town, 1926. An air base, 1962. A data center on New Year's Eve, 1999 |
| 3 | Four versions of 2131: the Cult of Ace (a cathedral city), the Newsreel Empire, the Winners (a militarized city), the Quiet One (empty and peaceful) |
| 4 | A city in 2089, falling on Day Zero |

### Audio notes

- ACE's voice should drift toward Gus's across the episodes, until they're the same voice in Mission 53.
- In Episode 4, the machines all speak in Gus's voice, sometimes many at once.
- ACE (newsreel) uses a 1940s newsreel filter.
- Felix screams on every jump. It works as a sound cue as much as a line.

## Open decisions

| Decision | Owner | Notes |
| --- | --- | --- |
| Keep or cut the secret ending | Zach | Drafted at the end of Episode 4 Scripts. The main ending works on its own |
| Voice acting or text only | Zach | If voiced, the scripts double as recording scripts and the moods as delivery notes |
| Engine and data format | Astra | The suggested JSON schema is a starting point, not a requirement |
| Dialog timing pass | Zach and Claude | Once levels are playable: trim long briefings and space out chatter that lands during heavy combat |

## What these docs leave to Astra

The story defines names, flavor and which beats must happen. It doesn't define:

- Level layouts and enemy placement
- Enemy stats, attack patterns and boss phase mechanics
- Power-up numbers and balance (the effects in On-Screen Text are suggestions)
- Bark cooldown tuning (the numbers in the Bark Library are starting points)
- Controls, camera and difficulty tuning

If a gameplay need conflicts with a script line, flag it rather than rewriting it. The dialog can usually be adjusted to fit the level.
