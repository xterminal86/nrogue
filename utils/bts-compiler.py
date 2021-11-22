#!/usr/bin/python
# coding = utf8

import sys;

ShouldWriteToFile = False;

Script = '''
[TREE]
  [SEL]
    [COND p1="player_in_range"]
      [SEL]
        [COND p1="player_visible"]
          [SEQ]
            [TASK p1="save_player_pos"]
            [SEL]
              [COND p1="has_effect" p2="player" p3="Psd"]
                [SEL]
                  [TASK p1="move_away"]
                  [COND p1="player_in_range" p2="1"]
                    [TASK p1="attack_effect" p2="Psd"]
              [COND p1="player_in_range" p2="1"]
                [TASK p1="attack_effect" p2="Psd"]
              [TASK p1="chase_player"]
        [COND p1="has_effect" p2="player" p3="Psd"]
          [TASK p1="move_away"]
        [TASK p1="goto_last_player_pos"]
    [TASK p1="move_rnd"]
    [TASK p1="idle"]
'''

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

ParamNames = [
  "idle"                 ,
  "move_rnd"             ,
  "attack_basic"         ,
  "chase_player"         ,
  "move_away"            ,
  "save_player_pos"      ,
  "goto_last_player_pos" ,
  "attack_effect"        ,
  "attack_special"       ,
  "arrack_ranged"        ,
  "d100"                 ,
  "player_visible"       ,
  "player_can_move"      ,
  "player_in_range"      ,
  "player_energy"        ,
  "player_next_turn"     ,
  "turns_left"           ,
  "has_effect"           ,
  "hp_low"               ,
  "gt"                   ,
  "lt"                   ,
  "eq"                   ,
  "player"               ,
  "Psd"                  ,
  "end"                  ,
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
  paramCode = 0x01;

  for name in TaskNames:
    OpcodesByName[name] = paramCode;
    paramCode = paramCode + 1;

  paramCode = 0x01;

  # number value params

  for i in range(1, 41):
    paramStr = str(i);
    ParamCodesByName[paramStr] = i;
    paramCode = i;

  paramCode = paramCode + 1;

  # conditional params

  for name in ParamNames:
    ParamCodesByName[name] = paramCode;
    paramCode = paramCode + 1;

  # print(ParamCodesByName);

def main():
  global ShouldWriteToFile;

  if len(sys.argv) != 1:
    ShouldWriteToFile = True;

  InitContainers();

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

  align = 0;
  output = "";
  for n in Bytecode:
    tmp = "0x{:02X}".format(n);
    output += tmp;

    output += (' ' * (7 - len(tmp)));

    align = align + 1;

    if align == 8:
      output += '\n';
      align = 0;

  print("Resulting bytecode (len={0}):\n".format(len(Bytecode)));
  print(output);

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

  output += "};";

  print(output);
  print("");

  compression = float(len(Bytecode)) / float(len(Script)) * 100;

  print("Size reduction: {:0.2f}%\n".format(100 - compression));

  if ShouldWriteToFile == True:

    print("Writing script.btc...");

    with open("script.btc", "wb") as f:
      ba = bytearray(Bytecode);
      f.write(ba);

if __name__ == "__main__":
  main();
