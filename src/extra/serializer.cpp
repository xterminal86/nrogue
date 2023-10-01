#include "serializer.h"

void NRS::SetString(const std::string& s, size_t index)
{
  if (_content.size() <= index)
  {
    _content.resize(index + 1);
  }

  _content[index] = s;
}

// =============================================================================

const std::string& NRS::GetString(size_t index) const
{
  if (index >= _content.size())
  {
    return kEmptyString;
  }

  return _content[index];
}

// =============================================================================

void NRS::SetInt(int64_t value, size_t index)
{
  SetString(std::to_string(value), index);
}

// =============================================================================

int64_t NRS::GetInt(size_t index) const
{
  return std::stoll(GetString(index).data());
}

// =============================================================================

void NRS::SetUInt(uint64_t value, size_t index)
{
  SetString(std::to_string(value), index);
}

// =============================================================================

uint64_t NRS::GetUInt(size_t index) const
{
  return std::stoull(GetString(index).data());
}

// =============================================================================

void NRS::Clear()
{
  _content.clear();
  _children.clear();
  _childIndexByName.clear();
  _currentIndent = 0;
}

// =============================================================================

size_t NRS::ValuesCount() const
{
  return _content.size();
}

// =============================================================================

size_t NRS::ChildrenCount() const
{
  return _children.size();
}

// =============================================================================

NRS& NRS::operator[](const std::string& nodeName)
{
  if (_childIndexByName.count(nodeName) == 0)
  {
    _childIndexByName[nodeName] = _children.size();

    _children.push_back({ nodeName, NRS() });
  }

  return _children[_childIndexByName[nodeName]].second;
}

// =============================================================================

NRS& NRS::GetNode(const std::string& path)
{
  size_t pos = path.find_first_of('.');
  if (pos != std::string::npos)
  {
    std::string nodeName = path.substr(0, pos);
    if (_childIndexByName.count(nodeName) == 1)
    {
      return (*this)[nodeName].GetNode(path.substr(pos + 1, path.length()));
    }
    else
    {
      return (*this)[nodeName];
    }
  }
  else
  {
    return (*this)[path];
  }
}

// =============================================================================

std::string NRS::Serialize()
{
  _currentIndent = 0;
  std::stringstream ss;
  WriteIntl(*this, ss);
  return ss.str();
}

// =============================================================================

bool NRS::Save(const std::string& fileName)
{
  std::ofstream file(fileName);
  if (file.is_open())
  {
    std::string serialized = Serialize();
    file << serialized;
    file.close();
    return true;
  }

  return false;
}

// =============================================================================

void NRS::WriteIntl(const NRS& d, std::stringstream& ss)
{
  for (auto& item : d._children)
  {
    if (item.second._children.empty())
    {
      ss << IndentString(_indentMark, _currentIndent)
         << item.first << _kvSeparatorMark;

      size_t nItems = item.second.ValuesCount();
      for (size_t i = 0; i < nItems; i++)
      {
        bool itemsLeft = ((nItems - i) > 1);

        size_t x = item.second.GetString(i).find_first_of(_separatorCh);
        if (x != std::string::npos)
        {
          ss << "\"" << item.second.GetString(i) << "\""
            << (itemsLeft ? _separator : "");
        }
        else
        {
          ss << item.second.GetString(i)
            << (itemsLeft ? _separator : "");
        }
      }

      ss << "\n";
    }
    else
    {
      ss << IndentString(_indentMark, _currentIndent) << item.first << "\n";
      ss << IndentString(_indentMark, _currentIndent) << "{\n";

      _currentIndent++;

      WriteIntl(item.second, ss);

      ss << IndentString(_indentMark, _currentIndent) << "}\n";
    }
  }

  if (_currentIndent > 0)
  {
    _currentIndent--;
  }
}

// =============================================================================

void NRS::Deserialize(const std::string& data)
{
  //
  // Just in case.
  //
  Clear();

  std::string key;
  std::string value;

  std::stack<NRS*> tree;
  tree.push(this);

  std::istringstream iss(data);

  std::string line;
  while (std::getline(iss, line))
  {
    //
    // Remove all unwanted characters like whitespaces, tabs and stuff like that
    // from the beginning and the end of a line. We'll be using this frequently.
    //
    TrimString(line);

    if (!line.empty())
    {
      //
      // Search for key/value separator, ':' in our case.
      //
      size_t pos = line.find_first_of(_kvSeparatorCh);
      if (pos != std::string::npos)
      {
        //
        // If it's found, grab all that is before it as key.
        //
        key = line.substr(0, pos);
        TrimString(key);

        //
        // Grab the other part as value.
        //
        value = line.substr(pos + 1, line.length());
        TrimString(value);

        //
        // Now we need to check if value is a list of items.
        // Items itself may be quoted, so check that too.
        //
        bool inQuotes = false;
        std::string token;
        size_t tokenCount = 0;
        for (auto& c : value)
        {
          if (c == '\"')
          {
            inQuotes = !inQuotes;
          }
          else
          {
            if (inQuotes)
            {
              token.append(1, c);
            }
            else
            {
              if (c == _separatorCh)
              {
                TrimString(token);
                (*tree.top())[key].SetString(token, tokenCount);
                token.clear();
                tokenCount++;
              }
              else
              {
                token.append(1, c);
              }
            }
          }
        }

        if (!token.empty())
        {
          TrimString(token);
          (*tree.top())[key].SetString(token, tokenCount);
        }
      }
      //
      // Key/value separator was not found.
      // Check if this line is an indentation block, otherwise it is a new
      // object name which must be followed by open brace, so save it in key
      // variable, that will be pushed onto the stack when we hit said open
      // brace.
      //
      else
      {
        if (line[0] == '{')
        {
          tree.push(&(*tree.top())[key]);
        }
        else
        {
          if (line[0] == '}')
          {
            tree.pop();
          }
          else
          {
            key = line;
          }
        }
      }
    }
  }
}

// =============================================================================

bool NRS::Load(const std::string& fname)
{
  std::ifstream file(fname);
  if (file.is_open())
  {
    std::stringstream ss;
    while (!file.eof())
    {
      std::string line;
      std::getline(file, line);
      ss << line << "\n";
    }

    Deserialize(ss.str());

    return true;
  }

  return false;
}

// =============================================================================

std::string NRS::IndentString(const std::string& indentMark, size_t n)
{
  std::string res;
  for (size_t i = 0; i < n; i++)
  {
    res += indentMark;
  }

  return res;
}

// =============================================================================

void NRS::TrimString(std::string& str)
{
  str.erase(0, str.find_first_not_of(_trimCharacters.data()));
  str.erase(str.find_last_not_of(_trimCharacters.data()) + 1);
}
