#ifndef BTSDECOMPILER_H
#define BTSDECOMPILER_H

#include <vector>
#include <unordered_map>
#include <cstdint>
#include <string>

class BTSDecompiler
{
  public:
    std::string Decompile(const std::vector<uint8_t>& bytecode);

    void Init();

  private:
    std::unordered_map<uint8_t, std::string> _taskByOpcode;
    std::unordered_map<uint8_t, std::string> _paramByOpcode;

    const uint8_t Nop       = 0xFE;
    const uint8_t ParamsEnd = 0xFF;

    bool _initialized = false;
};

#endif // BTSDECOMPILER_H
