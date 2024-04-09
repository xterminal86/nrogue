#ifndef STRINGOBFUSCATOR_H
#define STRINGOBFUSCATOR_H

#ifdef MSVC_COMPILER
#include <cstdint>
#endif

//
// Based on the obfuscator class by Adam Yaxley.
//
// https://github.com/adamyaxley/Obfuscate
//
namespace StringObfuscator
{
  //
  // Poorly chosen key will affect the "quality" of obfuscation, so to speak.
  // In author's version he uses some kind of MurmurHash3 magic, I don't know
  // what that is so I'll just use some random bullshit value.
  //
  const uint64_t Key = 0xCAFEBABEDEADBEEF;

  constexpr void Cipher(char* data, size_t size, uint64_t key)
  {
    for (size_t i = 0; i < size; i++)
    {
      data[i] ^= char(key >> ((i % 8) * 8));
    }
  }

  // ---------------------------------------------------------------------------

  //
  // So called "non-type template parameter", N gets substituted directly as a
  // compile time value of specified type (size_t in this case).
  //
  template <size_t N>
  class Obfuscator
  {
    public:
      //
      // constexpr constructor constructs object at compile time, if it can.
      //
      explicit constexpr Obfuscator(const char* data)
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
      // Initialize array elements to zero.
      //
      char _data[N]{};

      bool _encrypted = true;
  };
}

// =============================================================================

//
// This lambda will be executed at runtime.
// So it looks like during compilation it will construct Obfuscator object,
// which will encrypt the string in constructor, but at runtime it will
// be decrypted via type conversion operator overload.
// When assigning to std::string usually you have to embrace (lol)
// the macro call with curly braces, but sometimes you don't.
// When you're assigning to class member variable in .cpp file,
// while member variable was declared in .h file, you have to drop curly braces.
// Maybe compiler can't deduce the type or something, I don't fucking know.
//
#define HIDE(cStyleString)                                                              \
  []() -> StringObfuscator::Obfuscator<sizeof(cStyleString) / sizeof(cStyleString[0])>& \
  {                                                                                     \
    constexpr size_t n = sizeof(cStyleString) / sizeof(cStyleString[0]);                \
    static auto obfuscator = StringObfuscator::Obfuscator<n>(cStyleString);             \
    return obfuscator;                                                                  \
  }()

#endif // STRINGOBFUSCATOR_H
