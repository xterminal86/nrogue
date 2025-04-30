#include "bts-decompiler.h"

#include "constants.h"
#include "util.h"

void BTSDecompiler::InitSpecific()
{
  std::unordered_map<std::string, uint8_t> opcodesByName;
  std::unordered_map<std::string, uint8_t> taskParamsByName;

  for (auto& kvp : GlobalConstants::BTSTaskNamesByName)
  {
    uint8_t opcode = static_cast<uint8_t>(kvp.second);
    opcodesByName.insert({ kvp.first, opcode });
  }

  const int maxNumParams = static_cast<uint8_t>(ScriptParamNames::IDLE) - 1;

  for (int i = 0; i <= maxNumParams; i++)
  {
    std::string paramStr = std::to_string(i);
    taskParamsByName[paramStr] = static_cast<uint8_t>(i);
  }

  for (auto& kvp : GlobalConstants::BTSParamNamesByName)
  {
    uint8_t opcode = static_cast<uint8_t>(kvp.second);
    taskParamsByName.insert({ kvp.first, opcode });
  }

  _taskByOpcode  = Util::FlipMap(opcodesByName);
  _paramByOpcode = Util::FlipMap(taskParamsByName);
}

// =============================================================================

std::string BTSDecompiler::Decompile(const std::vector<uint8_t>& bytecode)
{
  std::string script;

  size_t index = 0;
  uint8_t indent = 0;
  uint8_t opcode = 0;
  int paramCount = 1;

  while (index != bytecode.size())
  {
    indent = bytecode[index];
    opcode = bytecode[index + 1];

    std::string taskNameVal;
    std::string taskName;
    std::string strIndent;

    if (_taskByOpcode.count(opcode) == 1)
    {
      taskNameVal = _taskByOpcode[opcode];
    }

    taskName = "";

    if (taskNameVal.empty())
    {
      DebugLog("\tIllegal opcode: %u", opcode);
      opcode = Nop;
    }
    else
    {
      taskName = taskNameVal;
      strIndent = std::string(indent, ' ');
      script += Util::StringFormat("%s[%s", strIndent.data(), taskName.data());
    }

    index += 2;

    if (opcode == Nop)
    {
      continue;
    }

    if (taskName == "COND" || taskName == "TASK")
    {
      paramCount = 1;
      uint8_t curByte = bytecode[index];

      while (curByte != ParamsEnd)
      {
        std::string paramType = _paramByOpcode[curByte];
        script += Util::StringFormat(" p%i=\"%s\"",
                                     paramCount,
                                     paramType.data());
        paramCount++;
        index++;
        curByte = bytecode[index];
      }

      index++;
    }

    script += "]\n";
  }

  return script;
}
