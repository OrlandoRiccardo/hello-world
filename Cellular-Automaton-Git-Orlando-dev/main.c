#include <stdio.h>
#include <stdlib.h>
#include <time.h> // for srand(time(NULL))
#include <limits.h> // for MAX_INT
#include <math.h>
#include <string.h> // for filename management
#include "lodepng.h"
#include <stdint.h> // for uintptr_t
#include "grid.h"
#include "image.h"

// Esempio: faccio la media di tutte le celle in un intorno quadrato di raggio dato
Cell ruleAverage(Grid g, int x, int y, int radius) {
	int hitCount = 0; // celle valide
	float runningTotal = 0;
	Cell curCell;
	// Questo e' facilmente ottimizzabile
	for ( int i = -1*radius; i <= radius; i++) {
		for ( int j = -1*radius; j <= radius; j++) {
			if (0 <= x+i && x+i < g.width && 0 <= y+j && y+j < g.height ) {
				hitCount++;
				curCell = getCell(g, x+i, y+j);
				runningTotal += curCell.data;
			}
		}
	}
	curCell.data = runningTotal / hitCount;
	return curCell;
}

Cell ruleConvolve(Grid tgt, Grid op, int x, int y) {
    float runningCount = 0;
    int centerX = op.width/2;
    int centerY = op.height/2;
    Cell curCellTgt;
    Cell curCellOp;

    for ( int j = 0; j < op.height; j++) {
        for ( int i = 0; i < op.width; i++) {
            curCellTgt = getCell(tgt, (x-centerX+i)%tgt.width , (y-centerY+j)%tgt.height );
            curCellOp = getCell(op, i, j);
            runningCount += curCellOp.data * curCellTgt.data;
        }
    }

    curCellTgt.data = runningCount;
    return curCellTgt;
}

Cell ruleConway(Grid tgt, int x, int y) {
	int hitCount = 0; // celle valide
	Cell curCell;
	// Questo e' facilmente ottimizzabile
	for ( int i = -1; i <= 1; i++) {
		for ( int j = -1; j <= 1; j++) {
            curCell = getCell(tgt, (x+i)%tgt.width, (y+j)%tgt.height );
            if (curCell.data == 1) hitCount++;
        }
	}

	if (hitCount == 3) {
        curCell.data = 1;
        return curCell;
	} else if (hitCount == 4) {
        curCell = getCell(tgt, x, y);
        return curCell;
	}
	curCell.data = 0;
	return curCell;
}

void applyRuleConway(Grid *g) {
	Cell curCell;
	for ( int x = 0; x < g->width; x++) {
		for ( int y = 0; y < g->height; y++) {
			curCell = ruleConway( *g, x, y);
			setCell(g, x, y, curCell);
		}
	}
	commitGridUpdate(g);
}

void applyRuleConvolve(Grid *tgt, Grid op) {
    Cell curCell;
	for ( int x = 0; x < tgt->width; x++) {
		for ( int y = 0; y < tgt->height; y++) {
			curCell = ruleConvolve( *tgt, op, x, y);
			setCell(tgt, x, y, curCell);
		}
	}
	commitGridUpdate(tgt);
}

void applyRuleAverage(Grid *g) {
	Cell curCell;
	for ( int x = 0; x < g->width; x++) {
		for ( int y = 0; y < g->height; y++) {
			curCell = ruleAverage( *g, x, y, 5);
			setCell(g, x, y, curCell);
		}
	}
	commitGridUpdate(g);
}

void applyRuleNormalize(Grid *g, double tgtMin, double tgtMax) {
    Cell curCell = getCell(*g, 0, 0);
    double curMin = curCell.data;
    double curMax = curCell.data;
    double buf;
    // Find current minimum and maximum
    for (int j = 0; j<g->height; j++) {
        for (int i = 0; i<g->width; i++) {
            curCell = getCell(*g, i, j);
            buf = curCell.data;
            curMin = buf < curMin ? buf : curMin;
            curMax = buf > curMax ? buf : curMax;
        }
    }
    //printf("Normalize: curMin = %f; curMax = %f\n", curMin, curMax);

    // Now, actually normalize
    double distortion = (tgtMax-tgtMin)/(curMax-curMin);
    double data;
    for (int j = 0; j<g->height; j++) {
        for (int i = 0; i<g->width; i++) {
            curCell = getCell(*g, i, j);
            data = curCell.data;
            data = ((data-curMin)*distortion)+tgtMin;
            curCell.data = data;
            setCell(g, i, j, curCell);
        }
    }

    commitGridUpdate(g);
}


void applyRuleSetMass(Grid *g, double tgtMass) {
    Cell curCell;
    double curMass = 0;
    // Find current mass
    for (int j = 0; j < g->height; j++) {
        for (int i = 0; i < g->width; i++) {
            curCell = getCell(*g, i, j);
            curMass += curCell.data;
            // printf("SetMass: curMass = %f\n", curMass);
        }
    }
    printf("SetMass: Done: curMass = %f\n", curMass);

    // Now, actually normalize
    double distortion = tgtMass/curMass;
    for (int j = 0; j<g->height; j++) {
        for (int i = 0; i<g->width; i++) {
            curCell = getCell(*g, i, j);
            curCell.data = curCell.data * distortion;
            setCell(g, i, j, curCell);
        }
    }

    commitGridUpdate(g);
}

void slideshowRuleConvolve(Grid *g, Grid op, const char *filename) {
    char buffer[64];
    do {
        printf("Looping...\n");
        applyRuleConvolve(g, op);
        grid2PNG(*g, filename);
    } while (strcmp(fgets(buffer, 64, stdin), "exit\n") != 0);
    return;
}

void slideshowRuleConway(Grid *g, const char *filename) {
    char buffer[64];
    do {
        printf("Looping...\n");
        grid2PNG(*g, filename);
        applyRuleConway(g);
    } while (strcmp(fgets(buffer, 64, stdin), "exit\n") != 0);
    return;
}

// debug
void printGrid(Grid g) {
    // printf("Printing grid:\n");
    for (int j = 0; j < g.height; j++) {
        for (int i = 0; i < g.width; i++) {
            Cell c = getCell(g, i, j);
            printf("| %5f ", c.data);
        }
        printf("|\n\n");
    }
}

int main()
{
    Grid g = initGrid(192, 108);

    Grid op = initGrid(7, 7);

    Cell cell;
    cell.data = 1;

    srand(time(NULL));

    // Initialize g with random doubles in [0, 1)
    double tmp;
    for (int j = 0; j < g.height; j++) {
        for (int i = 0; i < g.width; i++) {

            tmp = rand();
            tmp = tmp / RAND_MAX;

            cell.data = tmp;
            setCell(&g, i, j, cell);
        }
    }
    commitGridUpdate(&g);


    // Initialize op with the inverse squared distance to center
    double centerX = (op.width-1) / 2; //[Federico] Non so se è intenzionale, ma qui e alla riga sotto, la divisione utilizzata è la divisione intera
    double centerY = (op.height-1) / 2;	//[Federico] Dovrebbe bastare dividere per 2.0
    for (int j = 0; j < op.height; j++) {
        for (int i = 0; i < op.width; i++) {
            double myDist = fabs(i-centerX)+fabs(j-centerY)+1;
            cell.data = pow( (1/myDist), 2);
            setCell(&op, i, j, cell);
        }
    }
    commitGridUpdate(&op);


/*
    // Initialize op with the x, y position of each cell
    double centerX = (op.width-1) / 2;
    double centerY = (op.height-1) / 2;
    for (int j = 0; j < op.height; j++) {
        for (int i = 0; i < op.width; i++) {
            cell.data = i+j;
            setCell(&op, i, j, cell);
        }
    }
    commitGridUpdate(&op);
*/

/*
    // Initialize op with a bottom-right-corner unbalanced distribution
    double centerX = (op.width-1) / 2;
    double centerY = (op.height-1) / 2;
    for (int j = 0; j < op.height; j++) {
        for (int i = 0; i < op.width; i++) {
            cell.data = 0;
            if (i > centerX && j > centerY) cell.data ++;
            setCell(&op, i, j, cell);
        }
    }
    commitGridUpdate(&op);
*/

    // Make op have a total value of 1
    printGrid(op);
    printf(" ----\n\n");
    applyRuleSetMass(&op, 1);
    printGrid(op);
    grid2PNG(op, "OP.png");

    // Convolve
    slideshowRuleConvolve(&g, op, "slideshow.png");

    grid2PNG(g, "G.png");


    destroyGrid(g);
    // destroyGrid(op);

    return 0;
}
