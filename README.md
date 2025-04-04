# DropTheNumber
A PC version of Drop The Number written with C++ and the SFML Graphics Library

Current version: v0.4.5 - Released on April 4, 2025

## About
Drop The Number is a popular puzzle-based game that can be found on various websites and platforms, most notably on iOS and Android. My main goal for this project is to create a PC port of one of my favorite mobile games while also gaining experience with using C++ and the SFML Graphics library. There is an official PC port of Drop the Number, but it uses alot of CPU resources when it shouldn't. This would be a less CPU-intensive take on that port.

##Changes since v0.4:
- complete overhaul of tile height checking system
- fixed buggy left and right merging, which are now enabled by default
- added checking for multiple merges (whether merging still needs to be done after a merge)

## Note about Merging Tiles
Left and right merging now work properly, along with scenarios where multiple merges occur. However, some merges, such as when both the left and bottom tiles have the same value as the dropped tile, do not work as they are supposed to. This will be fixed for v0.5.

## Getting Started (WIP)
### Prerequisites:
- SFML v2.x.x or later

Currently, I write this game with Visual Studio 2022 Community Edition running on Windows 10. To make it easier to build the source code, I have all of the files you need to build the solution in Visual Studio. Make sure to add all of the DLLs and texture files after building.

## What to expect in later versions
v0.5: Full Merging, Basic animations for tile merging and dropping<br />
v0.6: Level System, High Scores<br />
v0.7: Enhanced animations, new tile textures <br />
v0.8: Window Tab Menus, options menu, about page <br />
v0.9: Ability to resize window <br />
