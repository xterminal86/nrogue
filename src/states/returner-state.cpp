#include "returner-state.h"

#include "application.h"
#include "map.h"
#include "printer.h"
#include "rng.h"

void ReturnerState::Init()
{
}

// =============================================================================

void ReturnerState::Prepare()
{
  _playerRef = &Game::gApp.PlayerInstance;
}

// =============================================================================

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
        Game::gPrnt.AddMessage("The stone is not attuned");
      }
      else
      {
        Game::gPrnt.AddMessage("You invoke the returner...");

        auto str = Game::gMap.TeleportToExistingLevel(level, pos);
        if (!str.empty())
        {
          Game::gPrnt.AddMessage(str);
        }

        _itemRef->Data.Amount--;
      }

      Game::gApp.ChangeState(GameStates::MAIN_STATE);
    }
    break;

    case 'a':
    {
      _itemRef->Data.ReturnerPosition.first =
          Game::gMap.CurrentLevel->MapType_;

      if (_itemRef->Data.Prefix == ItemPrefix::CURSED)
      {
        Position p = GetRandomPositionAroundPlayer();
        _itemRef->Data.ReturnerPosition.second.first  = p.X;
        _itemRef->Data.ReturnerPosition.second.second = p.Y;
      }
      else
      {
        _itemRef->Data.ReturnerPosition.second.first  = _playerRef->PosX;
        _itemRef->Data.ReturnerPosition.second.second = _playerRef->PosY;
      }

      Game::gPrnt.AddMessage(
        "The stone has been attuned to this position"
      );

      _playerRef->FinishTurn();

      Game::gApp.ChangeState(GameStates::MAIN_STATE);
    }
    break;

    case VK_CANCEL:
      Game::gApp.ChangeState(GameStates::MAIN_STATE);
      break;

    default:
      break;
  }
}

// =============================================================================

void ReturnerState::Update(bool forceUpdate)
{
  if (_keyPressed != -1 || forceUpdate)
  {
    Game::gPrnt.Clear();

    _playerRef->CheckVisibility();

    Game::gMap.Draw();

    _playerRef->Draw();

    Game::gPrnt.PrintText(
      Printer::TerminalWidth - 1,
      Printer::TerminalHeight - 1,
      "What do you want to do with returner?",
      Printer::kAlignRight,
      Colors::White,
      Colors::Black
    );

    Game::gPrnt.PrintText(
      Printer::TerminalWidth - 1,
      Printer::TerminalHeight - 2,
      "(a) - attune, (u) - use, (q) - nothing",
      Printer::kAlignRight,
      Colors::White,
      Colors::Black
    );

    Game::gPrnt.Render();
  }
}

// =============================================================================

void ReturnerState::SetItemComponentRef(ItemComponent* item)
{
  _itemRef = item;
}

// =============================================================================

Position ReturnerState::GetRandomPositionAroundPlayer()
{
  Position res;

  int range = 5;

  int lx = _playerRef->PosX - range;
  int ly = _playerRef->PosY - range;
  int hx = _playerRef->PosX + range;
  int hy = _playerRef->PosY + range;

  std::vector<Position> positions;

  Position mapSize = Game::gMap.CurrentLevel->MapSize;

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

  int index = Game::gRng.RandomRange(0, positions.size());
  res = positions[index];

  return res;
}
