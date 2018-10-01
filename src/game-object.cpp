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
  
  // FIXME: rethink
  //
  // Compensate for different terminal sizes, 
  // so we get more circle-like visible area around player
  /*
  if (tw > th)
  {
    vry /= 2;
  }
  else if (tw < th)
  {
    vrx /= 2;
  }
  */
    
  // Update map around player
  
  auto map = Map::Instance().MapArray;
  
  auto mapCells = Util::GetRectAroundPoint(_posX, _posY, tw / 2, th / 2);
  for (auto& cell : mapCells)
  {
    map[cell.X][cell.Y].Visible = false;        
  }
    
  // Update visibility around player
    
  for (auto& cell : mapCells)
  {    
    float d = Util::LinearDistance(_posX, _posY, cell.X, cell.Y);

    if (d > (float)VisibilityRadius)
    {
      continue;
    }

    // Bresenham FoV

    auto line = Util::BresenhamLine(_posX, _posY, cell.X, cell.Y);
    for (auto& point : line)
    {       
      if (map[point.X][point.Y].Blocking)
      {
        DiscoverCell(point.X, point.Y);
        break;
      }
      
      DiscoverCell(point.X, point.Y);
    }    
  }
}
