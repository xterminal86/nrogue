#ifndef BTSDECOMPILER_H
#define BTSDECOMPILER_H

#include <vector>
#include <map>
#include <cstdint>
#include <string>

#include "singleton.h"

class BTSDecompiler : public Singleton<BTSDecompiler>
{
  public:
    std::string Decompile(const std::vector<uint8_t>& bytecode);

  protected:
    void InitSpecific() override;

  private:
    std::map<uint8_t, std::string> _taskByOpcode;
    std::map<uint8_t, std::string> _paramByOpcode;

    const uint8_t Nop = 0xFE;
    const uint8_t ParamsEnd = 0xFF;
};

#endif // BTSDECOMPILER_H
