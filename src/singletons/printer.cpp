#include "printer.h"

#include "application.h"
#include "util.h"

void Printer::Init()
{
#ifdef USE_SDL
  InitForSDL();
#else
  InitForCurses();
#endif
}

#ifdef USE_SDL
void Printer::InitForSDL()
{
  std::string tilesetFile = TILESET_FILE;

  _tileWidth = TILESET_WIDTH;
  _tileHeight = TILESET_HEIGHT;

  SDL_Surface* surf = IMG_Load(tilesetFile.data());
  if (surf)
  {
    _tileset = SDL_CreateTextureFromSurface(Application::Instance().Renderer, surf);
    SDL_FreeSurface(surf);

    int w = 0, h = 0;
    SDL_QueryTexture(_tileset, nullptr, nullptr, &w, &h);

    _tilesetWidth = w;
    _tilesetHeight = h;

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
  else
  {
    printf("Could not load tileset: %s!\n", SDL_GetError());
  }
}

void Printer::DrawTile(int x, int y, int tileIndex)
{
  TileInfo& tile = _tiles[tileIndex];

  SDL_Rect src;
  src.x = tile.X;
  src.y = tile.Y;
  src.w = _tileWidth;
  src.h = _tileHeight;

  int scaledW = Application::Instance().TileWidth;
  int scaledH = Application::Instance().TileHeight;

  SDL_Rect dst;
  dst.x = x;
  dst.y = y;
  dst.w = scaledW;
  dst.h = scaledH;

  SDL_RenderCopy(Application::Instance().Renderer, _tileset, &src, &dst);
}

void Printer::PrintFB(const int& x, const int& y,
                      int image,
                      const std::string& htmlColorFg,
                      const std::string& htmlColorBg)
{
  TileColor tc;

  int tileIndex = image;

  int scaledW = Application::Instance().TileWidth;
  int scaledH = Application::Instance().TileHeight;

  int posX = x * scaledW;
  int posY = y * scaledH;

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
  int scaledW = Application::Instance().TileWidth;
  int scaledH = Application::Instance().TileHeight;

  int px = x * scaledW;
  int py = y * scaledH;

  switch (align)
  {
    case kAlignCenter:
    {
      int pixelWidth = text.length() * scaledW;
      px -= pixelWidth / 2;
    }
    break;

    case kAlignRight:
    {
      int pixelWidth = text.length() * scaledW;
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

    px += scaledW;
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
  SDL_RenderPresent(Application::Instance().Renderer);
#endif
}

void Printer::AddMessage(std::string message)
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
