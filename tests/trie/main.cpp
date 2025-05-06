#include "trie.h"

#include "constants.h"

struct TestData
{
  std::string Prefix;
  UOSetS      Expected;
};

// =============================================================================

bool Test(Trie& t, const TestData& testData)
{
  printf("--- '%s' ---\n", testData.Prefix.data());

  auto res = t.FindAll(testData.Prefix);

  for (auto& item : res)
  {
    if (testData.Expected.count(item) == 0)
    {
      printf("FAIL! '%s' not found in test case!\n", item.data());
      return false;
    }
    else
    {
      printf("OK - expected has '%s'\n", item.data());
    }
  }

  for (auto& item : testData.Expected)
  {
    if (res.count(item) == 0)
    {
      printf("FAIL! expected '%s' not found in result!\n", item.data());
      return false;
    }
    else
    {
      printf("OK - '%s' is found in result\n", item.data());
    }
  }

  if (res.empty() and testData.Expected.empty())
  {
    printf("OK - nothing found\n");
  }

  return true;
}

// =============================================================================

int main(int argc, char* argv[])
{
  UOSetS dictionary =
  {
    "bar",
    "ban",
    "band",
    "bandit",
    "bark",
    "basic",
    "brand",
    "brandish",
    "ripple",
    "rover",
    "road",
    "right",
    "stop",
    "stipulation",
    "stipend"
  };

  Trie t(dictionary);

  // ---------------------------------------------------------------------------
  {
    TestData td =
    {
      "ba",
      { "bar", "ban", "band", "bandit", "bark", "basic" }
    };

    if (not Test(t, td))
    {
      return 1;
    }
  }
  // ---------------------------------------------------------------------------
  {
    TestData td =
    {
      "ban",
      { "ban", "band", "bandit" }
    };

    if (not Test(t, td))
    {
      return 1;
    }
  }
  // ---------------------------------------------------------------------------
  {
    TestData td =
    {
      "st",
      { "stop", "stipulation", "stipend" }
    };

    if (not Test(t, td))
    {
      return 1;
    }
  }
  // ---------------------------------------------------------------------------
  {
    TestData td =
    {
      "bard",
      {}
    };

    if (not Test(t, td))
    {
      return 1;
    }
  }
  // ---------------------------------------------------------------------------
  {
    TestData td =
    {
      "ri",
      { "ripple", "right" }
    };

    if (not Test(t, td))
    {
      return 1;
    }
  }
  // ---------------------------------------------------------------------------
  {
    TestData td =
    {
      "rip",
      { "ripple" }
    };

    if (not Test(t, td))
    {
      return 1;
    }
  }
  // ---------------------------------------------------------------------------
  {
    TestData td =
    {
      "",
      dictionary
    };

    if (not Test(t, td))
    {
      return 1;
    }
  }

  return 0;
}
