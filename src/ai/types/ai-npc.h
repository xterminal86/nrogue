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
  bool IsImmovable = false;
  bool IsAquainted = false;
  bool IsMale = true;
  bool CanSpeak = true;
  ServiceType ProvidesService = ServiceType::NONE;

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

    void Init(NPCType type, bool immovable, ServiceType serviceType);

    NPCData Data;

  protected:
    void PrepareScript() override;

  private:
    NPCType _npcType;

    void SetDataClaire();
    void SetDataCloud();
    void SetDataIan();
    void SetDataMiles();
    void SetDataPhoenix();
    void SetDataTigra();
    void SetDataSteve();
    void SetDataGimley();
    void SetDataMartin();
    void SetDataCasey();
    void SetDataMaya();
    void SetDataGriswold();

    void SetDataDefault();
};

#endif // AINPC_H
