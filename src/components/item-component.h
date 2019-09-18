#ifndef ITEMCOMPONENT_H
#define ITEMCOMPONENT_H

#include "item-data.h"
#include "component.h"

class ContainerComponent;

class ItemComponent : public Component
{
  public:
    ItemComponent();

    void Update() override;

    bool Use();
    void Transfer(ContainerComponent* destination = nullptr);
    void Inspect(bool overrideDescriptions = false);
    bool Equip();

    ItemData Data;    

  private:
    std::vector<std::string> GetWeaponInspectionInfo();    
    std::vector<std::string> GetReturnerInspectionInfo();    
    std::vector<std::string> GetArmorInspectionInfo();

    void AddModifiersInfo(std::vector<std::string>& res);
    void AddBonusesInfo(std::vector<std::string>& res);

    // std::map is sorted by key
    std::map<int, std::pair<StatsEnum, std::string>> _allStatNames =
    {
      { 0, { StatsEnum::STR, "STR" } },
      { 1, { StatsEnum::DEF, "DEF" } },
      { 2, { StatsEnum::MAG, "MAG" } },
      { 3, { StatsEnum::RES, "RES" } },
      { 4, { StatsEnum::SKL, "SKL" } },
      { 5, { StatsEnum::SPD, "SPD" } }
    };
};

#endif // ITEMCOMPONENT_H
