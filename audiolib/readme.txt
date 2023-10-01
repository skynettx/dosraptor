Rise of the Triad (v1.3 CD Version) Source Code Release - December 20, 2002.
(If you can't read this right in your editor, turn on word wrap)

Please note that this is being released without any kind of support from Apogee Software, Ltd / 3D Realms Entertainment.  We cannot and will not help in getting this running.  We do not guarantee that you will be able to get it to work, nor do we guarantee that it won't blow up your computer if you do try and use it.  Caveat Emptor - Use at your own risk.  Having said that, this source code release was compiled on December 7, 2002 using the materials in this archive.  Here's a note from one of the original ROTT programmers (Jim Dose) as to what will be needed to get this compiled:

------

You will need Watcom C v10.0b, which is what was used to compile the game originally.  This source code release was tested with that version.  Other versions are not guaranteed to work.  Later versions such as v11.0 are known to most likely not work because of changes in the way data types are handled.

You also need Borland's Turbo Assembler (TASM), but it is not required to build the full game if you use the .obj files that are already included.  To do a full rebuild of the C code without having TASM, delete all the .obj files except for the following files:

F_SCALE.OBJ
RT_DR_A.OBJ
RT_FC_A.OBJ
RT_SC_A.OBJ
RT_VH_A.OBJ

To compile ROTT, type "wmake all".  To compile the audio library, run "wmake.bat".

-----

Please note that while we are releasing the source code to Rise of the Triad, the game itself has not been released in the same manner (in other words, Rise of the Triad is still commercial software).  You can still buy the game from us by visiting http://www.3drealms.com.

Thanks to all the fans who have hung in there and waited for us to do something like this, we hope you enjoy it.  If you produce something cool with this source code, drop us a line at rott@3drealms.com.   In honor of the source code release, we contacted Tom Hall to reminicse a bit about the game, and we have some history from Scott Miller, President of Apogee. Their thoughts are below.

Furthermore, the release of the Rise of the Triad source code is dedicated to our late friend and cohort, William Scarboro.  William was one of the original Rise of the Triad programmers, and he unfortunately died of an asthma attack on August 9, 2002.

William was born March 2, 1971 in El Paso, TX. He was a graduate of Texas A&M with a degree in Computer Science. He came to work here back in 1993, and was the first programmer we hired back then when we started doing in house development. He was mainly known for his work on Rise of the Triad where he worked on actor code, weapon stuff, and the gibs. In fact, William was directly responsible for the /EKG gib cheat in ROTT.

Joe Siegler
Apogee Software / 3D Realms
Dec 20, 2002

==============================================================================================

RISE OF THE TRIAD: The Source Code Release
A Note from Tom Hall

It was 1992.  I had just worked through half of DOOM, a creative guy in a technologically-oriented company.  We parted ways and I left to start up "In-House Development" at Apogee (later 3D Realms). We started accruing a programmer here, and artist there. Soon we had a team.  I came in with a memo about something (like bonus dough for library functions or something), and at the end I said something like, "And once we complete all these, we will be... THE DEVELOPERS OF INCREDIBLE POWER!" The guys laughed and liked this a lot.  And thus we became the DIPs. :)

With a game called ROTT, and a bunch of DIPs, you're heading for some derision. :) We had a fairly inexperienced crew: me, inexperienced at management; the rest of the guys, fairly new to the industry. It started as WOLFENSTEIN 3D, Part 2 -- a "transition project" my heart wasn't in, really.  It wasn't my idea to do it, but it was something to do. Once we parted ways with that idea and id's involvement, things got rewritten, changed, and we came up with s-Quake functionality crammed into a Wolfenstein Plus/sub-DOOM engine.  It was an example of pushing a technology to do what it really can't do well.  We had a fun engine, but one that looked ugly, especially in the masked platforms -- they were paper thin!  You could have had a fun game without doing what it couldn't do... but we were trying to beat the Joneses. And once we split ways with the id thing, I made three bad decisions: a) to keep the art we had already, b) to not redesign a new game and c) not to move over to the Build engine, where we could have had a Duke/DOOM level game going decently quickly.

Ah well. Hindsight's 20/20.  I am too nice, I guess. Yet, ROTT, for all its tortured technology, all its semi-justified graphics, did have some stuff that there was to be proud of. It was the first game with Rocket Jumping, the first game with Jumppads, the first game with parental password and Violence level adjustment, the first game with Capture the Flag...  Plus, I believe, the record for the most cheat codes ever in a game!  And a Random Level creator!  And what about all those cool Deathmatch options! And speaking of Deathmatch, I recall William's wonderful Corpseyard deathmatch map, and his "totally heinous" insane Drunk Missiles!  Mr. Scarboro, you, and your tuna, egg, salsa, and Omega 3 fatty acid bowls of goo will be missed.  Wah-bgsht! To Mark, Jim, Nolan, Steve, Tim, Chuck, Susan, JoeSke, Big Joe, Marianna, Lee, Bobby, Robert and all, thanks for sharing a strange time with me, and so many crazy memories!  "Uh, guys... um...."  The Disturbathon.  "50 kills!" "Ooooooooooh...  Mmmmmmm..."  Going to that crazy Bazaar to digitize old weapons.  "Check out what I got the boulders to do now."  "Ass!" "Use the fish." "Bowooooooooo!"  "I'm lookin' for some hot buns..."

And on and on... And thanks to Scott and George, for providing a place for us to be crazy together, and sticking with it even though it wasn't blockbuster stuff. I do owe you a debt in what was an odd, disorienting time for me. In the end, a couple hundred thousand folks seem to have enjoyed it, so there was something there that they liked. "See, Charlie Brown?  It wasn't such a bad tree after all."

======

A little history...
by Scott Miller

ROTT, as it quickly became known, marked a turning point for Apogee.  It was our first in-house game since I started the company in 1987 with my home grown Kroz games, the games that started the shareware revolution that resulted in the launch of three of the most successful independent PC developers, Epic, Id, and us -- all three still kicking after 12+ years (as of Dec. '02).  It's hard to believe we're among the oldest of all surviving independent PC developers in the world now.  Before ROTT, Apogee, as we were then known, solely worked with outside development teams, often funding and helping organize these teams, and helping guide their game designs using our experience.  But around 1993 it was evident that this method wasn't going to work out much longer because as games got larger and more complicated to make, team sizes had to grow, too.  So, in 1994 we started hiring developers to form our own internal team, with the first of those hires including William Scarboro, Nolan Martin, Mark Dochtermann, Jim Dosé, and the ever creative Tom Hall to run the show.

As Tom notes above, ROTT was originally an Id-approved sequel to Wolfenstein 3-D, using the original Wolf engine.  The game was going to be called, Wolfenstein: Rise of the Triad, and explore what happened after Hitler's demise.  About 4-5 months into development, though, a surprise call from John Romero ended the project, and we were left with a lot of content specific to the Wolfenstein premise, which had to be rolled into a new game concept so that we didn't waste all that we'd done.  The result was a bit of a mish-mash, and as Tom says above, the project probably should've been restarted using our new Build engine, which our second internal team down the hall was using, making Duke Nukem 3D.

ROTT ended up selling several hundred thousand copies and making enough money to keep Duke 3D funded.  But it was at this time that we knew we  had to make a radical company change to shift with the times, and so we created the 3D Realms label, and began phasing out Apogee, which we were leaving behind with the glory days of shareware, and the arcade-style games Apogee was best known for.

We get a lot of requests to release the source code to many of the older Apogee games, but the problem is that Apogee does not own these games that were developed by external teams.

This source release is a long time coming, and hopefully it's not the last time we're able to do this.

Scott Miller, CEO / Founder

======

/*
Copyright (C) 1994-1995 Apogee Software, Ltd.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/
