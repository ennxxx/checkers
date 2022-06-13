// CCDSTRU - S11 Machine Project
// By:
// Desiderio, Patrick
// Pacheco, Paula
// Reyes, Aeinnor

#include <stdio.h>
#define MAX_ROWS 7
#define MAX_COLS 5

typedef struct position {
  int row;
  int col;
} position;

typedef struct posSet {
  // Player can only move around on 18 coordinates of the board
  int posCount; // total positions/pieces
  position posList[18];
} posSet;

typedef struct board {
  posSet Alpha; // Positions for Alpha
  posSet Beta;  // Positions for Beta
  posSet Free;  // Empty positions at the beginning of the game
} board;

typedef struct sets {
  posSet P; // Positions for P UNKNOWN YET
  posSet S; // Free spaces (Spaces that Alpha and Beta can move to)
  posSet E; // Starting position for Alpha
  posSet Y; // Starting position for Beta
} sets;

typedef struct play {
  int aTurn;   // Initialize as true; Passing turn to a player
  int over;    // Initialize as false; For a game to end
  int ok;      // Initialize as false; To determine whether a move is playable
  char winner; // To determine whether Alpha or Beta wins
  board gameboard;
  sets playset; // To move the pieces of the game
} play;

// Set relationships functions

// Search function that checks if the position(token) is part of the player
int
posChecker(posSet player, position token)
{
  int i, playerposcount, row, col;

  playerposcount = player.posCount;
  row = token.row;
  col = token.col;

  // Loop through the player's positions
  for (i = 0; i < playerposcount; i++) {
    if (player.posList[i].row == row) {
      if (player.posList[i].col == col) {
        // If the position is found, return the index
        return i;
      }
    }
  }

  // If the position is not found, return -1
  return -1;
}

/**
 * @Description - This function combines a position set with a list of positions
 *
 *
 * @param A - All possible positions of a player
 * @param B - Actual set of positions of the player
 * @return posSet AB
 */
posSet
setpositionUnion(posSet A, position B)
{
  posSet AB;
  int i, count;

  AB.posCount = 0;
  A.posCount = count;

  for (i = 0; i < count; i++) {
    if (posChecker(AB, A.posList[i]) == -1) {
      AB.posList[AB.posCount] = A.posList[i];
      AB.posCount += 1;
    }
  }

  if (posChecker(AB, B) == -1) {
    AB.posList[AB.posCount] = A.posList[i];
    AB.posCount += 1;
  }

  return AB;
}

// This function combines two position sets
posSet
setUnion(posSet A, posSet B)
{
  posSet AB;
  int i, countA, countB;

  AB.posCount = 0;
  A.posCount = countA;
  B.posCount = countB;

  for (i = 0; i < countA; i++) {
    if (posChecker(AB, A.posList[i]) == -1) {
      AB.posList[AB.posCount] = A.posList[i];
      AB.posCount += 1;
    }
  }

  for (i = 0; i < countB; i++) {
    if (posChecker(AB, B.posList[i]) == -1) {
      AB.posList[AB.posCount] = B.posList[i];
      AB.posCount += 1;
    }
  }

  return AB;
}

// Gets the difference of the first parameter of the function. It compares the
// two sets and groups all positions found in A that are not found in B.
posSet
setDiff(posSet A, posSet B)
{
  posSet AminusB;
  int i, count;

  AminusB.posCount = 0;
  A.posCount = count;
  for (i = 0; i < count; i++) {
    if (posChecker(B, A.posList[i]) == -1) {
      AminusB.posList[AminusB.posCount] = A.posList[i];
      AminusB.posCount += 1;
    }
  }

  return AminusB;
}

// Gets the difference of the position set and a list of positions. It compares
// the two and groups all positions found in A that are not in the list.
posSet
setpositionDiff(posSet A, position B)
{
  posSet AminusB;
  int i, count;

  AminusB.posCount = 0;
  A.posCount = count;

  for (i = 0; i < count; i++) {
    if (!(B.row == A.posList[i].row)) {
      if (!(B.col == A.posList[i].col)) {
        AminusB.posList[AminusB.posCount] = A.posList[i];
        AminusB.posCount += 1;
      }
    }
  }

  return AminusB;
}

// Initialize sets for default values for the game
sets
initSets()
{
  int row, col, i, poscountP, poscountS, poscountE, poscountY;
  sets allSet;

  // Initialize set P
  allSet.P.posCount = 0;
  for (row = 1; row <= MAX_ROWS; row++) {
    for (col = 1; col <= MAX_COLS; col++) {
      poscountP = allSet.P.posCount;
      allSet.P.posList[poscountP].row = row;
      allSet.P.posList[poscountP].col = col;
      poscountP += 1;
    }
  }

  // Initialize free spaces
  allSet.S.posCount = 0;
  for (row = 1; row <= MAX_ROWS; row++) {
    for (col = 1; col <= MAX_COLS; col++) {
      if (row % 2 == col % 2) // If both numbers are odd/even
      {
        // assign them as free spaces
        poscountS = allSet.S.posCount;
        allSet.S.posList[poscountS].row = row;
        allSet.S.posList[poscountS].col = col;
      }
    }
  }

  // Initialize the player positions of Alpha
  // The Alpha set includes free positions found in rows 6 and 7
  allSet.E.posCount = 0;
  for (i = 0; i < poscountS; i++) {
    if (allSet.S.posList[i].row >= 6) {
      poscountE = allSet.E.posCount;
      allSet.E.posList[poscountE] = allSet.S.posList[i];
      poscountE++;
    }
  }

  // Initialize the player positions of Beta
  // The Beta set includes free positions found in rows 1 and 2
  allSet.Y.posCount = 0;
  for (i = 0; i < poscountS; i++) {
    if (allSet.S.posList[i].row <= 2) {
      poscountY = allSet.Y.posCount;
      allSet.Y.posList[poscountY] = allSet.S.posList[i];
      poscountY++;
    }
  }

  return allSet;
}

// Initialize game values
play
initGame()
{
  play game;

  game.aTurn = 1;
  game.over = 0;
  game.ok = 0;
  game.playset = initSets();

  game.gameboard.Alpha = game.playset.E;
  game.gameboard.Beta = game.playset.Y;
  game.gameboard.Free =
      setDiff(game.playset.S, setUnion(game.playset.Y, game.playset.E));

  return game;
}

// Validates the input of the user
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
      printf("Invalid input. Please try again: ");
  }

  return input;
}

// Game Event Functions
play
NextPlayerMove(play game, position next, position prev)
{

  // Check if the next position if free for Alpha's turn
  if (game.aTurn && posChecker(game.gameboard.Alpha, prev) >= 0 &&
      prev.row == next.row + 1) {
    if (next.col == prev.col || next.col == prev.col ||
        prev.col == next.col + 1)
      game.ok = !(game.ok);
  }
  else if (game.aTurn)
    printf("\n That move is invalid!\n\n");

  // Check if the next position if free for Beta's turn
  if (!(game.aTurn) && posChecker(game.gameboard.Beta, prev) >= 0 &&
      next.row == prev.row + 1) {
    if (next.col == prev.col || next.col == prev.col + 1 ||
        prev.col == next.col + 1)
      game.ok = !(game.ok);
  }
  else if (!(game.aTurn))
    printf("\n That move is invalid!\n\n");

  // Prints the updated position of Alpha and set Beta's turn
  if (game.ok && game.aTurn) {
    if (posChecker(game.gameboard.Free, next) >= 0) {
      printf("\n Player Alpha's position: (%d,%d) --> (%d, %d) \n\n",
             prev.row,
             prev.col,
             next.row,
             next.col);
      game.gameboard.Alpha =
          setpositionUnion(setpositionDiff(game.gameboard.Alpha, prev), next);
      game.aTurn = !(game.aTurn);
      game.ok = !(game.ok);
    }
  }

  // Prints the updated position of Beta and set Alpha's turn
  if (game.ok && !(game.aTurn)) {
    if (posChecker(game.gameboard.Free, next) >= 0) {
      printf("\n Player Beta's position: (%d,%d) --> (%d, %d) \n\n",
             prev.row,
             prev.col,
             next.row,
             next.col);
      game.gameboard.Beta =
          setpositionUnion(setpositionDiff(game.gameboard.Beta, prev), next);
      game.aTurn = !(game.aTurn);
      game.ok = !(game.ok);
    }
  }

  // If the position is not found in the free spaces set (S), then the move made
  // by Beta is invalid
  if (game.ok && game.aTurn) {
    if (posChecker(game.gameboard.Beta, next) >= 0) {
      if (posChecker(game.playset.S, next) == -1) {
        printf("\n That move is invalid! \n\n");
        game.ok = !(game.ok);
      }
    }
  }

  // If Alpha moves to a space occupied by Beta, Alpha takes Beta's piece
  if (game.ok && game.aTurn) {
    if (posChecker(game.gameboard.Beta, next) >= 0) {
      if (posChecker(game.playset.S, next) >= 0) {
        printf(
            "\n Oh no! Player Beta's piece was consumed by Player Alpha!\n\n");
        game.gameboard.Beta = setpositionDiff(game.gameboard.Beta, next);
        game.gameboard.Alpha = setpositionUnion(game.gameboard.Alpha, next);
        game.aTurn = !(game.aTurn);
        game.ok = !(game.ok);
      }
    }
  }

  // If the position is not found in the free spaces set (S), then the move made
  // by Alpha is invalid
  if (game.ok && !(game.aTurn)) {
    if (posChecker(game.gameboard.Alpha, next) >= 0) {
      if (posChecker(game.playset.S, next) == -1) {
        printf("\n That move is invalid! \n\n");
        game.ok = !(game.ok);
      }
    }
  }

  // If Beta moves to a space occupied by Alpha, Beta takes Alpha's piece
  if (game.ok && !(game.aTurn)) {
    if (posChecker(game.gameboard.Alpha, next) >= 0) {
      if (posChecker(game.playset.S, next) >= 0) {
        printf(
            "\n Oh no! Player Alpha's piece was consumed by Player Beta!\n\n");
        game.gameboard.Alpha = setpositionDiff(game.gameboard.Alpha, next);
        game.gameboard.Beta =
            setpositionUnion(setpositionDiff(game.gameboard.Beta, prev), next);
        game.aTurn = !(game.aTurn);
        game.ok = !(game.ok);
      }
    }
  }

  return game;
}

play
GameOver(play game)
{
  posSet aSet, bSet;

  // If Alpha's pieces exists and are all moved from their starting positions,
  // and the Beta has no more pieces, then Alpha wins
  aSet = setDiff(game.gameboard.Alpha, game.playset.Y);
  if (game.gameboard.Beta.posCount == 0 || game.gameboard.Alpha.posCount > 0) {
    if (aSet.posCount == 0) {
      game.winner = 'A';
      game.over = !(game.over);
    }
  }

  // If Beta's pieces exists and are all moved from their starting positions,
  // and the Alpha has no more pieces, then Beta wins
  bSet = setDiff(game.gameboard.Beta, game.playset.E);
  if (game.gameboard.Alpha.posCount == 0 || game.gameboard.Beta.posCount > 0) {
    if (bSet.posCount == 0) {
      game.winner = 'B';
      game.over = !(game.over);
    }
  }

  return game;
}

void
displayBoard(play game)
{
  int i, j, k, x, y;

  char gameboard[7][5] = {{' ', ' ', ' ', ' ', ' '},
                          {' ', ' ', ' ', ' ', ' '},
                          {' ', ' ', ' ', ' ', ' '},
                          {' ', ' ', ' ', ' ', ' '},
                          {' ', ' ', ' ', ' ', ' '},
                          {' ', ' ', ' ', ' ', ' '},
                          {' ', ' ', ' ', ' ', ' '}};

  // Initializing the gameboard with the pieces of the game

  // Initialize free spaces
  for (i = 0; i < game.playset.S.posCount; i++) {
    gameboard[game.gameboard.Free.posList[i].row - 1]
             [game.gameboard.Free.posList[i].col - 1] = 'S';
  }

  // Initialize Alpha's pieces
  for (i = 0; i < game.gameboard.Alpha.posCount; i++) {
    gameboard[game.gameboard.Alpha.posList[i].row - 1]
             [game.gameboard.Alpha.posList[i].col - 1] = 'A';
  }

  // Initialize Beta's pieces
  for (i = 0; i < game.gameboard.Beta.posCount; i++) {
    gameboard[game.gameboard.Beta.posList[i].row - 1]
             [game.gameboard.Beta.posList[i].col - 1] = 'B';
  }

  // String variables for unicode characters
  char *horizontalBar = "\u2501";
  char *verticalBar = "\u2503";
  char *topLeftCorner = "\u250F";
  char *horizontalTRight = "\u2523";
  char *horizontalTLeft = "\u252B";
  char *topRightCorner = "\u2513";
  char *lowerRightCorner = "\u251B";
  char *upsideDownT = "\u253B";
  char *T = "\u2533";
  char *cross = "\u254B";
  char *lowerLeftCorner = "\u2517";

  // turn 205 to ("\u2501") horizontal bar
  // turn 186 to ("\u2503") vertical bar
  // turn 201 to ("\u250F") top left corner
  // turn 204 to ("\u2523") Horizontal T facing the right
  // turn 185 to ("\u252B") horixontal T facing the left
  // turn 187 to ("\u2513") top right corner
  // turn 188 to ("\u251B") lower right corner
  // turn 202 to ("\u253B") Upside down T
  // turn 203 to ("\u2533") T
  // turn 206 to ("\u254B") cross
  // turn 200 to ("\u2517") lower left corner

  printf("%s", horizontalBar);
  for (i = 1; i <= 23; i++) {
    if (i % 4 == 0) {
      printf("%s", T);
    }
    else {
      printf("%s", horizontalBar);
    }
  }
  printf("%s\n", topRightCorner);

  printf("%s   %s 1 %s 2 %s 3 %s 4 %s 5 %s  y\n",
         verticalBar,
         verticalBar,
         verticalBar,
         verticalBar,
         verticalBar,
         verticalBar,
         verticalBar);

  printf("%s", horizontalTRight);
  for (i = 1; i <= 23; i++) {
    if (i % 4 == 0) {
      printf("%s", cross);
    }
    else {
      printf("%s", horizontalBar);
    }
  }
  printf("%s\n", horizontalTLeft);

  for (x = 0; x < MAX_ROWS; x++) {
    printf("%s %d %s", verticalBar, i + 1, verticalBar);

    for (y = 0; y < MAX_COLS; y++) {
      printf(" %c %s", gameboard[x][y], verticalBar);
    }
    printf("\n");

    if (i < 6) {
      printf("%s", horizontalTRight);
      for (j = 1; j <= 23; j++) {
        if (j % 4 == 0) {
          printf("%s", cross);
        }
        else {
          printf("%s", horizontalBar);
        }
      }
      printf("%s\n", horizontalTLeft);
    }
  }

  printf("%s", lowerLeftCorner);
  for (i = 0; i < 23; i++) {
    if (i % 4 == 0) {
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
  play game;
  displayBoard(game);

  return 0;
}