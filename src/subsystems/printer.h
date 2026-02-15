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

#include "position.h"
#include "constants.h"
#include "msg-scroll-buffer.h"
#include "structs.h"

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

    //
    // Window dimensions for graphics mode.
    //
    static const int GraphicsWindowWidth;
    static const int GraphicsWindowHeight;

    static const double GraphicsWindowAspectRatio;

    //
    // Text alignment modes.
    //
    static const int kAlignLeft   = 0;
    static const int kAlignCenter = 1;
    static const int kAlignRight  = 2;

    static const int kShortLogMaxMessages = 5;

    const int& GetLastMessagesCount();

    std::vector<Position> DrawExplosion(const Position& pos, int range);

    ///
    /// \brief Clears framebuffer (ncurses) or renderer (SDL).
    ///
    /// Use this before all PrintFB calls.
    ///
    void Clear();

    ///
    /// \brief Prints framebuffer contents to the screen.
    ///
    /// Call this after all PrintFB calls.
    ///
    void Render();

    ///
    /// \brief Prints character from text tileset.
    /// \param x Horizontal position in character units.
    /// \param y Vertical position in character units.
    /// \param charIndex Tile index to draw from NameCP437.
    /// \param htmlColorFg Foreground color in 0xRRGGBB format.
    /// \param htmlColorBg Background color in 0xRRGGBB format.
    ///
    void PrintChar(const int x,
                   const int y,
                   int charIndex,
                   const uint32_t& htmlColorFg,
                   const uint32_t& htmlColorBg);

    ///
    /// \brief Prints string using text tileset.
    /// \param x Horizontal starting position in character units.
    /// \param y Vertical starting position in character units.
    /// \param text Text string to print.
    /// \param align Text horizontal alignment.
    /// \param htmlColorFg Foreground color in 0xRRGGBB format.
    /// \param htmlColorBg Background color in 0xRRGGBB format.
    ///
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
                    const uint32_t& headerFgColor = Colors::White,
                    const uint32_t& headerBgColor = Colors::Black,
                    const uint32_t& borderColor = Colors::White,
                    const uint32_t& borderBgColor = Colors::Black,
                    const uint32_t& bgColor = Colors::Black);

    const std::unordered_map<size_t, ColorPair>& GetValidColorsCache();
// -----------------------------------------------------------------------------
#else
// -----------------------------------------------------------------------------
    ///
    /// \brief Print text using pixel coordinates with additional tweaks.
    /// \param x Position of upper left corner of the first character tile.
    /// \param y Position of upper left corner of the first character tile.
    /// \param text Text to print.
    /// \param align Text alignment.
    /// \param fgColor Foreground text color.
    /// \param bgColor Background fill text color (Colors::None to leave it 
    /// transparent).
    /// \param scaleFactor Scale factor.
    /// \param shadowOffsetX Horizontal text shadow offset factor in pixels.
    /// \param shadowOffsetY Vertical text shadow offset factor in pixels.
    ///
    void PrintTextExt(int x,
                      int y,
                      const std::string& text,
                      int align,
                      uint32_t fgColor,
                      uint32_t bgColor,
                      double scaleFactor,
                      int shadowOffsetX = 0,
                      int shadowOffsetY = 0);

    ///
    /// \brief Draw tile from graphics tileset.
    /// \param x Position on the screen in tile cell units (e.g. 0 - 40).
    /// \param y Position on the screen in tile cell units (e.g. 0 - 24).
    /// \param tile Tile to draw.
    /// \param colorTint Color tint (0xFFFFFF to draw tile as is).
    ///
    void DrawGraphicsTile(int x,
                          int y,
                          GraphicTiles tile,
                          uint32_t colorTint = Colors::White);

    ///
    /// \brief Draw tile from graphics tileset using pixel coordinates.
    /// \param x position in pixels.
    /// \param y position in pixels.
    /// \param gti Information on how to draw a tile as a GraphicTileInfo struct
    /// variable.
    ///
    void DrawGraphicsTileExt(int x, int y, const GraphicTileInfo& gti);

    ///
    /// \brief Draw tile from substitute graphics tileset.
    /// \param x Position on the screen in tile cell units (e.g. 0 - 40).
    /// \param y Position on the screen in tile cell units (e.g. 0 - 24).
    /// \param image Tile index to draw from NameCP437.
    /// \param colorTint Color tint (0xFFFFFF to draw tile as is).
    ///
    void DrawSubstituteGraphicsTile(int x,
                                    int y,
                                    int image,
                                    uint32_t colorTint = Colors::White);

    void DrawWindow(const Position& leftCorner,
                    const Position& size,
                    const std::string& header = std::string{},
                    const uint32_t& headerFgColor = Colors::White,
                    const uint32_t& headerBgColor = Colors::Black,
                    const uint32_t& borderColor = Colors::White,
                    const uint32_t& borderBgColor = Colors::Black,
                    const uint32_t& bgColor = Colors::Black);

    void DrawRect(int x1, int y1,
                  int x2, int y2,
                  uint32_t color);

    const std::unordered_map<uint32_t, TileColor>& GetValidColorsCache();

    void SetRenderDst(const SDL_Rect& dst);

    SDL_Renderer* Renderer = nullptr;
    SDL_Window* Window     = nullptr;

    SDL_Rect GetWindowSize(int tileSize);

    const PairI& GetDefaultWindowSize();
    PairI& ResizedWindowSize();
    const PairI& GetTileWH();
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
                  Colors::White,
                  Colors::Black);
        #else
        PrintFB(x, y, arrowChar, Colors::White, Colors::Black);
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
                      Colors::Black,
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
                  Colors::White,
                  Colors::Black);
      }
    }

    static const int TextTileWidth  = 8;
    static const int TextTileHeight = 16;

    static const int SgTileSize = 16;

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

    // User defined.
    SDL_Texture* _graphicTileset = nullptr;

    // 16x16 CP437 chart
    SDL_Texture* _sgGraphicTileset = nullptr;

    // 8x16 CP437 chart
    SDL_Texture* _textTileset = nullptr;

    SDL_Texture* _frameBuffer = nullptr;

    int _graphicTilesetWidth  = 0;
    int _graphicTilesetHeight = 0;

    int _textTilesetWidth  = 0;
    int _textTilesetHeight = 0;

    std::vector<TileInfo> _textTilesInfo;
    std::vector<TileInfo> _graphicTilesInfo;
    std::vector<TileInfo> _sgTilesInfo;

    std::unordered_map<uint32_t, TileColor> _validColorsCache;

    PairI _defaultWindowSize;
    PairI _resizedWindowSize;
    PairI _tileWH;

    int _textCharsCountH = 0;
    int _textCharsCountV = 0;

    int _tilesCountH = 0;
    int _tilesCountV = 0;

    int _graphicTileSize = SgTileSize;

    double _sgScaleFactor = 1.0;

    //
    // Scaling of text tiles for different window size depending on custom
    // graphics tileset size.
    //
    int _textTileWidthScaled  = TextTileWidth;
    int _textTileHeightScaled = TextTileHeight;

    SDL_Rect _drawSrc;
    SDL_Rect _drawDst;

    SDL_Rect _renderDst;

    bool InitForSDL();
    bool SetWindowIcon();

    bool LoadTextTileset();
    bool LoadGraphicsTileset();
    bool LoadSubstituteGraphicTileset();

    void DrawFromTextTileset(int x, int y, int tileIndex);

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
