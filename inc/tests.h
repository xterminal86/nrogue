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

  inline void RoomTests()
  {
    printf("\nRoom layouts rotations:\n\n");

    for (auto& room : GlobalConstants::RoomLayouts)
    {
      printf("Layout:\n");

      for (auto& row : room)
      {
        printf("%s\n", row.data());
      }

      printf("CCW_90:\n");

      auto res = Util::RotateRoomLayout(room, RoomLayoutRotation::CCW_90);
      for (auto& s : res)
      {
        printf("\t%s\n", s.data());
      }

      printf("CCW_180:\n");

      res = Util::RotateRoomLayout(room, RoomLayoutRotation::CCW_180);
      for (auto& s : res)
      {
        printf("\t%s\n", s.data());
      }

      printf("CCW_270:\n");

      res = Util::RotateRoomLayout(room, RoomLayoutRotation::CCW_270);
      for (auto& s : res)
      {
        printf("\t%s\n", s.data());
      }
    }
  }

  inline void RNGTests()
  {
    printf("\nRNG tests:\n\n");

    printf("Random.Range(-10, 10):\n");

    int numberOfRolls = 100;

    for (int i = 0; i < numberOfRolls; i++)
    {
      printf("%i ", RNG::Instance().RandomRange(-10, 10));
    }

    printf("\n\nRandom.Range(0, 100):\n");

    for (int i = 0; i < numberOfRolls; i++)
    {
      printf("%i ", RNG::Instance().RandomRange(0, 100));
    }

    printf("\n\nRandom.Range(-20, -10):\n");

    for (int i = 0; i < numberOfRolls; i++)
    {
      printf("%i ", RNG::Instance().RandomRange(-20, -10));
    }

    printf("\n\nRandom.Range(100, 10):\n");

    for (int i = 0; i < numberOfRolls; i++)
    {
      printf("%i ", RNG::Instance().RandomRange(100, 10));
    }

    printf("\n\nRandom.Range(-10, -100):\n");

    for (int i = 0; i < numberOfRolls; i++)
    {
      printf("%i ", RNG::Instance().RandomRange(-10, -100));
    }

    printf("\n\nRandom.Range(10, -10):\n");

    for (int i = 0; i < numberOfRolls; i++)
    {
      printf("%i ", RNG::Instance().RandomRange(10, -10));
    }
  }

  inline void Run()
  {
    printf("***** START TESTS *****\n\n");

    TestLoS(4, 4, 2);
    printf("\n- o -\n");
    RoomTests();
    printf("\n- o -\n");
    RNGTests();

    printf("\n\n***** o *****\n");
  }  
}

#endif
