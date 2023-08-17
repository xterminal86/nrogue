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

    UseResult Use(GameObject* user);
    void Transfer(ContainerComponent* destination = nullptr);
    void Inspect(bool overrideDescriptions = false);
    void Break(GameObject* itemOwner);

    std::pair<std::string, StringV> GetInspectionInfo(bool overrideDescriptions = false);

    ItemData Data;

  protected:
    void PrepareAdditional() override;

  private:
    std::vector<std::string> GetWeaponInspectionInfo();
    std::vector<std::string> GetReturnerInspectionInfo();
    std::vector<std::string> GetArmorInspectionInfo();
    std::vector<std::string> GetAccessoryInspectionInfo();
    std::vector<std::string> GetWandInspectionInfo();

    void AddModifiersInfo(std::vector<std::string>& res);
    void AddBonusesInfo(std::vector<std::string>& res);
    void AppendStatBonuses(const std::unordered_map<ItemBonusType, int>& statBonuses, std::vector<std::string>& res);

    std::unordered_map<ItemBonusType, int> CountAllStatBonuses();

    int _nonZeroStatBonuses = 0;

    bool _nonStatBonusesPresent = false;

    // =========================================================================

    const std::unordered_map<StatsEnum, ItemBonusType> _bonusByStat =
    {
      { StatsEnum::STR, ItemBonusType::STR },
      { StatsEnum::DEF, ItemBonusType::DEF },
      { StatsEnum::MAG, ItemBonusType::MAG },
      { StatsEnum::RES, ItemBonusType::RES },
      { StatsEnum::SKL, ItemBonusType::SKL },
      { StatsEnum::SPD, ItemBonusType::SPD }
    };
};

#endif // ITEMCOMPONENT_H
