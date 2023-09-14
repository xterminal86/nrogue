#!/usr/bin/python3

CharsByIndex = {};

def CreateData():
  for i in range(0, 128):
    CharsByIndex[i] = chr(i);

################################################################################

def DecodeNumToChar(n):
  index = n & 0x000000FF;
  return CharsByIndex[index];

################################################################################

def Decode(data : list):
  print("Encoded data:");
  print(data);
  print("");

  tmp = "";
  print("\n");
  print(">"*80);
  for c in data:
    res = DecodeNumToChar(c);
    print(f"{ res } = { ord(res) } ({ c })");
    tmp += res;
  print("<"*80);
  print(tmp);

################################################################################

def main():
  CreateData();

  data = [
    50497, 16488, 24366, 1838, 53806, 42528, 15174, 31858, 19301,
    47219, 14184, 800, 63085, 64101, 58209, 41332, 36444, 8993,
  ];

  Decode(data);

################################################################################

if __name__ == "__main__":
  main();
