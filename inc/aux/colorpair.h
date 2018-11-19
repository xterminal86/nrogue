#ifndef COLORPAIR_H
#define COLORPAIR_H

struct NColor
{  
  short ColorIndex;
  short R;
  short G;
  short B;
};

struct ColorPair
{
  short PairIndex;
  NColor FgColor;
  NColor BgColor;
};

#endif