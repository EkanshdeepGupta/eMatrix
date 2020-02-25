#include <ncurses.h>
#include <stdlib.h>
#include <iostream>
#include <ctime> // for random
#include <signal.h>

using namespace std;

#define PAUSE 50 //milliseconds
#define ROWFREQ 0.03 //frequency with which to start a new row
#define MINLENGTH 3 //parameters for length of vertical chain
#define MAXLENGTH 15
#define ASCIIMIN 35 //parameters for permitted ascii characters
#define ASCIIMAX 122

bool resizeFlag = false;

struct matrixChar {
	char label;
	int flag;
	/*
	flag == 0: neutral
	flag == 1: color white
	*/
};

void executeMatrix(bool * terminateFlag);

void handleResizing(int signal) {
    endwin();
    refresh();
	initscr();
	resizeFlag = true;
}

int nextStepMutation(matrixChar ** charArray, int * rowSpawn, int localLINES, int localCOLS); //localLINES and localCOLS to try to minimize segfault on resize
char randomChar();
int printArray(matrixChar ** charArray, int localLINES, int localCOLS);


int main(int argc, char ** argv)  {
	srand((unsigned) time(0));

	initscr();
	refresh();
	noecho();
	nodelay(stdscr,TRUE);
	curs_set(0);
	
	start_color();
	init_pair(1, COLOR_GREEN, COLOR_BLACK);
	init_pair(2, COLOR_WHITE, COLOR_BLACK);

	signal(SIGWINCH, handleResizing);

	bool terminateFlag = false;
	while(!terminateFlag){
		executeMatrix(&terminateFlag);
	}

	endwin();

	return 0;
}

void executeMatrix(bool * terminateFlag) {
	resizeFlag = false;
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

	while(!resizeFlag && !*terminateFlag) {
		int keyVal = getch();
		if (keyVal == KEY_RESIZE) {
			handleResizing(0);
			break;
		}
		else if (keyVal != ERR) {
			*terminateFlag = true;
			break;
		}
		nextStepMutation(charArray, rowSpawn, LINES, COLS);
		printArray(charArray, LINES, COLS);
		refresh();

		struct timespec tim;
		tim.tv_sec  = PAUSE / 1000;
		tim.tv_nsec = (PAUSE % 1000) * 1000000L;
		nanosleep(&tim, NULL);
	}

	// for (int i=0; i < LINES; i++) {
	// 	delete charArray[i];
	// }

	// delete charArray;
}

int nextStepMutation(matrixChar ** charArray, int * rowSpawn, int localLINES, int localCOLS) {
	for (int i=localLINES-1; i>0; i--) {
		for (int j=0; j<localCOLS; j++) {
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

	for (int j=0; j<localCOLS; j++) {
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
	int num = ASCIIMIN + (rand() % (ASCIIMAX - ASCIIMIN + 1));

	return (char) num;
}

int printArray(matrixChar ** charArray, int localLINES, int localCOLS) {
	for (int i=0; i<localLINES; i++) {
		for (int j=0; j<localCOLS; j++) {
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