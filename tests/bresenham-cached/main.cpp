#include "bresenham-cached.h"

int main(int argc, char* argv[])
{
  BresenhamCached bc(40, 12);
  printf("%s", bc.GetStats().data());

  auto TestLine = [&bc](const Position& from, const Position& to)
  {
    static const std::string ruler(80, '-');

    printf("%s\n", ruler.data());
    printf("Testing: %s -> %s\n", from.ToString().data(), to.ToString().data());

    auto bresenhamLine = Util::BresenhamLine(from, to);

    /*
    printf("Util::BresenhamLine()\n");

    for (auto& i : bresenhamLine)
    {
      printf("%s ", i.ToString().data());
    }

    printf("\n");
    */

    Position truePos;

    const PositionV& cachedLine = bc.GetLineOffsets(from, to);

    /*
    printf("BresenhamCached::GetLine()\n");

    for (const Position& p : cachedLine)
    {
      truePos.X = from.X + p.X;
      truePos.Y = from.Y + p.Y;

      printf("%s ", truePos.ToString().data());
    }

    printf("\n");
    */

    bool ok = true;

    if (cachedLine.size() != bresenhamLine.size())
    {
      printf("[FAIL] size mismatch: %lu != %lu\n",
             cachedLine.size(),
             bresenhamLine.size());
      return;
    }

    for (size_t i = 0; i < cachedLine.size(); i++)
    {
      truePos.X = from.X + cachedLine[i].X;
      truePos.Y = from.Y + cachedLine[i].Y;

      if (truePos != bresenhamLine[i])
      {
        printf("%s != %s\n",
               truePos.ToString().data(),
               bresenhamLine[i].ToString().data());
        ok = false;
      }
    }

    if (ok)
    {
      for (const Position& p : cachedLine)
      {
        truePos.X = from.X + p.X;
        truePos.Y = from.Y + p.Y;

        printf("%s ", truePos.ToString().data());
      }

      printf("\n");

      printf("[OK]\n");
    }
  };

  TestLine({   5,   5 }, { -1,  2 });
  TestLine({   0,   0 }, {  0,  0 });
  TestLine({   0,   0 }, { 10,  5 });
  TestLine({  -1,  -3 }, { -5, -7 });
  TestLine({  -5,  -7 }, { -1, -3 });
  TestLine({ -50, -70 }, { -5, -3 });
  TestLine({   0,   0 }, { 45, 67 });
  TestLine({  45,  67 }, {  0,  0 });
  TestLine({ -10, -10 }, { 10, 10 });
  TestLine({   0,   0 }, { 10,  1 });
  TestLine({  10,   1 }, {  0,  0 });
  TestLine({  10,   1 }, {  0,  1 });
  TestLine({  10,   1 }, { 10,  2 });

  Position from(5, 3);
  Position to(20, -5);

  printf("\n");
  printf("True positions %s -> %s\n",
         from.ToString().data(),
         to.ToString().data());

  const PositionV& truePos = bc.GetLine(from, to);
  for (const Position& p : truePos)
  {
    printf("%s ", p.ToString().data());
  }

  printf("\n");

  return 0;
}
