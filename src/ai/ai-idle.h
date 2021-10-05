#ifndef AIIDLE_H
#define AIIDLE_H

#include "ai-model-base.h"

class AIIdle : public AIModelBase
{
  public:
    AIIdle();

  protected:
    void PrepareScript() override;
};

#endif // AIIDLE_H
