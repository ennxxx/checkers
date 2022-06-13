/*
        Contributing Members: Nino, John Kovie
                                                  Gumapac, Aaron Nicholas
                                                  Villamin, Franz Benedict
        Section: S15
*/
#include <stdio.h>

// nested struct :weary:
typedef struct pos {
  int x;
  int y;
} pos;

typedef struct set {
  int NOP; // No of positions/pieces
  pos pos[36];
} set;

typedef struct aset {
  set Y, E, S, P;
} aset;

typedef struct board {
  set alpha, beta, free;
} board;

typedef struct game {
  int over, ok, aTurn;
  char result;
  board board;
  aset sets;
} game;

// set/helper functions
int
search(set Player, pos Key)
{
  int i;

  for (i = 0; i < Player.NOP; i++)
    if (Player.pos[i].x == Key.x && Player.pos[i].y == Key.y)
      return i; // return index if key found
  return -1;    // return -1 if key not found
}

set
s2punion(set A, pos B)
{
  set C;
  int i;

  C.NOP = 0;
  for (i = 0; i < A.NOP; i++)
    if (search(C, A.pos[i]) == -1) {
      C.pos[C.NOP] = A.pos[i];
      C.NOP++;
    }

  if (search(C, B) == -1) {
    C.pos[C.NOP] = B;
    C.NOP++;
  }

  return C;
}

set
s2sunion(set A, set B)
{
  set C;
  int i;

  C.NOP = 0;
  for (i = 0; i < A.NOP; i++)
    if (search(C, A.pos[i]) == -1) {
      C.pos[C.NOP] = A.pos[i];
      C.NOP++;
    }

  for (i = 0; i < B.NOP; i++)
    if (search(C, B.pos[i]) == -1) {
      C.pos[C.NOP] = B.pos[i];
      C.NOP++;
    }

  return C;
}

set
s2sdiff(set A, set B)
{
  set C;
  int i;

  C.NOP = 0;
  for (i = 0; i < A.NOP; i++)
    if (search(B, A.pos[i]) == -1) {
      C.pos[C.NOP] = A.pos[i];
      C.NOP++;
    }

  return C;
}

set
s2pdiff(set A, pos B)
{
  set C;
  int i;

  C.NOP = 0;
  for (i = 0; i < A.NOP; i++)

    if (!(B.x == A.pos[i].x && B.y == A.pos[i].y)) {
      C.pos[C.NOP] = A.pos[i];
      C.NOP++;
    }

  return C;
}

int
promptUser(int minRange, int maxRange)
{
  int nResponse = 0;
  scanf("%d", &nResponse);
  if (nResponse >= minRange && nResponse <= maxRange) {
    return nResponse;
  }
  else {
    printf("\nInvalid input! (/T^T\\)\n");
    printf("Please try again: ");
    scanf("%*[^\n]");
    return promptUser(minRange, maxRange);
  }
}

// initialize game
aset
setinit()
{
  int i, x, y, counter;
  aset sets;

  // set P
  sets.P.NOP = 0;
  for (x = 1; x <= 7; x++)
    for (y = 1; y <= 5; y++) {
      sets.P.pos[sets.P.NOP].x = x;
      sets.P.pos[sets.P.NOP].y = y;
      sets.P.NOP++;
    }

  // set S
  sets.S.NOP = 0;
  for (x = 1; x <= 7; x++)
    for (y = 1; y <= 5; y++)
      if (x % 2 == y % 2) {
        sets.S.pos[sets.S.NOP].x = x;
        sets.S.pos[sets.S.NOP].y = y;
        sets.S.NOP++;
      }

  // set Y
  sets.Y.NOP = 0;
  for (i = 0; i < sets.S.NOP; i++) {
    if (sets.S.pos[i].x <= 2) {
      sets.Y.pos[sets.Y.NOP] = sets.S.pos[i];
      sets.Y.NOP++;
    }
  }

  // set E
  sets.E.NOP = 0;
  for (i = 0; i < sets.S.NOP; i++) {
    if (sets.S.pos[i].x >= 6) {
      sets.E.pos[sets.E.NOP] = sets.S.pos[i];
      sets.E.NOP++;
    }
  }

  return sets;
}

game
init()
{
  game game;

  game.over = 0;
  game.ok = 0;
  game.aTurn = 1;
  game.sets = setinit();

  game.board.alpha = game.sets.E;
  game.board.beta = game.sets.Y;
  game.board.free = s2sdiff(game.sets.P, s2sunion(game.sets.Y, game.sets.E));

  return game;
}

// game events
game
NextPlayerMove(game game, pos prev, pos next)
{
  if (game.aTurn && search(game.board.alpha, prev) >= 0 &&
      prev.x == next.x + 1 &&
      (next.y == prev.y || next.y == prev.y + 1 || prev.y == next.y + 1))
    game.ok = !(game.ok);
  else if (game.aTurn)
    printf("\n[1] Invalid move! (/T^T\\)\n\n");

  if (!(game.aTurn) && search(game.board.beta, prev) >= 0 &&
      next.x == prev.x + 1 &&
      (next.y == prev.y || next.y == prev.y + 1 || prev.y == next.y + 1))
    game.ok = !(game.ok);
  else if (!(game.aTurn))
    printf("\n[1] Invalid move! (/T^T\\)\n\n");

  if (game.ok && game.aTurn && search(game.board.free, next) >= 0) {
    printf("\nPlayer A moved a piece from (%d, %d) to (%d, %d).\n\n",
           prev.x,
           prev.y,
           next.x,
           next.y);
    game.board.alpha = s2punion(s2pdiff(game.board.alpha, prev), next);
    game.aTurn = !(game.aTurn);
    game.ok = !(game.ok);
  }

  if (game.ok && !(game.aTurn) && search(game.board.free, next) >= 0) {
    printf("\nPlayer B moved a piece from (%d, %d) to (%d, %d).\n\n",
           prev.x,
           prev.y,
           next.x,
           next.y);
    game.board.beta = s2punion(s2pdiff(game.board.beta, prev), next);
    game.aTurn = !(game.aTurn);
    game.ok = !(game.ok);
  }

  if (game.ok && game.aTurn && search(game.board.beta, next) >= 0 &&
      search(game.sets.S, next) == -1) {
    printf("\n[2] Invalid move! (/T^T\\)\n\n");
    game.ok = !(game.ok);
  }

  if (game.ok && game.aTurn && search(game.board.beta, next) >= 0 &&
      search(game.sets.S, next) >= 0) {
    printf("\nPlayer A took a piece from Player B.\n\n");
    game.board.beta = s2pdiff(game.board.beta, next);
    game.board.alpha = s2punion(s2pdiff(game.board.alpha, prev), next);
    game.aTurn = !(game.aTurn);
    game.ok = !(game.ok);
  }

  if (game.ok && !(game.aTurn) && search(game.board.alpha, next) >= 0 &&
      search(game.sets.S, next) == -1) {
    printf("\n[2] Invalid move! (/T^T\\)\n\n");
    game.ok = !(game.ok);
  }

  if (game.ok && !(game.aTurn) && search(game.board.alpha, next) >= 0 &&
      search(game.sets.S, next) >= 0) {
    printf("\nPlayer B took a piece from Player A.\n\n");
    game.board.alpha = s2pdiff(game.board.alpha, next);
    game.board.beta = s2punion(s2pdiff(game.board.beta, prev), next);
    game.aTurn = !(game.aTurn);
    game.ok = !(game.ok);
  }

  return game;
}

game
GameOver(game game)
{
  set testSet;

  testSet = s2sdiff(game.board.alpha, game.sets.Y);
  if (game.board.beta.NOP == 0 ||
      game.board.alpha.NOP > 0 && testSet.NOP == 0) {
    game.result = 'A';
    game.over = !(game.over);
  }

  testSet = s2sdiff(game.board.beta, game.sets.E);
  if (game.board.alpha.NOP == 0 ||
      game.board.beta.NOP > 0 && testSet.NOP == 0) {
    game.result = 'B';
    game.over = !(game.over);
  }

  return game;
}

// check dc link for this one
// use this format when printing: printf("\u01C0");
void
printBoard(game game)
{
  int i, j, K;
  char Board[7][5] = {{' ', ' ', ' ', ' ', ' '},
                      {' ', ' ', ' ', ' ', ' '},
                      {' ', ' ', ' ', ' ', ' '},
                      {' ', ' ', ' ', ' ', ' '},
                      {' ', ' ', ' ', ' ', ' '},
                      {' ', ' ', ' ', ' ', ' '},
                      {' ', ' ', ' ', ' ', ' '}};

  // intializing board
  // intializing free positions
  for (i = 0; i < game.board.free.NOP; i++)
    Board[game.board.free.pos[i].x - 1][game.board.free.pos[i].y - 1] = ' ';

  // intializing alpha pieces
  for (i = 0; i < game.board.alpha.NOP; i++)
    Board[game.board.alpha.pos[i].x - 1][game.board.alpha.pos[i].y - 1] = 'A';

  // intializing beta pieces
  for (i = 0; i < game.board.beta.NOP; i++)
    Board[game.board.beta.pos[i].x - 1][game.board.beta.pos[i].y - 1] = 'B';

  // board printing :skull: x7
  // i want everyone reading the following code to know that i tried my hardest
  // i dont know how people do the stupid ascii art stuff wtfF???

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

  printf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c\n",
         201,
         205,
         205,
         205,
         203,
         205,
         205,
         205,
         203,
         205,
         205,
         205,
         203,
         205,
         205,
         205,
         203,
         205,
         205,
         205,
         203,
         205,
         205,
         205,
         187);

  printf("%c   %c 1 %c 2 %c 3 %c 4 %c 5 %c  y\n",
         186,
         186,
         186,
         186,
         186,
         186,
         186,
         186,
         186);

  printf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c\n",
         204,
         205,
         205,
         205,
         206,
         205,
         205,
         205,
         206,
         205,
         205,
         205,
         206,
         205,
         205,
         205,
         206,
         205,
         205,
         205,
         206,
         205,
         205,
         205,
         185);

  for (i = 0; i < 7; i++) {
    printf("%c %d %c", 186, i + 1, 186);
    for (j = 0; j < 5; j++) {
      printf(" %c %c", Board[i][j], 186);
    }
    printf("\n");
    if (i < 6)
      printf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c\n",
             204,
             205,
             205,
             205,
             206,
             205,
             205,
             205,
             206,
             205,
             205,
             205,
             206,
             205,
             205,
             205,
             206,
             205,
             205,
             205,
             206,
             205,
             205,
             205,
             185);
  }

  printf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c\n",
         200,
         205,
         205,
         205,
         202,
         205,
         205,
         205,
         202,
         205,
         205,
         205,
         202,
         205,
         205,
         205,
         202,
         205,
         205,
         205,
         202,
         205,
         205,
         205,
         188);
  printf("  x\n\n");
}

int
main()
{

  int i;
  game game;
  pos prev, next;

  game = init();
  ///*
  while (!game.over) {

    printBoard(game);

    if (game.aTurn) {
      printf("Player A\'s Turn:\n\n");
      printf("Select a piece to move:\n");
      for (i = 0; i < game.board.alpha.NOP; i++) {
        printf("\t[%d] (%d, %d)\n",
               i + 1,
               game.board.alpha.pos[i].x,
               game.board.alpha.pos[i].y);
      }
      prev = game.board.alpha.pos[promptUser(1, game.board.alpha.NOP) - 1];
    }

    if (!game.aTurn) {
      printf("Player B\'s Turn:\n\n");
      printf("Select a piece to move:\n");
      for (i = 0; i < game.board.beta.NOP; i++) {
        printf("\t[%d] (%d, %d)\n",
               i + 1,
               game.board.beta.pos[i].x,
               game.board.beta.pos[i].y);
      }
      prev = game.board.beta.pos[promptUser(1, game.board.beta.NOP) - 1];
    }

    printf("Input the coordinate of desired destination.\nX: ");
    next.x = promptUser(1, 7);
    printf("Y: ");
    next.y = promptUser(1, 5);

    game = NextPlayerMove(game, prev, next);

    game = GameOver(game);
  } //*/

  if (game.result == 'A') {
    printf("\n\nPlayer A Wins!");
  }

  if (game.result == 'B') {
    printf("\n\nPlayer B Wins!");
  }
  /*
  printf("Alpha: ");
  for (i = 0; i < game.board.alpha.NOP; i++)
          printf("(%d, %d) ", game.board.alpha.pos[i].x,
  game.board.alpha.pos[i].y); printf("\nBeta: "); for (i = 0; i <
  game.board.beta.NOP; i++) printf("(%d, %d) ", game.board.beta.pos[i].x,
  game.board.beta.pos[i].y); printf("\nFree: "); for (i = 0; i <
  game.board.free.NOP; i++) printf("(%d, %d) ", game.board.free.pos[i].x,
  game.board.free.pos[i].y); printf("\n");//*/

  // printBoard(game);

  /*
  testSet = s2punion(game.sets.Y, game.sets.E.pos[0]);
  for (i = 0; i < testSet.NOP; i++){
          printf("(%d, %d) ", testSet.pos[i].x, testSet.pos[i].y);
  }//*/

  /*
  printf("S: ");
  for (i = 0; i < game.sets.S.NOP; i++)
          printf("(%d, %d) ", game.sets.S.pos[i].x, game.sets.S.pos[i].y);
  printf("\nE: ");
  for (i = 0; i < game.sets.E.NOP; i++)
          printf("(%d, %d) ", game.sets.E.pos[i].x, game.sets.E.pos[i].y);
  printf("\nY: ");
  for (i = 0; i < game.sets.Y.NOP; i++)
          printf("(%d, %d) ", game.sets.Y.pos[i].x, game.sets.Y.pos[i].y);
  printf("\n");//*/

  /*
  for (i = 0; i < game.board.free.NOP; i++)
          printf("(%d, %d) ", game.board.free.pos[i].x,
  game.board.free.pos[i].y); printf("\n");//*/

  return 0;
}

// helper
