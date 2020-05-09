#ifndef CREATETABLE_H
#define CREATETABLE_H

void readSettings(int argc, char* argv[], int* width, int* height, int* numMatch);
void readFile(FILE* settFile, int* width, int* height, int* numMatch);
void freeTable(int** board, int width, int height);

#endif
