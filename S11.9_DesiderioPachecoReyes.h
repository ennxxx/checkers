#define MAX_ROWS 7
#define MAX_COLS 5

typedef struct position {
  int row;
  int col;
} position;

typedef struct posSet {
  int posCount; // Total number of pieces
  struct position posList[36];
} posSet;

typedef struct sets {
  posSet P; // All spaces on the board
  posSet S; // Diagonally placed positions 
  posSet E; // Starting set for Alpha
  posSet Y; // Starting set for Beta
} sets;

typedef struct board {
  posSet Alpha; // Positions for Alpha
  posSet Beta;  // Positions for Beta
  posSet Free;  // Empty positions at the beginning of the game
} board;

typedef struct play {
  int aTurn;    // Passing turn to a player
  int over;     // For a game to end
  int ok;       // To determine whether a move is playable
  char winner;  // To determine whether Alpha or Beta wins
  sets playset; // To move the pieces of the game
  board gameboard;
} play;

// SYSTEM CHECKERS
int inputHandler(int min, int max);
int elemExists(posSet player, position token);

// SET RELATIONSHIPS
posSet setpositionUnion(posSet A, position B);
posSet setUnion(posSet A, posSet B);
posSet setDiff(posSet A, posSet B);
posSet setpositionDiff(posSet A, position B);

// INITIALIZATION
sets initSets();
play initGame();

// GAME EVENTS
play NextPlayerMove(play game, position next, position prev);
play GameOver(play game);
void displayBoard(play game);
