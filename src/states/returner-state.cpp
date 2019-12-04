#include "returner-state.h"

#include "application.h"
#include "map.h"
#include "printer.h"

void ReturnerState::Prepare()
{
  _playerRef = &Application::Instance().PlayerInstance;

  Printer::Instance().AddMessage("What do you want to do with returner?");
  Printer::Instance().AddMessage("(a) - attune, (u) - use, (q) - nothing");
}

void ReturnerState::HandleInput()
{
  _keyPressed = GetKeyDown();

  switch (_keyPressed)
  {
    case 'u':
    {
      auto level = _itemRef->Data.ReturnerPosition.first;
      Position pos =
      {
        _itemRef->Data.ReturnerPosition.second.first,
        _itemRef->Data.ReturnerPosition.second.second
      };

      if (level == MapType::NOWHERE)
      {
        Printer::Instance().AddMessage("The stone is not attuned!");
      }
      else
      {
        Printer::Instance().AddMessage("You invoke the returner");
        Map::Instance().TeleportToExistingLevel(level, pos);
        _itemRef->Data.Amount--;        
      }

      Application::Instance().ChangeState(GameStates::MAIN_STATE);
    }
    break;

    case 'a':
    {
      _itemRef->Data.ReturnerPosition.first = Map::Instance().CurrentLevel->MapType_;

      if (_itemRef->Data.Prefix == ItemPrefix::CURSED)
      {
        Position p = GetRandomPositionAroundPlayer();
        _itemRef->Data.ReturnerPosition.second.first = p.X;
        _itemRef->Data.ReturnerPosition.second.second = p.Y;
      }
      else
      {
        _itemRef->Data.ReturnerPosition.second.first = _playerRef->PosX;
        _itemRef->Data.ReturnerPosition.second.second = _playerRef->PosY;
      }

      Printer::Instance().AddMessage("The stone has been attuned to this position");

      _playerRef->FinishTurn();

      Application::Instance().ChangeState(GameStates::MAIN_STATE);
    }
    break;

    case 'q':
      Printer::Instance().AddMessage("Cancelled");
      Application::Instance().ChangeState(GameStates::MAIN_STATE);
      break;

    default:
      break;
  }
}

void ReturnerState::Update(bool forceUpdate)
{
  if (_keyPressed != -1 || forceUpdate)
  {
    Printer::Instance().Clear();

    _playerRef->CheckVisibility();

    Map::Instance().Draw();

    _playerRef->Draw();

    auto& msgs = Printer::Instance().Messages();
    auto msg1 = msgs.at(0);
    auto msg2 = msgs.at(1);

    Printer::Instance().PrintFB(Printer::Instance().TerminalWidth - 1,
                                Printer::Instance().TerminalHeight - 1,
                                msg1,
                                Printer::kAlignRight,
                                "#FFFFFF");

    Printer::Instance().PrintFB(Printer::Instance().TerminalWidth - 1,
                                Printer::Instance().TerminalHeight - 2,
                                msg2,
                                Printer::kAlignRight,
                                "#FFFFFF");

    Printer::Instance().Render();
  }
}

void ReturnerState::SetItemComponentRef(ItemComponent* item)
{
  _itemRef = item;
}

Position ReturnerState::GetRandomPositionAroundPlayer()
{
  Position res;

  int range = 5;

  int lx = _playerRef->PosX - range;
  int ly = _playerRef->PosY - range;
  int hx = _playerRef->PosX + range;
  int hy = _playerRef->PosY + range;

  std::vector<Position> positions;

  Position mapSize = Map::Instance().CurrentLevel->MapSize;

  for (int x = lx; x <= hx; x++)
  {
    for (int y = ly; y <= hy; y++)
    {
      if (x >= 1 && x < mapSize.X - 1
       && y >= 1 && y < mapSize.Y - 1)
      {
        positions.push_back({ x, y });
      }
    }
  }

  int index = RNG::Instance().RandomRange(0, positions.size());
  res = positions[index];

  return res;
}
