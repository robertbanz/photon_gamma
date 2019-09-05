# Prologue

My first experience with Photon software hacking was a modification to change the 200 point base score to 205 points (so you could easily tell who got base.) This was done by finding and replacing 0xC8 with 0xCD in various locations in the game software binary until it worked.

This was a big deal™ as during league and tournament play, resets due to shit batteries were a big problem. We would always keep a pod or two on the side entered up for a player to swap into, and someone had to be watching the scores to make sure that player didn’t get base twice. Adding the ‘5’ to the base score made it rather easy to correct any … mistakes.

![](https://lh4.googleusercontent.com/-FGNYchCmQo2gM8QJFRHvlu7f_yAFY_yPwL1c7M3SnfIO5MPaqWdc-thpmAl-kscU9zM_Eq0H-DOIetBUNi9nojZPLMRcjYuZz-zA_D-Iz-pcP0wkDpMPgep5qna7T1YtoDhKq6s)

# Introduction

After PhoCon 2019, the 35th Birthday of the game, I was reminiscing over how we got where we are now; I dug through old backups, and found I had kept a series of .zip[^1] files covering some stages of development of the GAMMA system that we were running at the Baltimore store the last year before we closed.

[^1]: Pile of .zip files == Revision Control

 
I recently uploaded these as a chain of commits to [robert_banz/photon_gamma](https://github.com/robertbanz/photon_gamma) on GitHub. The only changes made were a pass through clang-format for consistency (and to minimize diffs between changes)

## Thanks In Advance

None of this would have happened without a little help from my friends. Chris Hutson (aka Goob), and the owners at-the-time of the Baltimore Photon; Marc (SulSa), Nick (Orion), Russ, Papa Frog. Last, but certainly not least, many thanks to Chris Fanning (aka Atlas) from Long Island provided useful libraries to improve the user interface, and also the software that ran our BBS!

# The Beginning

Everything starts with an ending, right?

## The End of Wheaton

(winter 1991?) This all really starts with the end of the Wheaton Starbase. Marc Mueller (SulSa) purchased a bunch of the equipment, and a hand-full of pods, the ET innards, radio, and game computer ended up in my parents’ basement. I don’t quite recall what the play was at the time, but we certainly set the gear up to play 1:1s. I had purchased the LiteMaster II, dimmers, and a selection of can lights for a steal -- so with all that I think we had a set-up that was a similar playing experience to what Wheaton had when it was open. Small and boxy.

## Ingredients

-   Spotty documentation (Photon tech manuals)
    
-   Serial Ports and Alligator Clips
    
-   Lots of Spare Time
    

## Curiosity

I had begun reverse-engineering how “the system” worked, starting with the radio communication. If you’re familiar with the Photon tech manual, there was a nice chart of the polling cycle showing when pods and the central radio were supposed to communicate, but little else. According to schematics, communication between the game computer and radio was serial, so I did what anyone would do and hooked some alligator clips up to a serial cable and started poking around Baltimore’s central radio to see what’s what.

  

After awhile I had put together what was a basic serial “protocol analyser” that could spy on two lines (receive & transmit) while tagging timestamps and by looking at the data stream during game play was able to figure out the basics of what was going on.

  

I then got it into my head that “maybe I could write a thing”, and some people said “nah, you can’t write a thing” which then clearly became “well, I MUST certainly write a thing!”

# Getting Started

## Finding The Right Tempo

As I remember, the first problem was to figure out how to deal with the Central Radio. At the time I didn’t have any documentation for the API of the Slave ISA card[^2], so direct serial communication it was. I had read up on DOS interrupt handlers, and created one to trigger on an 8253 interrupt event and calculated a “reasonable” starting divisor[^3] from what was roughly the per-slot timings from the protocol analysis and timing graph. After that it was a matter of a binary-search of timer divisor values until I was able to find one that matched what the pods expected; it was close enough that it wouldn’t drift for hours.[^4] I remember many evenings of testing, such as making sure a pod entered into every slot showed up in the expected place.

[^2]: At some point I would figure out some stuff to interface with the Slave to control the CD player; it was possibly after I had gotten a copy of the original Photon source code in 1993 (long after Baltimore closed).
[^3]: The correct value is 0x4c38.
[^4]: I would later learn that I was really lucky that the clock on the 8253 could be divided by something that was sufficient. This wouldn’t be an issue until after the first PhoCon, and that’s another story.

The result was an [interrupt handler](https://github.com/robertbanz/photon_gamma/blob/master/gamma/gamma.c#L966) that did basically what the slave did.

## The ET

For some reason I decided that the system would work with the existing ET, probably because “more computers” and “laser barcode scanners” weren’t on-budget. I probably spent more time decoding the interface between the game computer and the ET than I did with the radio, but it wasn’t too bad. I did discover some interesting features, such as the ET being able to report back to the game computer where the ET operator was in the interface.

## The CD Interface

The Slave card[^5] had what was essentially a parallel port hooked up to some franken-remote that simulated button presses on a Sony CD player remote to control the game soundtrack. I don’t recall where I figured it out, but the interface to the franken-remote was not too hard to figure out and I was able to create an adaptor cable to allow it to connect to a regular IBM parallel port. (At some point I was able to figure out how to talk to the Slave to control the CD without my additional adaptor cable)

[^5]: The Slave card was an ISA board that contained bits of POD guts and was in charge of interfacing with the radio and doing the right things at the right time.

## Constraints

While the software was being developed on reasonably modern hardware at the time (I had a 486 with a few bytes of RAM), the target platform was still the IBM PC (or PC/XT) platform that we had on-site.

# Minimum Viable Product

After some field tests (some unsuccessful), there was something that worked well enough that It came online as the Game Computer on [Feburary 7, 1992](https://gameview.playphotonagain.com/game/baltimore92/start/) and would run until Photon Baltimore closed on [October 4, 1992](https://gameview.playphotonagain.com/game/baltimore92/5516/). Of course, there’s no point in replacing the software unless we were able to add some new features… (and remember, these were all being done on an IBM PC) 

When Photon Baltimore closed, this was the [source for the software we were running](https://github.com/robertbanz/photon_gamma/tree/2120dca65596f3713c2b3e546be4fc0d7a31b97f/gamma). To get it to work, you’ll need the Microsoft C 6 compiler, a machine with a VGA or CGA card and a monochrome card, an original working Photon ET, a Photon Central Radio, and some serial ports -- preferably from a Hostess 8-port Serial Adapter.

The software supported full dual-field operations for 10-on-10s (or 20-on-20s for single-field ops).

## Improved Game Data

Unlike the standard Photon software, the new system also stored player’s codenames instead of just their [passport numbers](https://gameview.playphotonagain.com/game/baltimore89/start/). (Later the play-by-play system would store all of the radio data from the game, but few of these snapshot files still exist.)

## Faster Cycle Time

By speeding up the interaction between the game computer and “data computer” (which in the new system, was just a dumb remote display), we could run games at ~7m intervals vs. 7m30s intervals.

## Real Time Pod Performance

From a tech’s perspective, the RTPP (and radio data monitor) enabled us to quickly identify equipment with bad radio performance. Player comes out complaining that they can’t put anyone down? Quick glance at the screen and you can confirm (or explain to them how much they just suck.)

## Play By Play

Premiering before the last Baltimore tournament of 1992, a “second screen” was present showing real-time play-by-play action.

## Updated Score Screens

Though proving to be unpopular (because nobody likes change), we began toying with new score screens that showed statistics about play, such as the number of times hit, shot, or hit their own player.

## Customized User Experience

This was a bit of a not-well-kept secret, but when certain passport numbers would show up in a game, we would pick their [favorite](https://github.com/robertbanz/photon_gamma/blob/master/gamma/gamma3.c#L543) (or [least favorite](https://github.com/robertbanz/photon_gamma/blob/master/gamma/gamma3.c#L560)) tracks.

## GameView BBS Door

Photon Baltimore hosted a BBS system back in the day, which included an [app](https://github.com/robertbanz/photon_gamma/tree/master/gameview) (people called them doors at the time) that allowed you to view the game database; the great grandfather of the current [gameview](https://gameview.playphotonagain.com/static/gvindex.html). It was the second most popular use of the BBS, behind playing TradeWars.

# Post Viable Product

After the closing of Baltimore, development on the game software didn’t completely stop. SulSa still had equipment and was looking to open a new store with it, and tinkering with the system made for a nice platform to further develop my programming skills.[^6]

[^6]: And our nerd-ball friends could still play Photon in my basement.


The most [modern commit point](https://github.com/robertbanz/photon_gamma/tree/a9a164509d6d24c3e645e19863a0631c2c8918a3) added support for VGA displays (with graphical interstitials), using a CDROM drive for the soundtrack, and added support for a “remote console” as running a PC with a dual monochrome/color display was pasé.

  

Sadly, not too long after I was concentrating on a “real job” so SulSa did not initially open his new facility with this software, but it might have been cool.
