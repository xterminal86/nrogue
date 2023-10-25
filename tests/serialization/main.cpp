#include "serializer.h"

#include "util.h"

std::string MakeOneliner(const std::string& stringObject)
{
  const std::string _unwantedCharacters = " \t\n\r\f\v";

  std::stringstream ss;

  bool inQuotes = false;

  for (auto& c : stringObject)
  {
    bool unwantedFound = std::find(_unwantedCharacters.begin(),
                                   _unwantedCharacters.end(),
                                   c) != _unwantedCharacters.end();

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

std::string GetBanner(const std::string& title)
{
  const int maxWidth = 80;

  std::stringstream ss;

  std::string decor(maxWidth, '*');
  int startPos = (maxWidth / 2) - (title.length() / 2);

  std::string titleLine(maxWidth, ' ');
  std::string fillerLine(maxWidth, ' ');

  titleLine[0] = '*';
  titleLine[maxWidth - 1] = '*';

  fillerLine[0] = '*';
  fillerLine[maxWidth - 1] = '*';

  for (size_t i = 0; i < title.length(); i++)
  {
    titleLine[startPos + i] = title[i];
  }

  ss << decor      << "\n"
     << fillerLine << "\n"
     << titleLine  << "\n"
     << fillerLine << "\n"
     << decor      << "\n";

  return ss.str();
}

// =============================================================================

#define PRINT_BANNER()                        \
  do {                                        \
    std::string banner = GetBanner(__func__); \
    printf("%s\n", banner.data());            \
  } while (false)

// =============================================================================

void TestSimple()
{
  PRINT_BANNER();

  const std::string decor(80, '-');

  const std::string pseudoJson = R"(
key1 : value1,
key2 : value2,
list : item1/item2/"it,e/m3",
)";

  NRS dso;
  dso.FromStringObject(pseudoJson);

  NRS obj;
  obj["key1"].SetString("value1");
  obj["key2"].SetString("value2");
  obj["list"].SetString("item1",   0);
  obj["list"].SetString("item2",   1);
  obj["list"].SetString("it,e/m3", 2);

  std::string sos = obj.ToStringObject();

  printf("%s\n", decor.data());
  printf("%s\n", sos.data());
  printf("%s\n", decor.data());

  std::string dsos = dso.ToStringObject();
  printf("%s\n", decor.data());
  printf("%s\n", dsos.data());
  printf("%s\n", decor.data());

  printf("%s\n", (sos == dsos) ? "OK" : "FAIL!");

  printf("%s\n", dso.DumpObjectStructureToString().data());
  printf("%s\n", obj.DumpObjectStructureToString().data());

  printf("\n");
}

// =============================================================================

void TestComplex()
{
  PRINT_BANNER();

  const std::string decor(80, '-');

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

  NRS dso;
  dso.FromStringObject(pseudoJson);

  NRS obj;

  // Actor1
  obj.GetNode("Actor1.id").SetString("123");
  obj.GetNode("Actor1.name").SetString("John Doe");

  obj.GetNode("Actor1.inventory").SetString("item1", 0);
  obj.GetNode("Actor1.inventory").SetString("item2", 1);
  obj.GetNode("Actor1.inventory").SetString("item3", 2);

  obj.GetNode("Actor1.inner.ik1").SetString("iv1");
  obj.GetNode("Actor1.inner.ik2").SetString("iv2");

  // Actor2

  obj.GetNode("Actor2.id").SetString("456");
  obj.GetNode("Actor2.name").SetString("DummyName");

  obj.GetNode("Actor2.skills.sword").SetString("1");
  obj.GetNode("Actor2.skills.armor").SetString("2");
  obj.GetNode("Actor2.skills.athletics").SetString("3");

  obj.GetNode("Actor2.magic.air").SetString("0");
  obj.GetNode("Actor2.magic.earth").SetString("1");
  obj.GetNode("Actor2.magic.fire").SetString("3");
  obj.GetNode("Actor2.magic.water").SetString("0");

  std::string sos = obj.ToStringObject();

  printf("%s\n", decor.data());
  printf("%s\n", sos.data());
  printf("%s\n", decor.data());

  std::string dsos = dso.ToStringObject();
  printf("%s\n", decor.data());
  printf("%s\n", dsos.data());
  printf("%s\n", decor.data());

  printf("%s\n", (sos == dsos) ? "OK" : "FAIL!");

  printf("%s\n", dso.DumpObjectStructureToString().data());
  printf("%s\n", obj.DumpObjectStructureToString().data());

  printf("\n");
}

// =============================================================================

void WithFile()
{
  PRINT_BANNER();

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

  const std::string fname = "serialized-to.txt";

  NRS dso;
  dso.FromStringObject(pseudoJson);

  dso.Save(fname);

  NRS obj;
  obj.Load(fname);

  std::string ol1 = dso.ToStringObject();
  std::string ol2 = obj.ToStringObject();

  printf("%s\n", (ol1 == ol2) ? "OK" : "FAIL!");

  printf("\n");
}

// =============================================================================

void Encrypt()
{
  PRINT_BANNER();

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

  const std::string decor(80, '-');

  NRS dso;
  dso.FromStringObject(pseudoJson);

  std::string sos = dso.ToStringObject();

  printf("%s\n", decor.data());

  size_t bytesCount = 0;
  for (auto& c : sos)
  {
    printf(".0x%02X.", (uint8_t)c);
    bytesCount++;

    if (bytesCount == 8)
    {
      printf(" ");
    }
    else if (bytesCount == 16)
    {
      printf("\n");
      bytesCount = 0;
    }
  }

  printf("\n");

  printf("%s\n", decor.data());

  std::string es = Util::Encrypt(sos);
  std::string ds = Util::Encrypt(es);

  printf("%s\n", decor.data());

  bytesCount = 0;
  for (auto& c : es)
  {
    printf(".0x%02X.", (uint8_t)c);

    bytesCount++;

    if (bytesCount == 8)
    {
      printf(" ");
    }
    else if (bytesCount == 16)
    {
      printf("\n");
      bytesCount = 0;
    }
  }

  printf("\n");

  printf("%s\n", decor.data());

  printf("%s\n", (sos == ds) ? "OK" : "FAIL!");

  printf("\n");
}

// =============================================================================

void NewConfig()
{
  PRINT_BANNER();

  std::string cfg = R"(
tileset : "resources/std-8x16-ck.bmp",
tile_w : 8,
tile_h : 16,
scale : 2.0,
fast_combat : 0,
fast_monster_movement : 0,
)";

  NRS config;
  config.FromStringObject(cfg);

  printf("%s\n", config.ToPrettyString().data());
  printf("%s\n", config.DumpObjectStructureToString().data());
}

// =============================================================================

void CheckSyntax()
{
  PRINT_BANNER();

  std::vector<std::pair<std::string, bool>> testCases =
  {
    // ------------------------------- gud -------------------------------------
    {
    R"(
key  : "value 1",
)",
    true
    },
    {
  R"(
key  : "value 1",
list : item1/item2/item3,
data : test,
)",
    true
    },
    {
  R"(
key  : "value 1",
obj : {
  in1 : v1,
  in2 : v2,
},
)",
    true
    },
    {
  R"(
key  : "value 1",
key2 : value2,
list : item1/item2/item3,
)",
    true
    },
    {
  R"(
key  : "value 1",
key2 : value2,
list : item1/"it/e,m2"/item3,
)",
    true
    },
    {
  R"(
obj : {
  innerObj : {
    ik1 : iv1,
    lst : item1/item2/"it,e/m 3",
    ik2 : iv2,
  },
  inner2 : huj,
},
outer : val,
)",
    true
    },
    {
  R"(
obj : {
  innerObj : {
    ik1 : iv1,
    lst : item1/item2/"it,e/m 3",
    ik2 : iv2,
  },
  inner2 : huj,
},
)",
    true
    },
    {
  R"(
obj : {
  innerObj : {
    ik1 : iv1,
    lst : item1/item2/"it,e/m 3",
    inner3 : {
      someKey : someValue,
    },
    inner4 : {
      inKey : inValue,
      inLst : i1/i2,
    },
  },
},
)",
    true
    },
    {
  R"(
key  : "value 1",
list : item1/item2/item3,
obj : {
  k1 : v1,
  lst : i1/i2,
  io : {
    k : v,
    l : i1/i2/i3,
  },
  item : data,
},
)",
    true
    },
    // ------------------------------- bad -------------------------------------
    {
  R"(
l{f : lol,
)",
    false
    },
    {
  R"(
lf : l"ol,
)",
    false
    },
    {
  R"(
lf : lol, {
)",
    false
    },
    {
  R"(
root : {
  key : "test
},
)",
    false
    },
    {
  R"(
root : {
  key : "test"
},
)",
    false
    },
    {
  R"(
root : {{
  key : "test",
},
)",
    false
    },
    {
  R"(
root : {
  key : value,
},
b
)",
    false
    },
    {
  R"(
root : {
  key : value,
  lst : one//
},
)",
    false
    },
    {
  R"(
root : {
  key : value,
  lst : one/two/three
},
)",
    false
    },
    {
  R"(
root : {
  key : value,
  lst : one/two/three,
}
)",
    false
    },
    {
  R"(
root :{:
)",
    false
    },
    {
  R"(
root : {
  : value,
},
)",
    false
    },
    {
  R"(
root : {
  key : value,
  {
    a : b,
  },
},
)",
    false
    },
    {
  R"(
root : {
  key : value,
  : {
    a : b,
  },
},
)",
    false
    },
    {
  R"(
root : {
  key : value,
)",
    false
    },
};

  const std::string decor(80, '-');

  std::vector<int> failedTests;

  int testNum = 0;

  for (auto& [str, good] : testCases)
  {
    NRS obj;
    std::string oneliner = MakeOneliner(str);
    printf("%s\n", decor.data());
    printf("%s\n", oneliner.data());
    printf("is good? : %s\n", good ? "yes" : "no");
    bool ok = obj.CheckSyntax(str);

    if (ok)
    {
      obj.FromStringObject(str);
      printf("%s\n", obj.DumpObjectStructureToString().data());
    }

    if (ok != good)
    {
      failedTests.push_back(testNum);
    }

    printf("%s\n", (ok == good) ? "PASSED" : "FAILED!");
    printf("%s\n", decor.data());

    testNum++;
  }

  if (!failedTests.empty())
  {
    printf("Some tests failed:\n");

    for (auto& i : failedTests)
    {
      printf("no. %d\n", i);
    }
  }
  else
  {
    printf("All tests passed!\n\n");
  }
}

// =============================================================================

void SaveGameTest()
{
  PRINT_BANNER();

  std::string data = R"(
map : {
  0  : "#####################",
  1  : "#...................#",
  2  : "#...................#",
  3  : "#...................#",
  4  : "#...................#",
  5  : "#...................#",
  6  : "#...................#",
  7  : "#...................#",
  8  : "#...................#",
  9  : "#...................#",
  10 : "#...................#",
  11 : "#...................#",
  12 : "#...................#",
  13 : "#...................#",
  14 : "#...................#",
  15 : "#...................#",
  16 : "#...................#",
  17 : "#...................#",
  18 : "#...................#",
  19 : "#...................#",
  20 : "#...................#",
  21 : "#####################",
},
)";

  NRS map;
  map.FromStringObject(data);
  std::string dump = map.DumpObjectStructureToString();
  printf("%s\n", dump.data());
  printf("%s\n", map.ToStringObject().data());
}

// =============================================================================

int main(int argc, char* argv[])
{
  //TestSimple();
  //TestComplex();
  //WithFile();
  //Encrypt();
  //NewConfig();
  //CheckSyntax();
  SaveGameTest();

  return 0;
}
