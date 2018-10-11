#include "printer.h"

#include "application.h"

void Printer::Init()
{
  int mx = 0;
  int my = 0;

  getmaxyx(stdscr, my, mx);

  TerminalWidth = mx;
  TerminalHeight = my;

  // Enforce colors of standard ncurses colors
  // because some colors aren't actually correspond to their
  // "names", e.g. COLOR_BLACK isn't actually black, but grey,
  // so we redefine it
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

  return std::move(ret);
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

    //mvprintw(12, 10, "%i => %i %i %i", fg.ColorIndex, fg.R, fg.G, fg.B);
    //mvprintw(13, 10, "%i => %i %i %i", bg.ColorIndex, bg.R, bg.G, bg.B);

    ColorPair cp = { _colorPairsGlobalIndex++, fg, bg };
    _colorMap[hash] = cp;

    init_pair(_colorMap[hash].PairIndex, _colorIndexMap[hashFg], _colorIndexMap[hashBg]);

    //mvprintw(11, 10, "%u %u %u", hashFg, hashBg, _colorGlobalIndex);
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

void Printer::Print(const int& x, const int& y, const chtype& ch, const std::string& htmlColorFg, const std::string& htmlColorBg)
{
  size_t hash = GetOrSetColor(htmlColorFg, htmlColorBg);

  attron(COLOR_PAIR(_colorMap[hash].PairIndex));
  mvaddch(y, x, ch);
  attroff(COLOR_PAIR(_colorMap[hash].PairIndex));
}

void Printer::AddMessage(std::string message)
{
  if (_inGameMessages.size() > kMaxGameLogMessages)
  {
    _inGameMessages.pop_back();
  }

  _inGameMessages.insert(_inGameMessages.begin(), message);

  ShowLastMessage = true;
}

void Printer::PrintFB(const int& x, const int& y,
                                 const chtype& ch,
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

  auto tmp = htmlColorFg;
  std::string tmpFg;
  if (tmp != GlobalConstants::BlackColor && tmp != GlobalConstants::FogOfWarColor)
  {
    tmpFg = "#FFFFFF";
  }
  else
  {
    tmpFg = tmp;
  }

  tmp = htmlColorBg;
  std::string tmpBg;
  if (tmp != GlobalConstants::BlackColor && tmp != GlobalConstants::FogOfWarColor && tmp != "#FFFFFF")
  {
    tmpBg = (htmlColorFg.length() != 0) ? "#000000" : tmpFg;
  }
  else
  {
    tmpBg = tmp;
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

void Printer::Clear()
{
  for (int x = 0; x < TerminalWidth; x++)
  {
    for (int y = 0; y < TerminalHeight; y++)
    {
      PrintFB(x, y, ' ', "#000000");
    }
  }
}

void Printer::Render()
{
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
}
