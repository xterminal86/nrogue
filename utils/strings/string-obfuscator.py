#!/usr/bin/python3
# coding = utf8

import random;
import sys;

FullFill = 0xFFFFFFFF;
LowFill  = 0x000000FF;
HighFill = 0xFFFFFF00;

Size = 16
MaxNum = 2**Size;

CharsByIndex = {};
IndexByChars = {};

MyString = "";

MyStringData = [];
ObfuscatedString = [];

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
    IndexByChars[chr(i)] = i;
    CharsByIndex[i] = chr(i);

################################################################################

def ObfuscateString(str):
  for c in str:
    res = EncodeChar(c);
    ObfuscatedString.append(res);
  print("Obfuscated bytes:");
  print(ObfuscatedString);

################################################################################

def DecodeString(str):
  tmp = "";
  print("\n");
  print(">"*80);
  for c in str:
    res = DecodeNumToChar(c);
    print(f"{ res } = { ord(res) } ({ c })");
    tmp += res;
  print("<"*80);
  print(tmp);

################################################################################

def ToCppVector(list):
  print("");

  res = f"std::vector<uint{ Size }_t> script = \n";
  res += "{\n";

  countLines = 0;
  for item in list:
    res += "{0}, ".format(item);
    countLines = countLines + 1;
    if countLines > 8:
      countLines = 0;
      res += "\n";
  res += "\n};\n";
  print(res);

################################################################################

def main():
  if len(sys.argv) == 2:
    MyString = sys.argv[1];
  else:
    print(f"Usage: { sys.argv[0] } <STRING>");
    exit(0);

  CreateData();
  ObfuscateString(MyString);
  DecodeString(ObfuscatedString);
  ToCppVector(ObfuscatedString);

################################################################################

if __name__ == "__main__":
  main();
