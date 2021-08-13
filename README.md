# Auto Tile Test Application

**Auto-tile bitmask setter**
- sets surrounding tile bitmasks in a tileset

**Auto-tile Mapper**
- Draw a tilemap, and the program will fill in the tiles from the tileset based on their surrounding tile bitmasks

**Controls**
- Press L to load
- Press S to save
- LMB place something
- RMB erase something

**Details**
- Reference used: https://gamedevelopment.tutsplus.com/tutorials/how-to-use-tile-bitmasking-to-auto-tile-your-level-layouts--cms-25673
- For the sake of experimentation, the "Tilemap" is essentially just an array of Uint32s. The Uint32 element represents the columns, and the index of each Uint32 represents the row.
- 1 = Solid Tile exists
- 0 = Solid Tile does not exist
