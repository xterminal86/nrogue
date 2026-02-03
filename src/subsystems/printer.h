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

#include "colorpair.h"
#include "position.h"
#include "constants.h"
#include "msg-scroll-buffer.h"

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

///
/// \brief Everything you need to draw stuff on the screen.
///
class Printer
{
  public:
    //
    // Screen width and height in character cells.
    //
    static size_t TerminalWidth;
    static size_t TerminalHeight;

    static const int kAlignLeft = 0;
    static const int kAlignCenter = 1;
    static const int kAlignRight = 2;

    static const int kShortLogMaxMessages = 5;

    const int& GetLastMessagesCount();

    std::vector<Position> DrawExplosion(const Position& pos, int range);

    /// Clears framebuffer (ncurses) or renderer (SDL)
    /// Use this before all PrintFB calls
    void Clear();

    /// Prints framebuffer contents to the screen
    /// Call this after all PrintFB calls
    void Render();

    void PrintChar(const int x,
                   const int y,
                   int charIndex,
                   const uint32_t& htmlColorFg,
                   const uint32_t& htmlColorBg);

    void PrintText(const int x,
                   const int y,
                   const std::string& text,
                   int align,
                   const uint32_t& htmlColorFg,
                   const uint32_t& htmlColorBg);
#ifndef USE_SDL
// -----------------------------------------------------------------------------
    /// Print text at (x, y) directly to the screen,
    /// with (0, 0) at upper left corner and y increases down
    void Print(const int& x, const int& y,
               const std::string& text,
               int align,
               const uint32_t& htmlColorFg,
               const uint32_t& htmlColorBg);

    void Print(const int& x, const int& y,
               const int& ch,
               const uint32_t& htmlColorFg,
               const uint32_t& htmlColorBg);

    /// Print to "framebuffer" instead of directly to the screen
    void PrintFB(const int& x, const int& y,
                 const int& ch,
                 const uint32_t& htmlColorFg,
                 const uint32_t& htmlColorBg);

    void PrintFB(const int& x, const int& y,
                 const std::string& text,
                 int align,
                 const uint32_t& htmlColorFg,
                 const uint32_t& htmlColorBg);

    void DrawWindow(const Position& leftCorner,
                    const Position& size,
                    const std::string& header = std::string{},
                    const uint32_t& headerFgColor = Colors::WhiteColor,
                    const uint32_t& headerBgColor = Colors::BlackColor,
                    const uint32_t& borderColor = Colors::WhiteColor,
                    const uint32_t& borderBgColor = Colors::BlackColor,
                    const uint32_t& bgColor = Colors::BlackColor);

    const std::unordered_map<size_t, ColorPair>& GetValidColorsCache();
// -----------------------------------------------------------------------------
#else
// -----------------------------------------------------------------------------
    void DrawGraphicsTile(int x, int y, GraphicTiles tile, uint32_t color);

    void DrawWindow(const Position& leftCorner,
                    const Position& size,
                    const std::string& header = std::string{},
                    const uint32_t& headerFgColor = Colors::WhiteColor,
                    const uint32_t& headerBgColor = Colors::BlackColor,
                    const uint32_t& borderColor = Colors::WhiteColor,
                    const uint32_t& borderBgColor = Colors::BlackColor,
                    const uint32_t& bgColor = Colors::BlackColor);

    void DrawRect(int x1, int y1,
                  int x2, int y2,
                  uint32_t color);

    const std::unordered_map<uint32_t, TileColor>& GetValidColorsCache();

    void SetRenderDst(const SDL_Rect& dst);

    SDL_Renderer* Renderer = nullptr;
    SDL_Window* Window     = nullptr;

    SDL_Rect GetWindowSize(int tileSize);

    const PairI& GetDefaultWindowSize();
    PairI& GetResizedWindowSize();
    const PairI& GetTileWHScaled();
// -----------------------------------------------------------------------------
#endif

    /// Add message to the game log
    void AddMessage(const GameLogMessageData& data);

    //
    // Some overloads so that I don't have to edit AddMessage()
    // all over the codebase.
    //
    void AddMessage(const std::string& message);
    //
    void AddMessage(const std::string& message,
                    const uint32_t& fgColor);
    //
    void AddMessage(const std::string& message,
                    const uint32_t& fgColor,
                    const uint32_t& bgColor);

    void InitMsgBufferObj();

    GameLogMessageData* GetLastMessage();

    void ResetMessagesToDisplay();

    const std::vector<GameLogMessageData*>& GetLastMessages();
    const std::vector<GameLogMessageData*>& Messages();

    MsgScrollBuffer<GameLogMessageData>& GetMsgBufferObj();

    bool ShowLastMessage;

    //
    // NOTE: for debugging purposes.
    //
    int ColorsUsed();

    bool IsReady();

    void Init();

    template <typename MsgScrollBufferClass>
    void DrawScrollBars(const MsgScrollBufferClass& buffer)
    {
      auto DrawArrow = [this](int x, int y, int arrowChar)
      {
        #ifdef USE_SDL
        //PrintFB(x, y, arrowChar, Colors::WhiteColor, Colors::BlackColor);
        PrintChar(x,
                  y,
                  arrowChar,
                  Colors::WhiteColor,
                  Colors::BlackColor);
        #else
        PrintFB(x, y, arrowChar, Colors::WhiteColor, Colors::BlackColor);
        #endif
      };

      MessageBufferScrollState s = buffer.GetScrollState();
      switch (s)
      {
        case MessageBufferScrollState::NONE:
          break;

        default:
        {
          for (int y = 2; y < (int)TerminalHeight - 1; y++)
          {
            PrintChar(TerminalWidth - 1,
                      y,
                      ' ',
                      Colors::BlackColor,
                      Colors::ShadesOfGrey::Eight);
          }

          #ifdef USE_SDL
          int arrowDown = (s == MessageBufferScrollState::BOTTOM)
                          ? 'x'
                          : (int)NameCP437::DARROW_2;
          int arrowUp   = (s == MessageBufferScrollState::TOP)
                          ? 'x'
                          : (int)NameCP437::UARROW_2;
          #else
          int arrowDown = (s == MessageBufferScrollState::BOTTOM)
                          ? 'x'
                          : ACS_DARROW;
          int arrowUp   = (s == MessageBufferScrollState::TOP)
                          ? 'x'
                          : ACS_UARROW;
          #endif
          DrawArrow(TerminalWidth - 1, TerminalHeight - 1, arrowDown);
          DrawArrow(TerminalWidth - 1, 1, arrowUp);
        }
        break;
      }

      //
      // Draw scroll progress.
      //
      if (s != MessageBufferScrollState::NONE)
      {
        double progress = buffer.GetScrollProgress();

        //
        //   <-- 0
        // ^
        // |                    --
        // |                      |
        // |                      |
        // |                      | markerMoveArea
        // |                      |
        // | <-- aboveDownArrow --
        // V
        //   <-- TerminalHeight
        //
        // progress goes from 0.0 (bottom) to 1.0 (top).
        //

        int aboveDownArrow = TerminalHeight - 2;
        int markerMoveArea = TerminalHeight - 4;
        PrintChar(TerminalWidth - 1,
                  aboveDownArrow - (int)((double)markerMoveArea * progress),
                  '=',
                  Colors::WhiteColor,
                  Colors::BlackColor);
      }
    }

  private:
    #ifndef USE_SDL
    bool ContainsColorMap(size_t hashToCheck);
    bool ColorIndexExists(size_t hashToCheck);

    NColor GetNColor(const uint32_t& htmlColor);
    size_t GetOrSetColor(const uint32_t& htmlColorFg,
                         const uint32_t& htmlColorBg);
    std::pair<int, int> AlignText(int x,
                                  int y,
                                  int align,
                                  const std::string& text);

    void PrepareFrameBuffer();

    std::unordered_map<size_t, ColorPair> _colorMap;
    std::unordered_map<size_t, short> _colorIndexMap;

    short _colorPairsGlobalIndex = 1;
    short _colorGlobalIndex = 8;

    std::vector<std::vector<FBPixel>> _frameBuffer;
    #endif

    bool _ok = false;

    using MsgBuffer = MsgScrollBuffer<GameLogMessageData>;
    std::unique_ptr<MsgBuffer> _inGameMessages;

    int _lastMessagesToDisplay = 0;

    int _messageRepeatCounter = 0;
    std::string _repeatingMessage;

    #ifndef USE_SDL
    bool InitForCurses();
    #else

    SDL_Texture* _graphicTileset = nullptr;
    SDL_Texture* _textTileset = nullptr;
    SDL_Texture* _frameBuffer = nullptr;

    int _graphicTilesetWidth  = 0;
    int _graphicTilesetHeight = 0;

    int _textTilesetWidth  = 0;
    int _textTilesetHeight = 0;

    std::vector<TileInfo> _textTiles;
    std::vector<TileInfo> _graphicTiles;

    std::unordered_map<uint32_t, TileColor> _validColorsCache;

    PairI _defaultWindowSize;
    PairI _resizedWindowSize;
    PairI _tileWHScaled;

    const int _textTileWidth  = 8;
    const int _textTileHeight = 16;

    int _textCharsCountH = 0;
    int _textCharsCountV = 0;

    int _graphicTileSize = 0;

    int _graphicTileSizeScaled = 16;

    SDL_Rect _drawSrc;
    SDL_Rect _drawDst;

    SDL_Rect _renderDst;

    bool InitForSDL();
    bool SetWindowIcon();

    bool LoadTextTileset();
    bool LoadGraphicsTileset();
    bool LoadSubstituteGraphicTileset();

    void DrawFromTextTileset(int x, int y, int tileIndex);
    void DrawFromGraphicsTileset(int x, int y, int tileIndex);

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
