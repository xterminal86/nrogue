# NROGUE
*ncurses roguelike challenge*

![screenshot](https://user-images.githubusercontent.com/11379604/108273698-f7167d00-7184-11eb-9946-0b7a9b0f6ed9.png)

I decided to challenge myself by making a game with minimal requirements possible.  
Just like in old times, when all you had was just one binary file.  
Since I'm working with Linux, I thought that making a game that uses just terminal libraries (e.g. ncurses)
seems like a perfect candidate. And what games are known to use terminal graphics? That's right, roguelikes.  
So I decided to make a roguelike.  
It probably should've been named **ncrogue** or something, but whatever...

When building with SDL2 you can use custom font image and scale it by creating
file called "config.txt" and placing it alongside with the game's binary.
Color key is magenta (R:255, G:0, B:255).
The format should be as follows (no empty lines):

```
FILE=resources/standard_8x16.bmp  
TILE_W=8  
TILE_H=16  
SCALE=2  
```
