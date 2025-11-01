#ifndef OBITUARYREPORTSTATE_H
#define OBITUARYREPORTSTATE_H

#include "gamestate.h"

#include "msg-scroll-buffer.h"

class ObituaryReportState : public GameState
{
  public:
    void Prepare() override;
    void HandleInput() override;
    void Update(bool forceUpdate = false) override;

  private:
    MsgScrollBuffer<std::string> _obituary{24, 5};
};

#endif // OBITUARYREPORTSTATE_H
