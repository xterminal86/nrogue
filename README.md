[![mingw](https://github.com/xterminal86/nrogue/actions/workflows/mingw.yml/badge.svg?branch=master)](https://github.com/xterminal86/nrogue/actions/workflows/mingw.yml)
[![MSVC](https://github.com/xterminal86/nrogue/actions/workflows/msvc.yml/badge.svg)](https://github.com/xterminal86/nrogue/actions/workflows/msvc.yml)
[![ncurses](https://github.com/xterminal86/nrogue/actions/workflows/ncurses.yml/badge.svg)](https://github.com/xterminal86/nrogue/actions/workflows/ncurses.yml)
[![linux](https://github.com/xterminal86/nrogue/actions/workflows/linux.yml/badge.svg)](https://github.com/xterminal86/nrogue/actions/workflows/linux.yml)

![GitHub Downloads (all assets, all releases)](https://img.shields.io/github/downloads/xterminal86/nrogue/total)

# NROGUE

*ncurses roguelike challenge*

![](resources/github-front/title.png)

I decided to challenge myself by making a game with minimal requirements possible.
Just like in old times, when all you had was just one binary file.
Since I'm working with Linux, I thought that making a game that uses just terminal libraries (e.g. ncurses)
seems like a perfect candidate. And what games are known to use terminal graphics? That's right, roguelikes.
So I decided to make a roguelike.
It probably should've been named **ncrogue** or something, but whatever...

You can build either version, SDL2 or ncurses, check comments in main.cpp.
Game has config file where certain parameters can be tweaked (check `config-template.txt`).

<TABLE>
  <TR>
    <TD colspan="2" align="center"><B>SCREENSHOTS</B></TD>
  </TR>
  <TR>
    <TD><IMG src="resources/github-front/in-game.png" title="in-game"></TD>
    <TD><IMG src="resources/github-front/in-game-battle.png" title="battle"></TD>
  </TR>
  <TR>
    <TD><IMG src="resources/github-front/in-game-charsheet.png" title="charsheet"></TD>
    <TD><IMG src="resources/github-front/in-game-shop.png" title="shop"></TD>
  </TR>
</TABLE>
