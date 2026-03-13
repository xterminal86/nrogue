#include "message-box-state.h"
#include "printer.h"
#include "application.h"

#ifndef USE_SDL
#include "util.h"
#endif

// =============================================================================

void MessageBoxState::HandleInput()
{
  _keyPressed = GetKeyDown();

  if (_keyPressed != -1)
  {
    switch (_type)
    {
      case MessageBoxType::WAIT_FOR_INPUT:
      {
        if (_keyPressed == VK_ENTER || _keyPressed == 'q')
        {
          Game::gApp.CloseMessageBox();
        }
      }
      break;

      default:
        Game::gApp.CloseMessageBox();
        break;
    }
  }
}

// =============================================================================

void MessageBoxState::Update(bool forceUpdate)
{
  if (_keyPressed != -1 || forceUpdate)
  {
    switch (_windowToShow)
    {
      case WindowToShow::TEXT:
        DisplayText();
        break;

      case WindowToShow::ACTOR_STATS:
        DisplayActorStats();
        break;

      default:
      {
        ConsoleLog("Unexpected window to show type %d!", (int)_windowToShow);
        Game::gPrnt.Clear();
      }
      break;
    }

    Game::gPrnt.Render();
  }
}

// =============================================================================

void MessageBoxState::DisplayText()
{
  uint32_t headerBgColor = Colors::MessageBoxHeaderBg;
  if (_borderColor == Colors::MessageBoxRedBorder)
  {
    headerBgColor = 0x660000;
  }

  Game::gPrnt.DrawWindow(_leftCorner,
                          _windowSize,
                          _header,
                          Colors::White,
                          headerBgColor,
                          _borderColor,
                          Colors::Black,
                          _bgColor);

  int offset = 0;
  for (auto& s : _message)
  {
    Game::gPrnt.PrintText(
      _tw / 2,
      _th / 2 - _message.size() / 2 + offset,
      s,
      Printer::kAlignCenter,
      Colors::White,
      _bgColor
    );

    offset++;
  }
}

// =============================================================================

void MessageBoxState::DisplayActorStats()
{
  if (_actorRef == nullptr)
  {
    ConsoleLog("[ERR] MessageBoxState::DisplayActorStats() - _actorRef is null!");
    return;
  }

#ifdef USE_SDL
  GraphicTileInfo gti = _actorRef->Graphic;

  // FIXME: draw from substitute tileset if graphic tile is not set or
  // 'UseGraphics' is off.
  if (gti.Tile == GraphicTiles::NONE)
  {
    gti.Tile = GraphicTiles::Z_UNKNOWN;
  }

  gti.ScaleFactor = 8;

  int wndTileUnitsWidth =
      Game::gApp.AppData.WindowWidth / Game::gApp.GameConfig.TileSize;
  int wndTileUnitsHeight =
      Game::gApp.AppData.WindowHeight / Game::gApp.GameConfig.TileSize;

  int wndHalfW = wndTileUnitsWidth / 2;
  int wndHalfH = wndTileUnitsHeight / 2;

  //
  // Since window is drawn using text tileset (which is 8x16), we have to take
  // that into account during calculation of window position and size.
  //
  Game::gPrnt.DrawWindow({ _twHalf - 20, _thHalf - 6 },
                         { 40, 11 },
                         (_actorRef->Type == GameObjectType::PLAYER) ?
                         Game::gApp.PlayerInstance.Name :
                         _actorRef->ObjectName);

  DrawStatsBlock({ _twHalf + 2, _thHalf - 3});

  Game::gPrnt.DrawGraphicsTileExt(wndHalfW - 8, wndHalfH - 4, gti);
#else
  Game::gPrnt.DrawWindow({ _twHalf - 11, _thHalf - 3 },
                         { 22, 8 },
                         (_actorRef->Type == GameObjectType::PLAYER) ?
                         Game::gApp.PlayerInstance.Name :
                         _actorRef->ObjectName);

  DrawStatsBlock({ _twHalf - 8, _thHalf - 2 });
#endif
}

// =============================================================================

void MessageBoxState::DrawStatsBlock(const Position& start)
{
  if (_actorRef == nullptr)
  {
    ConsoleLog("[ERR] MessageBoxState::DrawStatsBlock() - _actorRef is null!");
    return;
  }

  // ---------------------------------------------------------------------------
  auto PrintValue =
  [this](const Position& pos, int value, const std::string& tag)
  {
    std::string total = Util::StringFormat("%s:%s%2d",
                                           tag.data(),
                                           (value < 10) ? "" : " ",
                                           value);

    Game::gPrnt.PrintText(pos.X,
                          pos.Y,
                          total,
                          Printer::kAlignLeft,
                          Colors::White,
                          Colors::Black);

    return total;
  };
  // ---------------------------------------------------------------------------

  PrintValue({ start.X, start.Y     }, _actorRef->Attrs.Lvl.Get(),      "LVL");
  PrintValue({ start.X, start.Y + 2 }, _actorRef->Attrs.HP.Min().Get(), "HP");
  PrintValue({ start.X, start.Y + 3 }, _actorRef->Attrs.MP.Min().Get(), "MP");
  PrintValue({ start.X, start.Y + 5 }, _actorRef->Attrs.Rating(),       "CR");

  PrintValue({ start.X + 10, start.Y     }, _actorRef->Attrs.Str.Get(), "STR");
  PrintValue({ start.X + 10, start.Y + 1 }, _actorRef->Attrs.Def.Get(), "DEF");
  PrintValue({ start.X + 10, start.Y + 2 }, _actorRef->Attrs.Mag.Get(), "MAG");
  PrintValue({ start.X + 10, start.Y + 3 }, _actorRef->Attrs.Res.Get(), "RES");
  PrintValue({ start.X + 10, start.Y + 4 }, _actorRef->Attrs.Skl.Get(), "SKL");
  PrintValue({ start.X + 10, start.Y + 5 }, _actorRef->Attrs.Spd.Get(), "SPD");
}

// =============================================================================

void MessageBoxState::SetMessage(MessageBoxType type,
                                 const std::string& header,
                                 const std::vector<std::string>& message,
                                 const uint32_t& borderColor,
                                 const uint32_t& bgColor)
{
  _windowToShow = WindowToShow::TEXT;

  _type = type;
  _header = header;
  _message = message;
  _bgColor = bgColor;
  _borderColor = borderColor;

  size_t len = 0;
  for (auto& s : _message)
  {
    if (s.length() > len)
    {
      len = s.length();
    }
  }

  //
  // In case header is longer than message.
  //
  if (len < _header.length())
  {
    len = _header.length();
  }

  //
  // Taking into account message size (usually > 0, so as is),
  // and that it's not actually a total count of number of rows,
  // but addition that we must add to the y1,
  // so 5 rows means we must add 4 if message.size = 1, 5 if 2 and so on.
  //
  int rows = 3 + _message.size();

  int x1 = _tw / 2 - len / 2 - 3;
  int y1 = _th / 2 - _message.size() / 2 - 2;
  int x2 = x1 + len + 5;
  int y2 = y1 + rows;

  _leftCorner = { x1, y1 };

#ifdef USE_SDL
  _windowSize = { x2 - x1, y2 - y1 };
#else
  _windowSize = { x2 - x1 + 1, y2 - y1 + 1 };
#endif
}

// =============================================================================

void MessageBoxState::SetActorStats(GameObject* actor)
{
  _type         = MessageBoxType::ANY_KEY;
  _windowToShow = WindowToShow::ACTOR_STATS;
  _actorRef     = actor;
}
