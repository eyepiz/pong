// pong.c
// Eduardo Yepiz
// eyepiz@nd.edu
//
// This program is a remake of pong. First to 5 wins. There are
// 2 modes: obstacles vs. no obstacles. The user configures this
// in the start-up menu. The player on the left moves up and down
// with the W & S keys. The player on the right moves up and down
// with the K & I keys. Have fun!

#include "gfx2.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

// for the ball
typedef struct {
	int rad;
	int xc;
	int yc;
	int vx;
	int vy;
} Ball;

// for each "paddle"
typedef struct {
	int w;
	int h;
	int xc;
	int yc;
	int vy;
	int score;
} Rect;

// for each obstacle
typedef struct {
	int w;
	int h;
	int xc;
	int yc;
	int vy;
} Block;

// function prototypes
void init_values(Ball *, Rect *, Rect *, int, int);
void draw_objects(Ball, Rect, Rect);
void move_ball(Ball *);
void bounce_off(Ball *, Rect *, Rect *, int, int);
void draw_rectangle(int, int, int, int);
void move_rectangles(Ball, Rect *, Rect *, char, int);
void display_score(Rect, Rect, char [], char [], int, int);
bool display_menu(int, int, bool *);
void random_color(void);
bool display_winner(Rect, Rect, int, int);
int random_posneg(int);
void increase_speed(Ball *, Rect *, Rect *, int);
void init_obstacles(Block *, Block *, int, int);
void draw_obstacles(Block, Block);
void move_obstacles(Block *, Block *, int);
void bounce_obstacles(Ball *, Block *, Block *, int);

int main()
{
	int wd = 1000, ht = 800, pause = 20000, mrgn = 40;
	bool game = true, menu = true, blocks = false;
	char player1[20], player2[20];
	Rect leftpad, rightpad;		// the two paddles
	Block block1, block2;		// the two obstacles
	Ball theball;		// the ball
	int ct = 0;
	char c;

	srand(time(0));		// randomize the randomizer

	// initialize values (velocities, coordinates, dimensions, etc.)
	init_values(&theball, &leftpad, &rightpad, wd, ht);

	gfx_open(wd, ht, "Project");
	random_color();

	while (game) {
		while (menu) {
			// start by displaying the main menu
			menu = display_menu(wd, ht, &blocks);
			if (blocks)
				// initialize the obstacles' values when needed
				init_obstacles(&block1, &block2, wd, ht);
		}

		gfx_clear();
		// draw the obstacles when necessary
		if (blocks)
			draw_obstacles(block1, block2);
		// draw the ball and the paddles
		draw_objects(theball, leftpad, rightpad);
		// display each player's score
		display_score(leftpad, rightpad, player1, player2, wd, mrgn);
		gfx_flush();
		usleep(pause);

		// do this only if obstacles were enabled
		if (blocks) {
			// move the obstacles up and down
			move_obstacles(&block1, &block2, ht);
			// bounce off of the obstacles if necessary
			bounce_obstacles(&theball, &block1, &block2, wd);
		}

		move_ball(&theball);	// move the ball
		// "bounce" off the wall or paddle when necessary
		bounce_off(&theball, &leftpad, &rightpad, wd, ht);

		ct++;
		// increase the speed of the ball and the sensitivity of the
		// paddles
		increase_speed(&theball, &leftpad, &rightpad, ct);

		if (gfx_event_waiting()) {
			c = gfx_wait();
			// respond and move the paddle according to user input
			move_rectangles(theball, &leftpad, &rightpad, c, ht);
			if (c == 'q') game = false;
		}

		// first to five wins -- display the winning message
		if (leftpad.score == 5 || rightpad.score == 5) {
			game = display_winner(leftpad, rightpad, wd, ht);
		}
	}

	return 0;
}

// initialize ball and paddle values (velocities, coordinates, dimensions, etc.)
void init_values(Ball *theball, Rect *leftpad, Rect *rightpad, int wd, int ht)
{
	theball->rad = 10;
	theball->xc = wd / 2;
	theball->yc = ht / 2;
	theball->vx = 6;
	theball->vy = 2;
	// randomize whether it goes up, down, left, or right
	theball->vx = random_posneg(theball->vx);
	theball->vy = random_posneg(theball->vy);

	leftpad->w = 10;
	leftpad->h = 40;
	leftpad->xc = leftpad->w;
	leftpad->yc = ht/2;
	leftpad->vy = 30;
	leftpad->score = 0;

	rightpad->w = leftpad->w;
	rightpad->h = leftpad->h;
	rightpad->xc = wd - rightpad->w - 1;
	rightpad->yc = leftpad->yc;
	rightpad->vy = leftpad->vy;
	rightpad->score = 0;
}

// draw the circle and two "paddles"
void draw_objects(Ball theball, Rect leftpad, Rect rightpad)
{
	gfx_circle(theball.xc, theball.yc, theball.rad);
	draw_rectangle(leftpad.xc, leftpad.yc, leftpad.w, leftpad.h);
	draw_rectangle(rightpad.xc, rightpad.yc, rightpad.w, rightpad.h);
}

// move the ball up, down, left, or right
void move_ball(Ball *theball)
{
	theball->xc += theball->vx;
	theball->yc += theball->vy;
}

// bounce the ball whenever needed
void bounce_off(Ball *theball, Rect *leftpad, Rect *rightpad, int wd, int ht)
{
	// bounce it off the bottom
	if ((theball->yc + theball->rad) > ht) {
		theball->vy = -theball->vy;
		theball->yc = ht - theball->rad;
	}
	// bounce it off the top
	if ((theball->yc - theball->rad) < 0) {
		theball->vy = -theball->vy;
		theball->yc = theball->rad;
	}

	// bounce it off the left paddle
	if ((theball->xc - theball->rad) <= (leftpad->xc + leftpad->w)) {
		if ((theball->yc < (leftpad->yc + leftpad->h)) && (theball->yc > (leftpad->yc - leftpad->h))) {
			theball->vx = -theball->vx;
			// theball->vy = 2;
			theball->vy = random_posneg(theball->vy);
			theball->xc = leftpad->xc + leftpad->w + theball->rad;
			random_color();
		}
	}
	// bounce it off the right paddle
	if ((theball->xc + theball->rad) >= (rightpad->xc - rightpad->w)) {
		if ((theball->yc < (rightpad->yc + rightpad->h)) && (theball->yc > (rightpad->yc - rightpad->h))) {
			theball->vx = -theball->vx;
			// theball->vy = 2;
			theball->vy = random_posneg(theball->vy);
			theball->xc = rightpad->xc - rightpad->w - theball->rad;
			random_color();
		}
	}

	// return the ball to the middle
	if (theball->xc > wd) {
		theball->xc = wd / 2;
		theball->yc = ht / 2;
		// randomize whether it goes up, down, left, or right
		theball->vx = random_posneg(theball->vx);
		theball->vy = 2;
		theball->vy = random_posneg(theball->vy);
		(leftpad->score)++;		// increment the score
	}
	// repeat for this case
	if (theball->xc < 0) {
		theball->xc = wd / 2;
		theball->yc = ht / 2;
		theball->vx = random_posneg(theball->vx);
		theball->vy = 2;
		theball->vy = random_posneg(theball->vy);
		(rightpad->score)++;
	}
}

// draw rectangle with x-y coordinates at the center
void draw_rectangle(int x, int y, int w, int h)
{
	gfx_line(x+w, y+h, x+w, y-h);
	gfx_line(x+w, y-h, x-w, y-h);
	gfx_line(x-w, y-h, x-w, y+h);
	gfx_line(x-w, y+h, x+w, y+h);
}

// move the paddles according to user input
void move_rectangles(Ball theball, Rect *leftpad, Rect *rightpad, char c, int ht)
{
	// left paddle case
	// check that the paddle stays within the screen
	if ((c == 'w') && ((leftpad->yc - leftpad->h) > 0))		// move up
		leftpad->yc -= leftpad->vy;
	else if ((c == 's') && ((leftpad->yc + leftpad->h) < ht))		// move down
		leftpad->yc += leftpad->vy;
	// right paddle case -- repeat same steps as previous one
	if ((c == 'i') && ((rightpad->yc - rightpad->h) > 0))
		rightpad->yc -= rightpad->vy;
	else if ((c == 'k') && ((rightpad->yc + rightpad->h) < ht))
		rightpad->yc += rightpad->vy;
}

// display the current score at the top of the screen
void display_score(Rect lpad, Rect rpad, char p1[], char p2[], int wd, int mrgn)
{
	sprintf(p1, "player one -  %d", lpad.score);
	sprintf(p2, "player two -  %d", rpad.score);
	gfx_text(mrgn, mrgn, p1);
	gfx_text(wd-(3*mrgn), mrgn, p2);
}

// display the menu at the start of the game
bool display_menu(int wd, int ht, bool *blocks)
{
	gfx_text((wd/2) - 20, ht/2, "welcome to pong");
	gfx_text((wd/2) - 45, (ht/2) + 20, "press TAB for obstacles");
	gfx_text((wd/2) - 33, (ht/2) + 40, "press SPACE to play");
	char c = gfx_wait();
	// activate the obstacles
	if (c == 9) {
		gfx_text((wd/2) - 110, (ht/2) + 80, "obstacles enabled - are you up for the challenge?");
		*blocks = true;
	}
	if (c == 32) return false;	// start when a space is entered
}

// randomize a color
void random_color(void)
{
	int n = rand() % 5 + 1;		// each number corresponds to a color
	switch (n) {
		case 1:
			gfx_color(255, 0, 0);		// red
			break;
		case 2:
			gfx_color(0, 0, 255);		// blue
			break;
		case 3:
			gfx_color(255, 255, 0);	// yellow
			break;
		case 4:
			gfx_color(255, 0, 255);	// pink
			break;
		case 5:
			gfx_color(0, 204, 0);		// green
			break;
	}
}

// display the congratulatory message/winner at the end of the game
bool display_winner(Rect leftpad, Rect rightpad, int wd, int ht)
{
	char c;
	while (1) {
		gfx_clear();
		// the first to five points wins
		if (leftpad.score == 5)
			gfx_text((wd/2) - 20, ht/2, "player one wins!");
		else if (rightpad.score == 5)
			gfx_text((wd/2) - 20, ht/2, "player two wins!");
		gfx_text((wd/2) - 30, (ht/2) + 20, "press SPACE to exit");
		c = gfx_wait();
		if (c == 32) break;		// exit when a space is entered
	}
	return false;
}

// randomize a positive or negative number
int random_posneg(int num)
{
	int n;
	while (1) {
		n = rand() % 3 - 1;
		if (n != 0) {		// exit once it doesn't equal zero
			break;
		}
	}
	return (num * n);		// return the randomized positive/negative number
}

// increase the ball speed and paddle sensitivity
void increase_speed(Ball *ball, Rect *lpad, Rect *rpad, int num)
{
	if ((num % 1000) == 0) {
		// increase paddle sensitivity
		lpad->vy += 5;
		rpad->vy += 5;
		// check if the ball is moving up, down, left, or right
		// and increase accordingly
		if (ball->vx > 0)
			ball->vx++;
		else if (ball->vx < 0)
			ball->vx--;
		if (ball->vy > 0)
			ball->vy++;
		else if (ball->vy < 0)
			ball->vy--;
	}
}

// initialize both obstacles (dimensions, coordinates, speed, etc.)
void init_obstacles(Block *block1, Block *block2, int wd, int ht)
{
	block1->w = 5;
	block1->h = 100;
	block1->xc = wd/2;
	block1->yc = (ht/2) - (ht/5);
	block1->vy = -2;

	block2->w = block1->w;
	block2->h = block1->h;
	block2->xc = block1->xc;
	block2->yc = (ht/2) + (ht/5);
	block2->vy = block1->vy;
}

// draw the obstacles
void draw_obstacles(Block block1, Block block2)
{
	draw_rectangle(block1.xc, block1.yc, block1.w, block1.h);
	draw_rectangle(block2.xc, block2.yc, block2.w, block2.h);
}

// move the obstacles up or down
void move_obstacles(Block *block1, Block *block2, int ht)
{
	// change direction at the top
	if ((block1->yc - block1->h) <= 0) {
		block1->vy = -block1->vy;
		block2->vy = block1->vy;
	}
	// change direction at the bottom
	if ((block2->yc + block2->h) >= ht) {
		block2->vy = -block2->vy;
		block1->vy = block2->vy;
	}
	block1->yc += block1->vy;
	block2->yc += block2->vy;
}

// bounce the ball off of the obstacles
void bounce_obstacles(Ball *ball, Block *b1, Block *b2, int wd)
{
	// for when the ball is in the right half of the screen and moving to the left
	if ((ball->xc > (wd/2)) && (ball->vx < 0)) {
		if ((ball->xc - ball->rad) <= ((wd/2) + b1->w)) {
			// within the range of the first block
			if ((ball->yc < (b1->yc + b1->h)) && (ball->yc > (b1->yc - b1->h))) {
				ball->vx = -ball->vx;
				ball->xc = b1->xc + b1->w + ball->rad;
			}
			// within the range of the second block
			if ((ball->yc < (b2->yc + b2->h)) && (ball->yc > (b2->yc - b2->h))) {
				ball->vx = -ball->vx;
				ball->xc = b2->xc + b2->w + ball->rad;
			}
		}
	}

	// for when the ball is in the left half of the screen and moving to the right
	// repeat every step for this case
	if ((ball->xc < (wd/2)) && (ball->vx > 0)) {
		if ((ball->xc + ball->rad) >= ((wd/2) - b1->w)) {
			if ((ball->yc < (b1->yc + b1->h)) && (ball->yc > (b1->yc - b1->h))) {
				ball->vx = -ball->vx;
				ball->xc = b1->xc - b1->w - ball->rad;
			}
			if ((ball->yc < (b2->yc + b2->h)) && (ball->yc > (b2->yc - b2->h))) {
				ball->vx = -ball->vx;
				ball->xc = b2->xc - b2->w - ball->rad;
			}
		}
	}
}
