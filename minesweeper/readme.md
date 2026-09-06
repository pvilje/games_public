# MineSweeper

## Background

This is pretty much a clone of the original Minesweeper previously available in Windows. Added a few modes I thought was missing.

## Background Story

The war was bad on this local village. Tons of people was forced to join the army. The men became soldiers, basically cannon food, women were nurses. All did their part, but tragically most died...
Now the war is over but the memory remains. In fact not only the memory, there are also tons of evidence left of the horrific tragedies that has scarred the people and this land.
No grown woman or man leave home without taking a second look over their shoulders, was that sound a war siren? Does that person look like a spy? The memories continue to haunt the living.

What about the children you say? Well, unfortunatelly it is even worse for them. Not only are they innocent of all of this, they are also the most common victims. You see, children loves to play outside and this close to the war there are "leftover" mines everywhere. It is not safe to go outside and play soccer anymore, it is not safe to go outside and play hide & seek. 

THIS is what you aim to change! The least thing we can do is to make the life of our children easier and safer! It is now up to you to map up all of the land and clear the safe parts! Good Luck!

### Winning

If you manage to claer a land from all mines you "win", there will be no real celebrations, no one will thank you. But deep down in your heart you'll know you did a good thing, plus you survived. You will however get the option to go home and get you well deserved sleep (close the game), knowing there are still minefields out there where children are blown into pieces. Or enter a new field, where you'll likelly die. It's up to you. There really are no true winners in this game.

## Gameplay

Left-click a cell will either reveal a number 1 - 8, empty space, or a mine.
You loose the game if you left-click a mine. You win if you open all non-mine cells.
If you hit a number, that number tells you how many mines are in the scope of that digit. For example a 2 tells you that in exactly 2 of the eight adjecent cells there are mines. 

Hitting an empty space will open a path in all directions stretching until it hits numbered cells.

Right-click a cell will flag it as a mine. Flagged cells cannot be opened. Right-click the cell again to remove the flag. Use this to mark known mines as it will also decrease the mines left counter.

Middleclick a digit will open up all unopened adjecent cells if the exact amount of mines are flagged next to that digit. For example middle-clicking a 2 will open up all unopened adjecent cells IF exactly 2 adjecent cells are already flagged.

### MAX

Max mode will simply check you screen resolution and provide a grid that fills the entire screen. The mine density matches Expert mode.

### 3D

Finally it is here! Minesweeper in 3d! simple as that. Basically why I made this game, I think it's kinda fun.

## Developer's corner.

### TODO

* [x] Build board
* [x] Left Click
* [x] Flood fill
* [x] Header
* * [x] Mines left (possible to win)
* * [x] Timer
* [x] Right click
* [x] Middle click
* [ ] The font is a bit hard to read, maybe change it?
* [ ] Make sure starting click is not a mine? or maybe not, real life aint that kind...
* [ ] Make sure no deadly patterns are generated, where you have to guess. Maybe not, again, real life aint that kind...
* [x] Show all mines if failed,
* [x] and show incorrectly flagged mines
* [x] SHow all mines when successful, in green maybe.
* [ ] Can the gameover/game won dialogs be movable, do I want them to be?
* [ ] Clean up, more Macros / functions?
* [x] Some digit colors might wanna be changed.
* [-] Paranoid version, (confirm all clicks. ;) ), WON'T DO! NO ONE WANTS THIS! HORRIBLE IDEA!!!
* [ ] text version? for terminals.. maybe...
* [x] Maximum size version
* [ ] billboards in 3d version does not have transparent background in game, but source file does. (maybe a bug in raylib, or maybe I need to set some flags for the texture)
* [ ] Clean up!!!!!! so much clean up to do, remove unused files etc. get to it! 
* [ ] Do I want to add the background story to the game somehow? not sure.

### KNOWN BUGS
 *  ================
 * [ ] Weird flicker in Windows on gameover, but also gives a semicool effect so might leace it in.
 * [x] game over opens a ton of mines, way more than actually present

### Changelog

I guess we are around version 1'ish now
