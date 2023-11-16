#include "util.h"
#include "rng.h"

int main(int argc, char* argv[])
{
  if (argc < 2)
  {
    printf("Usage: %s <N> [<ALLOW_DOUBLE_VOWELS>]\n", argv[0]);
    printf("<N>                   - number of \"names\" to generate\n");
    printf("<ALLOW_DOUBLE_VOWELS> - any value\n");
    return 0;
  }

  RNG::Instance().Init();

  //RNG::Instance().SetSeed(1698773628612500435);

  printf("Seed = %llu\n", RNG::Instance().Seed);

  bool allowDoubleVowels = (argc > 2);

  size_t namesCount = std::stoull(argv[1]);

  if (namesCount == 0)
  {
    printf("<N> = 0\n");
    return 0;
  }

  size_t namesCountTmp = namesCount;
  size_t digits = 0;

  while (namesCountTmp != 0)
  {
    namesCountTmp /= 10;
    digits++;
  }

  std::stringstream ss;

  ss << "%" << digits << "lu. %s\n";

  for (size_t i = 0; i < namesCount; i++)
  {
    std::string name = Util::GenerateName(allowDoubleVowels, false);
    printf(ss.str().data(), i + 1, name.data());
  }

  return 0;
}
