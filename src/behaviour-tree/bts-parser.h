#ifndef BTSPARSER_H
#define BTSPARSER_H

#include <string>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <algorithm>

#ifdef __MINGW64__
#include <stdint.h>
#endif

struct ScriptNode
{
  int Indent;
  std::string NodeName;
  std::unordered_map<std::string, std::string> Params;

  void Print();
};

class GameObject;

using ConstructionOrder = std::vector<std::pair<const ScriptNode*, const ScriptNode*>>;

class BTSParser
{
  public:
    void Init(GameObject* objRef = nullptr);
    void Reset();
    void ParseFromString(const std::string& script);
    void ParseFromFile(const std::string& filename);
    void Print();

    const std::vector<ScriptNode>& ParsedData();

    void FormTree(bool printDebug = false);

    const ConstructionOrder& GetConstructionOrder();

  private:
    void ParseLine(int indent, const std::string& line);
    void ReadTag(const std::string& tagData, int indent);
    void ReadParam(ScriptNode& nodeToFill, const std::string& paramsLine);

    int FindMaxIndent();

    std::unordered_map<ScriptNode*, bool> _traverseMap;

    std::vector<ScriptNode> _parsedData;
    std::string _rawText;

    int _maxIndent = 0;

    ConstructionOrder _constructionOrder;

    GameObject* _goRef;

    std::unordered_map<std::string, uint8_t> _paramCodesByName;
};

#endif // BTSPARSER_H
