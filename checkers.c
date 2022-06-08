// CCDSTRU - S11 Machine Project
// By:
// Desiderio, Patrick
// Pacheco, Paula
// Reyes, Aeinnor

#include <stdio.h>

typedef struct position
{
	int x;
	int y;
} position;

typedef struct move
{
	// Player can only move around on 18 coordinates of the board
	position moves[18];
} moves;

typedef struct board
{
	moves Alpha; // Positions for Alpha
	moves Beta;	 // Positions for Beta
	moves Free;  // Empty positions at the beginning of the game
} board;

typedef struct sets
{
	moves E; // Starting position for Alpha
	moves Y; // Starting position for Beta
	moves S; // Move Set
} sets;

typedef struct play
{
	int aTurn;		// Initialize as true; Passing turn to a player
	int over;		// Initialize as false; For a game to end
	int ok;			// Initialize as false; To determine whether a move is playable
	char result;	// To determine whether Alpha or Beta wins
	sets playset;	// To move the pieces of the game
} play;

// Initializing Sets (Patrick)
sets setInit()
{
	// Set E
	// Set Y
	// Set S
}

// Initializing Set Relationships (Aein)
sets setmoveUnion()
{

}
sets setUnion()
{

}
sets setDiff()
{

}
sets setmoveDiff()
{

}

// Main Functions
play initialize()
{

}
play NextPlayerMove()
{

}
play GameOver()
{

}

int main()
{
	// Display board here (Paula)
	return 0;
}