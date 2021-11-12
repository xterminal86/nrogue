#ifndef PRINTER_H
#define PRINTER_H

#include <string>
#include <unordered_map>
#include <vector>

#ifndef USE_SDL
#include <ncurses.h>
#else
#include "SDL2/SDL.h"
#endif

#include "singleton.h"
#include "colorpair.h"
#include "position.h"
#include "constants.h"

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
};
#else
struct FBPixel
{
  size_t ColorPairHash;
  int Character;
};
#endif

struct GameLogMessageData
{
  std::string Message;
  std::string FgColor = Colors::WhiteColor;
  std::string BgColor = Colors::BlackColor;
};

/// Singleton for ncurses text printing
class Printer : public Singleton<Printer>
{
  public:
    static size_t TerminalWidth;
    static size_t TerminalHeight;

    static const int kAlignLeft = 0;
    static const int kAlignCenter = 1;
    static const int kAlignRight = 2;

    void Init() override;

    std::vector<Position> DrawExplosion(Position pos, int range);

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
                const std::string& htmlColorBg = Colors::BlackColor);

    void Print(const int& x, const int& y,
                const int& ch,
                const std::string& htmlColorFg,
                const std::string& htmlColorBg = Colors::BlackColor);

    /// Print to "framebuffer" instead of directly to the screen
    void PrintFB(const int& x, const int& y,
                  const int& ch,
                  const std::string& htmlColorFg,
                  const std::string& htmlColorBg = Colors::BlackColor);

    void PrintFB(const int& x, const int& y,
                  const std::string& text,
                  int align,
                  const std::string& htmlColorFg,
                  const std::string& htmlColorBg = Colors::BlackColor);

    void PrintFB(const int& x, const int& y,
                  const std::string& text,
                  size_t scale,
                  int align,
                  const std::string& htmlColorFg,
                  const std::string& htmlColorBg = Colors::BlackColor);

    const std::unordered_map<size_t, ColorPair>& GetValidColorsCache();
#else
    void PrintFB(const int& x, const int& y,
                 int image,
                 const std::string& htmlColorFg,
                 const std::string& htmlColorBg = Colors::BlackColor);

    void PrintFB(const int& x, const int& y,
                 const std::string& text,
                 int align,
                 const std::string& htmlColorFg,
                 const std::string& htmlColorBg = Colors::BlackColor);

    void PrintFB(const int& x, const int& y,
                 const std::string& text,
                 size_t scale,
                 int align,
                 const std::string& htmlColorFg,
                 const std::string& htmlColorBg = Colors::BlackColor);

    void DrawWindow(const Position& leftCorner,
                    const Position& size,
                    const std::string& header = std::string{},
                    const std::string& headerFgColor = Colors::WhiteColor,
                    const std::string& headerBgColor = Colors::BlackColor,
                    const std::string& borderColor = Colors::WhiteColor,
                    const std::string& borderBgColor = Colors::BlackColor,
                    const std::string& bgColor = Colors::BlackColor);

    const std::unordered_map<std::string, TileColor>& GetValidColorsCache();

    void SetRenderDst(const SDL_Rect& dst);

    float GetTileAspectRatio();

    const std::pair<int, int>& GetTileWH();
    const std::pair<int, int>& GetTileWHScaled();
#endif

    /// Add message to the game log
    void AddMessage(const GameLogMessageData& data);

    // Some overloads so that I don't have to edit AddMessage()
    // all over the codebase.
    void AddMessage(const std::string& message);
    //
    void AddMessage(const std::string& message,
                    const std::string& fgColor);
    //
    void AddMessage(const std::string& message,
                    const std::string& fgColor,
                    const std::string& bgColor);

    GameLogMessageData GetLastMessage();

    void ResetMessagesToDisplay();

    std::vector<GameLogMessageData> GetLastMessages();
    std::vector<GameLogMessageData>& Messages();

    bool ShowLastMessage;

    // NOTE: for debugging purposes
    int ColorsUsed();

  private:
    #ifndef USE_SDL
    bool ContainsColorMap(size_t hashToCheck);
    bool ColorIndexExists(size_t hashToCheck);

    NColor GetNColor(const std::string& htmlColor);
    size_t GetOrSetColor(const std::string& htmlColorFg, const std::string& htmlColorBg);
    std::pair<int, int> AlignText(int x, int y, int align, const std::string& text);

    void PrepareFrameBuffer();

    std::unordered_map<size_t, ColorPair> _colorMap;
    std::unordered_map<size_t, short> _colorIndexMap;

    short _colorPairsGlobalIndex = 1;
    short _colorGlobalIndex = 8;

    std::vector<std::vector<FBPixel>> _frameBuffer;
    #endif

    std::vector<GameLogMessageData> _inGameMessages;
    std::vector<GameLogMessageData> _lastMessages;

    int _lastMessagesToDisplay = 0;

    const size_t kMaxGameLogMessages = 100;

    int _messageRepeatCounter = 0;
    std::string _repeatingMessage;

    #ifndef USE_SDL
    void InitForCurses();
    #else
    SDL_Texture* _tileset = nullptr;
    SDL_Texture* _frameBuffer = nullptr;

    int _tilesetWidth = 0;
    int _tilesetHeight = 0;

    std::vector<TileInfo> _tiles;
    std::unordered_map<char, int> _tileIndexByChar;

    std::unordered_map<std::string, TileColor> _validColorsCache;

    int _tileWidth = 0;
    int _tileHeight = 0;

    int _tileWidthScaled = 0;
    int _tileHeightScaled = 0;

    float _tileAspectRatio = 0.0f;

    std::pair<int, int> _tileWH;
    std::pair<int, int> _tileWHScaled;

    SDL_Rect _renderDst;

    void InitForSDL();

    void DrawTile(int x, int y, int tileIndex);
    void DrawTile(int x, int y, int tileIndex, size_t scale);

    TileColor ConvertHtmlToRGB(const std::string& htmlColor);
    #endif

    bool IsColorStringValid(const std::string& htmlColor);
};

#endif
