Dune Reloaded
=============

1. Credits
2. What is Dune Reloaded?
3. Controls
4. Installation instructions



Credits
=======

Author of the program: PIERRE CLEMENT
Possibly includes (not verified) some source code from :
 - "big" 's HSQ file format dumper
 - "Ryomah" 's HNM file format dumper
 - "honza.c" 's LZ compressed files extractor

All nicknames and programs above refer to the thread "Rewriting Cryo's Dune 1 : it seems possible !!! " on the Dune2k.com website.
Direct URL : http://forum.dune2k.com/index.php?/topic/17217-rewriting-cryos-dune-1-it-seems-possible/


What is Dune Reloaded?
======================
This is not a finished game. This is only a tiny tech demo meant to show the extraction and rendering process of Cryo's Dune original game files.
Please note, however, that the dunes landscape rendered in the middle of the screen has nothing to do with the original technology. It's an independant Voxel engine (eye candy!).


Installation instructions
========================= 
- You need to own the original Dune game *
- Copy the game files directly into /Data/Dune
- Run /Data/DuneR.exe

If the Dune's files are missing in /Data/Dune, you'll get a runtime error
You might also get an error if you're missing some Visual Studio files (not tested). They can potentially be found there : http://www.microsoft.com/downloads/en/details.aspx?FamilyID=9b2da534-3e03-4391-8a4d-074b9f2bc1bf&displaylang=en

* Some people consider that getting the game from Abandonware websites is not a copyright infringement, and that such practices are tolerated by major companies regarding very old programs that are not distributed anymore. This is not true, and that's not the case for Dune. Dune has been property of Virgin Interactive, and is now property of Electronic Arts. You may not use this program if you are not a legal owner of the original Dune videogame.


Controls
========
Esc = skip intro
F1 = display scrolling lanscape (Voxel)
F2 = display a character's face
F3 = display a background
F4 = display the pseudo-3D view of the planet.


Compilation instructions
========================
- The code for extracting original game files has no dependencies
- The on-screen rendering part of this tech demo relies on SDL

