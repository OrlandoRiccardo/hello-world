#include <stdio.h>
#include <stdlib.h>
#include "grid.h"

/*
typedef struct Cell
typedef struct Grid
*/

Grid initGrid(unsigned width, unsigned height) {
	Grid g;
	g.width = width;
	g.height = height;

    g.curCell = calloc( width * height, sizeof(Cell) );
    g.newCell = calloc( width * height, sizeof(Cell) );

	return g;
}

void destroyGrid(Grid g) {
    free(g.curCell);
    free(g.newCell);
    return;
}

void commitGridUpdate(Grid *g) {
	Cell *tmp = g->curCell;
	g->curCell = g->newCell;
	g->newCell = tmp;
}

Cell getCell(Grid g, int x, int y) {
	if ( x >= g.width || y >= g.height || x<0 || y<0 ) {
		exit(-1);
	}
	return g.curCell[ x + y * g.width ];
}

void setCell(Grid *g, int x, int y, Cell cell) {
	if ( x >= g->width || y >= g->height || x<0 || y<0 ) {
		exit(-1);
	}
	g->newCell[ x + y * g->width ] = cell;
	return;
}
