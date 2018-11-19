#ifndef AINPC_H
#define AINPC_H

#include <map>
#include <vector>
#include <string>

#include "constants.h"
#include "ai-model-base.h"

using TextLines = std::vector<std::string>;

struct NPCData
{
  bool IsStanding = false;
  bool IsAquainted = false;
  bool IsMale = true;

  std::string UnacquaintedDescription;

  std::string Name;
  std::string Job;

  std::string NameResponse;
  std::string JobResponse;
  std::map<MapType, std::vector<TextLines>> GossipResponsesByMap;
};

class AINPC : public AIModelBase
{
  public:
    AINPC();

    void Init(NPCType type, bool standing);

    void Update() override;

    const NPCData& Data()
    {
      return _data;
    }

  private:
    NPCType _npcType;

    NPCData _data;

    void RandomMovement();
};

#endif // AINPC_H
