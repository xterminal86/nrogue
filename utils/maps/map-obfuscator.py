#!/usr/bin/python3
# coding = utf8

import random;
import sys;

FullFill = 0xFFFFFFFF;
LowFill  = 0x000000FF;
HighFill = 0xFFFFFF00;

CharsByIndex = {};
IndexByChars = {};

Size = 16
MaxNum = 2**Size;

MyMap = [];

################################################################################

def EncodeChar(c):
  indexToEncode = IndexByChars[c];
  rndNum = random.randint(0, MaxNum);
  rndNum = (rndNum & HighFill);
  rndNum = rndNum + indexToEncode;
  return rndNum;

################################################################################

def EncodeNum(n):
  rndNum = random.randint(0, MaxNum);
  rndNum = (rndNum & HighFill);
  rndNum = rndNum + n;
  return rndNum;

################################################################################

def DecodeNumToChar(n):
  index = n & LowFill;
  return CharsByIndex[index];

################################################################################

def DecodeNum(n):
  return (n & LowFill);

################################################################################

def CreateData():
  for i in range(0, 128):
    index = i;
    IndexByChars[chr(i)] = index;
    CharsByIndex[index] = chr(i);

################################################################################

def ToCppVector(oMap):
  #decor = "*"*80;
  #print("\n{0}\n".format(decor));

  print();

  res = f"std::vector<std::vector<std::pair<uint{ Size }_t, uint{ Size }_t>>> map = \n";
  res += "{\n";

  for block in oMap:
    res += "  {";
    first = True;
    for pair in block:
      ocn = pair[0];
      occ = pair[1];
      if not first:
        res += ",";
      res += "{";
      res += f"{ocn},{occ}";
      res += "}";

      if first:
        first = False;

    res += "},\n";
  res = res[:-2];
  res += "\n";
  res += "};\n";
  print(res);

################################################################################

def DeobfuscateMap(map):
  decodedMap = [];
  for item in map:
    line = "";
    for pair in item:
      oci = pair[0];
      occ = pair[1];
      ch = DecodeNumToChar(oci);
      cc = DecodeNum(occ);
      for x in range(cc):
        line += ch;
    decodedMap.append(line);

  for item in decodedMap:
    print(item);

################################################################################

def ObfuscateMap(map):
  blocks = [];
  obfuscatedMap = [];
  for item in map:
    block = [];
    curChar = item[0];
    acc = 1;
    for c in item[1:]:
      if curChar != c:
        block.append([curChar, acc]);
        curChar = c;
        acc = 1;
      else:
        acc = acc + 1;
    block.append([curChar, acc]);
    blocks.append(block);

  #print("<"*80);
  #print(blocks);
  #print("="*80);

  oBlocks = [];
  for line in blocks:
    block = [];
    for item in line:
      cn = item[0];
      cc = int(item[1])
      ocn = EncodeChar(cn);
      occ = EncodeNum(cc);
      pair = [ ocn, occ ];
      block.append(pair);
    oBlocks.append(block);

  #print(oBlocks);
  #print(">"*80);

  ToCppVector(oBlocks);
  DeobfuscateMap(oBlocks);

################################################################################

def main():
  for line in sys.stdin:
    MyMap.append(line.strip());

  CreateData();
  ObfuscateMap(MyMap);

################################################################################

if __name__ == "__main__":
  main();
