#ifndef LOOTGENERATORS_H
#define LOOTGENERATORS_H

class GameObject;

namespace LootGenerators
{
  void DropLoot(GameObject* go);
  // ---------------------------------------
  void Rat(GameObject* go);
  void MadMiner(GameObject* go);
  void Shelob(GameObject* go);
  void Kobold(GameObject* go);
};

#endif // LOOTGENERATORS_H
