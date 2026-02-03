#include "printer.h"

#include "application.h"
#include "map.h"
#include "util.h"
#include "base64-strings.h"

#ifdef DEBUG_BUILD
#include "logger.h"
#endif

size_t Printer::TerminalWidth  = 80;
size_t Printer::TerminalHeight = 24;

void Printer::Init()
{
  if (_ok)
  {
    return;
  }

#ifdef USE_SDL
  _ok = InitForSDL();
#else
  _ok = InitForCurses();
#endif
}

// =============================================================================

////////////////////////////////////////////////////////////////////////////////

#ifdef USE_SDL
bool Printer::LoadTextTileset()
{
  auto res = Util::Base64_Decode(Base64Strings::Tileset8x16Base64);
  auto bytes = Util::ConvertStringToBytes(res);
  SDL_RWops* data = SDL_RWFromMem(bytes.data(), bytes.size());
  SDL_Surface* surf = SDL_LoadBMP_RW(data, 1);
  if (!surf)
  {
    ConsoleLog("[ERR] could not load tileset from memory: '%s'",
               SDL_GetError());
    return false;
  }

  SDL_SetColorKey(surf, SDL_TRUE, SDL_MapRGB(surf->format, 0xFF, 0, 0xFF));
  _textTileset = SDL_CreateTextureFromSurface(Renderer, surf);
  if (_textTileset == nullptr)
  {
    ConsoleLog("[ERR] SDL_CreateTextureFromSurface() fail: '%s'\n",
               SDL_GetError());
    return false;
  }

  SDL_FreeSurface(surf);

  int w = 0, h = 0;
  SDL_QueryTexture(_textTileset, nullptr, nullptr, &w, &h);

  _textTilesetWidth  = w;
  _textTilesetHeight = h;

  return true;
}

// =============================================================================

bool Printer::LoadSubstituteGraphicTileset()
{
  auto gameConfig = Game::gApp.GameConfig;

  auto res = Util::Base64_Decode(Base64Strings::GraphicsTileset16x16Base64);
  auto bytes = Util::ConvertStringToBytes(res);
  SDL_RWops* data = SDL_RWFromMem(bytes.data(), bytes.size());
  SDL_Surface* surf = SDL_LoadBMP_RW(data, 1);
  if (!surf)
  {
    ConsoleLog("[ERR] could not load tileset from memory: '%s'",
               SDL_GetError());
    return false;
  }

  SDL_SetColorKey(surf, SDL_TRUE, SDL_MapRGB(surf->format, 0xFF, 0, 0xFF));
  _graphicTileset = SDL_CreateTextureFromSurface(Renderer, surf);
  if (_graphicTileset == nullptr)
  {
    ConsoleLog("[ERR] SDL_CreateTextureFromSurface() fail: '%s'\n",
               SDL_GetError());
    return false;
  }

  SDL_FreeSurface(surf);

  int w = 0, h = 0;
  SDL_QueryTexture(_graphicTileset, nullptr, nullptr, &w, &h);

  _graphicTilesetWidth  = w;
  _graphicTilesetHeight = h;

  _graphicTileSize = 16;
  _graphicTileSizeScaled =
      (int)((double)_graphicTileSize * gameConfig.ScaleFactor);

  return true;
}

// =============================================================================

bool Printer::LoadGraphicsTileset()
{
  auto gameConfig = Game::gApp.GameConfig;

  SDL_Surface* surf = SDL_LoadBMP(gameConfig.TilesetFilename.data());
  if (surf)
  {
    SDL_SetColorKey(surf, SDL_TRUE, SDL_MapRGB(surf->format, 0xFF, 0, 0xFF));
    _graphicTileset = SDL_CreateTextureFromSurface(Renderer, surf);
    if (_graphicTileset == nullptr)
    {
      ConsoleLog("[ERR] SDL_CreateTextureFromSurface() fail: '%s'\n",
                 SDL_GetError());
      return false;
    }

    SDL_FreeSurface(surf);

    int w = 0, h = 0;
    SDL_QueryTexture(_graphicTileset, nullptr, nullptr, &w, &h);

    if ((w % gameConfig.TileSize) != 0 &&
        (h % gameConfig.TileSize) != 0)
    {
      ConsoleLog("[ERR] invalid tileset size %dx%d! Must be divisible by %d",
                 w, h, Game::gApp.GameConfig.TileSize);
      return false;
    }

    _graphicTilesetWidth  = w;
    _graphicTilesetHeight = h;

    _graphicTileSize = gameConfig.TileSize;
    _graphicTileSizeScaled =
        (int)((double)_graphicTileSize * gameConfig.ScaleFactor);
  }

  return true;
}

// =============================================================================

bool Printer::SetWindowIcon()
{
  auto res = Util::Base64_Decode(Base64Strings::IconBase64);
  auto bytes = Util::ConvertStringToBytes(res);
  SDL_RWops* data = SDL_RWFromMem(bytes.data(), bytes.size());
  if (data == nullptr)
  {
    ConsoleLog("[ERR] failed to read from memory! '%s'", SDL_GetError());
    return false;
  }

  SDL_Surface* surf = SDL_LoadBMP_RW(data, 1);
  if (!surf)
  {
    ConsoleLog("[ERR] could not load image from memory: '%s'", SDL_GetError());
    return false;
  }

  SDL_SetColorKey(surf, SDL_TRUE, SDL_MapRGB(surf->format, 0xFF, 0, 0xFF));
  SDL_SetWindowIcon(Window, surf);
  SDL_FreeSurface(surf);

  return true;
}

// =============================================================================

const PairI& Printer::GetDefaultWindowSize()
{
  return _defaultWindowSize;
}

// =============================================================================

PairI& Printer::GetResizedWindowSize()
{
  return _resizedWindowSize;
}

// =============================================================================

const PairI& Printer::GetTileWHScaled()
{
  return _tileWHScaled;
}

// =============================================================================

//
// Returns SDL_Rect with initial window position in x, y
// and window size in w, h
//
SDL_Rect Printer::GetWindowSize(int tileSize)
{
  SDL_Rect res;

  int ww = 0, wh = 0;

  if (Game::gApp.GameConfig.UseGraphics)
  {
    ww = Game::gApp.GameConfig.TileSize * 40;
    wh = Game::gApp.GameConfig.TileSize * 20;
  }
  else
  {
    ww =  8 * GlobalConstants::TerminalStdWidth;
    wh = 16 * GlobalConstants::TerminalStdHeight;
  }

  Game::gApp.GameConfig.WindowWidth  = ww;
  Game::gApp.GameConfig.WindowHeight = wh;

  res.w = ww;
  res.h = wh;

  SDL_DisplayMode dm;
  SDL_GetCurrentDisplayMode(0, &dm);

  //
  // Subtract current display size from created window size
  // to determine starting window position,
  // which should be relatively centered.
  //
  int wx = dm.w / 2 - ww / 2;
  int wy = dm.h / 2 - wh / 2;

  res.x = wx;
  res.y = wy;

  return res;
}

// =============================================================================

bool Printer::InitForSDL()
{
  Printer::TerminalWidth  = GlobalConstants::TerminalStdWidth;
  Printer::TerminalHeight = GlobalConstants::TerminalStdHeight;

  SDL_Rect rect = GetWindowSize(Game::gApp.GameConfig.TileSize);

  _defaultWindowSize = { rect.w, rect.h };

  _resizedWindowSize = _defaultWindowSize;

  Window = SDL_CreateWindow("nrogue",
                            rect.x, rect.y,
                            rect.w, rect.h,
                            SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
  if (Window == nullptr)
  {
    ConsoleLog("[ERR] SDL_CreateWindow fail: '%s'", SDL_GetError());
    return false;
  }

  //
  // NOTE: it looks like "direct3d" is Direct3D 9 (which is kinda slow
  // and reports some weird error / warning in stdout after maximizing
  // the window).
  // "direct3d11" works OK, but produces another weird behaviour:
  // when monster attacks player, attack animation displays every other time
  // while player's attack animation works fine.
  // "direct3d12" seems to have no such issues, but WTF is this?!
  // What the hell is one supposed to choose?!
  //

#if defined(MSVC_COMPILER) || defined(__WIN64__) || defined(__WIN32__)
  SDL_SetHint(SDL_HINT_RENDER_DRIVER, "direct3d12");
#else
  SDL_SetHint(SDL_HINT_RENDER_DRIVER, "opengl");
#endif

  Renderer = SDL_CreateRenderer(Window,
                                -1,
                                SDL_RENDERER_ACCELERATED |
                                SDL_RENDERER_TARGETTEXTURE);
  if (Renderer == nullptr)
  {
    ConsoleLog("[WAR] SDL_CreateRenderer() fail: '%s'", SDL_GetError());
    ConsoleLog("Trying software mode...");

    Renderer = SDL_CreateRenderer(Window,
                                  -1,
                                  SDL_RENDERER_SOFTWARE |
                                  SDL_RENDERER_TARGETTEXTURE);
    if (Renderer == nullptr)
    {
      ConsoleLog("[ERR] SDL_CreateRenderer() fail: '%s'", SDL_GetError());
      return false;
    }
  }

  SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 255);

  if (!SetWindowIcon())
  {
    return false;
  }

  auto& gameConfig = Game::gApp.GameConfig;

  if (!LoadTextTileset())
  {
    return false;
  }

  bool useSubstituteTileset = true;
  if (gameConfig.UseGraphics)
  {
    useSubstituteTileset = false;

    if (!LoadGraphicsTileset())
    {
      useSubstituteTileset = true;
    }
  }

  if (useSubstituteTileset)
  {
    if (!LoadSubstituteGraphicTileset())
    {
      return false;
    }
  }

  rect = GetWindowSize(_graphicTileSize);

  SDL_SetWindowPosition(Window, rect.x, rect.y);
  SDL_SetWindowSize(Window, rect.w, rect.h);

  ConsoleLog("[INFO] Window size: %dx%d",
             gameConfig.WindowWidth,
             gameConfig.WindowHeight);

  _frameBuffer = SDL_CreateTexture(Renderer,
                                   SDL_PIXELFORMAT_RGBA32,
                                   SDL_TEXTUREACCESS_TARGET,
                                   gameConfig.WindowWidth,
                                   gameConfig.WindowHeight);

  if (_frameBuffer == nullptr)
  {
    ConsoleLog("[ERR] SDL_CreateTexture() fail: '%s'\n", SDL_GetError());
    return false;
  }

  _tileWHScaled = { gameConfig.TileSize, gameConfig.TileSize };

  _defaultWindowSize.first  = gameConfig.WindowWidth;
  _defaultWindowSize.second = gameConfig.WindowHeight;

  Game::gPrnt.SetRenderDst(
    {
      0,
      0,
      _defaultWindowSize.first,
      _defaultWindowSize.second
    }
  );

  for (int y = 0; y < _textTilesetHeight; y += _textTileHeight)
  {
    for (int x = 0; x < _textTilesetWidth; x += _textTileWidth)
    {
      TileInfo ti = { x, y };

      _textTiles.push_back(ti);
    }
  }

  for (int y = 0; y < _graphicTilesetHeight; y += Game::gApp.GameConfig.TileSize)
  {
    for (int x = 0; x < _graphicTilesetWidth; x += Game::gApp.GameConfig.TileSize)
    {
      TileInfo ti = { x, y };

      _graphicTiles.push_back(ti);
    }
  }

  //
  // Hacky way of doing it but that's C++ for you.
  //
  for (int i = (int)NameCP437::FIRST; i < (int)NameCP437::LAST_ELEMENT; i++)
  {
    NameCP437 key = static_cast<NameCP437>(i);
    GlobalConstants::CP437IndexByType[key] = i;
  }

  for (int i = (int)GraphicTiles::FIRST; i < (int)GraphicTiles::LAST; i++)
  {
    GraphicTiles key = static_cast<GraphicTiles>(i);
    GlobalConstants::GraphicTileByType[key] = i;
  }

  _textCharsCountH = gameConfig.WindowWidth / 8;
  _textCharsCountV = gameConfig.WindowHeight / 16;

  ConsoleLog("[INFO] character cell count: %dx%d",
             _textCharsCountH,
             _textCharsCountV);

  Printer::TerminalWidth  = _textCharsCountH;
  Printer::TerminalHeight = _textCharsCountV;

  return true;
}

// =============================================================================

void Printer::DrawWindow(const Position& leftCorner,
                         const Position& size,
                         const std::string& header,
                         const uint32_t& headerFgColor,
                         const uint32_t& headerBgColor,
                         const uint32_t& borderColor,
                         const uint32_t& borderBgColor,
                         const uint32_t& bgColor)
{
  auto res = Util::GetPerimeter(leftCorner.X, leftCorner.Y,
                                size.X, size.Y, true);

  int x = leftCorner.X;
  int y = leftCorner.Y;

  int ulCorner = GlobalConstants::CP437IndexByType[NameCP437::ULCORNER_2];
  int urCorner = GlobalConstants::CP437IndexByType[NameCP437::URCORNER_2];
  int dlCorner = GlobalConstants::CP437IndexByType[NameCP437::DLCORNER_2];
  int drCorner = GlobalConstants::CP437IndexByType[NameCP437::DRCORNER_2];
  int hBarU    = GlobalConstants::CP437IndexByType[NameCP437::HBAR_2];
  int hBarD    = GlobalConstants::CP437IndexByType[NameCP437::HBAR_2];
  int vBarL    = GlobalConstants::CP437IndexByType[NameCP437::VBAR_2];
  int vBarR    = GlobalConstants::CP437IndexByType[NameCP437::VBAR_2];

  /*
  int ulCorner = GlobalConstants::CP437IndexByType[NameCP437::ULCORNER_3];
  int urCorner = GlobalConstants::CP437IndexByType[NameCP437::URCORNER_3];
  int dlCorner = GlobalConstants::CP437IndexByType[NameCP437::DLCORNER_3];
  int drCorner = GlobalConstants::CP437IndexByType[NameCP437::DRCORNER_3];
  int hBarU    = GlobalConstants::CP437IndexByType[NameCP437::HBAR_3U];
  int hBarD    = GlobalConstants::CP437IndexByType[NameCP437::HBAR_3D];
  int vBarL    = GlobalConstants::CP437IndexByType[NameCP437::VBAR_3L];
  int vBarR    = GlobalConstants::CP437IndexByType[NameCP437::VBAR_3R];
  */

  // Fill background

  if (bgColor != Colors::None)
  {
    for (int i = x + 1; i < x + size.X; i++)
    {
      for (int j = y + 1; j < y + size.Y; j++)
      {
        PrintChar(i, j, ' ', Colors::WhiteColor, bgColor);
      }
    }
  }

  // Corners

  PrintChar(x,          y,          ulCorner, borderColor, borderBgColor);
  PrintChar(x + size.X, y,          urCorner, borderColor, borderBgColor);
  PrintChar(x,          y + size.Y, dlCorner, borderColor, borderBgColor);
  PrintChar(x + size.X, y + size.Y, drCorner, borderColor, borderBgColor);

  // Horizontal bars

  for (int i = x + 1; i < x + size.X; i++)
  {
    PrintChar(i, y,          hBarU, borderColor, borderBgColor);
    PrintChar(i, y + size.Y, hBarD, borderColor, borderBgColor);
  }

  // Vertical bars

  for (int i = y + 1; i < y + size.Y; i++)
  {
    PrintChar(x,          i, vBarL, borderColor, borderBgColor);
    PrintChar(x + size.X, i, vBarR, borderColor, borderBgColor);
  }

  if (header.length() != 0)
  {
    std::string lHeader = header;
    lHeader.insert(0, " ");
    lHeader.append(" ");

    int stringPixelWidth = (lHeader.length() * _graphicTileSizeScaled);
    int headerPosX = x * _graphicTileSizeScaled;
    int headerPosY = y * _graphicTileSizeScaled;

    // size.X actually gives + 1 (see corners section of PrintFBs above),
    // so if size.X = 4 it means span from x to x + 4 end point.
    // Thus, we either must align to string.length() / 2, or
    // adjust header's position by adding additional _tileWidthScaled / 2,
    // depending on size.X
    //
    // It's too long to explain thoroughly,
    // just open some graphics editor and see for yourself.

    if (size.X % 2 != 0)
    {
      int toAdd = ((size.X + 1) / 2) * _graphicTileSizeScaled;
      headerPosX += toAdd;
      headerPosX -= stringPixelWidth / 2;
    }
    else
    {
      int toAdd = (size.X / 2) * _graphicTileSizeScaled;
      headerPosX += toAdd;
      headerPosX -= stringPixelWidth / 2;
      headerPosX += _graphicTileSizeScaled / 2;
    }

    for (auto& c : lHeader)
    {
      ConvertHtmlToRGB(headerBgColor);
      SDL_SetTextureColorMod(_graphicTileset,
                             _convertedHtml.R,
                             _convertedHtml.G,
                             _convertedHtml.B);

      DrawFromTextTileset(headerPosX, headerPosY, (int)NameCP437::BLOCK);

      ConvertHtmlToRGB(headerFgColor);
      SDL_SetTextureColorMod(_graphicTileset,
                             _convertedHtml.R,
                             _convertedHtml.G,
                             _convertedHtml.B);

      DrawFromTextTileset(headerPosX, headerPosY, (c < 0) ? (c + 256) : c);

      headerPosX += _graphicTileSizeScaled;
    }
  }
}

// =============================================================================

void Printer::DrawRect(int x1, int y1,
                       int x2, int y2,
                       uint32_t color)
{
  if (SDL_GetRenderTarget(Renderer) == nullptr)
  {
    SDL_SetRenderTarget(Renderer, _frameBuffer);
  }

  TileInfo& ti = _textTiles[(int)NameCP437::BLOCK];

  _drawSrc.x = ti.X;
  _drawSrc.y = ti.Y;
  _drawSrc.w = _graphicTileSize;
  _drawSrc.h = _graphicTileSize;

  _drawDst.x = x1;
  _drawDst.y = y1;
  _drawDst.w = std::abs(x2 - x1);
  _drawDst.h = std::abs(y2 - y1);

  ConvertHtmlToRGB(color);
  SDL_SetTextureColorMod(_graphicTileset,
                         _convertedHtml.R,
                         _convertedHtml.G,
                         _convertedHtml.B);

  SDL_RenderCopy(Renderer, _graphicTileset, &_drawSrc, &_drawDst);
}

// =============================================================================

void Printer::DrawFromTextTileset(int x, int y, int tileIndex)
{
  if (tileIndex < 0 || tileIndex >= (int)_textTiles.size())
  {
    ConsoleLog("[ERR] invalid tile index %d", tileIndex);
    return;
  }

  TileInfo& tile = _textTiles[tileIndex];

  _drawSrc.x = tile.X;
  _drawSrc.y = tile.Y;
  _drawSrc.w = _textTileWidth;
  _drawSrc.h = _textTileHeight;

  _drawDst.x = x;
  _drawDst.y = y;
  _drawDst.w = _textTileWidth;
  _drawDst.h = _textTileHeight;

  if (SDL_GetRenderTarget(Renderer) == nullptr)
  {
    SDL_SetRenderTarget(Renderer, _frameBuffer);
  }

  SDL_RenderCopy(Renderer, _textTileset, &_drawSrc, &_drawDst);
}

// =============================================================================

void Printer::DrawFromGraphicsTileset(int x, int y, int tileIndex)
{
  if (tileIndex < 0 || tileIndex >= (int)_graphicTiles.size())
  {
    ConsoleLog("[ERR] invalid tile index %d", tileIndex);
    return;
  }

  TileInfo& ti = _graphicTiles[tileIndex];

  _drawSrc.x = ti.X;
  _drawSrc.y = ti.Y;
  _drawSrc.w = _graphicTileSize;
  _drawSrc.h = _graphicTileSize;

  _drawDst.x = x;
  _drawDst.y = y;
  _drawDst.w = _graphicTileSize;
  _drawDst.h = _graphicTileSize;

  if (SDL_GetRenderTarget(Renderer) == nullptr)
  {
    SDL_SetRenderTarget(Renderer, _frameBuffer);
  }

  SDL_RenderCopy(Renderer, _graphicTileset, &_drawSrc, &_drawDst);
}

// =============================================================================

void Printer::DrawGraphicsTile(int x, int y, GraphicTiles tile, uint32_t color)
{
  ConvertHtmlToRGB(color);
  SDL_SetTextureColorMod(_graphicTileset,
                         _convertedHtml.R,
                         _convertedHtml.G,
                         _convertedHtml.B);

  DrawFromGraphicsTileset(x, y, (int)tile);
}

// =============================================================================

void Printer::ConvertHtmlToRGB(const uint32_t& htmlColor)
{
  if (_validColorsCache.count(htmlColor) == 1)
  {
    _convertedHtml = _validColorsCache[htmlColor];
    return;
  }

  _convertedHtml.R = ((htmlColor & _maskR) >> 16);
  _convertedHtml.G = ((htmlColor & _maskG) >> 8);
  _convertedHtml.B = (htmlColor & _maskB);

  _validColorsCache[htmlColor] = _convertedHtml;
}

// =============================================================================

const std::unordered_map<uint32_t, TileColor>& Printer::GetValidColorsCache()
{
  return _validColorsCache;
}

// =============================================================================

void Printer::SetRenderDst(const SDL_Rect& dst)
{
  _renderDst = dst;
}
#endif

////////////////////////////////////////////////////////////////////////////////

#ifndef USE_SDL
bool Printer::InitForCurses()
{
  int mx = 0;
  int my = 0;

  getmaxyx(stdscr, my, mx);

  TerminalWidth = mx;
  TerminalHeight = my;

  //
  // Enforce colors of standard ncurses colors
  // because some colors aren't actually correspond to their
  // "names", e.g. COLOR_BLACK isn't actually black, but grey,
  // so we redefine it.
  //
  init_color(COLOR_BLACK,   0,    0,    0);
  init_color(COLOR_WHITE,   1000, 1000, 1000);
  init_color(COLOR_RED,     1000, 0,    0);
  init_color(COLOR_GREEN,   0,    1000, 0);
  init_color(COLOR_BLUE,    0,    0,    1000);
  init_color(COLOR_CYAN,    0,    1000, 1000);
  init_color(COLOR_MAGENTA, 1000, 0,    1000);
  init_color(COLOR_YELLOW,  1000, 1000, 0);

  PrepareFrameBuffer();

  return true;
}

// =============================================================================

const std::unordered_map<size_t, ColorPair>& Printer::GetValidColorsCache()
{
  return _colorMap;
}

// =============================================================================

bool Printer::ContainsColorMap(size_t hashToCheck)
{
  return (_colorMap.count(hashToCheck) == 1);
}

// =============================================================================

bool Printer::ColorIndexExists(size_t hashToCheck)
{
  return (_colorIndexMap.count(hashToCheck) == 1);
}

// =============================================================================

NColor Printer::GetNColor(const uint32_t& htmlColor)
{
  NColor ret;

  int valueR = ((htmlColor & _maskR) >> 16);
  int valueG = ((htmlColor & _maskG) >> 8);
  int valueB = (htmlColor  & _maskB);

  //
  // ncurses color component has range from 0 to 1000
  //
  int scaledValueR = (valueR / 255.0) * 1000;
  int scaledValueG = (valueG / 255.0) * 1000;
  int scaledValueB = (valueB / 255.0) * 1000;

  ret.R = scaledValueR;
  ret.G = scaledValueG;
  ret.B = scaledValueB;

  return ret;
}

// =============================================================================

size_t Printer::GetOrSetColor(const uint32_t& htmlColorFg,
                              const uint32_t& htmlColorBg)
{
  std::string fgColorStr = Util::StringFormat("#%06X", htmlColorFg);
  std::string bgColorStr = Util::StringFormat("#%06X", htmlColorBg);

  std::string composition = fgColorStr + bgColorStr;

  std::hash<std::string> hasher;

  size_t hash = hasher(composition);

  if (!ContainsColorMap(hash))
  {
    auto fg = GetNColor(htmlColorFg);
    auto bg = GetNColor(htmlColorBg);

    short hashFg = hasher(fgColorStr);
    short hashBg = hasher(bgColorStr);

    if (!ColorIndexExists(hashFg))
    {
      fg.ColorIndex = _colorGlobalIndex;
      _colorIndexMap[hashFg] = _colorGlobalIndex++;
      init_color(fg.ColorIndex, fg.R, fg.G, fg.B);
    }
    else
    {
      fg.ColorIndex = _colorIndexMap[hashFg];
    }

    if (!ColorIndexExists(hashBg))
    {
      bg.ColorIndex = _colorGlobalIndex;
      _colorIndexMap[hashBg] = _colorGlobalIndex++;
      init_color(bg.ColorIndex, bg.R, bg.G, bg.B);
    }
    else
    {
      bg.ColorIndex = _colorIndexMap[hashBg];
    }

    ColorPair cp = { fg, bg, _colorPairsGlobalIndex++ };
    _colorMap[hash] = cp;

    init_pair(_colorMap[hash].PairIndex,
              _colorIndexMap[hashFg],
              _colorIndexMap[hashBg]);
  }

  return hash;
}

// =============================================================================

std::pair<int, int> Printer::AlignText(int x,
                                       int y,
                                       int align,
                                       const std::string& text)
{
  std::pair<int, int> res;

  int tx = x;
  int ty = y;

  switch (align)
  {
    case kAlignRight:
      //
      // We have to compensate for new position after shift.
      //
      // E.g., print (80, 10, kAlignRight, "Bees")
      // will start from 76 position (tx -= text.length())
      // so it will actually end at 76 (B), 77 (e), 78 (e), 79 (s)
      // This way we either should not subtract 1 from TerminalWidth
      // when printing right aligned text at the end of the screen,
      // or make this hack.
      //
      tx++;

      tx -= text.length();
      break;

    case kAlignCenter:
      tx -= text.length() / 2;
      break;

    // Defaulting to left alignment
    default:
      break;
  }

  res.first = ty;
  res.second = tx;

  return res;
}

// =============================================================================

void Printer::Print(const int& x,
                    const int& y,
                    const std::string& text,
                    int align,
                    const uint32_t& htmlColorFg,
                    const uint32_t& htmlColorBg)
{
  size_t hash = GetOrSetColor(htmlColorFg, htmlColorBg);
  auto textPos = AlignText(x, y, align, text);

  attron(COLOR_PAIR(_colorMap[hash].PairIndex));
  mvprintw(textPos.first, textPos.second, text.data());
  attroff(COLOR_PAIR(_colorMap[hash].PairIndex));
}

// =============================================================================

void Printer::Print(const int& x,
                    const int& y,
                    const int& ch,
                    const uint32_t& htmlColorFg,
                    const uint32_t& htmlColorBg)
{
  size_t hash = GetOrSetColor(htmlColorFg, htmlColorBg);

  attron(COLOR_PAIR(_colorMap[hash].PairIndex));
  mvaddch(y, x, ch);
  attroff(COLOR_PAIR(_colorMap[hash].PairIndex));
}

// =============================================================================

void Printer::PrintFB(const int& x, const int& y,
                      const int& ch,
                      const uint32_t& htmlColorFg,
                      const uint32_t& htmlColorBg)
{
  if (x < 0 || x > (int)TerminalWidth - 1
   || y < 0 || y > (int)TerminalHeight - 1)
  {
    return;
  }

  //
  // Black & White mode for Windows due to PDCurses not handling colors
  // correctly.
  //

  #if !(defined(__unix__) || defined(__linux__))

  uint32_t tmpFg;
  uint32_t tmpBg;

  if (htmlColorFg == Colors::BlackColor
   && htmlColorBg == Colors::BlackColor)
  {
    // For invisible and not discovered tiles

    tmpFg = Colors::BlackColor;
    tmpBg = Colors::BlackColor;
  }
  else if (htmlColorBg != Colors::BlackColor)
  {
    tmpFg = Colors::BlackColor;
    tmpBg = (htmlColorBg == Colors::FogOfWarColor)
            ? Colors::FogOfWarColor
            : Colors::WhiteColor;
  }
  else
  {
    tmpFg = (htmlColorFg == Colors::FogOfWarColor)
            ? Colors::FogOfWarColor
            : Colors::WhiteColor;
    tmpBg = Colors::BlackColor;
  }

  size_t hash = GetOrSetColor(tmpFg, tmpBg);

  #else

  size_t hash = GetOrSetColor(htmlColorFg, htmlColorBg);

  #endif

  _frameBuffer[x][y].Character = ch;
  _frameBuffer[x][y].ColorPairHash = hash;
}

// =============================================================================

void Printer::PrintFB(const int& x, const int& y,
                       const std::string& text,
                       int align,
                       const uint32_t& htmlColorFg,
                       const uint32_t& htmlColorBg)
{
  auto textPos = AlignText(x, y, align, text);

  int xOffset = 0;
  for (auto& c : text)
  {
    // Coordinates are swapped because
    // in framebuffer we don't work in ncurses coordinate system
    PrintFB(textPos.second + xOffset,
            textPos.first,
            c,
            htmlColorFg,
            htmlColorBg);
    xOffset++;
  }
}

// =============================================================================

void Printer::DrawWindow(const Position& leftCorner,
                         const Position& size,
                         const std::string& header,
                         const uint32_t& headerFgColor,
                         const uint32_t& headerBgColor,
                         const uint32_t& borderColor,
                         const uint32_t& borderBgColor,
                         const uint32_t& bgColor)
{
  int x = leftCorner.X;
  int y = leftCorner.Y;

  int ulCorner = ACS_ULCORNER;
  int urCorner = ACS_URCORNER;
  int dlCorner = ACS_LLCORNER;
  int drCorner = ACS_LRCORNER;
  int hBar     = ACS_VLINE;
  int vBar     = ACS_HLINE;

  Position sizeCopy = size;

  if (!header.empty() && ((size_t)size.X > header.length()))
  {
    bool headerDiv = (header.length() % 2 == 0);
    bool sizeDiv   = (size.X % 2 == 0);

    if (sizeDiv && !headerDiv)
    {
      sizeCopy.X--;
    }
    else if (!sizeDiv && headerDiv)
    {
      sizeCopy.X++;
    }
  }

  //
  // Total size must take into account starting point
  //
  // E.g.:
  //
  // (0, 0) - (6, 3)
  //
  // X:
  //
  // 0 1 2 3 4 5
  //                Y:
  // 1 2 3 4 5 6
  // |---------| 1  0
  // |         | 2  1
  // |---------| 3  2
  //
  //
  // But char at point (0;0) counts as 1 unit of length
  // so we must subtract it.
  //
  int xTo = x + sizeCopy.X - 1;
  int yTo = y + sizeCopy.Y - 1;

  // Fill background

  if (bgColor != Colors::None)
  {
    for (int i = x; i <= xTo; i++)
    {
      for (int j = y; j <= yTo; j++)
      {
        PrintFB(i, j, ' ', Colors::WhiteColor, bgColor);
      }
    }
  }

  // Corners

  PrintFB(x,     y, ulCorner, borderColor, borderBgColor);
  PrintFB(xTo,   y, urCorner, borderColor, borderBgColor);
  PrintFB(x,   yTo, dlCorner, borderColor, borderBgColor);
  PrintFB(xTo, yTo, drCorner, borderColor, borderBgColor);

  // Vertical bars

  for (int i = x + 1; i < xTo; i++)
  {
    PrintFB(i, y, vBar, borderColor, borderBgColor);
    PrintFB(i, yTo, vBar, borderColor, borderBgColor);
  }

  // Horizontal bars

  for (int i = y + 1; i < yTo; i++)
  {
    PrintFB(x, i, hBar, borderColor, borderBgColor);
    PrintFB(xTo, i, hBar, borderColor, borderBgColor);
  }

  if (header.length() != 0)
  {
    std::string lHeader = header;
    lHeader.insert(0, " ");
    lHeader.append(" ");

    int offset = (sizeCopy.X - lHeader.length()) / 2;

    PrintFB(x + offset,
            y,
            lHeader,
            Printer::kAlignLeft,
            headerFgColor,
            headerBgColor);
  }
}

// =============================================================================

void Printer::PrepareFrameBuffer()
{
  for (size_t x = 0; x < TerminalWidth; x++)
  {
    std::vector<FBPixel> row;

    for (size_t y = 0; y < TerminalHeight; y++)
    {
      FBPixel s;

      s.ColorPairHash = -1;
      s.Character = ' ';

      row.push_back(s);
    }

    _frameBuffer.push_back(row);
  }
}
#endif

// =============================================================================

void Printer::Clear()
{
#ifndef USE_SDL
  for (size_t x = 0; x < TerminalWidth; x++)
  {
    for (size_t y = 0; y < TerminalHeight; y++)
    {
      PrintFB(x, y, ' ', Colors::BlackColor, Colors::BlackColor);
    }
  }
#else
  SDL_SetRenderTarget(Renderer, _frameBuffer);
  SDL_RenderClear(Renderer);
#endif
}

// =============================================================================

void Printer::PrintChar(const int x,
                        const int y,
                        int charIndex,
                        const uint32_t& htmlColorFg,
                        const uint32_t& htmlColorBg)
{
#ifdef USE_SDL
  int px = x * _textTileWidth;
  int py = y * _textTileHeight;

  if (htmlColorBg != Colors::None)
  {
    ConvertHtmlToRGB(htmlColorBg);
    SDL_SetTextureColorMod(_textTileset,
                           _convertedHtml.R,
                           _convertedHtml.G,
                           _convertedHtml.B);

    DrawFromTextTileset(px, py, (int)NameCP437::BLOCK);
  }

  ConvertHtmlToRGB(htmlColorFg);
  SDL_SetTextureColorMod(_textTileset,
                         _convertedHtml.R,
                         _convertedHtml.G,
                         _convertedHtml.B);

  DrawFromTextTileset(px, py, charIndex);
#else
  PrintFB(x, y, charIndex, htmlColorFg, htmlColorBg);
#endif
}

// =============================================================================

void Printer::PrintText(const int x,
                        const int y,
                        const std::string& text,
                        int align,
                        const uint32_t& htmlColorFg,
                        const uint32_t& htmlColorBg)
{
#ifdef USE_SDL
  int px = x * _textTileWidth;
  int py = y * _textTileHeight;

  switch (align)
  {
    case kAlignLeft:
      break;

    case kAlignCenter:
    {
      int pixelWidth = text.length() * _textTileWidth;
      px -= pixelWidth / 2;
    }
    break;

    case kAlignRight:
    {
      int pixelWidth = text.length() * _textTileWidth;
      px -= pixelWidth;
    }
    break;

    default:
      ConsoleLog("[WAR] unexpected alignment value %d", align);
      break;
  }

  for (auto& c : text)
  {
    if (htmlColorBg != Colors::None)
    {
      ConvertHtmlToRGB(htmlColorBg);
      SDL_SetTextureColorMod(_textTileset,
                             _convertedHtml.R,
                             _convertedHtml.G,
                             _convertedHtml.B);

      DrawFromTextTileset(px, py, (int)NameCP437::BLOCK);
    }

    ConvertHtmlToRGB(htmlColorFg);
    SDL_SetTextureColorMod(_textTileset,
                           _convertedHtml.R,
                           _convertedHtml.G,
                           _convertedHtml.B);

    DrawFromTextTileset(px, py, (c < 0) ? (c + 256) : c);

    px += _textTileWidth;
  }
#else
  PrintFB(x, y, text, align, htmlColorFg, htmlColorBg);
#endif
}

// =============================================================================

void Printer::Render()
{
#ifndef USE_SDL
  for (size_t x = 0; x < TerminalWidth; x++)
  {
    for (size_t y = 0; y < TerminalHeight; y++)
    {
      attron(COLOR_PAIR(_colorMap[_frameBuffer[x][y].ColorPairHash].PairIndex));
      mvaddch(y, x, _frameBuffer[x][y].Character);
      attroff(COLOR_PAIR(_colorMap[_frameBuffer[x][y].ColorPairHash].PairIndex));
    }
  }

  refresh();
#else
  SDL_SetRenderTarget(Renderer, nullptr);
  SDL_RenderClear(Renderer);
  SDL_RenderCopy(Renderer, _frameBuffer, nullptr, &_renderDst);
  SDL_RenderPresent(Renderer);
#endif
}

// =============================================================================

std::vector<Position> Printer::DrawExplosion(const Position& pos, int aRange)
{
  std::vector<Position> cellsAffected =
      Util::GetAreaDamagePointsFrom(pos, aRange);

  for (int range = 1; range <= aRange; range++)
  {
    auto res = Util::GetAreaDamagePointsFrom(pos, range);
    for (auto& p : res)
    {
      int drawX = p.X + Game::gMap.CurrentLevel->MapOffsetX;
      int drawY = p.Y + Game::gMap.CurrentLevel->MapOffsetY;

      if (Game::gMap.CurrentLevel->MapArray[p.X][p.Y]->Visible)
      {
        Game::gPrnt.PrintChar(
          drawX,
          drawY,
          'x',
          Colors::RedColor,
          Colors::BlackColor
        );
      }
    }

    Game::gPrnt.Render();

    Util::Sleep(20);

    Game::gApp.ForceDrawMainState();
  }

  return cellsAffected;
}

// =============================================================================

void Printer::AddMessage(const std::string& message)
{
  AddMessage(GameLogMessageData
             {
               message,
               Colors::WhiteColor,
               Colors::BlackColor
             });
}

// =============================================================================

void Printer::AddMessage(const std::string& message,
                         const uint32_t& fgColor)
{
  AddMessage(GameLogMessageData{ message, fgColor, Colors::BlackColor });
}

// =============================================================================

void Printer::AddMessage(const std::string& message,
                         const uint32_t& fgColor,
                         const uint32_t& bgColor)
{
  AddMessage(GameLogMessageData{ message, fgColor, bgColor });
}

// =============================================================================

void Printer::AddMessage(const GameLogMessageData& data)
{
  //
  // There are messages in log and current message is the same as the last one.
  //
  if (!_inGameMessages->IsEmpty() && (_repeatingMessage == data.Message))
  {
    _messageRepeatCounter++;
    GameLogMessageData* lastMsg = _inGameMessages->LastMessage();
    if (lastMsg != nullptr)
    {
      *lastMsg =
      {
        Util::StringFormat("(x%i) %s",
                           _messageRepeatCounter,
                           _repeatingMessage.data()),
        data.FgColor,
        data.BgColor
      };
    }
  }
  //
  // Otherwise log is empty or it's a different message.
  //
  else
  {
    _messageRepeatCounter = 1;
    _inGameMessages->AddMessage(
    {
      data.Message,
      data.FgColor,
      data.BgColor
    });
    _lastMessagesToDisplay++;
  }

  _repeatingMessage = data.Message;

  _lastMessagesToDisplay = Util::Clamp(_lastMessagesToDisplay,
                                       0,
                                       kShortLogMaxMessages);

  ShowLastMessage = true;
}

// =============================================================================

void Printer::InitMsgBufferObj()
{
  if (TerminalHeight == 0)
  {
    ConsoleLog("[WAR] Printer::InitMsgBufferObj() - TerminalHeight is 0!");
  }

  _inGameMessages = std::make_unique<MsgBuffer>(TerminalHeight - 1, 5);
}

// =============================================================================

const std::vector<GameLogMessageData*>& Printer::GetLastMessages()
{
  return _inGameMessages->GetLastMessages(_lastMessagesToDisplay);
}

// =============================================================================

GameLogMessageData* Printer::GetLastMessage()
{
  return _inGameMessages->GetLastMessage();
}

// =============================================================================

void Printer::ResetMessagesToDisplay()
{
  _lastMessagesToDisplay = 0;
  _messageRepeatCounter = 1;
  _repeatingMessage.clear();
}

// =============================================================================

const std::vector<GameLogMessageData*>& Printer::Messages()
{
  return _inGameMessages->GetMessages();
}

// =============================================================================

MsgScrollBuffer<GameLogMessageData>& Printer::GetMsgBufferObj()
{
  return *_inGameMessages.get();
}

// =============================================================================

int Printer::ColorsUsed()
{
  #ifndef USE_SDL
  return _colorMap.size();
  #else
  return _validColorsCache.size();
  #endif
}

// =============================================================================

bool Printer::IsReady()
{
  return _ok;
}

// =============================================================================

const int& Printer::GetLastMessagesCount()
{
  return _lastMessagesToDisplay;
}
