#include "serializer.h"

const std::string UnwantedCharacters = " .\t\n\r\f\v";

std::string MakeOneliner(const std::string& pseudoJson)
{
  std::stringstream ss;

  bool inQuotes = false;

  for (auto& c : pseudoJson)
  {
    bool unwantedFound = std::find(UnwantedCharacters.begin(),
                                   UnwantedCharacters.end(),
                                   c) != UnwantedCharacters.end();

    if (c == '\"')
    {
      inQuotes = !inQuotes;
    }

    if (inQuotes || !unwantedFound)
    {
      ss << c;
    }
  }

  return ss.str();
}

// =============================================================================

void PrettyPrintOneliner(const std::string& oneliner)
{
  std::vector<std::string> lines;

  std::stringstream ss;

  size_t indent = 0;

  char prevChar = '\0';

  for (auto& c : oneliner)
  {
    switch(c)
    {
      case ':':
      {
        ss << " " << c << " ";
      }
      break;

      case '{':
      {
        ss << c;
        std::string line = ss.str();
        line.insert(0, indent, ' ');
        lines.push_back(line);
        ss.str(std::string());
        indent += 2;
      }
      break;

      case ',':
      {
        ss << c;
        if (prevChar == '}')
        {
          indent -= 2;
        }
        std::string line = ss.str();
        line.insert(0, indent, ' ');
        lines.push_back(line);
        ss.str(std::string());
      }
      break;

      default:
        ss << c;
        break;
    }

    prevChar = c;
  }

  for (auto& line : lines)
  {
    printf("%s\n", line.data());
  }
}

// =============================================================================

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
  /*
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
  */

  /*
  const std::string pseudoJson = R"(
Actor1 : {
  id        : 123,
  name      : "John Doe",
  inventory : item1/item2/item3,
  inner : {
    ik1 : iv1,
    ik2 : iv2,
  },
},
Actor2 : {
  id     : 456,
  name   : DummyName,
  skills : {
    sword     : 1,
    armor     : 2,
    athletics : 3,
  },
  magic : {
    air   : 0,
    earth : 1,
    fire  : 3,
    water : 0,
  },
},
)";
  */

  /*
  const std::string pseudoJson = R"(
root : {
  key   : value,
  list  : item1/item2/item3,
  inner : {
    ik1 : iv1,
    il  : i1/i2/i3,
  },
},
)";

  std::string oneliner = MakeOneliner(pseudoJson);
  printf("%s\n", oneliner.data());

  PrettyPrintOneliner(oneliner);
  */

  //
  // Root level items, simple object
  //

  /*
  NRS obj;
  obj["key1"].SetString("value1");
  obj["key2"].SetString("value2");
  obj["list"].SetString("item1", 0);
  obj["list"].SetString("item2", 1);
  obj["list"].SetString("item3", 2);
  */

  //
  // Compound simple object
  //

  /*
  NRS obj;
  obj["root"]["inner"].SetString("value");
  obj["root"]["list"].SetString("item1", 0);
  obj["root"]["list"].SetString("item2", 1);
  obj["root"]["list"].SetString("item2", 2);
  */

  //
  // Compound complex object
  //

  NRS obj;
  obj["root"]["key"].SetString("value");

  obj["root"]["list"].SetString("item1", 0);
  obj["root"]["list"].SetString("item2", 1);
  obj["root"]["list"].SetString("item3", 2);

  obj["root"]["inner"]["ik1"].SetString("iv1");

  obj["root"]["inner"]["il"].SetString("i1", 0);
  obj["root"]["inner"]["il"].SetString("i2", 1);
  obj["root"]["inner"]["il"].SetString("i,3", 2);

  std::string t = obj.ToStringObject();

  const std::string decor(80, '-');

  printf("To:\n");
  printf("%s\n", decor.data());
  printf("%s\n", t.data());
  printf("%s\n", decor.data());

  obj.FromStringObject(t);
  //obj.FromStringObject(pseudoJson);

  printf("From:\n");
  printf("%s\n", decor.data());
  printf("%s\n", obj.ToPrettyString().data());
  printf("%s\n", decor.data());

  //Dump(obj);

  return 0;
}