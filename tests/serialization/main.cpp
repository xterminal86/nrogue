#include "serializer.h"

void Dump(NRS& object)
{
  const std::string decor(80, '=');

  std::string res = object.Serialize();
  printf("%s\n", decor.data());
  printf("%s\n", res.data());
  printf("%s\n", decor.data());
}

// =============================================================================

int main(int argc, char* argv[])
{
  NRS sf;

  auto& obj = sf["object1"];
  obj["name"].SetString("Dummy Actor");
  obj["hitpoints"].SetUInt(40);

  auto& skls = obj["skills"];
  skls["sword"].SetUInt(1);
  skls["armor"].SetUInt(2);
  skls["athletics"].SetUInt(3);

  auto& inv = obj["inventory"];

  inv.SetString("item1", 0);
  inv.SetString("item2", 1);
  inv.SetString("item3", 2);
  inv.SetString("it,em", 3);

  sf["object2"]["name"].SetString("Object 2");

  //
  // Prints 2: two root objects.
  //
  printf("%lu\n", sf.ChildrenCount());

  std::string skill = sf.GetNode("object1.skills.sword").GetString();
  printf("sword = %s\n", skill.data());

  Dump(sf);

  const std::string data = R"(
GameObject
{
  name : Dummy Actor
  hitpoints : 40
  skills
  {
    sword : 1
    armor : 2
    athletics : 3
  }
  inventory : item1, item2, item3
}
)";

  NRS ds;
  ds.Deserialize(data);

  Dump(ds);

  ds.Load("serialized-object.txt");

  Dump(ds);

  //
  // Root object has only 1 element - GameObject
  //
  printf("children = %lu\n", ds.ChildrenCount());

  //
  // Total 4 - name, hitpoints, skills, inventory
  //
  printf("children = %lu\n", ds["GameObject"].ChildrenCount());

  printf("%lu\n", ds["GameObject"]["inventory"].ValuesCount());

  //ds.Write("serialized-saved.txt");

  return 0;
}
