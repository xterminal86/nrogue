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

    /// Print text at (x, y) directly to the screen,
    /// with (0, 0) at upper left corner and y increases down
    void Print(const int& x, const int& y,
                const std::string& text,
                int align,
                const std::string& htmlColorFg,
                const std::string& htmlColorBg = "#000000");

    void Print(const int& x, const int& y,
                const chtype& ch,
                const std::string& htmlColorFg,
                const std::string& htmlColorBg = "#000000");

    /// Print to "framebuffer" instead of directly to the screen
    void PrintFB(const int& x, const int& y,
                  const chtype& ch,
                  const std::string& htmlColorFg,
                  const std::string& htmlColorBg = "#000000");

    void PrintFB(const int& x, const int& y,
                  const std::string& text,
                  int align,
                  const std::string& htmlColorFg,
                  const std::string& htmlColorBg = "#000000");

    /// Clears framebuffer (use this before PrintFB calls)
    void Clear();

    /// Prints framebuffer contents to the screen
    /// (call this after all PrintFB calls)
    void Render();

    /// Add message to the game log
    void AddMessage(std::string message);

    std::string GetLastMessage()
    {
      return (_inGameMessages.size() > 0) ? _inGameMessages.front() : std::string();
    }

    std::vector<std::string> GetLastMessages()
    {
      std::vector<std::string> res;

      int count = 0;
      for (auto& m : _inGameMessages)
      {
        res.push_back(m);

        count++;

        if (count >= 3)
        {
          break;
        }
      }

      return res;
    }

    const std::vector<std::string>& Messages() { return _inGameMessages; }

    bool ShowLastMessage;

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
