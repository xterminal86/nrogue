#ifndef PRINTER_H
#define PRINTER_H

#include <string>
#include <map>
#include <vector>

#ifndef USE_SDL
#include <ncurses.h>
#else
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#endif

#include "singleton.h"
#include "colorpair.h"
#include "position.h"

#ifdef USE_SDL
struct TileColor
{
  int R = 0;
  int G = 0;
  int B = 0;
};

struct TileInfo
{
  int X = 0;
  int Y = 0;
  std::string Data;
};

#else
struct FBPixel
{
  size_t ColorPairHash;
  int Character;
};
#endif

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

    std::vector<Position> DrawExplosion(Position pos);

    /// Clears framebuffer (ncurses) or renderer (SDL)
    /// Use this before all PrintFB calls
    void Clear();

    /// Prints framebuffer contents to the screen
    /// Call this after all PrintFB calls
    void Render();

#ifndef USE_SDL
    /// Print text at (x, y) directly to the screen,
    /// with (0, 0) at upper left corner and y increases down
    void Print(const int& x, const int& y,
                const std::string& text,
                int align,
                const std::string& htmlColorFg,
                const std::string& htmlColorBg = "#000000");

    void Print(const int& x, const int& y,
                const int& ch,
                const std::string& htmlColorFg,
                const std::string& htmlColorBg = "#000000");

    /// Print to "framebuffer" instead of directly to the screen
    void PrintFB(const int& x, const int& y,
                  const int& ch,
                  const std::string& htmlColorFg,
                  const std::string& htmlColorBg = "#000000");

    void PrintFB(const int& x, const int& y,
                  const std::string& text,
                  int align,
                  const std::string& htmlColorFg,
                  const std::string& htmlColorBg = "#000000");    
#else
    void PrintFB(const int& x, const int& y,
                 int image,
                 const std::string& htmlColorFg,
                 const std::string& htmlColorBg = "#000000");

    void PrintFB(const int& x, const int& y,
                 const std::string& text,
                 int align,
                 const std::string& htmlColorFg,
                 const std::string& htmlColorBg = "#000000");

    void DrawImage(const int& x, const int& y, SDL_Texture* tex);
    void DrawWindow(const Position& leftCorner,
                    const Position& size,
                    const std::string& header = std::string{},
                    const std::string& headerFgColor = "#FFFFFF",
                    const std::string& headerBgColor = "#000000",
                    const std::string& borderColor = "#FFFFFF",
                    const std::string& borderBgColor = "#000000",
                    const std::string& bgColor = "#000000");

#endif

    /// Add message to the game log
    void AddMessage(const std::string& message);

    std::string GetLastMessage()
    {
      return (_inGameMessages.size() > 0) ? _inGameMessages.front() : std::string();
    }

    void ResetMessagesToDisplay()
    {
      _lastMessages.clear();
      _lastMessagesToDisplay = 0;
    }

    std::vector<std::string> GetLastMessages();

    std::vector<std::string>& Messages() { return _inGameMessages; }

    bool ShowLastMessage;

    // FIXME: debug
    int ColorsUsed()
    {
      #ifndef USE_SDL
      return _colorMap.size();
      #else
      return -1;
      #endif
    }

  private:
#ifndef USE_SDL
    bool ContainsColorMap(size_t hashToCheck);
    bool ColorIndexExists(size_t hashToCheck);

    NColor GetNColor(const std::string& htmlColor);
    size_t GetOrSetColor(const std::string& htmlColorFg, const std::string& htmlColorBg);
    std::pair<int, int> AlignText(int x, int y, int align, const std::string& text);

    void PrepareFrameBuffer();

    std::map<size_t, ColorPair> _colorMap;
    std::map<size_t, short> _colorIndexMap;

    short _colorPairsGlobalIndex = 1;
    short _colorGlobalIndex = 8;

    std::vector<std::vector<FBPixel>> _frameBuffer;
#endif

    std::vector<std::string> _inGameMessages;    
    std::vector<std::string> _lastMessages;

    int _lastMessagesToDisplay = 0;

    const int kMaxGameLogMessages = 100;

    #ifndef USE_SDL
    void InitForCurses();
    #else
    SDL_Texture* _tileset = nullptr;
    SDL_Texture* _frameBuffer = nullptr;

    int _tilesetWidth = 0;
    int _tilesetHeight = 0;

    std::vector<TileInfo> _tiles;
    std::map<char, int> _tileIndexByChar;

    int _tileWidth = 0;
    int _tileHeight = 0;

    int _tileWidthScaled = 0;
    int _tileHeightScaled = 0;

    void InitForSDL();

    void DrawTile(int x, int y, int tileIndex);

    TileColor ConvertHtmlToRGB(const std::string& htmlColor);

    #endif

    std::vector<Position> GetAreaDamagePointsFrom(Position from, int range);
};

#endif
