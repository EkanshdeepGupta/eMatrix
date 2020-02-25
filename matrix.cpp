#include <ncurses.h>
#include <stdlib.h>
#include <iostream>
#include <ctime> // for random

using namespace std;

#define PAUSE 50 //milliseconds
#define ROWFREQ 0.03 //frequency with which to start a new row
#define MINLENGTH 3 //parameters for length of vertical chain
#define MAXLENGTH 15


// ascii values from 35 to 122 (inclusive)

struct matrixChar {
	char label;
	int flag;
	/*
	flag == 0: neutral
	flag == 1: color white
	*/
};

int nextStepMutation(matrixChar ** charArray, int * rowSpawn);
char randomChar();
int printArray(matrixChar ** charArray);

int main(int argc, char ** argv)  {
	srand((unsigned) time(0));

	initscr();
	refresh();
	noecho();
	start_color();
	init_pair(1, COLOR_GREEN, COLOR_BLACK);
	init_pair(2, COLOR_WHITE, COLOR_BLACK);

	curs_set(0);

	matrixChar **charArray;
	int rowSpawn[COLS];

	charArray = new matrixChar *[LINES];
	for (int i=0; i < LINES; i++) {

		charArray[i] = new matrixChar[COLS];

		for (int j=0; j<COLS; j++) {
			charArray[i][j].label = ' ';
			charArray[i][j].flag = 0;
		}
	}

	for (int j=0; j < COLS; j++) {
		rowSpawn[j] = 0;
	}

	while(true) {
		nextStepMutation(charArray, rowSpawn);
		printArray(charArray);
		refresh();

		struct timespec tim;
		tim.tv_sec  = PAUSE / 1000;
		tim.tv_nsec = (PAUSE % 1000) * 1000000L;
		nanosleep(&tim, NULL);
	}

	getch();
	endwin();

	return 0;
}

int nextStepMutation(matrixChar ** charArray, int * rowSpawn) {
	for (int i=LINES-1; i>0; i--) {
		for (int j=0; j<COLS; j++) {
			if (charArray[i-1][j].label != ' ' && charArray[i][j].label == ' ')	{
				charArray[i][j].label = randomChar();
				charArray[i][j].flag = 1;
				charArray[i-1][j].flag = 0;
			}

			if ((charArray[i-1][j].label == ' ') && (charArray[i][j].label != ' ')) {
				charArray[i][j].label = ' ';
				charArray[i][j].flag = 0;

			}
		}	
	}

	for (int j=0; j<COLS; j++) {
		//cerr << rowSpawn[j] << endl;
		if (rowSpawn[j] > 0) {
			rowSpawn[j]--;
		}

		if (rowSpawn[j] == 0) {
			double x = ((double) rand() / (RAND_MAX));
			if (x < ROWFREQ) {
				rowSpawn[j] = (int) ((double)(x * (MAXLENGTH - MINLENGTH + 1) / ROWFREQ) + MINLENGTH);
				charArray[0][j].label = randomChar();
				charArray[0][j].flag = 1;
			}
			else {
				charArray[0][j].label = ' ';
			}
		}
	}


	return 0;
}

char randomChar() {
	int num = 35 + (rand() % 88);

	return (char) num;
}

int printArray(matrixChar ** charArray) {
	for (int i=0; i<LINES; i++) {
		for (int j=0; j<COLS; j++) {
			if (charArray[i][j].label == ' ' || charArray[i][j].flag == 0) {
				attron(COLOR_PAIR(1));
				mvaddch(i, j, charArray[i][j].label);
				attroff(COLOR_PAIR(1));
			}
			else {
				attron(COLOR_PAIR(2));
				mvaddch(i, j, charArray[i][j].label);
				attroff(COLOR_PAIR(2));
				charArray[i][j].flag = 0;
			}
		}
	}

	return 0;
}