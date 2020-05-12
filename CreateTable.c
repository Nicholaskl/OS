/*
 * File: createTable.c
 * File Created: Friday, 4th October 2019
 * Author: Nicholas Klvana-Hooper
 * -----
 * Last Modified: Monday, 21st October 2019
 * Modified By: Nicholas Klvana-Hooper
 * -----
 * Purpose: Reads settings file and deals with table freeing
 * Reference: readSettings based on Lecture 6 from UCP (Reading file)
              Accessed on the 21st of October 2019
 */
#include <stdio.h>
#include <stdlib.h>
#include "CreateTable.h"

/*
 * SUBMODULE: readSettings
 * IMPORT: argc(int), argv[](char), width(int*), height(int*), numMatch(int*)
 * EXPORT: void
 * ASSERTION: Opens the file for reading, checks for errors and closes file
 * REFERENCE: Submodule based on Lecture 6 from UCP (Reading File)
 */
void readSettings(int argc, char* argv[], int* width, int* height, int* numMatch)
{
    /* Opens file given from command-line */
    FILE* settFile = fopen(argv[1], "r");

    /* Checks if the file wasn't opened */
    if(settFile == NULL)
    {
        perror(ERRCLR "Error: could not open Settings File" CLR);
    }
    /* Otherwise read file and then close */
    else
    {
        /* reads file that was opened */
        readFile(settFile, width, height, numMatch);

        /* Checks if error while reading */
        if(ferror(settFile))
        {
            perror(ERRCLR "Error when reading from Settings\n" CLR);
        }
        /* otherwise close file */
        else
        {
            fclose(settFile);
        }
    }
}

/*
 * SUBMODULE: readFile
 * IMPORT: settFile(FILE*), width(int*), height(int*), numMatch(int*)
 * EXPORT: void
 * ASSERTION: Reads the settings within the file opened
 */
void readFile(FILE* settFile, int* width, int* height, int* numMatch)
{
    char line[5];
    int value;
    char setting;
    int i;

    /* There are 3 setting values to be read (M,N,K) */

    for(i = 0; i < 3; i++)
    {
        /* Gets a line and scans in values (ie. use format M=5) */
        fgets(line, 5, settFile);
        sscanf(line, "%c=%d", &setting, &value);

        /* Sets setting variable to the corresponding input */
        if(setting == 'M')
        {
            if(*height == -1)
            {
                *height = value;
            }
            else
            {
                *height = 1;
                printf(ERRCLR "Duplicate value for M found\n" CLR);
            }
        }
        else if(setting == 'N')
        {
            if(*width == -1)
            {
                *width = value;
            }
            else
            {
                *width = 1;
                printf(ERRCLR "Duplicate value for N found\n" CLR);
            }
        }
        else if(setting == 'K')
        {
            if(*numMatch == -1)
            {
                *numMatch = value;
            }
            else
            {
                *numMatch = 0;
                printf(ERRCLR "Duplicate value for K found\n" CLR);
            }
        }
        /* If not one of the characters needed outputs error msg */
        else
        {
            printf(ERRCLR "Wrong character found in Settings\n" CLR);
        }
    }

    /* Checks if any values are invalid and prints message */
    if(*height <= 0)
    {
        printf( ERRCLR "Error with height (M) value (could be missing)\n" CLR);
    }
    if(*width <= 0)
    {
        printf(ERRCLR "Error with width (N) value (could be missing)\n" CLR);
    }
    if(*numMatch < 0)
    {
        printf(ERRCLR "Error with number match (K) value (could be missing)\n" CLR);
    } /* Otherwise values are correct */
}

/*
 * SUBMODULE: freeTable
 * IMPORT: board(int**), width(int), height(int)
 * EXPORT: void
 * ASSERTION: Frees the 2D board array
 */
void freeTable(int** board, int width, int height)
{
    int i;

    /* Frees each row within the board array */
    for(i = 0; i < height; i++)
    {
        free(board[i]);
    }

    /* Frees the reamaining 1D array */
    free(board);
}
