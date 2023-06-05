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
  uint32_t R = 0;
  uint32_t G = 0;
  uint32_t B = 0;
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

  uint32_t FgColor = Colors::WhiteColor;
  uint32_t BgColor = Colors::BlackColor;
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

    std::vector<Position> DrawExplosion(const Position& pos, int range);

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
               const uint32_t& htmlColorFg,
               const uint32_t& htmlColorBg = Colors::BlackColor);

    void Print(const int& x, const int& y,
               const int& ch,
               const uint32_t& htmlColorFg,
               const uint32_t& htmlColorBg = Colors::BlackColor);

    /// Print to "framebuffer" instead of directly to the screen
    void PrintFB(const int& x, const int& y,
                 const int& ch,
                 const uint32_t& htmlColorFg,
                 const uint32_t& htmlColorBg = Colors::BlackColor);

    void PrintFB(const int& x, const int& y,
                 const std::string& text,
                 int align,
                 const uint32_t& htmlColorFg,
                 const uint32_t& htmlColorBg = Colors::BlackColor);

    void PrintFB(const int& x, const int& y,
                 const std::string& text,
                 size_t scale,
                 int align,
                 const uint32_t& htmlColorFg,
                 const uint32_t& htmlColorBg = Colors::BlackColor);

    void DrawWindow(const Position& leftCorner,
                    const Position& size,
                    const std::string& header = std::string{},
                    const uint32_t& headerFgColor = Colors::WhiteColor,
                    const uint32_t& headerBgColor = Colors::BlackColor,
                    const uint32_t& borderColor = Colors::WhiteColor,
                    const uint32_t& borderBgColor = Colors::BlackColor,
                    const uint32_t& bgColor = Colors::BlackColor);

    const std::unordered_map<size_t, ColorPair>& GetValidColorsCache();
#else
    void PrintFB(const int& x, const int& y,
                 int image,
                 const uint32_t& htmlColorFg,
                 const uint32_t& htmlColorBg = Colors::BlackColor);

    void PrintFB(const int& x, const int& y,
                 const std::string& text,
                 int align,
                 const uint32_t& htmlColorFg,
                 const uint32_t& htmlColorBg = Colors::BlackColor);

    void PrintFB(const int& x, const int& y,
                 const std::string& text,
                 size_t scale,
                 int align,
                 const uint32_t& htmlColorFg,
                 const uint32_t& htmlColorBg = Colors::BlackColor);

    void DrawWindow(const Position& leftCorner,
                    const Position& size,
                    const std::string& header = std::string{},
                    const uint32_t& headerFgColor = Colors::WhiteColor,
                    const uint32_t& headerBgColor = Colors::BlackColor,
                    const uint32_t& borderColor = Colors::WhiteColor,
                    const uint32_t& borderBgColor = Colors::BlackColor,
                    const uint32_t& bgColor = Colors::BlackColor);

    void DrawLine(int x1, int y1,
                  int x2, int y2,
                  uint32_t color, uint8_t width = 1);

    const std::unordered_map<uint32_t, TileColor>& GetValidColorsCache();

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
                    const uint32_t& fgColor);
    //
    void AddMessage(const std::string& message,
                    const uint32_t& fgColor,
                    const uint32_t& bgColor);

    GameLogMessageData GetLastMessage();

    void ResetMessagesToDisplay();

    std::vector<GameLogMessageData> GetLastMessages();
    std::vector<GameLogMessageData>& Messages();

    bool ShowLastMessage;

    // NOTE: for debugging purposes
    int ColorsUsed();

  protected:
    void InitSpecific() override;

  private:
    #ifndef USE_SDL
    bool ContainsColorMap(size_t hashToCheck);
    bool ColorIndexExists(size_t hashToCheck);

    NColor GetNColor(const uint32_t& htmlColor);
    size_t GetOrSetColor(const uint32_t& htmlColorFg,
                         const uint32_t& htmlColorBg);
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

    std::unordered_map<uint32_t, TileColor> _validColorsCache;

    int _tileWidth = 0;
    int _tileHeight = 0;

    int _tileWidthScaled = 0;
    int _tileHeightScaled = 0;

    float _tileAspectRatio = 0.0f;

    std::pair<int, int> _tileWH;
    std::pair<int, int> _tileWHScaled;

    SDL_Rect _drawSrc;
    SDL_Rect _drawDst;

    SDL_Rect _renderDst;

    void InitForSDL();

    void DrawTile(int x, int y, int tileIndex);
    void DrawTile(int x, int y, int tileIndex, size_t scale);

    //
    // Here lies data after last ConvertHtmlToRGB() call.
    //
    TileColor _convertedHtml;

    void ConvertHtmlToRGB(const uint32_t& htmlColor);
    #endif

    const uint32_t _maskR = 0x00FF0000;
    const uint32_t _maskG = 0x0000FF00;
    const uint32_t _maskB = 0x000000FF;
};

#endif
