#ifndef CONSTANTS_H
#define CONSTANTS_H

// 262, 259, 339, 260, 350, 261, 360, 258, 338

#if defined(__unix__) || defined(__linux__)

  #define NUMPAD_7  262
  #define NUMPAD_8  259
  #define NUMPAD_9  339
  #define NUMPAD_4  260
  #define NUMPAD_5  350
  #define NUMPAD_6  261
  #define NUMPAD_1  360
  #define NUMPAD_2  258
  #define NUMPAD_3  338

#else

  #define NUMPAD_7  KEY_A1
  #define NUMPAD_8  KEY_A2
  #define NUMPAD_9  KEY_A3
  #define NUMPAD_4  KEY_B1
  #define NUMPAD_5  KEY_B2
  #define NUMPAD_6  KEY_B3
  #define NUMPAD_1  KEY_C1
  #define NUMPAD_2  KEY_C2
  #define NUMPAD_3  KEY_C3

#endif

namespace GlobalConstants
{
  static const int MapX = 100; // 160
  static const int MapY = 100; // 45

  static const std::string BlackColor = "#000000";
  static const std::string WallColor = "#888888";
  static const std::string TreeColor = "#00AA00";
  static const std::string FloorColor = "#444444";
  static const std::string FogOfWarColor = "#222222";
}

#endif
