# Unrealistic-Engine
3D ASCII Dungeon Crawler

# What you can do
* Make worlds:
First line should be in the format of LENGTHxHEIGHT, use 1 for walls, 0 for empty space, 2 for the player spawn point. There's a very basic graphic system in place that is a little more complicated. Use any letter and then below make something like this
LETTER:
LENGTHxHEIGHT
1234
2234
3334
4444

The following is optional and must be put directly under the graphic:
LETTER_data
spritemode
end_data

spritemode will only render the front, eventually an alpha letter will be added. Way more options will be added, but more on that below.

* See worlds:
You can walk around (a and d for turning, q and e are strafing, w and s are forward and backwards).
You can see stuff, not walk thru stuff.

# To Come
* Graphic system changes
It's really hard to accurately resize stuff especially when dealing with ASCII, heard about the curses libabry maybe something will make it easier. There's some bugs that need to be fixed. Add alpha character. Figure out what's a good size for a graphic.

* Combat system
I want to focus on battle out comes, like broken bones, poisoning, bleeding, etc. Too often the character gets hurt but then just runs around with 1 hp and that's bull. Want to focus on heavy/light attacks and atriubtes. Heavy is easy to dodge, but can lead to broken bones. Light makes you more vunlurable (less armor), but it's fast and causes stuff like pisoning and bleeding. Probably will be really obnoxious to play.

* Programble data for the graphic
It's an engine so i want graphics to be prograble like enemies, doors, McGuffins, etc. 
