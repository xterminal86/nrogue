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
      PlayerClass Class;
      size_t WorldSeed;
      std::string FileNameTruncated;
      std::string Filename;
    };

    std::map<std::string, ReplayFileData> _files;

    const ReplayFileData& ReadReplayFileData(const std::string& fname);

    ReplayFileData _replayFileData;

    int _cursorIndex = 0;
    int _scrollOffset = 0;

    const size_t _replayNameCharsLimit = 26;

    const int _maxFilesToDisplay = 20;

    int _replaySpeed = 0;
};

#endif // REPLAYSTARTSTATE_H
