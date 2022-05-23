#ifndef PICKUPITEMSTATE_H
#define PICKUPITEMSTATE_H

#include "select-item-state-base.h"

class GameObject;
class Player;

using Item  = std::pair<int, GameObject*>;
using Items = std::vector<Item>;

class PickupItemState : public SelectItemStateBase
{
  public:
    void Init() override;
    void Prepare() override;
    void Setup(const Items& items);

  protected:
    void ProcessInput() final;
    void DrawSpecific() final;

  private:
    bool PickupItem(const Item& item);

    void RebuildDisplayList();

    Player* _playerRef = nullptr;

    Items _itemsList;

    std::map<char, int> _itemsListIndexByChar;
    std::vector<std::string> _displayLines;
};

#endif // PICKUPITEMSTATE_H
