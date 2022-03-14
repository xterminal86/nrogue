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

    std::pair<std::string, StringsArray2D> GetInspectionInfo(bool overrideDescriptions = false);

    ItemData Data;

  private:
    std::vector<std::string> GetWeaponInspectionInfo();
    std::vector<std::string> GetReturnerInspectionInfo();
    std::vector<std::string> GetArmorInspectionInfo();
    std::vector<std::string> GetAccessoryInspectionInfo();
    std::vector<std::string> GetWandInspectionInfo();

    void AddModifiersInfo(std::vector<std::string>& res);
    void AddBonusesInfo(std::vector<std::string>& res);
    void AppendStatBonuses(const std::map<ItemBonusType, int>& statBonuses, std::vector<std::string>& res);

    std::map<ItemBonusType, int> CountAllStatBonuses();

    int _nonZeroStatBonuses = 0;

    bool _nonStatBonusesPresent = false;
};

#endif // ITEMCOMPONENT_H
