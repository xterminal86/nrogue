#ifndef TESTS_H
#define TESTS_H

#include "util.h"

namespace Tests
{  
  /// Test Bresenham line return vector in all directions
  inline void TestLoS(int x, int y, int range)
  {
    printf("Starting Bresenham LoS test from (%i %i) in range %i...\n", x, y, range);
    printf("The path should always start from (%i %i)\n\n", x, y);

    int sx = x - range;
    int sy = y - range;
    int ex = x + range;
    int ey = y + range;

    for (int fx = sx; fx <= ex; fx++)
    {
      for (int fy = sy; fy <= ey; fy++)
      {
        auto res = Util::BresenhamLine(x, y, fx, fy);
    
        printf ("[%i;%i] -> [%i;%i]\n", x, y, fx, fy);
        
        for (auto& i : res)
        {
          printf("(%i %i) - ", i.X, i.Y);
        }
        
        bool cond = (res[0].X == x && res[0].Y == y);
        if (!cond)
        {
          printf("*** FAILED! ***\n");  
        }
        
        printf("\n");
      }
    }    
  }

  inline void Run()
  {
    printf("***** START TESTS *****\n\n");

    TestLoS(4, 4, 2);

    printf("\n\n***** o *****\n");
  }  
}

#endif