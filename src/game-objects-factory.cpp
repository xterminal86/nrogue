#include "game-objects-factory.h"

#include "constants.h"
#include "rng.h"

void GameObjectsFactory::Init()
{
}

GameObject* GameObjectsFactory::CreateGameObject()
{
  GameObject* go = new GameObject();

  int length = RNG::Instance().Random() % GlobalConstants::Alphabet.length() + 1;

  std::string name;
  for (int i = 0; i < length; i++)
  {
    int index = RNG::Instance().Random() % GlobalConstants::Alphabet.length();
    char ch = GlobalConstants::Alphabet[index];
    name += ch;
  }

  go->ObjectName = name;

  return go;
}
