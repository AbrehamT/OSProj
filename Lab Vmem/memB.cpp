// CS 370 2D Memory Access Program

// Compilation:
//	gcc -Wall -pedantic -o prog prog.cpp

#include <sys/time.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

#define ROWS 526075		// formula:  (free * 200) / 4096
#define COLS 4096

// *********************************************************************

int main(int argc, char *argv[])
{
	int	cnt = 0;
	int	**arr = NULL;


	// allocate the 2D array
	arr = (int **)malloc(ROWS * sizeof(int *));
	for (int i=0; i<ROWS; i++)
		arr[i] = (int *)malloc(COLS * sizeof(int));

	// populate the array
	for (int c=0; c < COLS; c++)
		for (int r=0; r < ROWS; r++)
			arr[r][c] = cnt++;

	// deallocate the 2D array
	free(arr);

	return	0;
}

