#include "bts-parser.h"

#include "util.h"
#include "game-object.h"
#include "logger.h"

void BTSParser::Init(GameObject* objRef)
{
  _goRef = objRef;

  Reset();
}

void BTSParser::Reset()
{
  _parsedData.clear();
  _rawText.clear();
  _traverseMap.clear();
  _constructionOrder.clear();

  _maxIndent = 0;
}

void BTSParser::ParseFromFile(const std::string& filename)
{
  std::ifstream f(filename);
  if (!f.is_open())
  {
    DebugLog("Can't open %s!\n", filename.data());
    return;
  }

  std::string text;

  std::string line;
  while (getline(f, line))
  {
    text += line;
    text += '\n';
  }

  f.close();

  _rawText = text;

  ParseFromString(text);

  //DebugLog("%s\n", _rawText.data());
}

void BTSParser::ParseFromString(const std::string& script)
{
  Reset();

  if (script.empty())
  {
    std::string objName = (_goRef != nullptr) ? _goRef->ObjectName : "<nullptr>";
    auto str = Util::Instance().StringFormat("%s: script on %s is empty!", __PRETTY_FUNCTION__, objName.data());
    Logger::Instance().Print(str, true);

    DebugLog("%s\n", str.data());

    return;
  }

  _rawText = script;

  std::string line;
  int indent = 0;
  bool charHit = false;
  for (auto& c : script)
  {
    if (c == ' ' && !charHit)
    {
      indent++;
    }

    if (c == '\n')
    {
      if (_maxIndent < indent)
      {
        _maxIndent = indent;
      }

      ParseLine(indent, line);
      indent = 0;
      line.clear();
      charHit = false;
    }
    else
    {
      if (c != ' ')
      {
        charHit = true;
      }

      line += c;
    }
  }
}

void BTSParser::Print()
{
  for (auto& i : _parsedData)
  {
    i.Print();
  }

  DebugLog("Max indent: %i\n", _maxIndent);
}

void BTSParser::ParseLine(int indent, const std::string& line)
{
  bool onlySpaces = (line.find_first_not_of(' ') == std::string::npos);

  if (line.empty() || onlySpaces)
  {
    return;
  }

  bool readStart = false;
  std::string tagData;
  for (auto& c : line)
  {
    if (c == '[')
    {
      readStart = true;
      continue;
    }
    else if (c == ']')
    {
      readStart = false;
      break;
    }

    if (readStart)
    {
      tagData += c;
    }
  }

  ReadTag(tagData, indent);

  //DebugLog("indent: %i, %s\n", indent, line.data());
}

void BTSParser::ReadTag(const std::string& tagData, int indent)
{
  ScriptNode n;

  n.Indent = indent;

  std::vector<std::string> res = Util::Instance().StringSplit(tagData, ' ');
  n.NodeName = res[0];

  for (size_t i = 1; i < res.size(); i++)
  {
    // Remove quotes from string. Yeah, that's how you do it in C++.
    //
    // https://en.cppreference.com/w/cpp/algorithm/remove
    //
    // ¯\_(ツ)_/¯

    res[i].erase(std::remove(res[i].begin(), res[i].end(), '"'), res[i].end());

    ReadParam(n, res[i]);
  }

  _parsedData.push_back(n);
}

void BTSParser::ReadParam(ScriptNode& nodeToFill, const std::string& paramsLine)
{
  auto res = Util::Instance().StringSplit(paramsLine, '=');
  nodeToFill.Params[res[0]] = res[1];
}

const std::vector<ScriptNode>& BTSParser::ParsedData()
{
  return _parsedData;
}

void BTSParser::FormTree(bool printDebug)
{
  if (printDebug)
  {
    DebugLog("*** FORMING TREE ***\n\n");
  }

  _traverseMap.clear();

  if (printDebug)
  {
    DebugLog("parsed nodes data:\n");
    DebugLog("------------------\n");
  }

  for (auto& i : _parsedData)
  {
    _traverseMap.emplace((ScriptNode*)&i, false);

    if (printDebug)
    {
      std::string tabs(i.Indent, '.');
      std::string addInfo = (i.Params.size() != 0 ? i.Params["p1"].data() : std::string());
      DebugLog("%s 0x%X %s (%s)\n", tabs.data(), &i, i.NodeName.data(), addInfo.data());
    }
  }

  if (printDebug)
  {
    DebugLog("------------------\n\n");
    DebugLog("tree formed:\n\n");
  }

  std::map<ScriptNode*, bool> nodesCreatedMap;

  int indent = FindMaxIndent();
  while (indent != 0)
  {
    const ScriptNode* parent = nullptr;

    for (size_t i = 0; i < _parsedData.size(); i++)
    {
      if (_traverseMap.at((ScriptNode*)&_parsedData[i]))
      {
        continue;
      }

      if (_parsedData[i].Indent == indent)
      {
        _constructionOrder.push_back({ &_parsedData[i], parent });

        if (printDebug)
        {
          std::string addInfo1 = (_parsedData[i].Params.size() != 0 ? _parsedData[i].Params["p1"].data() : std::string());
          std::string addInfo2 = (parent->Params.size() != 0 ? parent->Params.at("p1").data() : std::string());

          std::string tabs(indent, '.');
          DebugLog("%s 0x%X %s (%s) <- 0x%X %s (%s)\n",
                   tabs.data(),
                  &_parsedData[i],
                   _parsedData[i].NodeName.data(),
                   addInfo1.data(),
                   parent,
                   parent->NodeName.data(),
                   addInfo2.data());
        }

        _traverseMap.at((ScriptNode*)&_parsedData[i]) = true;
        break;
      }

      parent = &_parsedData[i];
    }

    indent = FindMaxIndent();
  }
}

int BTSParser::FindMaxIndent()
{
  int indent = 0;

  for (auto& i : _parsedData)
  {
    if (!_traverseMap.at((ScriptNode*)&i) && i.Indent > indent)
    {
      indent = i.Indent;
    }
  }

  return indent;
}

const ConstructionOrder& BTSParser::GetConstructionOrder()
{
  return _constructionOrder;
}

void ScriptNode::Print()
{
  std::string tabs(Indent, ' ');

  DebugLog("%s0x%X\n", tabs.data(), this);
  DebugLog("%sNodeName: %s\n", tabs.data(), NodeName.data());
  DebugLog("%sIndent  : %i\n", tabs.data(), Indent);
  DebugLog("%sParams:\n", tabs.data());
  DebugLog("%s{\n", tabs.data());

  #ifdef DEBUG_BUILD
  for (auto& kvp : Params)
  {
    DebugLog("%s\t%s : %s\n", tabs.data(), kvp.first.data(), kvp.second.data());
  }
  #endif

  DebugLog("%s}\n", tabs.data());
}
