#include <unordered_map>
#include <cstdint>
#include <cstdio>
#include <unordered_set>

#include "position.h"

const char* kOk     = "OK";
const char* kFailed = "FAILED!";

int main(int argc, char* argv[])
{
  std::unordered_map<Position, Position> test;

  int32_t xRange = 40;
  int32_t yRange = 12;

  size_t total = (xRange + xRange) * (yRange + yRange);

  for (int32_t x = -xRange; x < xRange; x++)
  {
    for (int32_t y = -yRange; y < yRange; y++)
    {
      Position p(x, y);
      test[p] = p;
    }
  }

  printf("Test map size %lu == %lu - %s\n",
         test.size(),
         total,
         (test.size() == total) ? kOk : kFailed);

  bool itemsOk = true;

  for (int32_t x = -xRange; x < xRange; x++)
  {
    for (int32_t y = -yRange; y < yRange; y++)
    {
      Position p(x, y);
      if (test.count(p) != 1)
      {
        printf("Item <%d.%d> was not found!\n", x, y);
        itemsOk = false;
        goto exit;
      }

      Position& item = test[p];
      if (item.X != x or item.Y != y)
      {
        printf("Value mismatch: actual <%d.%d>, expected <%d.%d>\n",
               item.X, item.Y, x, y);
        itemsOk = false;
        goto exit;
      }
    }
  }

exit:

  printf("Items check %s\n", (itemsOk ? kOk : kFailed));

  return 0;
}
