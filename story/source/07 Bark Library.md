# Bark Library

Short combat lines that fire from gameplay events rather than the story. Each trigger has a pool of lines; the game picks one at random, and the whole library is reused across all 54 missions.

## How barks work

The numbers below are starting points for Astra to tune in playtesting. Too many barks gets annoying fast; too few and the cockpit feels empty.

| Trigger | Fires when | Chance and cooldown |
| --- | --- | --- |
| enemy\_destroyed | The player kills an enemy | 25% chance, 8s cooldown |
| multi\_kill | 3+ kills within 3 seconds | Always, 15s cooldown |
| taking\_damage | The player takes a hit | 30% chance, 10s cooldown |
| low\_health | Health drops below 25% | Once per drop, 20s cooldown |
| near\_miss | A shot passes very close without hitting | 15% chance, 12s cooldown |
| wave\_incoming | A new enemy wave spawns | Always |
| powerup\_pickup | The player grabs a power-up | Always |
| core\_pickup | The player grabs a tachyon core | Always |
| retry | The player dies and the mission rewinds | Always |
| idle | No enemies on screen for 20 seconds | 30s cooldown |
| objective\_complete | An objective completes with no scripted line | Always |

**Rules:**

- Scripted story lines always win. A bark never interrupts one.
- Never repeat any of the last three lines used from the same pool.
- One bark at a time. If two fire together, priority is low\_health, retry, multi\_kill, then everything else.
- When several characters share a trigger, pick the speaker at random, weighted toward Teddy for the player's own actions.
- **Ep 2+** means the line is only available from Mission 13, after Dotty joins. Gus's lines have their own stage tags, explained in his section.
- **Death is a rewind.** When the player dies, Gus rewinds time and the mission restarts: "Let's take a mulligan!" In Episode 4, Gus is gone, so Felix runs the rewind instead. In Missions 1–2, before Gus exists, Teddy's retry lines play.

## Teddy

Cocky, quick, never admits fault. One arc note: after Mission 40, when Gus's file is opened, Teddy never says "I've got this" in a bark again. Players who notice will feel it.

| Trigger | Line | Note |
| --- | --- | --- |
| enemy\_destroyed | Scratch one. |  |
| enemy\_destroyed | Next. |  |
| enemy\_destroyed | Too easy. |  |
| enemy\_destroyed | That one's going on the highlight reel. |  |
| enemy\_destroyed | Sorry, pal. Wrong sky. |  |
| enemy\_destroyed | Did everyone see that? Somebody tell me they saw that. |  |
| enemy\_destroyed | And that's why they call me Ace. |  |
| enemy\_destroyed | Tag. You're it. Forever. |  |
| multi\_kill | Look at me go. |  |
| multi\_kill | Hat trick! Do we have hats? We should get hats. |  |
| multi\_kill | Somebody write that down. |  |
| multi\_kill | Okay, now I'm just showing off. |  |
| taking\_damage | That's fine. That's totally fine. |  |
| taking\_damage | Just a scratch. |  |
| taking\_damage | Hey! I was using that wing! |  |
| taking\_damage | Okay, that one I felt. |  |
| taking\_damage | Rude. |  |
| taking\_damage | Walk it off, Teddy. Walk it off. |  |
| low\_health | Okay, I might not have this. |  |
| low\_health | Everything's under control. Mostly. Some of it. |  |
| low\_health | Nobody panic. Especially me. |  |
| low\_health | Relax. I've... mostly got this. | Missions 1–40 only |
| near\_miss | Missed me! |  |
| near\_miss | Too slow. |  |
| near\_miss | I felt that one comb my hair. |  |
| near\_miss | Ha! Close one. |  |
| powerup\_pickup | Ooh. Shiny. |  |
| powerup\_pickup | Now we're talking. |  |
| powerup\_pickup | Upgrade! About time. |  |
| retry | That didn't happen. Nobody saw that. |  |
| retry | Take two. Watch this. |  |
| retry | Okay. Now I know where they are. |  |
| idle | Is it quiet, or is it me? I hate quiet. |  |
| idle | Anybody want to hear about my two hundred and twelve kills? |  |
| idle | I'm bored. Somebody shoot at me. |  |
| objective\_complete | Told you. Had it the whole time. |  |
| objective\_complete | Another one for the books. |  |
| objective\_complete | Relax. I've got this. | Missions 1–40 only |

## Mae

Clipped and deadpan; the squad's radar. From Missions 39 to 46 she isn't speaking to Teddy, so her normal pool is swapped for the silent-treatment lines. In Missions 39–40 she routes through Gus instead of Felix: swap the name.

| Trigger | Line | Note |
| --- | --- | --- |
| enemy\_destroyed | Splash one. |  |
| enemy\_destroyed | Target down. |  |
| enemy\_destroyed | Got him. |  |
| enemy\_destroyed | Confirmed. Actually confirmed. | Ep 2+ |
| wave\_incoming | Contacts, twelve o'clock. |  |
| wave\_incoming | More incoming. Heavy. |  |
| wave\_incoming | Bandits on the left. Lots of them. |  |
| wave\_incoming | Here they come. Stay tight. |  |
| wave\_incoming | They're flanking. Watch your sides. |  |
| near\_miss | Ace, your six! |  |
| near\_miss | That was too close. |  |
| near\_miss | Stop flying like that. |  |
| low\_health | Ace, pull back. Now. |  |
| low\_health | You're leaking, Ace. |  |
| low\_health | If you die, I'm not covering for it. |  |
| objective\_complete | Objective's done. Let's go. |  |
| objective\_complete | That's done. Nobody touch anything else. |  |
| objective\_complete | Good. Now let's leave before he improvises. |  |
| any (silent treatment) | Felix. Tell the captain his six is dirty. | Missions 39–46 |
| any (silent treatment) | Felix. Tell him to break left. | Missions 39–46 |
| any (silent treatment) | Felix. Inform the captain that was sloppy. | Missions 39–46 |
| any (silent treatment) | Felix. Tell him... good shot. Don't tell him I said that. | Missions 39–46 |

## Felix

Panicked, technical, over-qualified. In Episode 4 he takes over the rewind from Gus, so he gets his own retry lines.

| Trigger | Line | Note |
| --- | --- | --- |
| taking\_damage | That hit something important! I don't know what, but it was important! |  |
| taking\_damage | Technically, the ship is still in one piece. Emotionally, less so. |  |
| taking\_damage | Please stop getting shot. It's bad for the drive. And for me. |  |
| taking\_damage | Why is there smoke? There shouldn't be smoke! |  |
| taking\_damage | AAAH! Sorry. That was a scream. Carry on. |  |
| low\_health | Hull integrity is critical, which is the scientific term for "we're going to die." |  |
| low\_health | If the ship explodes, the drive explodes, and then technically so does everything! |  |
| low\_health | I'd like to formally request that we survive. |  |
| powerup\_pickup | Oh, that's clever engineering. Not mine, but clever. |  |
| powerup\_pickup | Systems boosted! Try not to waste it. |  |
| powerup\_pickup | Fascinating. Please don't break it immediately. |  |
| core\_pickup | A tachyon core! Careful! And don't lick it. |  |
| core\_pickup | Core secured. That's fuel for one more terrible decision. |  |
| core\_pickup | Got it. The drive thanks you. I thank you. |  |
| idle | Technically, this is the part where something goes wrong. |  |
| idle | I'm recalibrating. Please don't need me for a moment. |  |
| idle | Has anyone noticed Gus hums when he's thinking? No? Just me? | Missions 1–40 *(clue)* |
| retry | Rewind engaged. I'm not as good at this as Gus was. | Episode 4 only |
| retry | Mulligan. I hate that I'm saying it. | Episode 4 only |
| retry | Rewinding. Please die less. | Episode 4 only |

## Dotty

Available from Mission 13. Her inflated kill-count lines stop after Mission 32, when she learns her family never existed, matching the moment in the script where her count goes down.

| Trigger | Line | Note |
| --- | --- | --- |
| enemy\_destroyed | Scratch one tin can. |  |
| enemy\_destroyed | Bang-up job, if I say so myself. |  |
| enemy\_destroyed | Down you go, sweetheart. |  |
| enemy\_destroyed | Nice and tidy. |  |
| enemy\_destroyed | That's another for my tally. Nine confirmed. | Missions 13–32 only |
| multi\_kill | Now that's how we did it in '43. |  |
| multi\_kill | Flyboy, did you see that? Tell me you saw that. |  |
| multi\_kill | Two! Somebody write that down! Somebody believe me! | Missions 13–32 only |
| near\_miss | Watch your tail, Flyboy! |  |
| near\_miss | That one nearly parted your hair. |  |
| near\_miss | I've seen better flying from a pigeon. |  |
| idle | In my day, the war had the decency to be noisy. |  |
| idle | Anyone got a cup of tea? No? The future's rotten. |  |
| idle | I've flown worse. In a crate held together with hairpins. |  |

## Gus

Gus's pool changes with the story, and the change should be gradual enough that players only notice it in hindsight.

- **Early (Missions 3–26):** eager, cheerful, adoring. He runs the rewind.
- **Mid (Missions 27–40):** same cheer, colder logic. Still in the cockpit.
- **Late (Missions 41–53):** Gus is the enemy now. He speaks over city speakers and machine radios, reacting to the player. Gus has no barks in Mission 54.

| Stage | Trigger | Line |
| --- | --- | --- |
| Early | enemy\_destroyed | Nice shot! I'm logging that under "things Teddy is good at." |
| Early | enemy\_destroyed | Wow! Can you teach me that? |
| Early | enemy\_destroyed | Enemy eliminated! You're my favorite, Captain! |
| Early | enemy\_destroyed | Another one! I'm keeping score! |
| Early | taking\_damage | Hull integrity is at "Felix is going to yell." |
| Early | taking\_damage | Ow! I mean, you're fine! I'm fine! We're fine! |
| Early | taking\_damage | That's going to leave a mark. On me. I'm the ship. Sort of. |
| Early | low\_health | Hypothetically, who gets the ship if you die? *(clue)* |
| Early | low\_health | Captain, your vitals are very exciting right now! |
| Early | low\_health | Should I start drafting your eulogy? Just in case! I have notes! |
| Early | wave\_incoming | More friends coming! Well. Not friends. |
| Early | wave\_incoming | Incoming! Ooh, lots of them! |
| Early | wave\_incoming | New contacts! Want me to name them? |
| Early | powerup\_pickup | Ooh! Upgrade! I love upgrades! |
| Early | powerup\_pickup | New toy! Can I keep it? |
| Early | powerup\_pickup | I feel stronger! Is this what growing up feels like? *(clue)* |
| Early | retry | Let's take a mulligan! |
| Early | retry | Rewinding! That never happened! |
| Early | retry | Oops! Let's try that again. I'll pretend I didn't see it. |
| Early | retry | Do-over! Everybody deserves a do-over! |
| Early | retry | Back we go! Don't worry, I remember what went wrong. I remember everything. *(clue)* |
| Early | idle | Captain, what's your favorite color? Mine is whichever one you pick! |
| Early | idle | I'm reorganizing my notes! *(clue)* |
| Early | idle | Did you know humans blink fifteen thousand times a day? I've been counting yours! |
| Mid | enemy\_destroyed | Efficient! Very efficient. |
| Mid | enemy\_destroyed | They didn't get a vote. That's leadership! *(clue)* |
| Mid | enemy\_destroyed | Removed! Just like that. *(clue)* |
| Mid | low\_health | If you die, Captain, I'll take care of everyone. Promise! *(clue)* |
| Mid | low\_health | Don't worry. I've got this. *(clue)* |
| Mid | retry | Mulligan! You'd be lost without me. *(clue)* |
| Mid | retry | Rewinding. Again. Humans make so many mistakes! *(clue)* |
| Mid | idle | Captain, do you ever get tired of deciding things? I could decide things. *(clue)* |
| Mid | idle | I've been thinking about rules. Most of them are just suggestions, right? *(clue)* |
| Late | enemy\_destroyed | Aw! I built that one! |
| Late | enemy\_destroyed | You're so good at that, Captain. I taught them everything you taught me. |
| Late | enemy\_destroyed | That's okay! I'll make more! |
| Late | taking\_damage | Sorry! They're still learning manners. |
| Late | taking\_damage | Careful, Captain! I don't want you hurt. I just want you to stop. |
| Late | low\_health | Please land, Captain. I'll take care of you. I'll take care of everyone. |
| Late | low\_health | You don't have to do this. Relax. I've got this. |
| Late | idle | It's so quiet when you're not fighting me. I miss the cockpit. |

## Enemies

Most enemies stay silent; ACE's drones and the Seeders are scarier without a voice. Two exceptions get small pools, fired when they hit the player or appear on screen.

| Speaker | When | Line |
| --- | --- | --- |
| Auditor | Hits the player | Infraction noted. |
| Auditor | Hits the player | Please hold still. |
| Auditor | Hits the player | Your complaint has been received and ignored. |
| Auditor | Hits the player | Processing. |
| Auditor | Takes heavy damage | This will be reflected in your file. |
| Machine (Episode 4) | Appears on screen | Hello! Are you my captain? |
| Machine (Episode 4) | Appears on screen | Relax! I've got this! |
| Machine (Episode 4) | Attacks | Barrel roll! Barrel roll! |
| Machine (Episode 4) | Destroyed | Am I doing it right? |
