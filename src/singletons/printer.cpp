#include "printer.h"

#include "application.h"
#include "map.h"
#include "util.h"
#include "base64-strings.h"

#ifdef DEBUG_BUILD
#include "logger.h"
#endif

size_t Printer::TerminalWidth = 0;
size_t Printer::TerminalHeight = 0;

void Printer::InitSpecific()
{
  _inGameMessages.reserve(kMaxGameLogMessages * 2);
  _lastMessages.reserve(10);

#ifdef USE_SDL
  _ok = InitForSDL();
#else
  _ok = InitForCurses();
#endif
}

// =============================================================================

#ifdef USE_SDL
bool Printer::InitForSDL()
{
  auto& gameConfig = Application::Instance().GameConfig;

  std::string tilesetFile = gameConfig.TilesetFilename;

  _tileWidth = 0;
  _tileHeight = 0;

  SDL_Surface* surf = SDL_LoadBMP(tilesetFile.data());
  if (surf)
  {
    SDL_SetColorKey(surf, SDL_TRUE, SDL_MapRGB(surf->format, 0xFF, 0, 0xFF));

    _tileset = SDL_CreateTextureFromSurface(Application::Instance().Renderer, surf);
    if (_tileset == nullptr)
    {
      ConsoleLog("SDL_CreateTextureFromSurface() fail: %s\n", SDL_GetError());
      return false;
    }

    SDL_FreeSurface(surf);

    _tileWidth  = gameConfig.TileWidth;
    _tileHeight = gameConfig.TileHeight;
  }
  else
  {
    auto str = Util::StringFormat("***** Could not load tileset: %s! *****\nFalling back to embedded.\n", SDL_GetError());
    ConsoleLog("%s\n", str.data());
    LogPrint(str, true);

    _tileWidth = 8;
    _tileHeight = 16;

    SDL_Rect rect = Application::Instance().GetWindowSize(_tileWidth, _tileHeight);

    SDL_SetWindowPosition(Application::Instance().Window, rect.x, rect.y);
    SDL_SetWindowSize(Application::Instance().Window, rect.w, rect.h);

    auto res = Util::Base64_Decode(Base64Strings::Tileset8x16Base64);
    auto bytes = Util::ConvertStringToBytes(res);
    SDL_RWops* data = SDL_RWFromMem(bytes.data(), bytes.size());
    surf = SDL_LoadBMP_RW(data, 1);
    if (!surf)
    {
      auto str = Util::StringFormat("***** Could not load from memory: %s *****\n", SDL_GetError());
      ConsoleLog("%s\n", str.data());
      LogPrint(str, true);
      return false;
    }

    SDL_SetColorKey(surf, SDL_TRUE, SDL_MapRGB(surf->format, 0xFF, 0, 0xFF));
    _tileset = SDL_CreateTextureFromSurface(Application::Instance().Renderer, surf);
    if (_tileset == nullptr)
    {
      ConsoleLog("SDL_CreateTextureFromSurface() fail: %s\n", SDL_GetError());
      return false;
    }

    SDL_FreeSurface(surf);
  }

  _tileWidthScaled = _tileWidth * gameConfig.ScaleFactor;
  _tileHeightScaled = _tileHeight * gameConfig.ScaleFactor;

  _tileAspectRatio = (double)_tileWidth / (double)_tileHeight;

  _tileWH       = { _tileWidth,       _tileHeight       };
  _tileWHScaled = { _tileWidthScaled, _tileHeightScaled };

  int w = 0, h = 0;
  SDL_QueryTexture(_tileset, nullptr, nullptr, &w, &h);

  _tilesetWidth = w;
  _tilesetHeight = h;

  _frameBuffer = SDL_CreateTexture(Application::Instance().Renderer,
                                   SDL_PIXELFORMAT_RGBA32,
                                   SDL_TEXTUREACCESS_TARGET,
                                   gameConfig.WindowWidth,
                                   gameConfig.WindowHeight);

  if (_frameBuffer == nullptr)
  {
    ConsoleLog("SDL_CreateTexture() fail: %s\n", SDL_GetError());
    return false;
  }

  char asciiIndex = 0;
  int tileIndex = 0;
  for (int y = 0; y < h; y += _tileHeight)
  {
    for (int x = 0; x < w; x += _tileWidth)
    {
      TileInfo ti;
      ti.X = x;
      ti.Y = y;
      _tiles.push_back(ti);

      _tileIndexByChar[asciiIndex] = tileIndex;

      asciiIndex++;
      tileIndex++;
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
        PrintFB(i, j, ' ', Colors::WhiteColor, bgColor);
      }
    }
  }

  // Corners

  PrintFB(x,          y,          ulCorner, borderColor, borderBgColor);
  PrintFB(x + size.X, y,          urCorner, borderColor, borderBgColor);
  PrintFB(x,          y + size.Y, dlCorner, borderColor, borderBgColor);
  PrintFB(x + size.X, y + size.Y, drCorner, borderColor, borderBgColor);

  // Horizontal bars

  for (int i = x + 1; i < x + size.X; i++)
  {
    PrintFB(i, y,          hBarU, borderColor, borderBgColor);
    PrintFB(i, y + size.Y, hBarD, borderColor, borderBgColor);
  }

  // Vertical bars

  for (int i = y + 1; i < y + size.Y; i++)
  {
    PrintFB(x,          i, vBarL, borderColor, borderBgColor);
    PrintFB(x + size.X, i, vBarR, borderColor, borderBgColor);
  }

  if (header.length() != 0)
  {
    std::string lHeader = header;
    lHeader.insert(0, " ");
    lHeader.append(" ");

    int stringPixelWidth = (lHeader.length() * _tileWidthScaled);
    int headerPosX = x * _tileWidthScaled;
    int headerPosY = y * _tileHeightScaled;

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
      int toAdd = ((size.X + 1) / 2) * _tileWidthScaled;
      headerPosX += toAdd;
      headerPosX -= stringPixelWidth / 2;
    }
    else
    {
      int toAdd = (size.X / 2) * _tileWidthScaled;
      headerPosX += toAdd;
      headerPosX -= stringPixelWidth / 2;
      headerPosX += _tileWidthScaled / 2;
    }

    for (auto& c : lHeader)
    {
      ConvertHtmlToRGB(headerBgColor);
      SDL_SetTextureColorMod(_tileset,
                             _convertedHtml.R,
                             _convertedHtml.G,
                             _convertedHtml.B);
      DrawTile(headerPosX, headerPosY, 219);

      ConvertHtmlToRGB(headerFgColor);
      SDL_SetTextureColorMod(_tileset,
                             _convertedHtml.R,
                             _convertedHtml.G,
                             _convertedHtml.B);
      DrawTile(headerPosX, headerPosY, c);

      headerPosX += _tileWidthScaled;
    }
  }
}

// =============================================================================

void Printer::DrawRect(int x1, int y1,
                       int x2, int y2,
                       uint32_t color)
{
  if (SDL_GetRenderTarget(Application::Instance().Renderer) == nullptr)
  {
    SDL_SetRenderTarget(Application::Instance().Renderer, _frameBuffer);
  }

  TileInfo& ti = _tiles[219];

  _drawSrc.x = ti.X;
  _drawSrc.y = ti.Y;
  _drawSrc.w = _tileWidth;
  _drawSrc.h = _tileHeight;

  _drawDst.x = x1;
  _drawDst.y = y1;
  _drawDst.w = std::abs(x2 - x1);
  _drawDst.h = std::abs(y2 - y1);

  ConvertHtmlToRGB(color);
  SDL_SetTextureColorMod(_tileset,
                         _convertedHtml.R,
                         _convertedHtml.G,
                         _convertedHtml.B);

  SDL_RenderCopy(Application::Instance().Renderer, _tileset, &_drawSrc, &_drawDst);
}

// =============================================================================

void Printer::DrawTile(int x, int y, int tileIndex)
{
  TileInfo& tile = _tiles[tileIndex];

  _drawSrc.x = tile.X;
  _drawSrc.y = tile.Y;
  _drawSrc.w = _tileWidth;
  _drawSrc.h = _tileHeight;

  _drawDst.x = x;
  _drawDst.y = y;
  _drawDst.w = _tileWidthScaled;
  _drawDst.h = _tileHeightScaled;

  if (SDL_GetRenderTarget(Application::Instance().Renderer) == nullptr)
  {
    SDL_SetRenderTarget(Application::Instance().Renderer, _frameBuffer);
  }

  SDL_RenderCopy(Application::Instance().Renderer, _tileset, &_drawSrc, &_drawDst);
}

// =============================================================================

void Printer::DrawTile(int x, int y, int tileIndex, size_t scale)
{
  size_t tileScaleW = (scale <= 1) ? _tileWidthScaled : _tileWidthScaled * ((scale - 1) * 3);
  size_t tileScaleH = (scale <= 1) ? _tileHeightScaled : _tileHeightScaled * ((scale - 1) * 3);

  size_t offsetX = (scale <= 1) ? 0 : tileScaleW / 3;
  size_t offsetY = (scale <= 1) ? 0 : tileScaleH / 3;

  TileInfo& tile = _tiles[tileIndex];

  _drawSrc.x = tile.X;
  _drawSrc.y = tile.Y;
  _drawSrc.w = _tileWidth;
  _drawSrc.h = _tileHeight;

  _drawDst.x = x - offsetX;
  _drawDst.y = y - offsetY;
  _drawDst.w = tileScaleW;
  _drawDst.h = tileScaleH;

  if (SDL_GetRenderTarget(Application::Instance().Renderer) == nullptr)
  {
    SDL_SetRenderTarget(Application::Instance().Renderer, _frameBuffer);
  }

  SDL_RenderCopy(Application::Instance().Renderer, _tileset, &_drawSrc, &_drawDst);
}

// =============================================================================

void Printer::PrintFB(const int& x, const int& y,
                      int image,
                      const uint32_t& htmlColorFg,
                      const uint32_t& htmlColorBg)
{
  int posX = x * _tileWidthScaled;
  int posY = y * _tileHeightScaled;

  if (htmlColorBg != Colors::None)
  {
    ConvertHtmlToRGB(htmlColorBg);
    SDL_SetTextureColorMod(_tileset,
                           _convertedHtml.R,
                           _convertedHtml.G,
                           _convertedHtml.B);
    DrawTile(posX, posY, 219);
  }

  ConvertHtmlToRGB(htmlColorFg);
  SDL_SetTextureColorMod(_tileset,
                         _convertedHtml.R,
                         _convertedHtml.G,
                         _convertedHtml.B);
  DrawTile(posX, posY, image);
}

// =============================================================================

void Printer::PrintFB(const int& x, const int& y,
                      const std::string& text,
                      int align,
                      const uint32_t& htmlColorFg,
                      const uint32_t& htmlColorBg)
{
  int px = x * _tileWidthScaled;
  int py = y * _tileHeightScaled;

  switch (align)
  {
    case kAlignCenter:
    {
      int pixelWidth = text.length() * _tileWidthScaled;
      px -= pixelWidth / 2;
    }
    break;

    case kAlignRight:
    {
      int pixelWidth = text.length() * _tileWidthScaled;
      px -= pixelWidth;
    }
    break;
  }

  for (auto& c : text)
  {
    if (htmlColorBg != Colors::None)
    {
      ConvertHtmlToRGB(htmlColorBg);
      SDL_SetTextureColorMod(_tileset,
                             _convertedHtml.R,
                             _convertedHtml.G,
                             _convertedHtml.B);
      DrawTile(px, py, 219);
    }

    ConvertHtmlToRGB(htmlColorFg);
    SDL_SetTextureColorMod(_tileset,
                           _convertedHtml.R,
                           _convertedHtml.G,
                           _convertedHtml.B);
    DrawTile(px, py, c);

    px += _tileWidthScaled;
  }
}

// =============================================================================

void Printer::PrintFB(const int& x,
                      const int& y,
                      const std::string& text,
                      size_t scale,
                      int align,
                      const uint32_t& htmlColorFg,
                      const uint32_t& htmlColorBg)
{
  size_t tileScaleW = (scale <= 1)
                      ? _tileWidthScaled
                      : _tileWidthScaled * ((scale - 1) * 3);
  //size_t tileScaleH = (scale <= 1) ? _tileHeightScaled : _tileHeightScaled * ((scale - 1) * 3);

  int px = x * _tileWidthScaled;
  int py = y * _tileHeightScaled;

  switch (align)
  {
    case kAlignCenter:
    {
      int pixelWidth = text.length() * tileScaleW;
      px -= pixelWidth / 2;
    }
    break;

    case kAlignRight:
    {
      int pixelWidth = text.length() * tileScaleW;
      px -= pixelWidth;
    }
    break;
  }

  for (auto& c : text)
  {
    if (htmlColorBg != Colors::None)
    {
      ConvertHtmlToRGB(htmlColorBg);
      SDL_SetTextureColorMod(_tileset,
                             _convertedHtml.R,
                             _convertedHtml.G,
                             _convertedHtml.B);
      DrawTile(px, py, 219, scale);
    }

    ConvertHtmlToRGB(htmlColorFg);
    SDL_SetTextureColorMod(_tileset,
                           _convertedHtml.R,
                           _convertedHtml.G,
                           _convertedHtml.B);
    DrawTile(px, py, c, scale);

    px += tileScaleW;
  }
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

// =============================================================================

double Printer::GetTileAspectRatio()
{
  return _tileAspectRatio;
}

// =============================================================================

const std::pair<int, int>& Printer::GetTileWH()
{
  return _tileWH;
}

// =============================================================================

const std::pair<int, int>& Printer::GetTileWHScaled()
{
  return _tileWHScaled;
}

#endif

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

    ColorPair cp = { _colorPairsGlobalIndex++, fg, bg };
    _colorMap[hash] = cp;

    init_pair(_colorMap[hash].PairIndex, _colorIndexMap[hashFg], _colorIndexMap[hashBg]);
  }

  return hash;
}

// =============================================================================

std::pair<int, int> Printer::AlignText(int x, int y, int align, const std::string& text)
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

  // Black & White mode for Windows due to PDCurses not handling colors correctly

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
    PrintFB(textPos.second + xOffset, textPos.first, c, htmlColorFg, htmlColorBg);
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

void Printer::PrintFB(const int& x, const int& y,
                      const std::string& text,
                      size_t scale,
                      int align,
                      const uint32_t& htmlColorFg,
                      const uint32_t& htmlColorBg)
{
  //
  // No scaling in ncurses :-(
  //
  PrintFB(x, y, text, align, htmlColorFg, htmlColorBg);
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
  SDL_SetRenderTarget(Application::Instance().Renderer, _frameBuffer);
  SDL_RenderClear(Application::Instance().Renderer);
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
  SDL_SetRenderTarget(Application::Instance().Renderer, nullptr);
  SDL_RenderClear(Application::Instance().Renderer);
  SDL_RenderCopy(Application::Instance().Renderer, _frameBuffer, nullptr, &_renderDst);
  SDL_RenderPresent(Application::Instance().Renderer);
#endif
}

// =============================================================================

std::vector<Position> Printer::DrawExplosion(const Position& pos, int aRange)
{
  std::vector<Position> cellsAffected = Util::GetAreaDamagePointsFrom(pos, aRange);

  for (int range = 1; range <= aRange; range++)
  {
    auto res = Util::GetAreaDamagePointsFrom(pos, range);
    for (auto& p : res)
    {
      int drawX = p.X + Map::Instance().CurrentLevel->MapOffsetX;
      int drawY = p.Y + Map::Instance().CurrentLevel->MapOffsetY;

      if (Map::Instance().CurrentLevel->MapArray[p.X][p.Y]->Visible)
      {
        Printer::Instance().PrintFB(drawX, drawY, 'x', Colors::RedColor);
      }
    }

    Printer::Instance().Render();

    Util::Sleep(20);

    Application::Instance().ForceDrawMainState();
  }

  return cellsAffected;
}

// =============================================================================

void Printer::AddMessage(const std::string& message)
{
  AddMessage(GameLogMessageData{ message, Colors::WhiteColor, Colors::BlackColor });
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
  if (!_inGameMessages.empty() && (_repeatingMessage == data.Message))
  {
    _messageRepeatCounter++;
    auto newStr = Util::StringFormat("(x%i) %s", _messageRepeatCounter, _repeatingMessage.data());
    _inGameMessages.front() = { newStr, data.FgColor, data.BgColor };
  }
  else
  {
    _messageRepeatCounter = 1;
    _inGameMessages.insert(_inGameMessages.begin(), { data.Message, data.FgColor, data.BgColor });
    _lastMessagesToDisplay++;
  }

  _repeatingMessage = data.Message;

  if (_inGameMessages.size() > kMaxGameLogMessages)
  {
    _inGameMessages.pop_back();
  }

  _lastMessagesToDisplay = Util::Clamp(_lastMessagesToDisplay, 0, 5);

  ShowLastMessage = true;
}

// =============================================================================

std::vector<GameLogMessageData> Printer::GetLastMessages()
{
  _lastMessages.clear();

  int count = 0;
  for (auto& m : _inGameMessages)
  {
    _lastMessages.push_back(m);

    count++;

    if (count >= _lastMessagesToDisplay)
    {
      break;
    }
  }

  return _lastMessages;
}

// =============================================================================

GameLogMessageData Printer::GetLastMessage()
{
  return (_inGameMessages.size() > 0)
      ? _inGameMessages.front()
      : GameLogMessageData();
}

// =============================================================================

void Printer::ResetMessagesToDisplay()
{
  _lastMessages.clear();
  _lastMessagesToDisplay = 0;
  _messageRepeatCounter = 1;
  _repeatingMessage.clear();
}

// =============================================================================

std::vector<GameLogMessageData>& Printer::Messages()
{
  return _inGameMessages;
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
