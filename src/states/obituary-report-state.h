#ifndef OBITUARYREPORTSTATE_H
#define OBITUARYREPORTSTATE_H

#include "gamestate.h"

#include "msg-scroll-buffer.h"

class ObituaryReportState : public GameState
{
  public:
    ObituaryReportState();

    void Prepare() override;
    void HandleInput() override;
    void Update(bool forceUpdate = false) override;

  private:
    using MsgBuffer = MsgScrollBuffer<std::string>;
    std::unique_ptr<MsgBuffer> _obituary;
};

#endif // OBITUARYREPORTSTATE_H
