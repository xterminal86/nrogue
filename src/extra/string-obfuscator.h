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
#if 0
#define HIDE(cStyleString)                                                              \
  []() -> StringObfuscator::Obfuscator<sizeof(cStyleString) / sizeof(cStyleString[0])>& \
  {                                                                                     \
    constexpr size_t n = sizeof(cStyleString) / sizeof(cStyleString[0]);                \
    static auto obfuscator = StringObfuscator::Obfuscator<n>(cStyleString);             \
    return obfuscator;                                                                  \
  }()
#endif

// =============================================================================

//
// Hide characters inside uint64_t bytes.
// Hopefully this will help us get rid of compiler warnings in MSVC.
//
namespace StringConcealer
{
  template <size_t StringLength, size_t IntlLength>
  class Concealer
  {
    public:
      //
      // Without constexpr ctor strings can be found inside binary executable.
      //
      constexpr Concealer(const char(&string)[StringLength])
      {
        size_t dataIndex = 0;
        uint8_t maskIndex = 0;
        for (size_t i = 0; i < StringLength; i++)
        {
          _data[dataIndex] |= ((uint64_t)string[i] << ((7 - maskIndex) * 8));

          maskIndex++;

          if (maskIndex == 8)
          {
            dataIndex++;
            maskIndex = 0;
          }
        }
      }

      std::string ToString()
      {
        std::string res;

        for (size_t i = 0; i < IntlLength; i++)
        {
          uint64_t v = _data[i];

          for (uint8_t i = 0; i < 8; i++)
          {
            char c = char(((v & 0xFF00000000000000) >> 56));

            if (c == 0)
            {
              return res;
            }

            res.append(1, c);

            v <<= 8;
          }
        }

        return res;
      }

      uint64_t* Data()
      {
        return _data;
      }

    private:
      uint64_t _data[IntlLength]{};
  };
}

// "As of C++17, lambdas are permitted in constant expressions."
// Which is what we're exploiting here using anonymous lambda.
// What this does is every time we use this macro we're creating a compile time
// object, which will then be "directly inserted" into resulting
// assembly as if an object has been prepared beforehand with method ToString
// called on it.
// You can paste this whole code into CompilerExplorer and see something like
// this:
//
// -----------------------------------------------------------------------------
// main::{lambda()#2}::operator()() const:
// push    rbp
// mov     rbp, rsp
// sub     rsp, 32
// mov     QWORD PTR [rbp-24], rdi
// mov     QWORD PTR [rbp-32], rsi
// mov     QWORD PTR [rbp-8], 8
// mov     QWORD PTR [rbp-16], 3
// mov     rax, QWORD PTR [rbp-24]
// mov     esi, OFFSET FLAT:main::{lambda()#2}::operator()() const::c
// mov     rdi, rax
// call    Concealer<8ul, 3ul>::ToString[abi:cxx11]()
// mov     rax, QWORD PTR [rbp-24]
// leave
// ret
// -----------------------------------------------------------------------------
//
// Notice the resulting Concealer<8, 3>::ToString() at the end (in this case).
//
// So TLDR - compiler compiles everything that is constexpr at compile time and
// then puts call to ToString() to be executed at runtime.
//

#define HIDE(string)                                            \
  []()                                                          \
  {                                                             \
    constexpr auto ln = (sizeof(string) == 0)                   \
    ? 0                                                         \
    : (sizeof(string) / sizeof(string[0]));                     \
    constexpr auto lt = ((ln / 8) == 0 ? 1 : (ln / 8)) + 1;     \
    static auto c = StringConcealer::Concealer<ln, lt>(string); \
    return c.ToString();                                        \
  }()

#endif // STRINGOBFUSCATOR_H
