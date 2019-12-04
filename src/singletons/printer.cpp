#include "printer.h"

#include "application.h"
#include "map.h"
#include "util.h"

void Printer::Init()
{
  if (_initialized)
  {
    return;
  }

#ifdef USE_SDL
  InitForSDL();
#else
  InitForCurses();
#endif

  _initialized = true;
}

#ifdef USE_SDL
void Printer::InitForSDL()
{
  std::string tilesetFile = Application::Instance().TilesetFilename;

  _tileWidth = 0;
  _tileHeight = 0;

  SDL_Surface* surf = IMG_Load(tilesetFile.data());
  if (surf)
  {
    _tileset = SDL_CreateTextureFromSurface(Application::Instance().Renderer, surf);
    SDL_FreeSurface(surf);

    _tileWidth = Application::Instance().TileWidth;
    _tileHeight = Application::Instance().TileHeight;
  }
  else
  {
    auto str = Util::StringFormat("***** Could not load tileset: %s! *****\nFalling back to embedded.\n", SDL_GetError());
    printf("%s\n", str.data());
    Logger::Instance().Print(str, true);

    _tileWidth = 8;
    _tileHeight = 16;

    auto res = Util::Base64_Decode(GlobalConstants::Tileset8x16Base64);
    auto bytes = Util::ConvertStringToBytes(res);
    SDL_RWops* data = SDL_RWFromMem(bytes.data(), bytes.size());
    surf = IMG_Load_RW(data, 1);
    if (!surf)
    {
      auto str = Util::StringFormat("***** Could not load from memory: %s *****\n", IMG_GetError());
      printf("%s\n", str.data());
      Logger::Instance().Print(str, true);
    }

    _tileset = SDL_CreateTextureFromSurface(Application::Instance().Renderer, surf);
    SDL_FreeSurface(surf);
  }

  _tileWidthScaled = _tileWidth * Application::Instance().ScaleFactor;
  _tileHeightScaled = _tileHeight * Application::Instance().ScaleFactor;

  int w = 0, h = 0;
  SDL_QueryTexture(_tileset, nullptr, nullptr, &w, &h);

  _tilesetWidth = w;
  _tilesetHeight = h;

  _frameBuffer = SDL_CreateTexture(Application::Instance().Renderer,
                                   SDL_PIXELFORMAT_RGBA32,
                                   SDL_TEXTUREACCESS_TARGET,
                                   Application::Instance().WindowWidth,
                                   Application::Instance().WindowHeight);

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
}

void Printer::DrawWindow(const Position& leftCorner,
                         const Position& size,
                         const std::string& header,
                         const std::string& headerFgColor,
                         const std::string& headerBgColor,
                         const std::string& borderColor,
                         const std::string& borderBgColor,
                         const std::string& bgColor)
{
  auto res = Util::GetPerimeter(leftCorner.X, leftCorner.Y,
                                size.X, size.Y, true);

  int x = leftCorner.X;
  int y = leftCorner.Y;

  int ulCorner = GlobalConstants::CP437IndexByType[NameCP437::ULCORNER_2];
  int urCorner = GlobalConstants::CP437IndexByType[NameCP437::URCORNER_2];
  int dlCorner = GlobalConstants::CP437IndexByType[NameCP437::DLCORNER_2];
  int drCorner = GlobalConstants::CP437IndexByType[NameCP437::DRCORNER_2];
  int hBarU = GlobalConstants::CP437IndexByType[NameCP437::HBAR_2];
  int hBarD = GlobalConstants::CP437IndexByType[NameCP437::HBAR_2];
  int vBarL = GlobalConstants::CP437IndexByType[NameCP437::VBAR_2];
  int vBarR = GlobalConstants::CP437IndexByType[NameCP437::VBAR_2];

  /*
  int ulCorner = GlobalConstants::CP437IndexByType[NameCP437::ULCORNER_3];
  int urCorner = GlobalConstants::CP437IndexByType[NameCP437::URCORNER_3];
  int dlCorner = GlobalConstants::CP437IndexByType[NameCP437::DLCORNER_3];
  int drCorner = GlobalConstants::CP437IndexByType[NameCP437::DRCORNER_3];
  int hBarU = GlobalConstants::CP437IndexByType[NameCP437::HBAR_3U];
  int hBarD = GlobalConstants::CP437IndexByType[NameCP437::HBAR_3D];
  int vBarL = GlobalConstants::CP437IndexByType[NameCP437::VBAR_3L];
  int vBarR = GlobalConstants::CP437IndexByType[NameCP437::VBAR_3R];
  */

  // Fill background

  for (int i = x + 1; i < x + size.X; i++)
  {
    for (int j = y + 1; j < y + size.Y; j++)
    {
      PrintFB(i, j, ' ', "#000000", bgColor);
    }
  }

  // Corners

  PrintFB(x, y, ulCorner, borderColor, borderBgColor);
  PrintFB(x + size.X, y, urCorner, borderColor, borderBgColor);
  PrintFB(x, y + size.Y, dlCorner, borderColor, borderBgColor);
  PrintFB(x + size.X, y + size.Y, drCorner, borderColor, borderBgColor);

  // Horizontal bars

  for (int i = x + 1; i < x + size.X; i++)
  {
    PrintFB(i, y, hBarU, borderColor, borderBgColor);
    PrintFB(i, y + size.Y, hBarD, borderColor, borderBgColor);
  }

  // Vertical bars

  for (int i = y + 1; i < y + size.Y; i++)
  {
    PrintFB(x, i, vBarL, borderColor, borderBgColor);
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
      TileColor tc = ConvertHtmlToRGB(headerBgColor);
      SDL_SetTextureColorMod(_tileset, tc.R, tc.G, tc.B);
      DrawTile(headerPosX, headerPosY, 219);

      tc = ConvertHtmlToRGB(headerFgColor);
      SDL_SetTextureColorMod(_tileset, tc.R, tc.G, tc.B);
      DrawTile(headerPosX, headerPosY, c);

      headerPosX += _tileWidthScaled;
    }
  }
}

void Printer::DrawImage(const int& x, const int& y, SDL_Texture* tex)
{
  int tw, th;
  SDL_QueryTexture(tex, nullptr, nullptr, &tw, &th);

  SDL_Rect src;
  src.x = 0;
  src.y = 0;
  src.w = tw;
  src.h = th;

  SDL_Rect dst;
  dst.x = x;
  dst.y = y;
  dst.w = tw;
  dst.h = th;

  if (SDL_GetRenderTarget(Application::Instance().Renderer) == nullptr)
  {
    SDL_SetRenderTarget(Application::Instance().Renderer, _frameBuffer);
  }

  SDL_RenderCopy(Application::Instance().Renderer, tex, &src, &dst);
}

void Printer::DrawTile(int x, int y, int tileIndex)
{
  TileInfo& tile = _tiles[tileIndex];

  SDL_Rect src;
  src.x = tile.X;
  src.y = tile.Y;
  src.w = _tileWidth;
  src.h = _tileHeight;

  SDL_Rect dst;
  dst.x = x;
  dst.y = y;
  dst.w = _tileWidthScaled;
  dst.h = _tileHeightScaled;

  if (SDL_GetRenderTarget(Application::Instance().Renderer) == nullptr)
  {
    SDL_SetRenderTarget(Application::Instance().Renderer, _frameBuffer);
  }

  SDL_RenderCopy(Application::Instance().Renderer, _tileset, &src, &dst);
}

void Printer::PrintFB(const int& x, const int& y,
                      int image,
                      const std::string& htmlColorFg,
                      const std::string& htmlColorBg)
{
  TileColor tc;

  int tileIndex = image;

  int posX = x * _tileWidthScaled;
  int posY = y * _tileHeightScaled;

  if (htmlColorBg.length() != 0)
  {
    tc = ConvertHtmlToRGB(htmlColorBg);
    SDL_SetTextureColorMod(_tileset, tc.R, tc.G, tc.B);
    DrawTile(posX, posY, 219);
  }

  tc = ConvertHtmlToRGB(htmlColorFg);
  SDL_SetTextureColorMod(_tileset, tc.R, tc.G, tc.B);
  DrawTile(posX, posY, image);
}

void Printer::PrintFB(const int& x, const int& y,
                      const std::string& text,
                      int align,
                      const std::string& htmlColorFg,
                      const std::string& htmlColorBg)
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
    TileColor tc;

    if (htmlColorBg.length() != 0)
    {
      tc = ConvertHtmlToRGB(htmlColorBg);
      SDL_SetTextureColorMod(_tileset, tc.R, tc.G, tc.B);
      DrawTile(px, py, 219);
    }

    tc = ConvertHtmlToRGB(htmlColorFg);
    SDL_SetTextureColorMod(_tileset, tc.R, tc.G, tc.B);
    DrawTile(px, py, c);

    px += _tileWidthScaled;
  }
}

TileColor Printer::ConvertHtmlToRGB(const std::string& htmlColor)
{
  TileColor res;

  std::string hexR = { htmlColor[1], htmlColor[2] };
  std::string hexG = { htmlColor[3], htmlColor[4] };
  std::string hexB = { htmlColor[5], htmlColor[6] };

  int valueR = strtol(hexR.data(), nullptr, 16);
  int valueG = strtol(hexG.data(), nullptr, 16);
  int valueB = strtol(hexB.data(), nullptr, 16);

  res.R = valueR;
  res.G = valueG;
  res.B = valueB;

  return res;
}

#endif

#ifndef USE_SDL
void Printer::InitForCurses()
{
  int mx = 0;
  int my = 0;

  getmaxyx(stdscr, my, mx);

  TerminalWidth = mx;
  TerminalHeight = my;

  // Enforce colors of standard ncurses colors
  // because some colors aren't actually correspond to their
  // "names", e.g. COLOR_BLACK isn't actually black, but grey,
  // so we redefine it.

  init_color(COLOR_BLACK, 0, 0, 0);
  init_color(COLOR_WHITE, 1000, 1000, 1000);
  init_color(COLOR_RED, 1000, 0, 0);
  init_color(COLOR_GREEN, 0, 1000, 0);
  init_color(COLOR_BLUE, 0, 0, 1000);
  init_color(COLOR_CYAN, 0, 1000, 1000);
  init_color(COLOR_MAGENTA, 1000, 0, 1000);
  init_color(COLOR_YELLOW, 1000, 1000, 0);

  PrepareFrameBuffer();
}

bool Printer::ContainsColorMap(size_t hashToCheck)
{
  for (auto& h : _colorMap)
  {
    if (h.first == hashToCheck)
    {
      return true;
    }
  }

  return false;
}

bool Printer::ColorIndexExists(size_t hashToCheck)
{
  for (auto& h : _colorIndexMap)
  {
    if (h.first == hashToCheck)
    {
      return true;
    }
  }

  return false;
}

NColor Printer::GetNColor(const std::string& htmlColor)
{
  NColor ret;

  std::string hexR = { htmlColor[1], htmlColor[2] };
  std::string hexG = { htmlColor[3], htmlColor[4] };
  std::string hexB = { htmlColor[5], htmlColor[6] };

  int valueR = strtol(hexR.data(), nullptr, 16);    
  int valueG = strtol(hexG.data(), nullptr, 16);    
  int valueB = strtol(hexB.data(), nullptr, 16);    

  // ncurses color component has range from 0 to 1000

  int scaledValueR = (valueR / 255.0f) * 1000;
  int scaledValueG = (valueG / 255.0f) * 1000;
  int scaledValueB = (valueB / 255.0f) * 1000;
  
  //printf("%s %s %s => %i %i %i\n", hexR.data(), hexG.data(), hexB.data(), c.R, c.G, c.B);

  ret.R = scaledValueR;
  ret.G = scaledValueG;
  ret.B = scaledValueB;

  return ret;
}

size_t Printer::GetOrSetColor(const std::string& htmlColorFg, const std::string& htmlColorBg)
{
  std::string composition = htmlColorFg + htmlColorBg;
  std::hash<std::string> hasher;

  size_t hash = hasher(composition);

  if (!ContainsColorMap(hash))
  {
    auto fg = GetNColor(htmlColorFg);
    auto bg = GetNColor(htmlColorBg);

    short hashFg = hasher(htmlColorFg);
    short hashBg = hasher(htmlColorBg);

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

std::pair<int, int> Printer::AlignText(int x, int y, int align, const std::string& text)
{
  std::pair<int, int> res;

  int tx = x;
  int ty = y;

  switch (align)
  {
    case kAlignRight:
      // We have to compensate for new position after shift.
      //
      // E.g., print (80, 10, kAlignRight, "Bees")
      // will start from 76 position (tx -= text.length())
      // so it will actually end at 76 (B), 77 (e), 78 (e), 79 (s)
      // This way we either should not subtract 1 from TerminalWidth
      // when printing right aligned text at the end of the screen,
      // or make this hack.
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

void Printer::Print(const int& x, const int& y, const std::string& text, int align, const std::string& htmlColorFg, const std::string& htmlColorBg)
{
  size_t hash = GetOrSetColor(htmlColorFg, htmlColorBg);
  auto textPos = AlignText(x, y, align, text);

  attron(COLOR_PAIR(_colorMap[hash].PairIndex));
  mvprintw(textPos.first, textPos.second, text.data());
  attroff(COLOR_PAIR(_colorMap[hash].PairIndex));   
}

void Printer::Print(const int& x, const int& y, const int& ch, const std::string& htmlColorFg, const std::string& htmlColorBg)
{
  size_t hash = GetOrSetColor(htmlColorFg, htmlColorBg);

  attron(COLOR_PAIR(_colorMap[hash].PairIndex));
  mvaddch(y, x, ch);
  attroff(COLOR_PAIR(_colorMap[hash].PairIndex));
}

void Printer::PrintFB(const int& x, const int& y,
                                 const int& ch,
                                 const std::string& htmlColorFg,
                                 const std::string& htmlColorBg)
{
  if (x < 0 || x > TerminalWidth - 1
   || y < 0 || y > TerminalHeight - 1)
  {
    return;
  }

  // Black & White mode for Windows due to PDCurses not handling colors correctly

  #if !(defined(__unix__) || defined(__linux__))

  std::string tmpFg;
  std::string tmpBg;

  if (htmlColorFg == "#000000" && htmlColorBg == "#000000")
  {
    // For invisible and not discovered tiles

    tmpFg = "#000000";
    tmpBg = "#000000";
  }
  else if (htmlColorBg != "#000000")
  {
    tmpFg = "#000000";
    tmpBg = (htmlColorBg == GlobalConstants::FogOfWarColor) ? GlobalConstants::FogOfWarColor : "#FFFFFF";
  }
  else
  {
    tmpFg = (htmlColorFg == GlobalConstants::FogOfWarColor) ? GlobalConstants::FogOfWarColor : "#FFFFFF";
    tmpBg = "#000000";
  }

  size_t hash = GetOrSetColor(tmpFg, tmpBg);

  #else

  size_t hash = GetOrSetColor(htmlColorFg, htmlColorBg);

  #endif

  _frameBuffer[x][y].Character = ch;
  _frameBuffer[x][y].ColorPairHash = hash;
}

void Printer::PrintFB(const int& x, const int& y,
                                 const std::string& text,
                                 int align,
                                 const std::string& htmlColorFg,
                                 const std::string& htmlColorBg)
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

void Printer::PrepareFrameBuffer()
{
  for (int x = 0; x < TerminalWidth; x++)
  {
    std::vector<FBPixel> row;

    for (int y = 0; y < TerminalHeight; y++)
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

void Printer::Clear()
{
#ifndef USE_SDL
  for (int x = 0; x < TerminalWidth; x++)
  {
    for (int y = 0; y < TerminalHeight; y++)
    {
      PrintFB(x, y, ' ', "#000000");
    }
  }
#else
  SDL_SetRenderTarget(Application::Instance().Renderer, _frameBuffer);
  SDL_RenderClear(Application::Instance().Renderer);
#endif
}

void Printer::Render()
{
#ifndef USE_SDL
  for (int x = 0; x < TerminalWidth; x++)
  {
    for (int y = 0; y < TerminalHeight; y++)
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
  SDL_RenderCopy(Application::Instance().Renderer, _frameBuffer, nullptr, nullptr);
  SDL_RenderPresent(Application::Instance().Renderer);
#endif
}

std::vector<Position> Printer::DrawExplosion(Position pos, int aRange)
{
  std::vector<Position> cellsAffected = GetAreaDamagePointsFrom(pos, aRange);

  for (int range = 1; range <= aRange; range++)
  {
    auto res = GetAreaDamagePointsFrom(pos, range);
    for (auto& p : res)
    {
      int drawX = p.X + Map::Instance().CurrentLevel->MapOffsetX;
      int drawY = p.Y + Map::Instance().CurrentLevel->MapOffsetY;

      if (Map::Instance().CurrentLevel->MapArray[p.X][p.Y]->Visible)
      {
        Printer::Instance().PrintFB(drawX, drawY, 'x', "#FF0000");        
      }
    }

    Printer::Instance().Render();

    //#ifndef USE_SDL
    Util::Sleep(20);
    //#endif

    Application::Instance().ForceDrawMainState();
  }

  return cellsAffected;
}

std::vector<Position> Printer::GetAreaDamagePointsFrom(Position from, int range)
{
  std::vector<Position> res;

  int lx = from.X - range;
  int ly = from.Y - range;
  int hx = from.X + range;
  int hy = from.Y + range;

  std::vector<Position> perimeterPoints;

  for (int x = lx; x <= hx; x++)
  {
    Position p1 = { x, ly };
    Position p2 = { x, hy };

    perimeterPoints.push_back(p1);
    perimeterPoints.push_back(p2);
  }

  for (int y = ly + 1; y <= hy - 1; y++)
  {
    Position p1 = { lx, y };
    Position p2 = { hx, y };

    perimeterPoints.push_back(p1);
    perimeterPoints.push_back(p2);
  }

  for (auto& p : perimeterPoints)
  {
    // Different lines can go through the same points
    // so a check if a point was already added is needed.
    auto line = Util::BresenhamLine(from, p);
    for (auto& point : line)
    {
      if (!Util::IsInsideMap(point, Map::Instance().CurrentLevel->MapSize))
      {
        continue;
      }

      auto it = std::find_if(res.begin(), res.end(),
                [&point](const Position& p) ->
                bool { return (p == point); });

      // If point was already added, skip it.
      if (it != res.end())
      {
        continue;
      }

      int d = Util::LinearDistance(from, point);

      auto cell = Map::Instance().CurrentLevel->MapArray[point.X][point.Y].get();
      auto obj = Map::Instance().CurrentLevel->StaticMapObjects[point.X][point.Y].get();

      bool cellOk = (!cell->Blocking);
      bool objOk = (obj == nullptr);

      if (cellOk && objOk && d <= range)
      {
        res.push_back({ cell->PosX, cell->PosY });
      }
      else
      {
        if (obj != nullptr && !obj->Attrs.Indestructible)
        {
          res.push_back({ cell->PosX, cell->PosY });
        }

        break;
      }
    }
  }

  return res;
}

void Printer::AddMessage(const std::string& message)
{
  _inGameMessages.insert(_inGameMessages.begin(), message);

  if (_inGameMessages.size() > kMaxGameLogMessages)
  {
    _inGameMessages.pop_back();
  }

  _lastMessagesToDisplay++;

  _lastMessagesToDisplay = Util::Clamp(_lastMessagesToDisplay, 0, 5);

  ShowLastMessage = true;
}

std::vector<std::string> Printer::GetLastMessages()
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

std::string Printer::GetLastMessage()
{
  return (_inGameMessages.size() > 0) ? _inGameMessages.front() : std::string();
}

void Printer::ResetMessagesToDisplay()
{
  _lastMessages.clear();
  _lastMessagesToDisplay = 0;
}

std::vector<std::string>& Printer::Messages()
{
  return _inGameMessages;
}

int Printer::ColorsUsed()
{
  #ifndef USE_SDL
  return _colorMap.size();
  #else
  return -1;
  #endif
}
