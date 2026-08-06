#!/usr/bin/python3

#
# https://www.albertford.com/shadowcasting/
#

import math

from fractions import Fraction

#
# Map should be fully enclosed by walls or algorithm will go out of bounds.
#
'''
Grid = [
  [ 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1 ],
  [ 1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1 ],
  [ 1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1 ],
  [ 1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1 ],
  [ 1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1 ],
  [ 1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1 ],
  [ 1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1 ],
  [ 1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1 ],
  [ 1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1 ],
  [ 1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1 ],
  [ 1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1 ],
  [ 1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1 ],
  [ 1,0,0,0,0,1,0,1,0,1,0,1,0,1,0,0,0,1 ],
  [ 1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1 ],
  [ 1,0,0,0,0,0,0,0,9,0,0,0,0,0,0,0,0,1 ],
  [ 1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1 ],
  [ 1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1 ],
  [ 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1 ],
];
'''

'''
Grid = [
  [ 1,1,1,1,1,1,1 ],
  [ 1,0,9,0,0,0,1 ],
  [ 1,1,0,1,0,1,1 ],
  [ 1,0,0,0,0,0,1 ],
  [ 1,0,0,0,0,0,1 ],
  [ 1,0,0,0,0,0,1 ],
  [ 1,1,1,1,1,1,1 ],
]
'''

Grid = [
  [ 1,1,1,1,1,1,1,1,1,1,1 ],
  [ 1,0,0,0,0,0,0,0,0,0,1 ],
  [ 1,0,0,0,0,0,0,0,0,0,1 ],
  [ 1,0,0,0,0,0,0,0,0,0,1 ],
  [ 1,0,0,0,0,0,0,0,0,0,1 ],
  [ 1,0,1,0,1,0,1,0,1,0,1 ],
  [ 1,0,0,0,0,0,0,0,0,0,1 ],
  [ 1,0,0,0,0,9,0,0,0,0,1 ],
  [ 1,0,0,0,0,0,0,0,0,0,1 ],
  [ 1,0,0,0,0,0,0,0,0,0,1 ],
  [ 1,1,1,1,1,1,1,1,1,1,1 ],
];

MapSize = len(Grid)

################################################################################

def mark_visible(x, y):
  Grid[x][y] = (Grid[x][y] | 2);

def is_blocking(x, y) -> bool:
  return ((Grid[x][y] & 1) == 1)

def reveal(tile, quadrant):
    x, y = quadrant.transform(tile)
    mark_visible(x, y)

def is_wall(tile, quadrant):

    if tile is None:
        return False

    x, y = quadrant.transform(tile)

    return (x < 0 or x >= MapSize or y < 0 or y >= MapSize) or is_blocking(x, y)

def is_floor(tile, quadrant):
    if tile is None:
        return False

    x, y = quadrant.transform(tile)

    return (x >= 0 and x < MapSize and y >= 0 and y < MapSize) and not is_blocking(x, y)

def print_map():
  for row in Grid:
    for cell in row:
      print(f"{ cell }", end="")
    print()

  print()

  for row in Grid:
    for cell in row:
      if (cell == 9):
        print("x", end="")
      elif (cell == 3):
        print("#", end="")
      elif (cell == 2):
        print(".", end="")
      elif (cell == 0 or cell == 1):
        print(" ", end="")
    print()


################################################################################

def slope(tile):
    row_depth, col = tile

    f = Fraction(2 * col - 1, 2 * row_depth)

    print(
      f"    slope({ tile }), row_depth = { row_depth }, col = { col } -> "
      f"Fraction( (2 * { col }) - 1, 2 * { row_depth }) -> "
      f"Fraction({ 2 * col - 1 }, { 2 * row_depth }) "
      f"row = { row_depth }, col = { col }, slope = { f }"
    )

    return f

################################################################################

def is_symmetric(row, tile):
    row_depth, col = tile

    tmp1 = row.depth * row.start_slope
    tmp2 = row.depth * row.end_slope

    cond = (col >= row.depth * row.start_slope
        and col <= row.depth * row.end_slope)

    print(
      f"    is_symmetric?({ row }, { tile }) -> "
      f"([{ col }] >= ({ row.depth } * { row.start_slope }) = { tmp1 } || "
      f"[{ col }] <= ({ row.depth } * { row.end_slope }) = { tmp2 }) "
      f"- { 'YES' if cond else 'NO' }"
    )

    return cond

################################################################################

def round_ties_up(n):
    print(f"round_ties_up({ n }) = { math.floor(n + 0.5) }")
    return math.floor(n + 0.5)

################################################################################

def round_ties_down(n):
    print(f"round_ties_down({ n }) = { math.ceil(n - 0.5) }")
    return math.ceil(n - 0.5)

################################################################################

class Row:
    def __init__(self, depth, start_slope, end_slope):
        self.depth = depth
        self.start_slope = start_slope
        self.end_slope = end_slope

    def tiles(self):
        print(f"Row::tiles() -> round_ties_up({ self.depth } * { self.start_slope }) = round_ties_up({ self.depth * self.start_slope })")
        print(f"Row::tiles() -> round_ties_down({ self.depth } * { self.end_slope }) = round_ties_down({ self.depth * self.end_slope })")
        min_col = round_ties_up(self.depth * self.start_slope)
        max_col = round_ties_down(self.depth * self.end_slope)
        print(f"Row::tiles({ self }) -> { min_col } : { max_col }")
        for col in range(min_col, max_col + 1):
            yield (self.depth, col)


    def next(self):
        r = Row(
            self.depth + 1,
            self.start_slope,
            self.end_slope
        )

        print(f"Row::next() -> { r }")

        return r

    def __repr__(self):
      return (
        "<"
        f"Row, depth = { self.depth }, "
        f"start_slope = { self.start_slope } "
        f"end_slope = { self.end_slope }"
        ">"
      )

################################################################################

class Quadrant:
    north = 0
    east  = 1
    south = 2
    west  = 3

    #
    # Actually it's this (at least it looks from printf debugging).
    #
    Names = {
      0 : "WEST",
      1 : "SOUTH",
      2 : "EAST",
      3 : "NORTH",
    }

    def __init__(self, cardinal, origin):
        self.cardinal = cardinal
        self.ox, self.oy = origin

    def transform(self, tile):
        row, col = tile

        if self.cardinal == self.north:
            return (self.ox + col, self.oy - row)

        if self.cardinal == self.south:
            return (self.ox + col, self.oy + row)

        if self.cardinal == self.east:
            return (self.ox + row, self.oy + col)

        if self.cardinal == self.west:
            return (self.ox - row, self.oy + col)

################################################################################

def scan_iterative(row, quadrant):
    print("="*80)
    print(f"Processing quadrant: { Quadrant.Names[quadrant.cardinal] }")
    print("="*80)
    rows = [row]
    while rows:
        row = rows.pop()
        prev_tile = None
        for tile in row.tiles():
            x,y = quadrant.transform(tile)
            print(f"  tile = { tile } | <{ x },{ y }>")
            if is_wall(tile, quadrant) or is_symmetric(row, tile):
                print(f"    reveal <{ x },{ y }>")
                reveal(tile, quadrant)

            if is_wall(prev_tile, quadrant) and is_floor(tile, quadrant):
                print(f"    wall -> floor")
                row.start_slope = slope(tile)
                print(f"    start_slope = { row.start_slope }")

            if is_floor(prev_tile, quadrant) and is_wall(tile, quadrant):
                print(f"    floor -> wall")
                next_row = row.next()
                next_row.end_slope = slope(tile)
                print(f"    end_slope = { next_row.end_slope }")
                print(f"    adding { next_row }")
                rows.append(next_row)

            prev_tile = tile

        #
        # This can execute only after the last tile in a row was scanned.
        # If it was floor we continue scanning, since there are still
        # potentially things to see at that gap. Start slope should've been
        # adjusted in 'for tile in row.tile()' loop.
        #
        if is_floor(prev_tile, quadrant):
            print(f"    is_floor - adding { row.next() }")
            rows.append(row.next())

    print("Done")

################################################################################

#
# Non-recursive implementation.
#
def compute_fov(origin, is_blocking, mark_visible):
  mark_visible(*origin)

  for i in range(4):
        quadrant = Quadrant(i, origin)
        row = Row(1, Fraction(-1), Fraction(1))
        scan_iterative(row, quadrant)

#
# Recursive implementation
#
'''
def compute_fov(origin, is_blocking, mark_visible):
    mark_visible(*origin)

    for i in range(4):
        print(f"Processing quadrant { Quadrant.Names[i] }...")
        quadrant = Quadrant(i, origin)

        def scan(row):
            prev_tile = None
            for tile in row.tiles():
                x,y = quadrant.transform(tile)
                if (x < 0 or x >= MapSize) or (y < 0 or y >= MapSize):
                  print(f"    { x } or { y }, fucking off")
                  break
                print(f"    scanning tile: { tile }, x = { x }, y = { y }")
                if is_wall(tile, quadrant) or is_symmetric(row, tile):
                    reveal(tile, quadrant)

                if is_wall(prev_tile, quadrant) and is_floor(tile, quadrant):
                    row.start_slope = slope(tile)

                if is_floor(prev_tile, quadrant) and is_wall(tile, quadrant):
                    next_row = row.next()
                    next_row.end_slope = slope(tile)
                    scan(next_row)

                prev_tile = tile

            if is_floor(prev_tile, quadrant):
                scan(row.next())

        first_row = Row(1, Fraction(-1), Fraction(1))

        scan(first_row)
'''

################################################################################

def main():
  start = tuple()
  x = 0
  y = 0
  startSet = False
  for row in Grid:
    for cell in row:
      if (cell == 9):
        start = (x, y)
        startSet = True
        break
      y += 1
    if startSet:
      break
    y = 0
    x += 1

  compute_fov(start, is_blocking, mark_visible)
  Grid[ start[0] ][ start[1] ] = 9
  print_map()

################################################################################

if __name__ == "__main__":
  main()
