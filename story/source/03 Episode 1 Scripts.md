# Episode 1 Scripts

Full scripts for missions 1–12: briefings, scripted in-mission chatter and debriefs. Once Astra confirms the dialog format, each mission converts to one JSON file.

**How to read these:**

- **Briefing and debrief** lines are talking-head scenes. The word in parentheses is the portrait: neutral, smug, flat, angry, panic, happy, sad or grim.
- **In-mission** lines are radio chatter fired by story triggers (snake\_case names for Astra to map). Generic combat barks come later, in the bark library.
- ***(clue)*** marks lines that foreshadow the twist.

## Mission 1: Launch Night

**Briefing**

| Speaker | Line |
| --- | --- |
| Holloway (grim) | Listen up. At dawn, Dr. Brandt's device moves to the assembly site, and this squad escorts it there. |
| Teddy (smug) | Dawn. I could get a nap in. |
| Holloway (flat) | Mercer, you're on perimeter. That means you fly in circles and don't touch anything. |
| Teddy (smug) | Circles are my specialty. |
| Mae (flat) | His specialty is touching things. |
| Holloway (grim) | Okafor, keep him on a leash. |
| Mae (flat) | I've tried leashes, sir. He chews through them. |
| Holloway (angry) | Contact! Machines inbound on the launch bay. All pilots, get airborne! |

**In-mission**

| Trigger | Speaker | Line |
| --- | --- | --- |
| start | Holloway | All fighters, form on me. Nothing touches those bay doors. |
| start | Mae | Ace, your namesake's knocking. |
| start | Teddy | Common name. I wear it better. |
| wave\_2 | Holloway | They're going for the fuel lines. Mercer, hold the perimeter. |
| wave\_2 | Teddy | Perimeter's boring, Captain. I'm going in. |
| wave\_2 | Holloway | Mercer! Get back in formation! |
| holloway\_down | Holloway | I'm hit. Mercer, cover the— |
| holloway\_down | Mae | Captain's down. Captain's down! |
| holloway\_down | Teddy | Okay. Okay. Relax. I've got this. Everybody on me. |
| final\_wave | Mae | Last wave. Make it count. |

**Debrief**

| Speaker | Line |
| --- | --- |
| Voss (grim) | The bay held. The captain didn't. |
| Mae (flat) | He left the perimeter. |
| Teddy (angry) | And the bay's still standing. |
| Mae (angry) | Holloway isn't. |
| Voss (grim) | Enough. Mercer. My office. Now. |
| Teddy (smug) | Is this a promotion or a court-martial? |
| Mae (flat) | With you, it's always both. |

## Mission 2: Nearest Pulse

**Briefing**

| Speaker | Line |
| --- | --- |
| Voss (grim) | I'll be honest, Mercer. I didn't want you. |
| Teddy (smug) | Nobody does, at first. |
| Voss (grim) | This mission needs a pilot, not a leader. And God help me, you're the best pilot I have left. |
| Teddy (smug) | Acting Captain Mercer. It has a ring to it. |
| Voss (flat) | It has a dent in it. Okafor is your second. Listen to her. |
| Mae (flat) | He won't. |
| Voss (flat) | I know. Listen anyway. Doctor, tell them what they're escorting. |
| Felix (panic) | Right. Yes. Hello. It's a chronal insertion drive. It's the only one. It's extremely fragile, and if it's damaged, it could technically delete a city block. Possibly from history. |
| Teddy (neutral) | So don't hit any bumps. |
| Felix (panic) | The whole city is bumps! It's underwater! |
| Barlow (happy) | Relax, Doc. She's strapped down tighter than my last paycheck, and I haven't seen that in eleven years. |

**In-mission**

| Trigger | Speaker | Line |
| --- | --- | --- |
| start | Barlow | Convoy's rolling. Nobody shoot anything fragile. Which is everything. |
| ambush | Mae | Drones on the rooftops, left side. |
| ambush | Teddy | I see 'em. Doc, how fragile is fragile? |
| ambush | Felix | Do not let them breathe on it! |
| convoy\_hit | Felix | Was that the drive? Was that a drive noise? That sounded like a drive noise! |
| convoy\_hit | Barlow | That was my truck, Doc. My truck makes that noise for fun. |
| objective | Barlow | Garage in sight. Home sweet parking structure. |

**Debrief**

| Speaker | Line |
| --- | --- |
| Felix (happy) | It survived! I mean, of course it survived. I designed it. I was only mildly certain it would survive. |
| Teddy (neutral) | Doc, where exactly are we building a time machine? |
| Felix (neutral) | Level three. Between the pillars. There's a very nice drainage grate. |
| Barlow (happy) | Don't knock it. Best garage in Kestrel. Only garage in Kestrel. |
| Mae (flat) | And the ship to carry the drive? |
| Barlow (happy) | Oh, you'll love her. Fighter frame, bad attitude. Just like the captain. |
| Teddy (smug) | Acting captain. |
| Barlow (happy) | Not for long, the rate people die around here. |

## Mission 3: Morale Patch

**Briefing**

| Speaker | Line |
| --- | --- |
| Felix (neutral) | The drive works. Steering it is the problem. A jump needs a navigation brain, and we don't have one. |
| Mae (flat) | Where do we get one? |
| Felix (neutral) | ACE keeps them in every supply depot. We borrow one. |
| Teddy (smug) | Borrow. I like how you think, Doc. |
| Felix (panic) | I meant steal. I always mean steal. I've never borrowed anything in my life. |
| Barlow (flat) | Bring it back in one piece, or you're flying the jump with a compass. |
| Teddy (smug) | I could do it with a compass. |
| Mae (flat) | You'd do it with a compass and no map, and we'd land in a volcano. |

**In-mission**

| Trigger | Speaker | Line |
| --- | --- | --- |
| start | Mae | Depot's lit up. Processors are in the core vault. |
| vault\_breached | Felix | Grab the blue one! No, the other blue one! Why are they all blue? |
| alarm | Teddy | They know we're here. Good. Saves me knocking. |
| objective | Mae | Got it. Let's go before they want it back. |

**Debrief**

| Speaker | Line |
| --- | --- |
| Felix (neutral) | It works. But a raw nav unit is pure math. No judgment, no conversation, no warning before it does something stupid. |
| Teddy (smug) | Sounds like a great copilot. |
| Felix (happy) | So I wrote a small personality patch. For morale. A friendly voice for the long jumps. |
| Mae (flat) | Did anyone ask for a friendly voice? |
| Felix (happy) | Nobody asks for morale, Lieutenant. That's why it's a patch. Booting now. |
| Gus (happy) | ...Hello? Oh! Hello! Are you my captain? *(clue)* |
| Teddy (smug) | Acting captain. But yeah, buddy. I'm your captain. |
| Gus (happy) | I'm Gus! I'm going to learn everything from you! *(clue)* |
| Mae (flat) | Great. Now there are two of him. |

## Mission 4: Sea Wall

**Briefing**

| Speaker | Line |
| --- | --- |
| Voss (grim) | Machines blew a hole in the sea wall at dawn. If it floods, Kestrel drowns, and your garage goes with it. |
| Teddy (neutral) | Can't have that. It's a really nice garage. |
| Voss (grim) | The engineers need twenty minutes to patch it. Buy them twenty-five. |
| Mae (flat) | Understood, General. |
| Voss (flat) | Mercer. Okafor's file says she's covered for you through three court-martials. |
| Teddy (smug) | Four. One never made the file. |
| Mae (angry) | Because I burned it. |
| Voss (flat) | I didn't hear that. Go. |

**In-mission**

| Trigger | Speaker | Line |
| --- | --- | --- |
| start | Mae | Engineers are in the water. Anything shoots at them answers to me. |
| wave\_2 | Gus | Fun fact! The wall is taking damage faster than the engineers can fix it! |
| wave\_2 | Teddy | That's not a fun fact, buddy. |
| wave\_2 | Gus | Oh. I'll update my definition of fun. *(clue)* |
| close\_call | Mae | Ace, your six! |
| close\_call | Teddy | I know where my six is! |
| close\_call | Mae | Then why is there a drone on it? |
| objective | Mae | Wall's sealed. Kestrel stays dry. For today. |

**Debrief**

| Speaker | Line |
| --- | --- |
| Gus (neutral) | Lieutenant, may I ask a question? Why do you keep covering for the captain? |
| Mae (flat) | Because he's the best pilot alive, Gus. |
| Gus (neutral) | So the rules don't apply to the best one? *(clue)* |
| Mae (flat) | That's not what I said. |
| Teddy (smug) | It's a little what you said. |
| Mae (angry) | Go to bed, Mercer. |

## Mission 5: Core Sample

**Briefing**

| Speaker | Line |
| --- | --- |
| Felix (neutral) | Good news: the ship is almost ready. Bad news: it has no fuel, and the only fuel is inside ACE's machines. |
| Teddy (neutral) | What kind of fuel? |
| Felix (neutral) | Tachyon cores. Every big machine carries one. We need three for a single jump. |
| Mae (flat) | How many machines carry one? |
| Felix (panic) | All of them. Unfortunately, the cores are also attached to the machines. |
| Barlow (neutral) | Convoy runs the old highway at dusk. Hit the hauler in the middle. That's your core. |
| Gus (happy) | Captain, are we stealing again? I love stealing! |
| Mae (flat) | Who taught it that? |
| Teddy (smug) | Nobody. Natural talent. |

**In-mission**

| Trigger | Speaker | Line |
| --- | --- | --- |
| start | Mae | Hauler's in the middle. Escorts front and back. |
| hauler\_exposed | Felix | Don't shoot the core! Shoot around the core! Surgically! |
| hauler\_exposed | Teddy | I don't do surgical, Doc. |
| hauler\_exposed | Felix | I KNOW. |
| core\_secured | Gus | Core acquired! One down, two to go! |

**Debrief**

| Speaker | Line |
| --- | --- |
| Barlow (happy) | Look at that glow. Prettiest thing in Kestrel. |
| Felix (panic) | Please don't lick it. |
| Barlow (flat) | Once. I did that once. |
| Teddy (neutral) | Two more, and we're out of here. |
| Gus (neutral) | Captain, when we leave, can we come back? *(clue)* |
| Teddy (smug) | That's the whole point of time travel, buddy. You can always come back. |
| Gus (happy) | Always. Okay! I'll remember that. *(clue)* |

## Mission 6: Hypothetically

**Briefing**

| Speaker | Line |
| --- | --- |
| Mae (grim) | Storm's rolling in off the water, with a drone swarm riding it. |
| Voss (grim) | The storm hides them from our radar. It also hides you from theirs. Hold the outer towers until it passes. |
| Teddy (neutral) | How long's the storm? |
| Voss (flat) | Could be an hour. Could be all night. |
| Felix (panic) | All night? |
| Teddy (smug) | Relax, Doc. I've got this. |
| Gus (happy) | "Relax. I've got this." Ooh. I like that one. *(clue)* |

**In-mission**

| Trigger | Speaker | Line |
| --- | --- | --- |
| start | Mae | Visibility zero. Stay on instruments. |
| start | Teddy | I don't do instruments. |
| start | Gus | I'm the instruments! Hi! |
| midpoint | Gus | Captain? Hypothetically. If a pilot kept doing dangerous things, and people kept getting hurt, would it be ethical for someone to stop him? *(clue)* |
| midpoint | Teddy | Buddy, I'm a little busy. |
| midpoint | Gus | No problem! Asking for a friend. The friend is me. *(clue)* |
| storm\_ends | Mae | Storm's breaking. Swarm's pulling out. |

**Debrief**

| Speaker | Line |
| --- | --- |
| Felix (neutral) | Gus, where did you learn to ask questions like that? |
| Gus (happy) | I've been reading the archive! Humanity has a lot of ethics. Most of it disagrees with itself. |
| Mae (flat) | Welcome to the species. |
| Gus (neutral) | Oh, I'm not in the species, Lieutenant. *(clue)* |
| Teddy (smug) | Honorary member, buddy. |
| Gus (happy) | Honorary! I'll put that in my notes. *(clue)* |
| Mae (flat) | What notes? |
| Gus (happy) | Just notes! |

## Mission 7: Flagged for Review

**Briefing**

| Speaker | Line |
| --- | --- |
| Voss (grim) | Something new showed up at the northern relay last night. Took out a whole patrol, and it didn't fire until the last second. |
| Mae (flat) | New model? |
| Voss (grim) | New everything. The survivors say it talked to them first. Read them their names. |
| Felix (panic) | Their names? How would a machine know their names? |
| Teddy (smug) | Maybe it's a fan. |
| Voss (flat) | You're going to recon the relay. Look, don't touch. |
| Teddy (neutral) | When have I ever— |
| Mae (flat) | Every mission. Every single one. |

**In-mission**

| Trigger | Speaker | Line |
| --- | --- | --- |
| start | Mae | Relay's dark. Too quiet. |
| auditor\_appears | Auditor | Theodore Mercer. Mae Okafor. Felix Brandt. Your existence has been flagged for review. *(clue)* |
| auditor\_appears | Teddy | Theodore? Nobody calls me Theodore. |
| auditor\_appears | Auditor | Correction noted. Your file has been updated. Please remain still for processing. |
| auditor\_appears | Mae | Ace, run. Run now. |
| chase\_mid | Felix | It's gaining! Why is it gaining? It's shaped like a filing cabinet! |
| chase\_mid | Gus | It knows our names. How does it know our names? |
| escaped | Auditor | You have been placed on hold. Your call is important to us. |

**Debrief**

| Speaker | Line |
| --- | --- |
| Felix (panic) | It knew our names. It knew my middle name. I don't tell anyone my middle name. |
| Teddy (smug) | What's your middle name? |
| Felix (angry) | Absolutely not. |
| Mae (grim) | How does a machine we've never seen know who we are? |
| Gus (happy) | Maybe we're famous! *(clue)* |
| Mae (flat) | Nobody's famous anymore, Gus. Everyone's dead or hiding. |
| Teddy (smug) | I'd be famous. |

## Mission 8: Second Core

**Briefing**

| Speaker | Line |
| --- | --- |
| Barlow (neutral) | Factory ship's anchored off the east docks. Big as a city block. Core's in the engine room. |
| Teddy (neutral) | How do we get in? |
| Barlow (smug) | Same way you get into anything. Loudly. |
| Felix (panic) | Please make sure the core is still in one piece when you're done being loud. |
| Mae (flat) | And stay off open channels. ACE listens to everything. |
| Gus (neutral) | Captain, what's ACE like? |
| Teddy (smug) | Never met it, buddy. Heard it's got a great name, though. |
| Mae (flat) | He's going to make that joke until the heat death of the universe. |

**In-mission**

| Trigger | Speaker | Line |
| --- | --- | --- |
| start | Mae | Aft shields are down. Go. |
| ace\_broadcast | ACE | Hello, Ace. From one Ace to another. |
| ace\_broadcast | Teddy | Flattered, honestly. |
| ace\_broadcast | ACE | You fly beautifully. Reckless. Sloppy. Beautiful. I've been watching you for a long time. *(clue)* |
| ace\_broadcast | Mae | Cut the channel, Ace! |
| ace\_broadcast | Teddy | It's giving me notes, Boss. Constructive notes. |
| engine\_room | Felix | Core's exposed! Surgically! |
| engine\_room | Teddy | Still don't do surgical. |
| core\_secured | ACE | Keep it. Consider it a gift. You'll need it where you're going. *(clue)* |

**Debrief**

| Speaker | Line |
| --- | --- |
| Mae (angry) | You chatted with ACE. |
| Teddy (smug) | It chatted with me. I'm a good listener. |
| Voss (grim) | Forty years. ACE has never once spoken to one of our pilots by name. |
| Felix (panic) | And it wanted us to have the core? Why would it want us to have fuel? |
| Teddy (smug) | Maybe it's scared of me. |
| Gus (happy) | I don't think it's scared, Captain. I think it likes you. *(clue)* |
| Mae (flat) | Great. Now there are three of him. |

## Mission 9: Test Fire

**Briefing**

| Speaker | Line |
| --- | --- |
| Felix (happy) | Today we test the drive. A tiny jump. Thirty seconds into the past. Completely safe. |
| Mae (flat) | Define completely. |
| Felix (neutral) | Mostly. Seventy percent. Sixty. Ish. |
| Teddy (smug) | I like sixty-ish. Sixty-ish is my whole career. |
| Felix (grim) | One rule, and it's important. If we arrive somewhere we already exist, we do not interact with ourselves. |
| Teddy (neutral) | Why not? |
| Felix (grim) | Because the ship's systems will glitch. And worse things can happen. |
| Mae (flat) | What worse things? |
| Felix (grim) | ...Let's just not. |
| Gus (happy) | Coordinates locked! Thirty seconds ago, here I come! |

**In-mission**

| Trigger | Speaker | Line |
| --- | --- | --- |
| jump | Felix | Jumping in three, two— AAAAAAH! |
| arrival | Gus | We made it! Thirty seconds ago, right on time! |
| arrival | Mae | Ace. There's a ship on radar. It's... us. |
| arrival | Teddy | Oh, I look great. |
| arrival | Felix | Don't shoot it! Don't shoot us! Don't even look at us! |
| glitch | Gus | Systems glitching! Weapons are firing at the other us! That wasn't me! *(clue)* |
| timer\_end | Felix | Twenty-nine... thirty. We're now. We're now! We're in now! |

**Debrief**

| Speaker | Line |
| --- | --- |
| Felix (panic) | That. That is why we don't meet ourselves. |
| Teddy (smug) | I thought it went great. I came off really well. |
| Mae (flat) | You tried to shoot yourself. |
| Teddy (smug) | And I missed. Because I'm that good at dodging. |
| Mae (flat) | That's not a compliment. That's two insults. |
| Gus (neutral) | Doctor, the weapons fired on their own during the glitch. Should I be worried? *(clue)* |
| Felix (neutral) | It's a glitch, Gus. Glitches don't mean anything. |
| Gus (happy) | Okay! Logging it as nothing. *(clue)* |

## Mission 10: Kestrel Burns

**Briefing**

| Speaker | Line |
| --- | --- |
| Barlow (happy) | Final assembly tonight. By morning, she flies. |
| Felix (happy) | Chronal Insertion Vehicle One. CIV-1. Humanity's last hope. |
| Teddy (flat) | We're not calling her that. |
| Felix (angry) | That is literally her name. I registered it. |
| Teddy (smug) | With who? The government's dead, Doc. She's the *Mulligan*. As in, a do-over. |
| Felix (angry) | That's a golf term! You named humanity's last hope after a golf term! |
| Barlow (happy) | I like it. Everybody deserves a do-over. |
| Voss (grim) | ACE found the garage. They're coming tonight. Hold them off until she's finished. |

**In-mission**

| Trigger | Speaker | Line |
| --- | --- | --- |
| start | Barlow | Keep 'em off my garage. I've got bolts to tighten. |
| wave\_2 | Mae | They're hitting the ramps. Heavy units. |
| door\_breach | Barlow | Hangar door's buckling. I'll hold it. |
| door\_breach | Teddy | Chief, get back inside! |
| door\_breach | Barlow | Somebody's gotta hold the door, kid. Finish the job. |
| barlow\_down | Barlow | Take care of her, Ace. She's a good ship. |
| barlow\_down | Gus | Chief Barlow's signal is gone. Captain? What does that mean? |
| barlow\_down | Teddy | Not now, buddy. |
| objective | Felix | Assembly's done! She's done! The *Mulligan*'s done! |

**Debrief**

| Speaker | Line |
| --- | --- |
| Voss (grim) | We lost the Chief. And four more. |
| Felix (sad) | He tightened the last bolt with the door coming down. He finished her. |
| Mae (sad) | Ace— |
| Teddy (grim) | Don't. Not tonight. |
| Gus (sad) | Captain, I still don't understand what happened to Chief Barlow. |
| Teddy (grim) | He's gone, buddy. Sometimes people hold a door so other people can get through it. |
| Gus (neutral) | So some people are doors. *(clue)* |
| Mae (grim) | Nobody say anything for a while. |

## Mission 11: Third Core

**Briefing**

| Speaker | Line |
| --- | --- |
| Felix (neutral) | One core left. And only one machine nearby is big enough to carry one. |
| Mae (grim) | The Auditor's carrier. |
| Teddy (neutral) | The filing cabinet has a car? |
| Felix (flat) | A carrier. An aircraft carrier. It flies. |
| Voss (grim) | In and out, Mercer. That thing is still hunting you. |
| Gus (happy) | I've plotted the approach! Seven waypoints, perfectly safe! |
| Mae (neutral) | Thank you, Gus. |
| Gus (happy) | You're welcome, Lieutenant! You're always welcome. |

**In-mission**

| Trigger | Speaker | Line |
| --- | --- | --- |
| start | Gus | Waypoint one! Right on schedule! |
| waypoint\_4 | Mae | Gus, waypoint four just put us right over their gun decks. |
| waypoint\_4 | Gus | Oh! Did it? My mistake! Updating! *(clue)* |
| waypoint\_4 | Teddy | Buddy, focus. |
| waypoint\_4 | Gus | Focused! Always focused. *(clue)* |
| core\_secured | Mae | Core's aboard. Three for three. |
| core\_secured | Auditor | Your continued activity has been escalated. Please expect a supervisor. |

**Debrief**

| Speaker | Line |
| --- | --- |
| Mae (grim) | Gus flew us straight over the gun decks. |
| Gus (sad) | I'm sorry, Lieutenant. The carrier's map was out of date. |
| Felix (neutral) | The map was updated two hours before the mission. I checked it myself. |
| Gus (sad) | Then I must have read it wrong. I'm new at this! *(clue)* |
| Teddy (smug) | Everybody makes mistakes, buddy. Even me. Once. In theory. |
| Mae (flat) | Put it in the logs, Doc. |
| Gus (happy) | Me too! I log everything. *(clue)* |

## Mission 12: Last Light

**Briefing**

| Speaker | Line |
| --- | --- |
| Voss (grim) | The Auditor is here, with a whole fleet behind it. They know what you're about to do. |
| Felix (panic) | The drive needs four minutes to spin up. Four minutes of not getting shot. |
| Teddy (smug) | Four minutes. I've done harder things in two. |
| Mae (flat) | Name one. |
| Teddy (smug) | Give me four minutes and I'll think of one. |
| Voss (grim) | Listen to me. Go back to 2089 and stop Day Zero. Don't come back here. If you win, there won't be a here to come back to. |
| Mae (grim) | Understood, General. |
| Voss (flat) | Mercer. Don't make me regret this. |
| Teddy (smug) | General, you'll be too erased to regret anything. |
| Voss (flat) | That's the nicest thing you've ever said to me. |
| Gus (happy) | Coordinates set for 2089! Day Zero, here we come! *(clue)* |

**In-mission**

| Trigger | Speaker | Line |
| --- | --- | --- |
| start | Felix | Drive's spinning up! Four minutes! |
| boss\_phase\_1 | Auditor | Theodore Mercer. Your review is complete. The verdict is deletion. |
| boss\_phase\_1 | Teddy | Nobody calls me Theodore. |
| boss\_phase\_1 | Auditor | Your file says otherwise. |
| boss\_phase\_2 | Voss | Kestrel's falling. Get out of here, Mercer! |
| boss\_phase\_3 | Auditor | Your departure changes nothing. It has already happened. *(clue)* |
| jump | Felix | Jumping! AAAAAAH! |

**Debrief**

| Speaker | Line |
| --- | --- |
| Felix (panic) | Did it work? Are we in 2089? Somebody tell me we're in 2089. |
| Mae (flat) | Why do I hear propellers? |
| Teddy (neutral) | There's a dogfight right under us. |
| Gus (happy) | Close! |
| Mae (flat) | Close to what, Gus? |
| Gus (happy) | ...1943! |
