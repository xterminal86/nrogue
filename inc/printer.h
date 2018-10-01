#ifndef PRINTER_H
#define PRINTER_H

#include <string>
#include <map>

#include <ncurses.h>

#include "singleton.h"
#include "colorpair.h"

/// Singleton for ncurses text printing
class Printer : public Singleton<Printer> 
{
 public:
  int TerminalWidth;
  int TerminalHeight;
  
  static const int kAlignLeft = 0;
  static const int kAlignCenter = 1;
  static const int kAlignRight = 2;

  void Init() override 
  {
    // Enforce colors of standard ncurses colors
    // because some colors aren't actually correspond to their
    // "names", e.g. COLOR_BLACK isn't actually black, but grey,
    // so we redefine it
    init_color(COLOR_BLACK, 0, 0, 0);
    init_color(COLOR_WHITE, 1000, 1000, 1000);
    init_color(COLOR_RED, 1000, 0, 0);
    init_color(COLOR_GREEN, 0, 1000, 0);
    init_color(COLOR_BLUE, 0, 0, 1000);
    init_color(COLOR_CYAN, 0, 1000, 1000);
    init_color(COLOR_MAGENTA, 1000, 0, 1000);
    init_color(COLOR_YELLOW, 1000, 1000, 0);
  }

  /// Print text at (x, y) on the screen, with (0, 0) at upper left corner
  /// and y increases down
  void Print(const int& x, const int& y, const std::string& text, int align,
                                         const std::string& htmlColorFg,
                                         const std::string& htmlColorBg = "#000000");
  void Print(const int& x, const int& y, const chtype& ch,
                                         const std::string& htmlColorFg,
                                         const std::string& htmlColorBg = "#000000");
 private:
  bool ContainsColorMap(size_t hashToCheck);
  bool ColorIndexExists(size_t hashToCheck);

  NColor GetNColor(const std::string& htmlColor);
  size_t GetOrSetColor(const std::string& htmlColorFg, const std::string& htmlColorBg);
  std::pair<int, int> AlignText(int x, int y, int align, const std::string& text);

  std::map<size_t, ColorPair> _colorMap;
  std::map<size_t, short> _colorIndexMap;

  short _colorPairsGlobalIndex = 1;
  short _colorGlobalIndex = 8;
};

#endif
