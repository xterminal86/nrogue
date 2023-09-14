#ifndef STRINGOBFUSCATOR_H
#define STRINGOBFUSCATOR_H

//
// Based on the obfuscator class by Adam Yaxley.
//
// https://github.com/adamyaxley/Obfuscate
//
namespace StringObfuscator
{
  //
  // Poorly chosen key will affect the "quality" of obfuscation, so to speak.
  // In author's version he uses some kind of MurmurHash3 magic,
  // but I'll just use some bullshit value.
  //
  constexpr uint64_t Key = 0xCAFEBABEDEADBEEF;

  constexpr void Cipher(char* data, size_t size, uint64_t key)
  {
    for (size_t i = 0; i < size; i++)
    {
      data[i] ^= char(key >> ((i % 8) * 8));
    }
  }

  // ---------------------------------------------------------------------------

  template <size_t N>
  class Obfuscator
  {
    public:
      //
      // constexpr constructor constructs object at compile time, if it can.
      //
      constexpr Obfuscator(const char* data)
      {
        for (size_t i = 0; i < N; i++)
        {
          _data[i] = data[i];
        }

        Cipher(_data, N, Key);
      }

      //
      // Type conversion overload.
      //
      operator const char*()
      {
        if (_encrypted)
        {
          Cipher(_data, N, Key);
          _encrypted = false;
        }

        return _data;
      }

    private:
      //
      // Must use {} here, or won't compile.
      //
      char _data[N]{};

      bool _encrypted = true;
  };
}

// =============================================================================

//
// It seems this lambda is implicitly constexpr. At least such thing is possible
// if lambda satisfies constexprness (since C++17).
//
#define HIDE(cStyleString)                                                              \
  []() -> StringObfuscator::Obfuscator<sizeof(cStyleString) / sizeof(cStyleString[0])>& \
  {                                                                                     \
    constexpr auto n = sizeof(cStyleString) / sizeof(cStyleString[0]);                  \
    static auto obfuscator = StringObfuscator::Obfuscator<n>(cStyleString);             \
    return obfuscator;                                                                  \
  }()

#endif // STRINGOBFUSCATOR_H
