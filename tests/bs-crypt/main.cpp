#include <chrono>
#include <functional>

#include "rng.h"
#include "util.h"

namespace FT = std::chrono;

using Clock = FT::system_clock;

const std::string decor(80, '-');

// =============================================================================

template <typename F>
void RunAndMeasure(const std::string& textToPrint, const F& fn)
{
  printf("%s\n", textToPrint.data());

  auto tp = Clock::now();

  if (fn.target_type() != typeid(void))
  {
    fn();
  }

  auto dur = Clock::now() - tp;

  printf("It took %.2f ms\n", FT::duration<double, std::milli>(dur).count());
  printf("%s\n", decor.data());
}

// =============================================================================

int main(int argc, char* argv[])
{
  RNG::Instance().Init();

  const std::string original = "This is a simple string test";

  std::function<void()> fn = [&original]()
  {
    Util::Encrypt(original);
  };

  RunAndMeasure("Encryption of simple string...", fn);

  std::string encrypted = Util::Encrypt(original);
  std::string decrypted = Util::Encrypt(encrypted);

  printf("Original string:\n");
  printf("%s\n\n", original.data());

  printf("Encrypted:\n");

  for (size_t i = 0; i < encrypted.length(); i++)
  {
    printf(".%X.", encrypted[i]);
  }

  printf("\n\n");

  printf("Decrypted:\n");
  printf("%s\n\n", decrypted.data());

  printf("%s\n\n", (original == decrypted) ? "OK" : "FAIL!");

  const size_t bigSize = 100000000;

  printf("Trying to encrypt string of length %llu...\n\n", bigSize);

  std::string decor(80, '-');

  std::string bigString;
  bigString.resize(bigSize);

  printf("Filling string with random characters...\n");

  for (size_t i = 0; i < bigSize; i++)
  {
    //
    // Turns out my Util::Rolld100 is quite slow for some reason...
    //
    bigString[i] = char(RNG::Instance().Random() % 256);
  }

  printf("BEFORE\n");
  printf("%s\n", decor.data());

  for (size_t i = 0; i < 30; i++)
  {
    printf("%c", bigString[i]);
  }

  printf(" ...\n");

  printf("%s\n", decor.data());

  fn = [&bigString]()
  {
    Util::Encrypt(bigString);
  };

  RunAndMeasure("Encryption of big string...", fn);

  encrypted = Util::Encrypt(bigString);

  printf("AFTER\n");
  printf("%s\n", decor.data());

  for (size_t i = 0; i < 30; i++)
  {
    printf("%c", encrypted[i]);
  }

  printf(" ...\n");

  printf("%s\n", decor.data());

  decrypted = Util::Encrypt(encrypted);

  printf("DECRYPTED\n");
  printf("%s\n", decor.data());

  for (size_t i = 0; i < 30; i++)
  {
    printf("%c", decrypted[i]);
  }

  printf(" ...\n");

  printf("%s\n", decor.data());

  printf("%s\n", (bigString == decrypted) ? "OK" : "FAIL!");

  return 0;
}
