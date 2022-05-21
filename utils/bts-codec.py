#!/usr/bin/python
# coding = utf8

#
# BTS scripts compiler / decompiler
#
# by xterminal86 21.05.2022
#
import sys;

ShouldWriteToFile = False;

Script = "";
Bytecode = [];

ParamsEnd = 0xFF;
Nop       = 0xFE;

Lines = [];

TaskNames = [
  "TREE",
  "SEL" ,
  "SEQ" ,
  "FAIL",
  "SUCC",
  "TASK",
  "COND"
];

OpcodesByName = {};
ParamCodesByName = {};

# Look this up in constants.cpp
# (BTSTaskNamesByName and BTSParamNamesByName)
ParamNames = [
  "idle"                ,
  "move_rnd"            ,
  "move_smart"          ,
  "move_away"           ,
  "attack"              ,
  "attack_ranged"       ,
  "break_stuff"         ,
  "pick_items"          ,
  "chase_player"        ,
  "save_player_pos"     ,
  "goto_last_player_pos",
  "goto_last_mined_pos" ,
  "mine_tunnel"         ,
  "mine_block"          ,
  "apply_effect"        ,
  "drink_potion"        ,
  "print_message"       ,
  "end"                 ,
  "d100"                ,
  "player_visible"      ,
  "player_can_move"     ,
  "player_in_range"     ,
  "player_energy"       ,
  "player_next_turn"    ,
  "turns_left"          ,
  "has_effect"          ,
  "has_equipped"        ,
  "hp_low"              ,
  "eq"                  ,
  "gt"                  ,
  "lt"                  ,
  "player"              ,
  "self"                ,
  "HEA"                 ,
  "NCK"                 ,
  "TRS"                 ,
  "BTS"                 ,
  "MAG"                 ,
  "WPN"                 ,
  "SLD"                 ,
  "RNG"                 ,
  "-"                   ,
  "HP"                  ,
  "MP"                  ,
  "Hid"                 ,
  "Shi"                 ,
  "Reg"                 ,
  "Ref"                 ,
  "PAb"                 ,
  "MAb"                 ,
  "Ths"                 ,
  "Par"                 ,
  "Tel"                 ,
  "Fly"                 ,
  "Bli"                 ,
  "Frz"                 ,
  "Bur"                 ,
  "Lgt"                 ,
  "Psd"                 ,
  # =================================
  "nop"
];

Bytecode = [];

def ParseLine(line):
  indentsParsed = False;
  exprStart = False;
  expr = "";
  indent = 0;
  for c in line:
    if c == ' ' and indentsParsed == False:
      indent = indent + 1;

    if exprStart == True and c != '[' and c != ']':
      expr += c;

    if c == '[':
      indentsParsed = True;
      exprStart = True;

  # print("{0} {1}".format(indent, expr));

  splitRes = expr.split(' ');

  commandLen = len(splitRes);
  opcode = OpcodesByName.get(splitRes[0], Nop);

  if opcode != Nop:
    Bytecode.append(indent);
    Bytecode.append(opcode);

    if commandLen > 1:
      for i in range(1, commandLen):
        paramParse = splitRes[i].split('=');
        key = paramParse[1];
        key = key.strip('"');
        Bytecode.append(ParamCodesByName[key]);
      Bytecode.append(ParamsEnd);

  else:
    print("\tillegal opcode: {0}".format(splitRes[0]));

def InitContainers():

  print("Total number of params: {0}\n".format(len(ParamNames)));

  paramCode = 0x01;

  for name in TaskNames:
    OpcodesByName[name] = paramCode;
    paramCode = paramCode + 1;

  paramCode = 0x01;

  # number value params

  maxParams = 100;

  for i in range(0, maxParams + 1):
    paramStr = str(i);
    ParamCodesByName[paramStr] = i;
    paramCode = i;

  paramCode = paramCode + 1;

  # conditional params

  for name in ParamNames:
    ParamCodesByName[name] = paramCode;
    paramCode = paramCode + 1;

  # print(ParamCodesByName);

def PrintBytecode():
  output = "";
  align = 0;

  for n in Bytecode:
    tmp = "0x{:02X}".format(n);
    output += tmp;

    output += (' ' * (7 - len(tmp)));

    align = align + 1;

    if align == 8:
      output += '\n';
      align = 0;

  print(output);
  print("\n");

def PrintCppContainer():
  print("\nC++ container:\n");

  output = "std::vector<uint8_t> bytecode =\n{\n  ";

  count = len(Bytecode);

  align = 0;

  for n in Bytecode:
    tmp = "0x{:02X}".format(n);
    output += tmp;

    if count != 1:
      output += ', ';

    output += (' ' * (4 - len(tmp)));

    align = align + 1;

    if align == 8:
      output += "\n";

      if count != 1:
        output += "  ";

      align = 0;

    count = count - 1;

  output += "\n};";

  print(output);
  print("");

def Compile():
  print(Script);
  print("Compiling...\n");

  Lines = Script.splitlines();
  # print(Lines);

  for line in Lines:
    line = line.rstrip();

    if len(line) == 0:
      continue;

    ParseLine(line);

  print("");

  print("Resulting bytecode (len={0}):\n".format(len(Bytecode)));

  PrintBytecode();

  PrintCppContainer();

  compression = float(len(Bytecode)) / float(len(Script)) * 100;

  print("Size reduction: {:0.2f}%\n".format(100 - compression));

  if ShouldWriteToFile == True:

    print("Writing script.btc...");

    with open("script.btc", "wb") as f:
      ba = bytearray(Bytecode);
      f.write(ba);

def Decompile():
  index = 0;
  indent = 0;
  opcode = 0;
  paramCount = 1;

  taskByOpcode = { v: k for k, v in OpcodesByName.items()    };
  paramByCode  = { v: k for k, v in ParamCodesByName.items() };

  PrintBytecode();

  PrintCppContainer();

  print("Decompiling...\n");

  decompiled = "";

  while index != len(Bytecode):

    indent = Bytecode[index];
    opcode = Bytecode[index + 1];

    taskNameVal = taskByOpcode.get(opcode);

    taskName = "";

    if taskNameVal == None:
      print("\tillegal opcode: {0}".format(opcode));
      opcode = Nop;
    else:
      taskName = taskNameVal;
      strIndent = (' ' * indent);
      decompiled += "{0}[{1}".format(strIndent, taskName);

    index = index + 2;

    if opcode == Nop:
      continue;

    if taskName == "COND" or taskName == "TASK":
      paramCount = 1;
      curByte = Bytecode[index];

      while curByte != ParamsEnd:
        paramType = paramByCode[curByte];
        decompiled += " p{0}=\"{1}\"".format(paramCount, paramType);
        paramCount = paramCount + 1;
        index = index + 1;
        curByte = Bytecode[index];

      # skip end of params marker
      index = index + 1;

    decompiled += "]\n";

  print(decompiled);

# ******************************************************************************

def PrintUsage(progName):
  print("Usage: {0} c|d [WRITE_TO_FILE] < stdin".format(progName));

if __name__ == "__main__":
  if len(sys.argv) < 2:
    PrintUsage(sys.argv[0]);
    exit(0);

  ShouldWriteToFile = (len(sys.argv) > 2);

  InitContainers();

  for line in sys.stdin:
    Script += line;

  if sys.argv[1] == "c":
    Compile();
  elif sys.argv[1] == "d":
    Bytecode = bytearray(Script);
    Decompile();
  else:
    PrintUsage(sys.argv[0]);
