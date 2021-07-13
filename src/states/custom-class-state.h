#ifndef CUSTOMCLASSSTATE_H
#define CUSTOMCLASSSTATE_H

#include "gamestate.h"

class Player;

using sspair = std::pair<std::string, std::string>;

class CustomClassState : public GameState
{
  public:
    void Init() override;
    void Cleanup() override;
    void Prepare() override;
    void Update(bool forceUpdate = false) override;
    void HandleInput() override;

    void InitPlayerAttributes(Player* playerRef);

  private:
    const int kBaseStatCost = 25;
    const int kGrowthRateCost = 1;
    const int kGrowthRateCostShift = 5;
    const int kTalentCost = 10;
    const int kStartingPoints = 300;

    int _points = kStartingPoints;

    int _startY = 1;

    int _cursorRows = 0;
    int _cursorCols = 0;

    int _cursorY = 0;

    #ifdef USE_SDL
    int _cursorImage = GlobalConstants::CP437IndexByType[NameCP437::RARROW_2];
    #else
    int _cursorImage = '>';
    #endif

    bool _warning = false;
    bool _displayWarning = false;

    std::map<PlayerStats, sspair> _statStringByType;
    std::map<PlayerStats, std::pair<int, int>> _statDataByType;

    std::map<int, PlayerStats> _playerStatByCursorRows =
    {
      { 0, PlayerStats::STR },
      { 1, PlayerStats::DEF },
      { 2, PlayerStats::MAG },
      { 3, PlayerStats::RES },
      { 4, PlayerStats::SKL },
      { 5, PlayerStats::SPD },
      { 6, PlayerStats::HP  },
      { 7, PlayerStats::MP  }
    };

    sspair GetStringsForStat(PlayerStats statType);
    void ModifyStat(bool isIncreasing, bool shiftPressed);

    int GetHp();
    int GetMp();
};

#endif // CUSTOMCLASSSTATE_H
