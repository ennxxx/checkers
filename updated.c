/** CCDSTRU - S11 Machine Project
 * By:
 * Desiderio, Patrick Frinze Matthew M.
 * Pacheco, Paula Enxi D.V.
 * Reyes, Aeinnor A.
 **/

#include <stdio.h>
#include <stdlib.h>
#ifdef WINDOWS
#include <windows.h>
#else
#include <unistd.h>
#endif

#define MAX_ROWS 7
#define MAX_COLS 5

typedef struct position {
  int row;
  int col;
} position;

typedef struct posSet {
  int posCount; // Total number of pieces
  position posList[36];
} posSet;

typedef struct sets {
  posSet P; // All positions on the board
  posSet S; // Positions that can be captured
  posSet E; // Starting position for Alpha
  posSet Y; // Starting position for Beta
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

// SYSTEM FUNCTIONS - Checks or modifies the system states

/**
 * @Description A function thats clears the console depending on the system
 *
 */
void
clearScreen()
{
#ifdef WINDOWS
  {
    system("cls");
  }
#else
  {
    system("clear");
  }
#endif
}

/**
 * @Description Changes text to a certain color
 *
 * @param nColor - An integer that represents a color
 */
void
displayColor(int nColor)
{
  switch (nColor) {
    case 0:
      printf("\033[0m"); // Reset
      break;
    case 1:
      printf("\033[0;31m"); // Red
      break;
    case 2:
      printf("\033[0;32m"); // Green
      break;
    case 3:
      printf("\033[0;33m"); // Yellow
      break;
    case 4:
      printf("\033[0;35m"); // Purple
      break;
    default:
      break;
  }
}

/**
 * @Description Checks for validity of user's input
 *
 * @param min - Lower limit of valid input
 * @param max - Upper limit of valid input
 * @return input if number is valid, print statement otherwise
 */
int
inputHandler(int min, int max)
{
  int input;
  int valid = 0;

  while (!valid) {
    scanf("%d", &input);
    if (input >= min && input <= max)
      valid = 1;
    else
      printf("\nInvalid input! Please try again: ");
  }

  return input;
}

/**
 * @Description Checks if a player's position on the board exists
 *
 * @param player - Total number of player's pieces
 * @param token - Position on the board based on row and column
 * @return index if position is found, -1 otherwise
 */
int
elemExists(posSet player, position token)
{
  int i;

  // Loop through the player's positions
  for (i = 0; i < player.posCount; i++) {
    if (player.posList[i].row == token.row) {
      if (player.posList[i].col == token.col) {
        // If the position is found, return the index
        return i;
      }
    }
  }

  // If the position is not found, return -1
  return -1;
}

// SET RELATIONSHIPS - Functions that deal with the correlation of the sets

/**
 * @Description Adds a position to an existing set
 *
 * @param A - Set of positions
 * @param B - Position to be added in the set
 * @return AB or the union of set and position
 */
posSet
setpositionUnion(posSet A, position B)
{
  posSet AB;
  int i;

  AB.posCount = 0;

  for (i = 0; i < A.posCount; i++) {
    if (elemExists(AB, A.posList[i]) == -1) {
      AB.posList[AB.posCount] = A.posList[i];
      AB.posCount += 1;
    }
  }

  if (elemExists(AB, B) == -1) {
    AB.posList[AB.posCount] = B;
    AB.posCount += 1;
  }

  return AB;
}

/**
 * @Description Union of the set of all possible positions
 *              between both players
 *
 * @param A - Set 1
 * @param B - Set 2
 * @return AB or the union of both sets
 */
posSet
setUnion(posSet A, posSet B)
{
  posSet AB;
  int i;

  AB.posCount = 0;

  for (i = 0; i < A.posCount; i++) {
    if (elemExists(AB, A.posList[i]) == -1) {
      AB.posList[AB.posCount] = A.posList[i];
      AB.posCount += 1;
    }
  }

  for (i = 0; i < B.posCount; i++) {
    if (elemExists(AB, B.posList[i]) == -1) {
      AB.posList[AB.posCount] = B.posList[i];
      AB.posCount += 1;
    }
  }

  return AB;
}

/**
 * @Description Calculates the difference of the first parameter of the
 *              function. Analyzes both sets before grouping all positions
 *              that are only found in A.
 *
 * @param A - Set 1
 * @param B - Set 2
 * @return AminusB or difference between sets
 */
posSet
setDiff(posSet A, posSet B)
{

  posSet AminusB;
  int i;

  AminusB.posCount = 0;

  for (i = 0; i < A.posCount; i++) {
    if (elemExists(B, A.posList[i]) == -1) {
      AminusB.posList[AminusB.posCount] = A.posList[i];
      AminusB.posCount += 1;
    }
  }

  return AminusB;
}

/**
 * @Description Removes a position from an existing set
 *
 * @param A - Set of positions
 * @param B - Position to be removed from the set
 * @return AminusB or a set with a removed position
 */
posSet
setpositionDiff(posSet A, position B)
{

  posSet AminusB;
  int i;

  AminusB.posCount = 0;

  for (i = 0; i < A.posCount; i++) {
    if (B.row == A.posList[i].row && B.col == A.posList[i].col) {
      // If the position is found, do not add it to the set
      continue;
    }
    else {
      AminusB.posList[AminusB.posCount] = A.posList[i];
      AminusB.posCount += 1;
    }
  }

  return AminusB;
}

// INITIALIZATION - Sets are initialized with their default values

/**
 * @Description Initialize sets with their default values for the game
 *
 * @return initialized sets
 */
sets
initSets()
{
  int row, col, i;
  sets allSet;

  // Initialize Set P (All Positions)
  allSet.P.posCount = 0;
  for (row = 1; row <= MAX_ROWS; row++) {
    for (col = 1; col <= MAX_COLS; col++) {
      allSet.P.posList[allSet.P.posCount].row = row;
      allSet.P.posList[allSet.P.posCount].col = col;
      allSet.P.posCount += 1;
    }
  }

  // Initialize Set S (Free Positions)
  allSet.S.posCount = 0;
  for (row = 1; row <= MAX_ROWS; row++) {
    for (col = 1; col <= MAX_COLS; col++) {
      if (row % 2 == col % 2) {
        allSet.S.posList[allSet.S.posCount].row = row;
        allSet.S.posList[allSet.S.posCount].col = col;
        allSet.S.posCount += 1;
      }
    }
  }

  // Initialize Set E (Player Alpha Positions)
  // The Alpha set includes free positions in rows 6 and 7
  allSet.E.posCount = 0;
  for (i = 0; i < allSet.S.posCount; i++) {
    if (allSet.S.posList[i].row >= 6) {
      allSet.E.posList[allSet.E.posCount] = allSet.S.posList[i];
      allSet.E.posCount += 1;
    }
  }

  // Initialize Set Y (Player Beta Positions)
  // The Beta set includes free positions in rows 1 and 2
  allSet.Y.posCount = 0;
  for (i = 0; i < allSet.S.posCount; i++) {
    if (allSet.S.posList[i].row <= 2) {
      allSet.Y.posList[allSet.Y.posCount] = allSet.S.posList[i];
      allSet.Y.posCount += 1;
    }
  }

  return allSet;
}

/**
 * @Description Initializes all game values with its default setting
 *
 * @return initialized game values
 */
play
initGame()
{
  play game;

  game.aTurn = 1;
  game.over = 0;
  game.ok = 0;
  game.winner = ' ';
  game.playset = initSets();

  game.gameboard.Alpha = game.playset.E;
  game.gameboard.Beta = game.playset.Y;
  game.gameboard.Free =
      setDiff(game.playset.P, setUnion(game.playset.Y, game.playset.E));

  return game;
}

// GAME EVENT FUNCTIONS - Functions that deal with the actual game movements

/**
 * @Description Checks what is happening in the game
 *
 * @param game - Struct that contains initialized game values
 * @param next - Succeeding move
 * @param prev - Previous move
 * @return game value
 */
play
NextPlayerMove(play game, position next, position prev)
{
  //  R  C
  // (a, b) = prev
  // (c, d) = next

  // Check if the next position is free for Alpha's turn
  if (game.aTurn && elemExists(game.gameboard.Alpha, prev) >= 0 &&
      prev.row == next.row + 1 &&
      (next.col == prev.col || next.col == prev.col + 1 ||
       prev.col == next.col + 1)) {
    game.ok = !(game.ok);
  }
  else if (game.aTurn) {
    displayColor(1);
    printf("\nWARNING: That move is invalid!");
    displayColor(0);
  }

  // Check if the next position is free for Beta's turn
  if (!(game.aTurn) && elemExists(game.gameboard.Beta, prev) >= 0 &&
      next.row == prev.row + 1 &&
      (next.col == prev.col || next.col == prev.col + 1 ||
       prev.col == next.col + 1)) {
    game.ok = !(game.ok);
  }
  else if (!(game.aTurn)) {
    displayColor(1);
    printf("\nWARNING: That move is invalid!");
    displayColor(0);
  }

  // Prints the updated position of Alpha and set Beta's turn
  if (game.ok && game.aTurn && elemExists(game.gameboard.Free, next) >= 0) {
    displayColor(4);
    printf("\nPlayer Alpha: (%d, %d) --> (%d, %d)",
           prev.row,
           prev.col,
           next.row,
           next.col);
    displayColor(0);
    game.gameboard.Alpha =
        setpositionUnion(setpositionDiff(game.gameboard.Alpha, prev), next);
    game.gameboard.Free =
        setpositionUnion(setpositionDiff(game.gameboard.Free, next), prev);
    game.aTurn = !(game.aTurn);
    game.ok = !(game.ok);
  }

  // Prints the updated position of Beta and set Alpha's turn
  if (game.ok && !(game.aTurn) && elemExists(game.gameboard.Free, next) >= 0) {
    displayColor(3);
    printf("\nPlayer Beta: (%d, %d) --> (%d, %d)",
           prev.row,
           prev.col,
           next.row,
           next.col);
    displayColor(0);
    game.gameboard.Beta =
        setpositionUnion(setpositionDiff(game.gameboard.Beta, prev), next);
    game.gameboard.Free =
        setpositionUnion(setpositionDiff(game.gameboard.Free, next), prev);
    game.aTurn = !(game.aTurn);
    game.ok = !(game.ok);
  }

  // If the position is not found in the free spaces set (S), then
  // the move made by Alpha is invalid
  if (game.ok && game.aTurn && elemExists(game.gameboard.Beta, next) >= 0 &&
      elemExists(game.playset.S, next) == -1) {
    displayColor(1);
    printf("\nWARNING: (%d, %d) cannot be captured!", next.row, next.col);
    displayColor(0);
    game.ok = !(game.ok);
  }

  // If Alpha moves to a space occupied by Beta, Alpha takes Beta's piece
  if (game.ok && game.aTurn && elemExists(game.gameboard.Beta, next) >= 0 &&
      elemExists(game.playset.S, next) >= 0) {
    displayColor(4);
    printf("\nPlayer Alpha: Captured (%d, %d)", next.row, next.col);
    displayColor(0);
    game.gameboard.Beta = setpositionDiff(game.gameboard.Beta, next);
    game.gameboard.Alpha =
        setpositionUnion(setpositionDiff(game.gameboard.Alpha, prev), next);
    game.gameboard.Free =
        setpositionUnion(setpositionDiff(game.gameboard.Free, prev), next);
    game.aTurn = !(game.aTurn);
    game.ok = !(game.ok);
  }

  // If the position is not found in the free spaces set (S), then
  // the move made by Beta is invalid
  if (game.ok && !(game.aTurn) && elemExists(game.gameboard.Alpha, next) >= 0 &&
      elemExists(game.playset.S, next) == -1) {
    displayColor(1);
    printf("\nWARNING: (%d, %d) cannot be captured!", next.row, next.col);
    displayColor(0);
    game.ok = !(game.ok);
  }

  // If Beta moves to a space occupied by Alpha, Beta takes Alpha's piece
  if (game.ok && !(game.aTurn) && elemExists(game.gameboard.Alpha, next) >= 0 &&
      elemExists(game.playset.S, next) >= 0) {
    displayColor(3);
    printf("\nPlayer Beta: Captured (%d, %d)", next.row, next.col);
    displayColor(0);
    game.gameboard.Alpha = setpositionDiff(game.gameboard.Alpha, next);
    game.gameboard.Beta =
        setpositionUnion(setpositionDiff(game.gameboard.Beta, prev), next);
    game.gameboard.Free = setpositionUnion(game.gameboard.Free, prev);
    game.aTurn = !(game.aTurn);
    game.ok = !(game.ok);
  }

  return game;
}

/**
 * @Description Decides the winner of the game based on the outcome
 *
 * @param game - Struct that contains initialized game values
 * @return game value
 */
play
GameOver(play game)
{
  // If Alpha's pieces exists and are all moved from their starting positions,
  // and the Beta has no more pieces, then Alpha wins
  if (game.gameboard.Beta.posCount == 0 ||
      game.gameboard.Alpha.posCount > 0 &&
          setDiff(game.gameboard.Alpha, game.playset.Y).posCount == 0) {
    game.winner = 'A';
    game.over = !(game.over);
  }

  else {
    game.over = game.over;
  }

  // If Beta's pieces exists and are all moved from their starting positions,
  // and the Alpha has no more pieces, then Beta wins
  if (game.gameboard.Alpha.posCount == 0 ||
      game.gameboard.Beta.posCount > 0 &&
          setDiff(game.gameboard.Beta, game.playset.E).posCount == 0) {
    game.winner = 'B';
    game.over = !(game.over);
  }

  else {
    game.over = game.over;
  }

  return game;
}

/**
 * @Description Displays the board of the game
 *
 * @param game - Struct that contains initialized game values
 */
void
displayBoard(play game)
{
  int x, y;

  char gameboard[7][5] = {{' ', ' ', ' ', ' ', ' '},
                          {' ', ' ', ' ', ' ', ' '},
                          {' ', ' ', ' ', ' ', ' '},
                          {' ', ' ', ' ', ' ', ' '},
                          {' ', ' ', ' ', ' ', ' '},
                          {' ', ' ', ' ', ' ', ' '},
                          {' ', ' ', ' ', ' ', ' '}};

  // Initializing the gameboard with the pieces of the game

  // Initialize free spaces
  for (x = 0; x < game.gameboard.Free.posCount; x++) {
    gameboard[game.gameboard.Free.posList[x].row - 1]
             [game.gameboard.Free.posList[x].col - 1] = ' ';
  }

  // Initialize Alpha's pieces
  for (x = 0; x < game.gameboard.Alpha.posCount; x++) {
    gameboard[game.gameboard.Alpha.posList[x].row - 1]
             [game.gameboard.Alpha.posList[x].col - 1] = 'A';
  }

  // Initialize Beta's pieces
  for (x = 0; x < game.gameboard.Beta.posCount; x++) {
    gameboard[game.gameboard.Beta.posList[x].row - 1]
             [game.gameboard.Beta.posList[x].col - 1] = 'B';
  }

  // String variables for unicode characters
  char *horizontalBar = "\u2501";
  char *verticalBar = "\u2503";
  char *topLeftCorner = "\u250F";
  char *topRightCorner = "\u2513";
  char *lowerRightCorner = "\u251B";
  char *lowerLeftCorner = "\u2517";
  char *horizontalTRight = "\u2523";
  char *horizontalTLeft = "\u252B";
  char *upsideDownT = "\u253B";
  char *T = "\u2533";
  char *cross = "\u254B";

  printf("\n\n      ★★★ Let's Play! ★★★\n\n");
  printf("       1   2   3   4   5    y\n");
  printf("     ");
  printf("%s", topLeftCorner);
  for (x = 1; x < 20; x++) {
    if (x % 4 == 0) {
      printf("%s", T);
    }
    else {
      printf("%s", horizontalBar);
    }
  }
  printf("%s\n", topRightCorner);

  for (x = 0; x < MAX_ROWS; x++) {
    printf("  %d  %s", x + 1, verticalBar);

    for (y = 0; y < MAX_COLS; y++) {
      if (x % 2 == y % 2 && x >= 2 && x <= 4) { // Unsafe Positions
        displayColor(1);
        printf(" %c ", gameboard[x][y]);
        displayColor(0);
        printf("%s", verticalBar);
      }
      else if (x % 2 == y % 2 && x > 4) {
        displayColor(4);
        printf(" %c ", gameboard[x][y]);
        displayColor(0);
        printf("%s", verticalBar);
      }
      else if (x % 2 == y % 2 && x < 2) {
        displayColor(3);
        printf(" %c ", gameboard[x][y]);
        displayColor(0);
        printf("%s", verticalBar);
      }
      else // Free Positions
        printf(" %c %s", gameboard[x][y], verticalBar);
    }
    printf("\n");

    if (x < 6) {
      printf("     ");
      printf("%s", horizontalTRight);
      for (y = 1; y < 20; y++) {
        if (y % 4 == 0) {
          printf("%s", cross);
        }
        else {
          printf("%s", horizontalBar);
        }
      }
      printf("%s\n", horizontalTLeft);
    }
  }

  printf("     ");
  printf("%s", lowerLeftCorner);
  for (x = 1; x < 20; x++) {
    if (x % 4 == 0) {
      printf("%s", upsideDownT);
    }
    else {
      printf("%s", horizontalBar);
    }
  }
  printf("%s\n", lowerRightCorner);
  printf("  x\n\n");
}

int
main()
{
  int i;
  play game;
  position prev, next;

  game = initGame();

  while (game.over == 0) {

    usleep(500000);
    clearScreen();
    displayBoard(game);

    // Player Alpha game set
    if (game.aTurn == 1) {
      displayColor(4);
      printf("Player Alpha's Turn!\n\n");
      displayColor(0);
      printf("Choose a piece to move:\n");
      for (i = 0; i < game.gameboard.Alpha.posCount; i++) {
        printf("[%d] (%d, %d)\n",
               i + 1,
               game.gameboard.Alpha.posList[i].row,
               game.gameboard.Alpha.posList[i].col);
      }
      printf("\nInput: ");
      prev = game.gameboard.Alpha
                 .posList[inputHandler(1, game.gameboard.Alpha.posCount) - 1];
    }

    // Player Beta game set
    if (game.aTurn != 1) {
      displayColor(3);
      printf("Player Beta's Turn!\n\n");
      displayColor(0);
      printf("Choose a piece to move:\n");
      for (i = 0; i < game.gameboard.Beta.posCount; i++) {
        printf("[%d] (%d, %d)\n",
               i + 1,
               game.gameboard.Beta.posList[i].row,
               game.gameboard.Beta.posList[i].col);
      }
      printf("\nInput: ");
      prev = game.gameboard.Beta
                 .posList[inputHandler(1, game.gameboard.Beta.posCount) - 1];
    }

    printf("\nWhere would you like to move your piece?\n");
    printf("x: ");
    next.row = inputHandler(1, 7);
    printf("y: ");
    next.col = inputHandler(1, 5);

    game = NextPlayerMove(game, next, prev);
    game = GameOver(game);
  }

  if (game.winner == 'A') {
    displayColor(2);
    printf("\nCongratulations! Player Alpha wins!\n\n");
    displayColor(0);
  }

  if (game.winner == 'B') {
    displayColor(2);
    printf("\nCongratulations! Player Beta wins!\n\n");
    displayColor(0);
  }

  return 0;
}
