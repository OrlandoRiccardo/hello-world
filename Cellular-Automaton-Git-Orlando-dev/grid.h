#ifndef _grid_h
#define _grid_h

// This is the basic cell.
//Should be good to add more data into this later: we'll just need to update the Rule functions to deal with them.
typedef struct Cell {
	double data;
	// stuff to be added later
} Cell;

// This is the grid. it has two arrays of Cells:
// curCell is only supposed to be read from;
// newCell is only supposed to be written to.
typedef struct Grid {
	unsigned width;
	unsigned height;
	Cell *curCell;
	Cell *newCell; // "working copy"
} Grid;


// Constructor
Grid initGrid(unsigned width, unsigned height);

// Destructor
void destroyGrid(Grid g);

// This swaps (the pointers to) curCell and newCell, effectively committing the writes done to the write-only newCell into the read-only curCell.
void commitGridUpdate(Grid *g);

// Reads cell in position x, y in grid g.curGrid
// calls exit(-1) if x or y are not in range.
Cell getCell(Grid g, int x, int y);

// Writes the given cell in position x, y of grid g.newCell.
// Calls exit(-1) if x or y is not in range.
void setCell(Grid *g, int x, int y, Cell cell);

#endif
