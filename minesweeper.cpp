#include <iostream>
#include <ctime>
#include <windows.h>
#include <iomanip>
#include <stdlib.h>
#include <chrono>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#define Closed_Cell 0
#define Exploded_Mine 1
#define Unchecked_Cell 2
#define Opened_Cell 3
#define Flag 4
#define Blank_Cell 0
#define Mine 9
using namespace std;

/*
Description:            A function that generates the game field (placement of safe and armed tiles)

Arguments:
    char** field        pointer to a pointer (allowing for the placement of tiles in the field)
    int row             rows of the field
    int col             columns of the field
    int mines           number of mines to be randomly placed in the field
    boolean autoplay    tells whether the game is on or not(true or false)

Returns:
    char** field        displays generated field
*/
char** gen_field(char** field, int row, int col, int mines, boolean autoplay) {
	int i, j, used_space = 0, skip = 0;
	field = new char* [row];
	for (i = 0; i < row; i++) {
		field[i] = new char[col];
		for (j = 0; j < col; j++)
			field[i][j] = Blank_Cell;
	}
	for (i = 0; i < row; i++) {
		for (j = 0; j < col; j++) {
			if (mines <= 0) {
				break;
			}
			if (field[i][j] == Mine) {
				skip++;
				continue;
			}
			if (rand() % ((row * col) - (i *col) - j - used_space + skip) == 0) { //ingenious random
				field[i][j] = Mine;
				mines--;
				used_space++;
				skip = i = 0;
				j = -1;
			}
		}
	}

	for (i = 0; i < row; i++) {
		for (j = 0; j < col; j++) {
			if (field[i][j] > 8) {
				if (i - 1 >= 0)
					field[i - 1][j]++;
				if (i + 1 < row)
					field[i + 1][j]++;
				if (j - 1 >= 0)
					field[i][j - 1]++;
				if (j + 1 < col)
					field[i][j + 1]++;
				if (i - 1 >= 0 && j - 1 >= 0)
					field[i - 1][j - 1]++;
				if (i - 1 >= 0 && j + 1 < col)
					field[i - 1][j + 1]++;
				if (i + 1 < row && j - 1 >= 0)
					field[i + 1][j - 1]++;
				if (i + 1 < row && j + 1 < col)
					field[i + 1][j + 1]++;
			}
		}
	}

	for (i = 0; i < row; i++) {
		for (j = 0; j < col; j++) {
			if (field[i][j] > 8)
				field[i][j] = Mine;
		}
	}
	return field;
}

/*
Description:            A function that checks the neighboring tiles (if open or unchecked)

Arguments:
    char** field        pointer to a pointer (allowing for the placement of tiles in the field)
    char** player_view  pointer to a pointer (players view of the tiles in the field)
    int check           counter if the tile is checked or not
    int i               tile placeholder for width
    int j               tile placeholder for length

Returns:
    int check           counts the checked or unchecked tiles
*/
int check_near_cell(char** field, char** player_view, int check, int i, int j) {
	if (field[i][j] == Blank_Cell && player_view[i][j] == Closed_Cell) {
		player_view[i][j] = Unchecked_Cell;
		check++;
	}
	if (field[i][j] > 0 && field[i][j] < 9 && player_view[i][j] == Closed_Cell) {
		player_view[i][j] = Opened_Cell;
	}
	return check;
}

/*
Description:            A function that checks the open space which works with the check_near_cell function

Arguments:
    char** field        pointer to a pointer (allowing for the placement of tiles in the field)
    char** player_view  pointer to a pointer (players view of the tiles in the field)
	int row             rows of the field
	int col             columns of the field
    int x               tile placeholder for width
    int y               tile placeholder for length

Returns:                None
*/
void open_space(char** field, char** player_view, int row, int col, int x, int y) {
	int i, j, check = 0, xDiv2 = x / 2;
	if (!(x % 2)) {
		if (y < row && xDiv2 < col) {
			if (player_view[y][xDiv2] == Closed_Cell && field[y][xDiv2] > 0 && field[y][xDiv2] < 9) {
				player_view[y][xDiv2] = Opened_Cell;
			}
			if (player_view[y][xDiv2] == Closed_Cell && field[y][xDiv2] == Blank_Cell) {
				player_view[y][xDiv2] = Unchecked_Cell;
				check++;
				for (i = 0; i < row; i++) {
					for (j = 0; j < col; j++) {
						if (player_view[i][j] == Unchecked_Cell) {
							player_view[i][j] = Opened_Cell;
							check--;
							if (i - 1 >= 0) {
								check = check_near_cell(field, player_view, check, i - 1, j);
							}
							if (i - 1 >= 0 && j - 1 >= 0) {
								check = check_near_cell(field, player_view, check, i - 1, j - 1);
							}
							if (j - 1 >= 0) {
								check = check_near_cell(field, player_view, check, i, j - 1);
							}
							if (i + 1 < row && j - 1 >= 0) {
								check = check_near_cell(field, player_view, check, i + 1, j - 1);
							}
							if (i + 1 < row) {
								check = check_near_cell(field, player_view, check, i + 1, j);
							}
							if (i + 1 < row && j + 1 < col) {
								check = check_near_cell(field, player_view, check, i + 1, j + 1);
							}
							if (j + 1 < col) {
								check = check_near_cell(field, player_view, check, i, j + 1);
							}
							if (i - 1 >= 0 && j + 1 < col) {
								check = check_near_cell(field, player_view, check, i - 1, j + 1);
							}
							if (check > 0)
								i = 0;
							j = -1;
						}
					}
				}
			}
			if (field[y][xDiv2] == Mine && player_view[y][xDiv2] != Flag)
				player_view[y][xDiv2] = Exploded_Mine;
		}
	}
}

/*
Description:            A function that enters the time used per player to be used in the leaderboard

Arguments:
	string name			name of the player
	int level			level of difficulty chosen by the player
	double time_used	elapsed time of the player

Returns:				None
*/
void leaderboard(string name, int level, double time_used)
{
    if (level == 1)
    {

        ofstream leaderboard("beginner.txt", fstream::app);

        if (leaderboard.is_open())
        {
            leaderboard << endl << time_used << "seconds    " << name;

        }
        leaderboard.close();

        return;
    }
    else if (level == 2)
    {
        ofstream leaderboard("intermediate.txt", fstream::app);

        if (leaderboard.is_open())
        {
            leaderboard << endl << time_used << " seconds    " << name;

        }
        leaderboard.close();

        return;
    }
    else if (level == 3)
    {
        ofstream leaderboard("expert.txt", fstream::app);

        if (leaderboard.is_open())
        {
            leaderboard << endl << time_used << " seconds    " << name;

        }
        leaderboard.close();

        return;
    }
}

/*
Description:            A function for that updates the leaderboard

Arguments:
    int level           level of difficulty selected by the player
	int scores			scores of the players based on time used

Returns:				None
*/
void updatehighscore(int level, int scores) {
  if (level == 1)
  {
      string line;
      vector <string> scores;
      ifstream leaderboard("beginner.txt");
      if (leaderboard.is_open())
      {
          while (getline(leaderboard,line))
          {
              scores.push_back(line);
          }
          leaderboard.close();
          sort(scores.begin(), scores.end());

		  cout << "|---------LEADERBOARD---------|" << endl;
		  cout << "|-----TIME-----|-----NAME-----|";
          for (int i=0; i < scores.size(); i++)
          {
              cout << scores[i] << endl;
          }
      }
      return;
  }
  else if (level == 2)
  {
      string line;
      vector <string> scores;
      ifstream leaderboard("intermediate.txt");
      if (leaderboard.is_open())
      {
          while (getline(leaderboard,line))
          {
              scores.push_back(line);
          }
          leaderboard.close();
          sort(scores.begin(), scores.end());

		  cout << "|---------LEADERBOARD---------|" << endl;
		  cout << "|-----TIME-----|-----NAME-----|";
          for (int i=0; i < scores.size(); i++)
          {
              cout << scores[i] << endl;
          }
      }
      return;
  }
  else if (level == 3)
  {
      string line;
      vector <string> scores;
      ifstream leaderboard("expert.txt");
      if (leaderboard.is_open())
      {
          while (getline(leaderboard,line))
          {
              scores.push_back(line);
          }
          leaderboard.close();
          sort(scores.begin(), scores.end());

		  cout << "|---------LEADERBOARD---------|" << endl;
		  cout << "|-----TIME-----|-----NAME-----|";
          for (int i=0; i < scores.size(); i++)
          {
              cout << scores[i] << endl;
          }
      }
      return;
  }


}

/*
Description:            A function that controls the game (almost all game functions are implemented here)

Arguments:
    char** field        pointer to a pointer (allowing for the placement of tiles in the field)
	int level			game difficulty (level) chosen by the player
	int row             rows of the field
	int col             columns of the field
    int mines           number of mines to be randomly placed in the field
    boolean autoplay    tells whether the game is on or not(true or false)

Returns:                None
*/
void play(char** field, int level, int row, int col, int mines, boolean autoplay) {
    clock_t start, end;
    double time_used;
    string name;
    int scores;
	HANDLE hIn, hOut;
	INPUT_RECORD InRec;
	DWORD NumRead;
	CONSOLE_CURSOR_INFO Curs;
	CONSOLE_SCREEN_BUFFER_INFO Buff;
	COORD zero = { 0,0 };
	boolean game_over = false, win = false;
	int flag = 0, clean = 2, x, y, i, j, input = 1, marked_mines = 0, opened_cells = 0, mines_at_beginning = mines, xDiv2;
	char** player_view = new char* [row];
	cout << "Enter name: ";
	cin >> name;
	for (i = 0; i < row; i++) {
		player_view[i] = new char[col];
		for (j = 0; j < col; j++)
			player_view[i][j] = Closed_Cell; //0-closed field, 3-open field
	}
	for (i = mines; (i /= 10) > 0;)
		clean++;
	system("cls");
	hIn = GetStdHandle(STD_INPUT_HANDLE);
	hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	GetConsoleCursorInfo(hOut, &Curs);
	SetConsoleMode(hIn, ENABLE_MOUSE_INPUT | ENABLE_EXTENDED_FLAGS);
	start = clock();
	while (!game_over) {
		Curs.bVisible = false;
		SetConsoleCursorInfo(hOut, &Curs);
		SetConsoleCursorPosition(hOut, zero);
		for (i = 0; i < row; i++) {
			for (j = 0; j < col; j++) {
				switch (player_view[i][j]) {
				case Closed_Cell: {
					SetConsoleTextAttribute(hOut, (WORD)(0 | 8));
					WriteConsoleW(hOut, L"■ ", wcslen(L"■ "), NULL, NULL);
					SetConsoleTextAttribute(hOut, (WORD)(0 | 15));
					break;
				}
				//mines
				case Exploded_Mine: {
					SetConsoleTextAttribute(hOut, (WORD)(0 | 4));
					WriteConsoleW(hOut, L"☼ ", wcslen(L"☼ "), NULL, NULL);
					SetConsoleTextAttribute(hOut, (WORD)(0 | 15));
					game_over = true;
					break;
				}
				//safe tiles (blank or with numbers)
				case Opened_Cell: {
					opened_cells++;
					switch (field[i][j]) {
					case Blank_Cell:{
						SetConsoleTextAttribute(hOut, (WORD)(0 | 8));
						WriteConsoleW(hOut, L"□ ", wcslen(L"□ "), NULL, NULL);
						SetConsoleTextAttribute(hOut, (WORD)(0 | 15));
						break;
					}
					case 1: {
						SetConsoleTextAttribute(hOut, (WORD)(0 | 9));
						cout << "1 ";
						SetConsoleTextAttribute(hOut, (WORD)(0 | 15));
						break;
					}
					case 2: {
						SetConsoleTextAttribute(hOut, (WORD)(0 | 10));
						cout << "2 ";
						SetConsoleTextAttribute(hOut, (WORD)(0 | 15));
						break;
					}
					case 3: {
						SetConsoleTextAttribute(hOut, (WORD)(0 | 12));
						cout << "3 ";
						SetConsoleTextAttribute(hOut, (WORD)(0 | 15));
						break;
					}
					case 4: {
						SetConsoleTextAttribute(hOut, (WORD)(0 | 5));
						cout << "4 ";
						SetConsoleTextAttribute(hOut, (WORD)(0 | 15));
						break;
					}
					case 5: {
						SetConsoleTextAttribute(hOut, (WORD)(0 | 4));
						cout << "5 ";
						SetConsoleTextAttribute(hOut, (WORD)(0 | 15));
						break;
					}
					case 6: {
						SetConsoleTextAttribute(hOut, (WORD)(0 | 1));
						cout << "6 ";
						SetConsoleTextAttribute(hOut, (WORD)(0 | 15));
						break;
					}
					case 7: {
						SetConsoleTextAttribute(hOut, (WORD)(0 | 2));
						cout << "7 ";
						SetConsoleTextAttribute(hOut, (WORD)(0 | 15));
						break;
					}
					case 8: {
						SetConsoleTextAttribute(hOut, (WORD)(0 | 8));
						cout << "8 ";
						SetConsoleTextAttribute(hOut, (WORD)(0 | 15));
						break;
					}
					case Mine: {
						SetConsoleTextAttribute(hOut, (WORD)(0 | 15));
						WriteConsoleW(hOut, L"☼ ", wcslen(L"☼ "), NULL, NULL);
					}
					}
					break;
				}
				//placing of flags
				case Flag: {
					SetConsoleTextAttribute(hOut, (WORD)(0 | 12));
					WriteConsoleW(hOut, L"⚐ ", wcslen(L"⚐ "), NULL, NULL);
					SetConsoleTextAttribute(hOut, (WORD)(0 | 15));
					if (field[i][j] == 9)
						marked_mines++;
					else
						marked_mines--;
				}
				}
			}
			cout << endl;
		}
		// mines at the beginning
		if (marked_mines == mines_at_beginning || opened_cells >= (row * col - mines_at_beginning))
			win = game_over = true;
		opened_cells = 0;
		marked_mines = 0;
		if (!game_over){
            cout << "\nMines left ";
            for (i = clean; i != 0; i--)
                cout << ' ';
            GetConsoleScreenBufferInfo(hOut, &Buff);
            Buff.dwCursorPosition.X -= clean;
            SetConsoleCursorPosition(hOut, Buff.dwCursorPosition);
            cout << mines;
            cout << "\ntype 0 to exit\n>";
		}
		Curs.bVisible = true;
		SetConsoleCursorInfo(hOut, &Curs);
		//left click -> game play
		//right click -> place flag
		if (!autoplay) {
			ReadConsoleInput(hIn, &InRec, 1, &NumRead);
			switch (InRec.EventType) {
			case MOUSE_EVENT: {
				if (InRec.Event.MouseEvent.dwButtonState == FROM_LEFT_1ST_BUTTON_PRESSED) {
					x = InRec.Event.MouseEvent.dwMousePosition.X;
					y = InRec.Event.MouseEvent.dwMousePosition.Y;
					open_space(field, player_view, row, col, x, y);
				}
				if (InRec.Event.MouseEvent.dwButtonState == RIGHTMOST_BUTTON_PRESSED) {
					x = InRec.Event.MouseEvent.dwMousePosition.X;
					y = InRec.Event.MouseEvent.dwMousePosition.Y;
					xDiv2 = x / 2;
					if (y < row && xDiv2 < col && !(x % 2)) {
						if (player_view[y][xDiv2] == Closed_Cell)
							flag = 1;
						if (player_view[y][xDiv2] == Flag)
							flag = 2;
						if (flag == 1) {
							player_view[y][xDiv2] = Flag;
							mines--;
						}
						if (flag == 2) {
							player_view[y][xDiv2] = Closed_Cell;
							mines++;
						}
						flag = 0;
					}
				}
				break;
			}
			case KEY_EVENT: {
				if (InRec.Event.KeyEvent.uChar.AsciiChar == '0') {
					game_over = true;
					cout << "0\n";
				}
			}
			}
		}
		else {
			y = rand() % row;
			x = rand() % (2 * col);
			open_space(field, player_view, row, col, x, y);
		}

	}
	if (game_over) {
		GetConsoleScreenBufferInfo(hOut, &Buff);
		Buff.dwCursorPosition.X--;
		Buff.dwCursorPosition.Y--;
		SetConsoleCursorPosition(hOut, Buff.dwCursorPosition);
		if (!win) {
			SetConsoleTextAttribute(hOut, (WORD)(0 | 4));
			cout << "GAME OVER     \n";
			cout << "Mines left: " << mines - 1 << endl;
			end = clock();
			time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
			cout << "Time taken : " << time_used << " seconds" << endl;
			SetConsoleTextAttribute(hOut, (WORD)(0 | 15));
			cout << "Press any button to exit...";
			do {
				ReadConsoleInput(hIn, &InRec, 1, &NumRead);
			} while (InRec.EventType != KEY_EVENT);

		}


		if (win) {
			SetConsoleTextAttribute(hOut, (WORD)(0 | 10));
			cout << "YOU WIN       \n";
			end = clock();
			time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
			cout << "Time taken : " << time_used << " seconds" << endl;
			leaderboard(name, level, time_used);;
			updatehighscore(level, scores);

		}
		SetConsoleTextAttribute(hOut, (WORD)(0 | 15));
		cout << "Press any button to exit...";
		do {
			ReadConsoleInput(hIn, &InRec, 1, &NumRead);
		} while (InRec.EventType != KEY_EVENT);
	}
	system("cls");
}

/*
Description:            Main function for the flow of the game
Arguments:
	int argc			counts the command line arguments passed by the users
	char* argv[]		an array (of undetermined size) of pointers to char for command line arguments
Returns:                None
*/
int main(int argc, char* argv[]) {
	srand(time(NULL));
	int input, nRow, nCol, level, mines;
	char** field = new char*;
	boolean field_was_generated = false;
	if (argc > 1) {
		nRow = atoi(argv[2]);
		nCol = atoi(argv[4]);
		mines = atoi(argv[6]);
		field = gen_field(field, nRow, nCol, mines, false);
		play(field, 0, nRow, nCol, mines, false);
	}
	else {
		do {
			cout << "1.play\n0.exit\n>";
			cin >> input;
			cout << '\n';
			switch (input) {
			case 1: {
				cout << "Choose Level Difficulty\n1.Beginner(9x9 10 mines)\n2.Intermediate(16x16 40mines)\n3.Expert(16x30 99mines)\n0.Go Back\n>";
				cin >> level;
				cout << '\n';
				switch (level) {
				case 1: {
					field = gen_field(field, 9, 9, 10, false);
					play(field, 1, 9, 9, 10, false);
					continue;
				}
				case 2: {
					field = gen_field(field, 16, 16, 40,false);
					play(field, 2, 16, 16, 40, false);
					continue;
				}
				case 3: {
					field = gen_field(field, 16, 30, 99, false);
					play(field, 3, 16, 30, 99, false);
					continue;
				}
				default: {
					level = 0;
					continue;
				}
				}
			}
			case 2: {
				nRow = rand() % 48 + 1;
				nCol = rand() % 105 + 1;
				mines = rand() % (nRow * nCol);
				field = gen_field(field, nRow, nCol, mines, true);
				play(field, level, nRow, nCol, mines, true);
				break;
			}
			default:input = 0;
			}
		} while (input != 0);
	}
}
