#ifndef PRINTER_H
#define PRINTER_H

#include <string>
#include <map>
#include <vector>

#include <ncurses.h>

#include "singleton.h"
#include "colorpair.h"

struct FBPixel
{
  size_t ColorPairHash;
  chtype Character;
};

/// Singleton for ncurses text printing
class Printer : public Singleton<Printer> 
{
  public:
    int TerminalWidth;
    int TerminalHeight;

    static const int kAlignLeft = 0;
    static const int kAlignCenter = 1;
    static const int kAlignRight = 2;

    void Init() override;

    /// Print text at (x, y) on the screen, with (0, 0) at upper left corner
    /// and y increases down
    void Print(const int& x, const int& y,
               const std::string& text,
               int align,
               const std::string& htmlColorFg,
               const std::string& htmlColorBg = "#000000");

    void Print(const int& x, const int& y,
               const chtype& ch,
               const std::string& htmlColorFg,
               const std::string& htmlColorBg = "#000000");

    std::string GetLastMessage()
    {
      return (_inGameMessages.size() > 0) ? _inGameMessages.front() : std::string();
    }

    const std::vector<std::string>& Messages() { return _inGameMessages; }

    void AddMessage(std::string message);

    void Clear();
    void Render();

    void PrintFB(const int& x, const int& y,
                            const chtype& ch,
                            const std::string& htmlColorFg,
                            const std::string& htmlColorBg = "#000000");

    void PrintFB(const int& x, const int& y,
                            const std::string& text,
                            int align,
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

    std::vector<std::string> _inGameMessages;
    const int kMaxGameLogMessages = 100;

    short _colorPairsGlobalIndex = 1;
    short _colorGlobalIndex = 8;

    void PrepareFrameBuffer();

    std::vector<std::vector<FBPixel>> _frameBuffer;
};

#endif
