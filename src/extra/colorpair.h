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
  NColor FgColor;
  NColor BgColor;
  short PairIndex;
};

#endif
