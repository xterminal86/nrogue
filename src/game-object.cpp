#include "game-object.h"

bool GameObject::Move(int dx, int dy)
{
  if (!Map::Instance().MapArray[_posX + dx][_posY + dy].Blocking)
  {
    _posX += dx;
    _posY += dy;
    
    return true;
  }
  
  return false;
}

void GameObject::CheckVisibility()
{
  int tw = Printer::Instance().TerminalWidth;
  int th = Printer::Instance().TerminalHeight;
  
  int vrx = VisibilityRadius;
  int vry = VisibilityRadius;
  
  // Compensate for different terminal sizes, 
  // so we get more circle-like visible area around player
  if (tw > th)
  {
    vry /= 2;
  }
  else if (tw < th)
  {
    vrx /= 2;
  }
  
  vrx = Util::Clamp(vrx, 1, VisibilityRadius);
  vry = Util::Clamp(vry, 1, VisibilityRadius);
  
  auto map = Map::Instance().MapArray;

  for (int x = 0; x < GlobalConstants::MapX; x++)
  {
    for (int y = 0; y < GlobalConstants::MapY; y++)
    {
      map[x][y].Visible = false;        
    }
  }

  int lx = _posX - vrx;
  int ly = _posY - vry;
  int hx = _posX + vrx;
  int hy = _posY + vry;
  
  lx = Util::Clamp(lx, 0, GlobalConstants::MapX - 1);
  ly = Util::Clamp(ly, 0, GlobalConstants::MapY - 1);
  hx = Util::Clamp(hx, 0, GlobalConstants::MapX - 1);
  hy = Util::Clamp(hy, 0, GlobalConstants::MapY - 1);

  for (int x = lx; x <= hx; x++)
  {
    for (int y = ly; y <= hy; y++)
    {
      float d = Util::LinearDistance(_posX, _posY, x, y);
      
      if (d < (float)VisibilityRadius)
      {
        map[x][y].Visible = true; 

        if (!map[x][y].Revealed)
        {
          map[x][y].Revealed = true;
        }
      }
    }        
  }
}
