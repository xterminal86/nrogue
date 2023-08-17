#include "custom-class-state.h"

#include "application.h"
#include "printer.h"
#include "util.h"

void CustomClassState::Init()
{
}

// =============================================================================

void CustomClassState::Prepare()
{
  _points = kStartingPoints;

  _warning = false;
  _displayWarning = false;

  _cursorRows = 0;
  _cursorCols = 0;

  _statDataByType[PlayerStats::STR] = { 0, 0 };
  _statDataByType[PlayerStats::DEF] = { 0, 0 };
  _statDataByType[PlayerStats::MAG] = { 0, 0 };
  _statDataByType[PlayerStats::RES] = { 0, 0 };
  _statDataByType[PlayerStats::SKL] = { 0, 0 };
  _statDataByType[PlayerStats::SPD] = { 0, 0 };

  _statDataByType[PlayerStats::HP] = { GetHp(), 0 };
  _statDataByType[PlayerStats::MP] = { 0, 0 };

  for (auto& kvp : _statDataByType)
  {
    _statStringByType[kvp.first] = GetStringsForStat(kvp.first);
  }
}

// =============================================================================

void CustomClassState::Cleanup()
{
}

// =============================================================================

void CustomClassState::HandleInput()
{
  _keyPressed = GetKeyDown();

  if (_keyPressed != -1)
  {
    switch (_keyPressed)
    {
      // NOTE: cannot use alternate keys because it clashes with '+' and '-'
      case NUMPAD_2:
      #ifdef USE_SDL
      case KEY_DOWN:
      #endif
        _cursorRows++;
        break;

      case NUMPAD_6:
      #ifdef USE_SDL
      case KEY_RIGHT:
      #endif
        _cursorCols++;
        break;

      case NUMPAD_4:
      #ifdef USE_SDL
      case KEY_LEFT:
      #endif
        _cursorCols--;
        break;

      case NUMPAD_8:
      #ifdef USE_SDL
      case KEY_UP:
      #endif
        _cursorRows--;
        break;

      //
      // Shift & '+' and '-'
      //
      case '>':
        ModifyStat(true, true);
        break;

      case '<':
        ModifyStat(false, true);
        break;

      //
      // '+' and '-'
      //
      case '.':
        ModifyStat(true, false);
        break;

      case ',':
        ModifyStat(false, false);
        break;

      case VK_CANCEL:
        Application::Instance().ChangeState(GameStates::SELECT_CLASS_STATE);
        break;

      case VK_ENTER:
      {
        if (_warning && !_displayWarning)
        {
          _displayWarning = true;
        }
        else if (!_warning || (_warning && _displayWarning))
        {
          Application::Instance().ChangeState(GameStates::ENTER_NAME_STATE);
        }
      }
      break;
    }

    _cursorCols = Util::Clamp(_cursorCols, 0, 1);
    _cursorRows = Util::Clamp(_cursorRows, 0, _playerStatByCursorRows.size() - 1);

    if (_playerStatByCursorRows[_cursorRows] == PlayerStats::HP
     || _playerStatByCursorRows[_cursorRows] == PlayerStats::MP)
    {
      _cursorCols = 0;
    }
  }

  _warning = (_points > 0);
}

// =============================================================================

void CustomClassState::Update(bool forceUpdate)
{
  if (_keyPressed != -1 || forceUpdate)
  {
    Printer::Instance().Clear();

    std::string pointsStr = Util::StringFormat("POINTS: %i", _points);
    Printer::Instance().PrintFB(_twHalf,
                                _startY,
                                pointsStr,
                                Printer::kAlignCenter,
                                Colors::WhiteColor);

    if (_cursorRows >= 6)
    {
      _cursorY = _cursorRows + 1;
    }
    else
    {
      _cursorY = _cursorRows;
    }

    for (auto& kvp : _statDataByType)
    {
      _statStringByType[kvp.first] = GetStringsForStat(kvp.first);
    }

    int count = 0;
    for (auto& kvp : _statStringByType)
    {
      Printer::Instance().PrintFB(_twHalf - 7,
                                  _startY + 2 + count,
                                  kvp.second.first.data(),
                                  Printer::kAlignLeft,
                                  Colors::WhiteColor);

      Printer::Instance().PrintFB(_twHalf + 2,
                                  _startY + 2 + count,
                                  kvp.second.second.data(),
                                  Printer::kAlignLeft,
                                  Colors::WhiteColor);
      count++;

      //
      // Additional space between base stats and HP MP
      //
      if (kvp.first == PlayerStats::SPD)
      {
        count++;
      }
    }

    if (_displayWarning)
    {
      Printer::Instance().PrintFB(_twHalf,
                                  Printer::TerminalHeight - 8,
                                  "[WARNING] unused points remaining",
                                  Printer::kAlignCenter,
                                  Colors::YellowColor);

      Printer::Instance().PrintFB(_twHalf,
                                  Printer::TerminalHeight - 7,
                                  "Press 'Enter' if that's OK",
                                  Printer::kAlignCenter,
                                  Colors::WhiteColor);
    }

    Printer::Instance().PrintFB(_twHalf - 9 + 10 * _cursorCols,
                                _startY + 2 + _cursorY,
                                _cursorImage,
                                Colors::WhiteColor);

    Printer::Instance().PrintFB(_twHalf,
                                Printer::TerminalHeight - 3,
                                "Arrow keys to navigate",
                                Printer::kAlignCenter,
                                Colors::WhiteColor);

    Printer::Instance().PrintFB(_twHalf,
                                Printer::TerminalHeight - 2,
                                "'.' or ',' to modify",
                                Printer::kAlignCenter,
                                Colors::WhiteColor);

    Printer::Instance().PrintFB(_twHalf,
                                Printer::TerminalHeight - 1,
                                "Hold [SHIFT] for 5% step",
                                Printer::kAlignCenter,
                                Colors::WhiteColor);

    Printer::Instance().PrintFB(1,
                                Printer::TerminalHeight - 1,
                                "'q' - go back",
                                Printer::kAlignLeft,
                                Colors::WhiteColor);

    Printer::Instance().PrintFB(Printer::TerminalWidth - 1,
                                Printer::TerminalHeight - 1,
                                "'Enter' - accept",
                                Printer::kAlignRight,
                                Colors::WhiteColor);

    Printer::Instance().Render();
  }
}

// =============================================================================

sspair CustomClassState::GetStringsForStat(PlayerStats statType)
{
  sspair res;

  std::string statName = GlobalConstants::StatNameByType.at(statType);

  if (statType != PlayerStats::HP && statType != PlayerStats::MP)
  {
    res.first  = Util::StringFormat("%s:  %i  ", statName.data(), _statDataByType[statType].first);
    res.second = Util::StringFormat("(%i%)", _statDataByType[statType].second);
  }
  else
  {
    std::string stars = "___";
    for (int i = 0; i < _statDataByType[statType].second; i++)
    {
      stars[i] = '*';
    }

    res.first = Util::StringFormat("%s:  %i  ", statName.data(), _statDataByType[statType].first);
    res.second = Util::StringFormat("(%s)", stars.data());
  }

  return res;
}

// =============================================================================

void CustomClassState::ModifyStat(bool isIncreasing, bool shiftPressed)
{
  _displayWarning = false;

  PlayerStats statFromCursor = _playerStatByCursorRows[_cursorRows];

  bool isBaseStat = (statFromCursor != PlayerStats::HP && statFromCursor != PlayerStats::MP);

  bool isHpMp = (statFromCursor == PlayerStats::HP || statFromCursor == PlayerStats::MP);

  //
  // NOTE: for now this is kinda hardcoded
  // since we don't have anything else right now,
  // but this might change in the future with addition of another columns
  // like skills, equipment etc.
  //
  bool isGrowthRate = (_cursorCols == 1);

  if (isIncreasing)
  {
    if (isBaseStat)
    {
      if (isGrowthRate)
      {
        int cost = (shiftPressed ? kGrowthRateCostShift : kGrowthRateCost);
        bool canBeIncreased = (_statDataByType[statFromCursor].second + cost <= 100);
        if (_points >= cost && canBeIncreased)
        {
          _points -= cost;
          _statDataByType[statFromCursor].second += cost;
        }
      }
      else
      {
        int cost = kBaseStatCost + _statDataByType[statFromCursor].first;
        if (_points >= cost)
        {
          _points -= cost;
          _statDataByType[statFromCursor].first++;
        }
      }
    }
    else if (isHpMp)
    {
      int cost = kTalentCost + _statDataByType[statFromCursor].second * kTalentCost;
      if (_points >= cost && _statDataByType[statFromCursor].second < 3)
      {
        _points -= cost;
        _statDataByType[statFromCursor].second++;
      }
    }
  }
  else
  {
    if (isBaseStat)
    {
      if (isGrowthRate)
      {
        int cost = shiftPressed ? kGrowthRateCostShift : kGrowthRateCost;
        bool canBeDecreased = (_statDataByType[statFromCursor].second - cost >= 0);
        if (canBeDecreased)
        {
          _points += cost;
          _statDataByType[statFromCursor].second -= cost;
        }
      }
      else
      {
        int cost = kBaseStatCost + (_statDataByType[statFromCursor].first - 1);
        if (_statDataByType[statFromCursor].first > 0)
        {
          _points += cost;
          _statDataByType[statFromCursor].first--;
        }
      }
    }
    else if (isHpMp)
    {
      int cost = kTalentCost + (_statDataByType[statFromCursor].second - 1) * kTalentCost;
      if (_statDataByType[statFromCursor].second > 0)
      {
        _points += cost;
        _statDataByType[statFromCursor].second--;
      }
    }
  }

  int hp = GetHp();
  _statDataByType[PlayerStats::HP].first = hp;

  int mp = GetMp();
  _statDataByType[PlayerStats::MP].first = mp;
}

// =============================================================================

int CustomClassState::GetHp()
{
  return (3 + 4 * (_statDataByType[PlayerStats::HP].second + 1));
}

// =============================================================================

int CustomClassState::GetMp()
{
  int mp = 0;
  for (int i = 0; i < _statDataByType[PlayerStats::MAG].first; i++)
  {
    mp += ((i + 1) + _statDataByType[PlayerStats::MP].second);
  }

  return mp;
}

// =============================================================================

void CustomClassState::InitPlayerAttributes(Player* playerRef)
{
  std::unordered_map<PlayerStats, Attribute&> map =
  {
    { PlayerStats::STR, playerRef->Attrs.Str },
    { PlayerStats::DEF, playerRef->Attrs.Def },
    { PlayerStats::MAG, playerRef->Attrs.Mag },
    { PlayerStats::RES, playerRef->Attrs.Res },
    { PlayerStats::SKL, playerRef->Attrs.Skl },
    { PlayerStats::SPD, playerRef->Attrs.Spd }
  };

  for (auto& kvp : map)
  {
    kvp.second.Set(_statDataByType[kvp.first].first);
    kvp.second.RaiseProbability = _statDataByType[kvp.first].second;
  }

  playerRef->Attrs.HP.Reset(_statDataByType[PlayerStats::HP].first);
  playerRef->Attrs.HP.Talents = _statDataByType[PlayerStats::HP].second;

  playerRef->Attrs.MP.Reset(_statDataByType[PlayerStats::MP].first);
  playerRef->Attrs.MP.Talents = _statDataByType[PlayerStats::MP].second;

  playerRef->Attrs.HungerSpeed.Set(1);

  // TODO: HungerRate, HealthRegenTurns, Skills

  // FIXME: determine proper values
  playerRef->Attrs.HungerRate.Set(1750);
  playerRef->HealthRegenTurns = 60;
}
