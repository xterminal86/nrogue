#ifndef REPLAYSTARTSTATE_H
#define REPLAYSTARTSTATE_H

#include "gamestate.h"

class ReplayStartState : public GameState
{
  public:
    void Prepare() override;
    void HandleInput() override;
    void Update(bool forceUpdate = false) override;

  private:
    struct ReplayFileData
    {
      std::string Timestamp;
      std::string PlayerName;
      PlayerClass Class = PlayerClass::SOLDIER;
      size_t WorldSeed = 0;
      std::string FileNameTruncated;
      std::string Filename;
    };

    std::map<std::string, ReplayFileData> _files;

    const ReplayFileData& ReadReplayFileData(const std::string& fname);

    ReplayFileData _replayFileData;

    int _cursorIndex  = 0;
    int _scrollOffset = 0;

    const size_t _replayNameCharsLimit = 26;

    const int _maxFilesToDisplay = 20;

    const std::string _selectionBarSpaces = std::string(28, ' ');

    int _replaySpeed = (int)ReplaySpeed::NORMAL;

    bool _deleteStarted = false;
};

#endif // REPLAYSTARTSTATE_H
