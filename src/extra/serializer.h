#ifndef SERIALIZER_H
#define SERIALIZER_H

#include <cstdint>
#include <vector>
#include <string>
#include <functional>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <stack>
#include <set>

//
// Based on savefile class courtesy of OneLoneCoder video:
//
// https://www.youtube.com/watch?v=jlS1Y2-yKV0
//
// Actual class is a part of his custom game engine:
//
// https://github.com/OneLoneCoder/olcPixelGameEngine/blob/master/utilities/olcUTIL_DataFile.h
//
// I modified the author's implementation a little bit to suit my personal code
// style by doing .h/.cpp splitting and removing of static methods. Also I made
// the format a little more JSON-like. Thus it can be made into oneliner
// and we can save space on whitespaces (lol) about 50% in the filesize
// department.
//
// The storage format looks like this:
//
// -----------------------------------------------------------------------------
// object1 : {
//   name : "Dummy Actor",
//   hitpoints : 40,
//   skills : {
//     sword : 1,
//     armor : 2,
//     athletics : 3,
//   },
//   inventory : item1/item2/item3/"it,e/m",
// },
// -----------------------------------------------------------------------------
//
// All items are key/value string pairs.
// It is possible to have a list of items in a value by separating items
// with '/'. If list item itself contains '/' and/or ',' then the whole item
// will be enclosed in quotes, as it is shown in the example above.
//
class NRS
{
  public:
    void SetString(const std::string& s, size_t index = 0);
    const std::string& GetString(size_t index = 0) const;

    void SetInt(int64_t value, size_t index = 0);
    int64_t GetInt(size_t index = 0) const;

    void SetUInt(uint64_t value, size_t index = 0);
    uint64_t GetUInt(size_t index = 0) const;

    void Clear();

    size_t ValuesCount() const;
    size_t ChildrenCount() const;

    bool Has(const std::string& nodeName);

    NRS& operator[](const std::string& nodeName);

    //
    // For more readable access to inner elements, e.g. "object.inventory"
    // instead of ["object"]["inventory"]
    //
    NRS& GetNode(const std::string& path);

    std::string ToStringObject();
    void FromStringObject(const std::string& so);

    bool CheckSyntax(const std::string& so);

    bool Save(const std::string& fileName);
    bool Load(const std::string& fname);

    std::string ToPrettyString();
    std::string DumpObjectStructureToString();

  private:
    std::string MakeOneliner(const std::string& stringObject);

    void WriteIntl(const NRS& d, std::stringstream& ss);

    void DriveStateMachine(const char currentChar, bool debug = false);

    //
    // Contains value elements (after ':' symbol).
    //
    std::vector<std::string> _content;

    //
    // We need to preserve serial order of data nodes added to our save data
    // instance, so we use vector. We can't use map since it's sorted by string
    // and we cannot use unordered_map since it doesn't guarantee order at all.
    // But we still need to access our nodes somehow. So the only way to do this
    // while preserving serial order is by using indices in a vector.
    // And since we're using indices there's no danger of "losing" access due to
    // vector's relocation in memory because we don't rely on object addresses,
    // just on their serial order.
    // Also "humans like to group things", so we can put data in a file in
    // whatever order that seems comfortable for us.
    //
    std::vector<std::pair<std::string, NRS>> _children;

    //
    // Mapping of children node names to their respective index
    // in a vector of children.
    //
    std::unordered_map<std::string, size_t> _childIndexByName;

    size_t _currentIndent = 0;

    const std::string kEmptyString;

    const std::string _unwantedCharacters = " \t\n\r\f\v";

    enum class ParsingState
    {
      UNDEFINED = -1,
      READING_KEY,
      KEY_DONE,
      READING_VALUE,
      VALUE_DONE,
      READING_VALUE_Q,
      VALUE_Q_DONE,
      READING_LIST,
      READING_OBJECT,
      OBJECT_DONE,
      ERROR
    };

    size_t _parsingScopeCount = 0;

    ParsingState _parsingState = ParsingState::UNDEFINED;

    const std::set<char> _transitionChars = { ':', '{', '}', ',', '"', '/' };
};

#endif // SERIALIZER_H
