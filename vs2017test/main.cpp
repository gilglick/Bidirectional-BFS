#include "glut.h"
#include <time.h>
#include <vector>
#include <iostream>
#include "Node.h"

using namespace std;

const int W = 600; // window width
const int H = 600; // window height

const int MSZ = 100;
const int WALL = 1;
const int SPACE = 2;
const int START = 3;
const int PATH = 5;
const int GRAY_S = 6;
const int BLACK_SPACE = 7;
const int TARGET = 8;
const int GRAY_T = 9;


int maze[MSZ][MSZ];

bool start_BFS = false;

vector <Node*> gray_s;
vector <Node*> gray_t;


void SetupMaze();

void init()
{
	//    RED, GREEN, BLUE
	glClearColor(0.8, 0, 0, 0);// color of window background

	srand(time(0));
	// define main axes
	glOrtho(-1, 1, -1, 1, -1, 1);

	SetupMaze();
}

void SetupMaze()
{
	int i, j;
	// set borders

	for (i = 0; i < MSZ; i++)
	{
		maze[i][0] = WALL;
		maze[i][MSZ - 1] = WALL;
		maze[0][i] = WALL;
		maze[MSZ - 1][i] = WALL;
	}

	for (i = 1; i < MSZ - 1; i++)
	{
		for (j = 1; j < MSZ - 1; j++)
		{
			if (i % 2 == 1) // mostly SPACEs (90%)
			{
				if (rand() % 10 > 0) maze[i][j] = SPACE;
				else maze[i][j] = WALL;
			}
			else // mostly WALLs (70%)
			{
				if (rand() % 10 > 2)maze[i][j] = WALL;
				else maze[i][j] = SPACE;
			}
		}
	}
	// set start point
	Node* start = new Node(MSZ / 2, MSZ / 2, nullptr);
	maze[start->GetRow()][start->GetCol()] = START;
	// set target point
	Node* target = new Node(rand() % MSZ, rand() % MSZ, nullptr);
	maze[target->GetRow()][target->GetCol()] = TARGET;

	// insert to gray_s the START Node
	gray_s.push_back(start);
	// insert to gray_t the TARGET Node
	gray_t.push_back(target);
}

void DrawMaze()
{
	int i, j;
	double dx, dy, x, y;

	dx = dy = 2.0 / MSZ; // 2.0 is the logical width and height of the window

	y = -1;
	for (i = 0; i < MSZ; i++, y += dy)
	{
		x = -1;
		for (j = 0; j < MSZ; j++, x += dx)
		{
			switch (maze[i][j]) {
			case WALL:
				glColor3d(0.2, 0.2, 0.2);
				break;
			case SPACE:
				glColor3d(1, 1, 1);
				break;
			case START:
				glColor3d(0.2, 0.4, 1);
				break;
			case TARGET:
				glColor3d(1, 0, 0);
				break;
			case BLACK_SPACE:
				glColor3d(1, 1, 0);
				break;
			case GRAY_S:
				glColor3d(0, 0.8, 0);
				break;
			case GRAY_T:
				glColor3d(0, 0.8, 0);
				break;
			case PATH:
				glColor3d(1, 0.5, 1);
				break;



			}// switch
			// draw one square in maze
			glBegin(GL_POLYGON);
			glVertex2d(x, y);
			glVertex2d(x, y + dy);
			glVertex2d(x + dx, y + dy);
			glVertex2d(x + dx, y);
			glEnd();
		}
	}
}

void BuildPath_s(Node* pCurrent)
{
	while (maze[pCurrent->GetRow()][pCurrent->GetCol()] != START)
	{
		maze[pCurrent->GetRow()][pCurrent->GetCol()] = PATH;
		pCurrent = pCurrent->GetParent();
	}
}

void BuildPath_t(Node* pCurrent)
{
	while (maze[pCurrent->GetRow()][pCurrent->GetCol()] != TARGET)
	{
		maze[pCurrent->GetRow()][pCurrent->GetCol()] = PATH;
		pCurrent = pCurrent->GetParent();
	}
}

void BFSIteration()
{
	Node* pCurrent_s;
	Node* pCurrent_t;

	Node* next;


	if (gray_s.empty() || gray_t.empty()) // there is  no solution
	{
		cout << "there is  no solution\n";
		start_BFS = false;
	}
	else
	{
		// 1. extract first element from gray
		pCurrent_s = gray_s[0];
		pCurrent_t = gray_t[0];
		gray_s.erase(gray_s.begin());
		gray_t.erase(gray_t.begin());

		// 2. paint pCurrent black
		if (maze[pCurrent_s->GetRow()][pCurrent_s->GetCol()] != START)
			maze[pCurrent_s->GetRow()][pCurrent_s->GetCol()] = BLACK_SPACE;
		if (maze[pCurrent_t->GetRow()][pCurrent_t->GetCol()] != TARGET)
			maze[pCurrent_t->GetRow()][pCurrent_t->GetCol()] = BLACK_SPACE;
		// 3. Add white neighbours of pCurrent to gray
		// check up
		if (maze[pCurrent_s->GetRow() + 1][pCurrent_s->GetCol()] == SPACE) // SPACE means that it is WHITE
		{
			maze[pCurrent_s->GetRow() + 1][pCurrent_s->GetCol()] = GRAY_S;
			next = new Node(pCurrent_s->GetRow() + 1, pCurrent_s->GetCol(), pCurrent_s); // create it
			gray_s.push_back(next);
		}
		else if (maze[pCurrent_s->GetRow() + 1][pCurrent_s->GetCol()] == GRAY_T) {
			cout << "the solution has been found\n";
			start_BFS = false;
			BuildPath_s(pCurrent_s);
			while (pCurrent_t->GetRow() != pCurrent_s->GetRow() + 1 || pCurrent_t->GetCol() != pCurrent_s->GetCol()) {
				pCurrent_t = gray_t[0];
				gray_t.erase(gray_t.begin());
			}
			BuildPath_t(pCurrent_t);
		}
		if (maze[pCurrent_t->GetRow() + 1][pCurrent_t->GetCol()] == SPACE) // SPACE means that it is WHITE
		{
			maze[pCurrent_t->GetRow() + 1][pCurrent_t->GetCol()] = GRAY_T;
			next = new Node(pCurrent_t->GetRow() + 1, pCurrent_t->GetCol(), pCurrent_t); // create it
			gray_t.push_back(next);
		}
		else if (maze[pCurrent_t->GetRow() + 1][pCurrent_t->GetCol()] == GRAY_S) {
			cout << "the solution has been found\n";
			start_BFS = false;
			BuildPath_t(pCurrent_t);
			while (pCurrent_s->GetRow() != pCurrent_t->GetRow() + 1 || pCurrent_t->GetCol() != pCurrent_s->GetCol()) {
				pCurrent_s = gray_s[0];
				gray_s.erase(gray_s.begin());
			}
			BuildPath_s(pCurrent_s);
		}


		// check right
		if (maze[pCurrent_s->GetRow()][pCurrent_s->GetCol() + 1] == SPACE) // SPACE means that it is WHITE
		{
			maze[pCurrent_s->GetRow()][pCurrent_s->GetCol() + 1] = GRAY_S;
			next = new Node(pCurrent_s->GetRow(), pCurrent_s->GetCol() + 1, pCurrent_s); // create it
			gray_s.push_back(next);
		}
		else if (maze[pCurrent_s->GetRow()][pCurrent_s->GetCol() + 1] == GRAY_T) {
			cout << "the solution has been found\n";
			start_BFS = false;
			BuildPath_s(pCurrent_s);
			while (pCurrent_t->GetRow() != pCurrent_s->GetRow() || pCurrent_t->GetCol() != pCurrent_s->GetCol() + 1) {
				pCurrent_t = gray_t[0];
				gray_t.erase(gray_t.begin());
			}
			BuildPath_t(pCurrent_t);
		}
		if (maze[pCurrent_t->GetRow()][pCurrent_t->GetCol() + 1] == SPACE) // SPACE means that it is WHITE
		{
			maze[pCurrent_t->GetRow()][pCurrent_t->GetCol() + 1] = GRAY_T;
			next = new Node(pCurrent_t->GetRow(), pCurrent_t->GetCol() + 1, pCurrent_t); // create it
			gray_t.push_back(next);
		}
		else if (maze[pCurrent_t->GetRow()][pCurrent_t->GetCol() + 1] == GRAY_S) {
			cout << "the solution has been found\n";
			start_BFS = false;
			BuildPath_t(pCurrent_t);

			while (pCurrent_s->GetRow() != pCurrent_t->GetRow() || pCurrent_t->GetCol() + 1 != pCurrent_s->GetCol()) {
				pCurrent_s = gray_s[0];
				gray_s.erase(gray_s.begin());
			}
			BuildPath_s(pCurrent_s);
		}

		// check down
		if (maze[pCurrent_s->GetRow() - 1][pCurrent_s->GetCol()] == SPACE) // SPACE means that it is WHITE
		{
			maze[pCurrent_s->GetRow() - 1][pCurrent_s->GetCol()] = GRAY_S;
			next = new Node(pCurrent_s->GetRow() - 1, pCurrent_s->GetCol(), pCurrent_s); // create it
			gray_s.push_back(next);
		}
		else if (maze[pCurrent_s->GetRow() - 1][pCurrent_s->GetCol()] == GRAY_T) {
			cout << "the solution has been found\n";
			start_BFS = false;
			BuildPath_s(pCurrent_s);
			while (pCurrent_t->GetRow() != pCurrent_s->GetRow() - 1 || pCurrent_t->GetCol() != pCurrent_s->GetCol()) {
				pCurrent_t = gray_t[0];
				gray_t.erase(gray_t.begin());
			}
			BuildPath_t(pCurrent_t);
		}
		if (maze[pCurrent_t->GetRow() - 1][pCurrent_t->GetCol()] == SPACE) // SPACE means that it is WHITE
		{
			maze[pCurrent_t->GetRow() - 1][pCurrent_t->GetCol()] = GRAY_T;
			next = new Node(pCurrent_t->GetRow() - 1, pCurrent_t->GetCol(), pCurrent_t); // create it
			gray_t.push_back(next);
		}
		else if (maze[pCurrent_t->GetRow() - 1][pCurrent_t->GetCol()] == GRAY_S) {
			cout << "the solution has been found\n";
			start_BFS = false;
			BuildPath_t(pCurrent_t);
			while (pCurrent_s->GetRow() != pCurrent_t->GetRow() - 1 || pCurrent_s->GetCol() != pCurrent_t->GetCol()) {
				pCurrent_s = gray_s[0];
				gray_s.erase(gray_s.begin());
			}
			BuildPath_s(pCurrent_s);
		}

		// check left
		if (maze[pCurrent_s->GetRow()][pCurrent_s->GetCol() - 1] == SPACE) // SPACE means that it is WHITE
		{
			maze[pCurrent_s->GetRow()][pCurrent_s->GetCol() - 1] = GRAY_S;
			next = new Node(pCurrent_s->GetRow(), pCurrent_s->GetCol() - 1, pCurrent_s); // create it
			gray_s.push_back(next);
		}
		else if (maze[pCurrent_s->GetRow()][pCurrent_s->GetCol() - 1] == GRAY_T) {
			cout << "the solution has been found\n";
			start_BFS = false;
			BuildPath_s(pCurrent_s);
			while (pCurrent_t->GetRow() != pCurrent_s->GetRow() || pCurrent_t->GetCol() != pCurrent_s->GetCol() - 1) {
				pCurrent_t = gray_t[0];
				gray_t.erase(gray_t.begin());
			}
			BuildPath_t(pCurrent_t);
		}
		if (maze[pCurrent_t->GetRow()][pCurrent_t->GetCol() - 1] == SPACE) // SPACE means that it is WHITE
		{
			maze[pCurrent_t->GetRow()][pCurrent_t->GetCol() - 1] = GRAY_T;
			next = new Node(pCurrent_t->GetRow(), pCurrent_t->GetCol() - 1, pCurrent_t); // create it
			gray_t.push_back(next);
		}
		else if (maze[pCurrent_t->GetRow()][pCurrent_t->GetCol() - 1] == GRAY_S) {
			cout << "the solution has been found\n";
			start_BFS = false;
			BuildPath_t(pCurrent_t);
			while (pCurrent_s->GetRow() != pCurrent_t->GetRow() || pCurrent_t->GetCol() - 1 != pCurrent_s->GetCol()) {
				pCurrent_s = gray_s[0];
				gray_s.erase(gray_s.begin());
			}
			BuildPath_s(pCurrent_s);
		}
	}
}


void display()
{
	glClear(GL_COLOR_BUFFER_BIT); // clean frame buffer

	DrawMaze();

	glutSwapBuffers(); // show all
}

void idle()
{
	if (start_BFS)
		BFSIteration();
	glutPostRedisplay(); // call to display again
}

void menu(int choice)
{
	if (choice == 1) // BFS
	{
		start_BFS = true;
	}

}


void main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(W, H);
	glutInitWindowPosition(200, 100);
	glutCreateWindow("HW1_Gil Glick");

	glutDisplayFunc(display); // refresh window function
	glutIdleFunc(idle); // kind of timer function

	glutCreateMenu(menu);
	glutAddMenuEntry("BFS", 1);
	glutAttachMenu(GLUT_RIGHT_BUTTON);

	init();

	glutMainLoop();
}