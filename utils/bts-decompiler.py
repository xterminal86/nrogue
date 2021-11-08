#!/usr/bin/python
# coding = utf8

ParamsEnd = 0xFF;
Nop       = 0xFE;

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

TaskByOpcode = {};
ParamByCode  = {};

Bytecode = [ 0x0, 0x1, 0x2, 0x2, 0x4, 0x7, 0x36, 0xff, 0x6, 0x2, 0x8, 0x7, 0x34, 0xff, 0xa, 0x3, 0xc, 0x6, 0x2e, 0xff, 0xc, 0x2, 0xe, 0x7, 0x3a, 0x3f, 0x40, 0xff, 0x10, 0x2, 0x12, 0x6, 0x2d, 0xff, 0x12, 0x7, 0x36, 0x1, 0xff, 0x14, 0x6, 0x30, 0x40, 0xff, 0xe, 0x7, 0x36, 0x1, 0xff, 0x10, 0x6, 0x30, 0x40, 0xff, 0xe, 0x6, 0x2c, 0xff, 0x8, 0x7, 0x3a, 0x3f, 0x40, 0xff, 0xa, 0x6, 0x2d, 0xff, 0x8, 0x6, 0x2f, 0xff, 0x4, 0x6, 0x2a, 0xff, 0x4, 0x6, 0x29, 0xff ];

def InitContainers():
  global TaskByOpcode;
  global ParamByCode;
  
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

  TaskByOpcode = { v: k for k, v in OpcodesByName.items()    };
  ParamByCode  = { v: k for k, v in ParamCodesByName.items() };

  '''
  print("Tasks:");
  print(TaskByOpcode);
  print("");
  print("Params:");
  print(ParamByCode);
  print("\n");
  '''

def Decompile():
  index = 0;
  indent = 0;
  opcode = 0;
  paramCount = 1;
  
  while index != len(Bytecode):

    indent = Bytecode[index];
    opcode = Bytecode[index + 1];
        
    taskNameVal = TaskByOpcode.get(opcode);

    taskName = "";

    if taskNameVal == None:
      print("\tunknown opcode: {0}".format(opcode));
      opcode = Nop;
    else:
      taskName = taskNameVal;
      strIndent = (' ' * indent);
      print("{0}{1}".format(strIndent, taskName));

    index = index + 2;
        
    if opcode == Nop:
      continue;

    if taskName == "COND" or taskName == "TASK":
      paramCount = 1;
      curByte = Bytecode[index];

      while curByte != ParamsEnd:        
        paramType = ParamByCode[curByte];
        print("{0}p{1} = {2}".format(strIndent, paramCount, paramType));
        paramCount = paramCount + 1;
        index = index + 1;
        curByte = Bytecode[index];

      # skip end of params marker
      index = index + 1;      

def main():

  InitContainers();
  
  print(Bytecode);
  print("");

  Decompile();

if __name__ == "__main__":
  main();
