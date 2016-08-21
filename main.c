/// standard libraries
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

/// empty board initialization as global array
char charRow[10] = {" ABCDEFGH "};
char numColumn[10] = {" 87654321 "};
char emptyBoard[10][10];

/// pieces board initialization as global array
char lastRowPieces[10] = {" RNBQKBNR "};
char firstRowPieces[10] = {" rnbqkbnr "};
char piecesArray[10][10];

/// For Undo & Redo
char moveString[100000];
char movesOfGame[10000][5];
int turnsCounterRow = 0; // As rows in above array.
int undoFlag = 0;
int redoFlag = 0;
int capturingTurns[1000];
int capturingTurnsCounter = 0;

/// Captured pieces when Undoing
char piecesForUndo[1000] = {};

/// captured pieces array initialization
char capturedPieces[9][4] = {{' ',' ',' ',' '}, {' ',' ',' ',' '}, {' ',' ',' ',' '},{' ',' ',' ',' '},{' ',' ',' ',' '},{' ',' ',' ',' '},{' ',' ',' ',' '},{' ',' ',' ',' '},{' ',' ',' ',' '}};

/// indices of checking pieces
int checkingPiecesPos[25];

/// Global Variables
int capturedPiecesArrayRowCount1 = 1, capturedPiecesArrayColCount1 = 0;
int capturedPiecesArrayRowCount2 = 1, capturedPiecesArrayColCount2 = 2;
int player = 1;
int turns = 0;
int counter = 0;

/// keeping track of the king's position
int KRowPos = 1;
int KColPos = 5;

int kRowPos = 8;
int kColPos = 5;

/// number of pieces checking the king
int numOfChecks = 0;

/// input positions
int initCol, initRow, finCol, finRow;
int promotionflag = 0;
int checkAfterUndo = 0;

/// functions prototypes
void mainBoard();
void pieces();
void boardPrinting();
void move();
void input();
void save();
void load();
void recordingMoves();
void undo();
void redo();

/// Movements
int check();
int pawnMovement1();
int pawnMovement2();
int rookMovement();
int knightMovement();
int bishopMovement();
int kingMovement();

/// Checking for danger
int checkForPlayer1();
int checkForPlayer2();
int checkForAllyKing();

///This functions check that the checked king can get out of check by moving itself...
int escapeKingRoute();

/// Promotion functions
int promotion();
int checkAfterPromotion();

/// Stalemate Function
int checkForStalemate();

/// Main function
int main()
{
    mainBoard();
    pieces();
    boardPrinting();
    while (1)
    {
        if (player == 1)
            printf("Player 1's Turn\n");
        else if (player == -1)
            printf("Player 2's Turn\n");

        input();
        int valid = 0 ;

        // while the move is invalid, re-prompt the user
        int Check = check();
        while (Check == 0)
        {
            input();
            Check = check();
        }
        int CheckForPlayer1 = checkForPlayer1();
        int CheckForPlayer2 = checkForPlayer2();

        // Making sure that no piece moves while its friendly kind is checked
        if (player == 1 && CheckForPlayer1 == 0)
        {
            move();
            recordingMoves();
            turnsCounterRow++;

            if(promotionflag == 1)
                CheckForPlayer2 = checkAfterPromotion();

            if(!checkForStalemate() && CheckForPlayer2==0)
            {
                boardPrinting();
                printf("\tStalemate!! \n\t DRAW!");
                break;
            }
            valid = 1;
        }
        else if (player == 1 && CheckForPlayer1 == 1)
        {
            printf("Invalid move , there will be a check for you\a\n");
        }


        if (player == -1 && CheckForPlayer2 == 0)
        {
            move();
            recordingMoves();
            turnsCounterRow++;
            if(promotionflag == 1)
                CheckForPlayer1 = checkAfterPromotion();
            if(!checkForStalemate() && CheckForPlayer1==0)
            {
                boardPrinting();
                printf("\tStalemate!! \n\t DRAW!");
                break;
            }
            valid = 1;
        }
        else if (player == -1 && CheckForPlayer2 == 1)
        {
            printf("Invalid move , there will be a check for you\n");
        }


        if(valid == 1)
        {
            // printing board
            boardPrinting();
            if(player == 1 && CheckForPlayer2 == 1)
            {
                if(escapeKingRoute() && !checkForAllyKing())
                {
                    printf("\tCheckmate!!.Player 1 Wins..\n\tThe End");
                    break;
                }
                else
                    printf("There is a Check For you / Take care!!\n");
            }
            else if(player == -1 && CheckForPlayer1 == 1)
            {
                if(escapeKingRoute() && !checkForAllyKing())
                {
                    printf("\tCheckmate!!.Player 2 Wins..\n\tThe End");
                    break;
                }
                else
                    printf("There is a Check For you / Take care!!\n");
            }
            player *= -1;
        }
    }
    return 0;
}

/// Creating main board
void mainBoard()
{
    int i, j;
    for (i = 1; i < 9; i++)
    {
        if (i % 2 == 0)
        {
            for (j = 1; j < 9; j++)
            {
                if (j % 2 == 0)
                {
                    emptyBoard[i][j] = '_';
                }
                else
                {
                    emptyBoard[i][j] = '.';
                }
            }
        }
        else
        {
            for (j = 1; j < 9; j++)
            {
                if (j % 2 == 0)
                {
                    emptyBoard[i][j] = '.';
                }
                else
                {
                    emptyBoard[i][j] = '_';
                }
            }
        }
    }

    for (j = 0; j < 10; j++)
    {
        emptyBoard[0][j] = charRow[j];
        emptyBoard[9][j] = charRow[j];
        emptyBoard[j][0] = numColumn[j];
        emptyBoard[j][9] = numColumn[j];
    }
}

/// Creating Pieces array
void pieces()
{
    int i, j;
    for (i = 0; i < 10; i++)
    {
        for (j = 0; j < 10; j++)
        {
            if (i == 2 && j != 0 && j != 9)
            {
                piecesArray[i][j] = 'P';
            }
            else if (i == 7 && j != 0 && j != 9)
            {
                piecesArray[i][j] = 'p';
            }
            else if (i == 1 && j != 0 && j != 9)
            {
                piecesArray[i][j] = lastRowPieces[j];
            }
            else if (i == 8 && j != 0 && j != 9)
            {
                piecesArray[i][j] = firstRowPieces[j];
            }
            else
            {
                piecesArray[i][j] = ' ';
            }
        }
    }
}

/// Printing the total board
void boardPrinting()
{
    int i , j;
    for(i = 0; i < 10; i++)
    {
        for(j = 0; j < 10 ; j++)
        {
            if (piecesArray[i][j] == ' ')
                printf("%c   ", emptyBoard[i][j]);
            else
                printf("%c   ", piecesArray[i][j]);
        }

        // printing captured pieces array
        for (j = 0; j < 4 && i <= 8; j++)
        {
            if(i == 0 && j == 0)
                printf("| Player1 || Player2 |");
            else if(j == 0 || j == 1 && i != 0 )
                printf("| %c  ", capturedPieces[i][j]);
            else if(j == 2 && i != 0)
                printf("|| %c  |", capturedPieces[i][j]);
            else if(j == 3 && i != 0)
                printf(" %c  |", capturedPieces[i][j]);
            if (i==0 && j==3)
                printf("\n");
        }
        printf("\n");

        if (i == 8)
            printf("\n");
    }
}

/// Taking input and validating it
void input()
{
    do
    {
        // taking input
        moveString[5] = '\0';
        printf("Enter move(col/row , col/row): ");
        gets(moveString);

        // Calling Save function
        while (!strcmp(moveString,"SAVE"))
        {
            save();
            printf("Your game has been saved\n");

            moveString[5] = '\0';
            printf("Enter move(col/row, col/row): ");
            gets(moveString);
        }

        // Calling Load function
        while (!strcmp(moveString,"LOAD"))
        {
            load();
            printf("Your game has been loaded\n");
            if(checkForPlayer1() || checkForPlayer2())
                printf("There is a Check For you / Take care!!\n");

            moveString[5] = '\0';
            if (player == 1)
                printf("Player 1's move\n");
            else if (player = -1)
                printf("Player 2's move\n");

            printf("Enter move (col/row, col/row): ");
            gets(moveString);
        }

        // Calling Undo function
        while (!strcmp(moveString,"UNDO") && turns > 0)
        {
            undo();
            moveString[5] = '\0';
            printf("Your game has been Undo-ed\n");
            if(checkAfterUndo == 1)
                printf("There is a Check For you / Take care!!\n");

            if (player == 1)
                printf("Player 1's move\n");
            else if (player = -1)
                printf("Player 2's move\n");
            printf("Enter move (col/row, col/row): ");
            gets(moveString);

            // Updating the turnsCounterRow
            if(!strcmp(moveString,"UNDO") && turns > 0)
            {
                turnsCounterRow--;
                counter = 0;
            }
        }

        if (turns <= 0 && !strcmp(moveString,"UNDO"))
            printf("This is the first turn! Can't undo any more!\n");

        // HERE GOES THE REDO FUNCTION CALL
        while (!strcmp(moveString, "REDO"))
        {
            if (redoFlag >= undoFlag)
            {
                printf("Can't redo any more!\n");
            }
            else
            {
                redo();
                printf("The game has been REDO-ed!\n");
            }

            moveString[5] = '\0';
            if(checkAfterUndo == 1)
                printf("There is a Check For you / Take care!!\n");

            if (player == 1)
                printf("Player 1's move\n");
            else if (player = -1)
                printf("Player 2's move\n");

            printf("Enter move (col/row, col/row): ");
            gets(moveString);

            // Updating turnsCounterRow
            if(!strcmp(moveString,"REDO"))
            {
                turnsCounterRow++;
            }
        }

        // Assigning Indices
        initCol = moveString[0];
        finCol = moveString[2];
        if (initCol >= 'A' && initCol <= 'Z' && finCol >= 'A' && finCol <= 'Z')
        {
            initCol = moveString[0] - 'A' + 1;
            finCol = moveString[2] - 'A' + 1;
        }
        else if (initCol >= 'a' && initCol <= 'z' && finCol >= 'a' && finCol <= 'z')
        {
            initCol = moveString[0] - 'a' + 1;
            finCol = moveString[2] - 'a' + 1;
        }
        initRow = 9 - (moveString[1] - 48);
        finRow = 9 - (moveString[3] - 48);
    }
    while (initCol < 1 || initCol > 8 || finCol < 1 || finCol > 8 || initRow < 1 || initRow > 8 || finRow < 1 || finRow > 8 || piecesArray[initRow][initCol] == ' ' || piecesArray[initRow][initCol] == piecesArray[finRow][finCol] || strlen(moveString) > 5);
}

/// Doing the desired move and adding captured pieces to the list, if there were any captured pieces.
void move()
{
    // CAPTURING ALGORITHM
    // if a piece from player 1 is captured
    if (islower(piecesArray[finRow][finCol]))
    {
        capturedPieces[capturedPiecesArrayRowCount1][capturedPiecesArrayColCount1] = piecesArray[finRow][finCol];
        if (capturedPiecesArrayRowCount1 <= 8)
        {
            capturedPiecesArrayRowCount1++;
        }
        if (capturedPiecesArrayRowCount1 == 9)
        {
            capturedPiecesArrayColCount1++;
            capturedPiecesArrayRowCount1 = 1;
        }
        capturingTurns[capturingTurnsCounter] = turns;
        capturingTurnsCounter++;
        piecesForUndo[turns] = piecesArray[finRow][finCol];
    }

    // if a piece from player 2 is captured
    else if (isupper(piecesArray[finRow][finCol]) )
    {
        capturedPieces[capturedPiecesArrayRowCount2][capturedPiecesArrayColCount2] = piecesArray[finRow][finCol];
        if (capturedPiecesArrayRowCount2 <= 8)
        {
            capturedPiecesArrayRowCount2++;
        }
        if (capturedPiecesArrayRowCount2 == 9)
        {
            capturedPiecesArrayColCount2++;
            capturedPiecesArrayRowCount2 = 1;
        }
        capturingTurns[capturingTurnsCounter] = turns;
        capturingTurnsCounter++;
        piecesForUndo[turns] = piecesArray[finRow][finCol];
    }

    // modifying pieces array and calling promotion function
    if(promotion() == 0)
    {
        system("cls");
        piecesArray[finRow][finCol] = piecesArray[initRow][initCol];
        if (piecesArray[initRow][initCol] == 'k')
        {
            kRowPos = finRow;
            kColPos = finCol;
        }
        else if (piecesArray[initRow][initCol] == 'K')
        {
            KRowPos = finRow;
            KColPos = finCol;
        }
        piecesArray[initRow][initCol] = ' ';
    }
    turns++;
}

/// check function, checks if the move is valid or not, Contains all movements functions calls.
int check()
{
    // Making sure no player moves the enemy's pieces
    if (player == 1)
    {
        while (piecesArray[initRow][initCol] == 'P' || piecesArray[initRow][initCol] == 'R' || piecesArray[initRow][initCol] == 'N' || piecesArray[initRow][initCol] == 'B' || piecesArray[initRow][initCol] == 'Q' || piecesArray[initRow][initCol] == 'K')
            input();
    }

    else if (player == -1)
    {
        while (piecesArray[initRow][initCol] == 'p' || piecesArray[initRow][initCol] == 'r' || piecesArray[initRow][initCol] == 'n' || piecesArray[initRow][initCol] == 'b' || piecesArray[initRow][initCol] == 'q' || piecesArray[initRow][initCol] == 'k')
            input();
    }
    // if not valid, return 0, else return 1
    int x = 0, x1 = 2, x2 = 2, x3 = 2, x4 = 2, x5 = 2, x6 = 2;

    if(x == 0)
        x1 = pawnMovement1();
    if(x1 == 0)
        x2 = pawnMovement2();
    if(x2 == 0)
        x3 = rookMovement();
    if(x3 == 0)
        x4 = knightMovement();
    if(x4 == 0)
        x5 = bishopMovement();
    if(x5 == 0)
        x6 = kingMovement();
    if(x1 == 1 || x2 == 1 || x3 == 1 || x4 == 1 || x5 == 1 || x6 == 1)
        return 1;
    else
        return 0;

}

/// player 1's pawn function
int pawnMovement1()
{
    // moving one step forward
    if (initRow - finRow == 1 && initCol == finCol && piecesArray[initRow][initCol] == 'p' && piecesArray[finRow][finCol] == ' ' )
    {
        if(initRow == 2 && strlen(moveString) == 5 && (moveString[4] == 'q' || moveString[4] == 'b' || moveString[4] == 'r' || moveString[4] == 'n'))
            return 1;
        else if(initRow != 2 && (strlen(moveString) == 4 || (strlen(moveString) == 5 && !isalpha(moveString[4]))))
            return 1;
        else
            return 0;
    }

    else if (initRow == 7 && initRow - finRow == 2 && piecesArray[initRow][initCol] == 'p' && initCol == finCol && piecesArray[finRow][finCol] == ' ' && isalpha(piecesArray[finRow+1][finCol]) == 0 && (strlen(moveString) == 4) )
    {
        return 1;
    }
    else if (initRow - finRow == 1 && piecesArray[initRow][initCol] == 'p' && abs(initCol - finCol) == 1 && isupper(piecesArray[finRow][finCol]) != 0 )
    {
        if(initRow == 2 && strlen(moveString) == 5 && (moveString[4] == 'q' || moveString[4] == 'b' || moveString[4] == 'r' || moveString[4] == 'n')) // Modified for promotion ^_^
            return 1;
        else if(initRow == 2)
            return 0;
        else if(strlen(moveString) == 4 && initRow != 2)
        {
            return 1;
        }
        else
            return 0;
    }
    else
        return 0;
}

/// player 2's pawn function
int pawnMovement2()
{
    if (finRow - initRow == 1 && initCol == finCol && piecesArray[initRow][initCol] == 'P' && piecesArray[finRow][finCol] == ' ' )
    {
        if(initRow == 7 && strlen(moveString) == 5 && (moveString[4] == 'Q' || moveString[4] == 'B' || moveString[4] == 'R' || moveString[4] == 'N'))
            return 1;
        else if(initRow != 7 && (strlen(moveString) == 4 || (strlen(moveString) == 5 && !isalpha(moveString[4]))))
            return 1;
        else
            return 0;
    }
    else if (initRow == 2 && finRow - initRow == 2 && piecesArray[initRow][initCol] == 'P' && initCol == finCol && piecesArray[finRow][finCol] == ' ' && isalpha(piecesArray[finRow-1][finCol]) == 0 )
    {
        return 1;
    }
    else if (finRow - initRow == 1 && piecesArray[initRow][initCol] == 'P' && abs(initCol - finCol) == 1 && islower(piecesArray[finRow][finCol]) != 0)
    {
        if(initRow == 7 && strlen(moveString) == 5 && (moveString[4] == 'Q' || moveString[4] == 'B' || moveString[4] == 'R' || moveString[4] == 'N'))
            return 1;
        else if(initRow == 7)
            return 0;
        else if(strlen(moveString) == 4 && initRow != 7)
        {
            return 1;
        }
        else
            return 0;
    }
    else
        return 0;

}

/// Rook function (queen is included)
int rookMovement()
{
    // does not capture a friendly piece
    if ((islower(piecesArray[finRow][finCol]) != 0 && player == 1) || (isupper(piecesArray[finRow][finCol]) != 0 && player == -1))
        return 0;
    // moving horizontally and can't leap over any piece
    else if (finRow == initRow && finCol != initCol && (piecesArray[initRow][initCol] == 'r' || piecesArray[initRow][initCol] == 'R' || piecesArray[initRow][initCol] == 'q' || piecesArray[initRow][initCol] == 'Q') && ((strlen(moveString) == 4) || (strlen(moveString) == 5 && promotionflag == 1)))
    {
        // if there are no pieces between initial and final positions, move, else, don't move.
        // moving from left to right
        if (initCol < finCol)
        {
            int count = initCol + 1;
            while (count < finCol)
            {
                if (piecesArray[initRow][count] != ' ')
                    return 0;
                count++;
            }
        }
        // moving from right to left
        if (finCol < initCol)
        {
            int count = finCol + 1;
            while (count < initCol)
            {
                if (piecesArray[initRow][count] != ' ')
                    return 0;
                count++;
            }
        }
        return 1;
    }
    // moving vertically
    else if (finCol == initCol && finRow != initRow && (piecesArray[initRow][initCol] == 'r' || piecesArray[initRow][initCol] == 'R' || piecesArray[initRow][initCol] == 'q' || piecesArray[initRow][initCol] == 'Q')&& ((strlen(moveString) == 4) || (strlen(moveString) == 5 && promotionflag == 1)))
    {
        // moving from up to down
        if (initRow < finRow)
        {
            int count = initRow + 1;
            while (count < finRow)
            {
                if (piecesArray[count][initCol] != ' ')
                    return 0;
                count++;
            }
        }
        //moving from down to up
        if (finRow < initRow)
        {
            int count = finRow + 1;
            while (count < initRow)
            {
                if (piecesArray[count][initCol] != ' ')
                    return 0;
                count++;
            }
        }
        return 1;
    }
    else
        return 0;
}

/// Knight's function
int knightMovement()
{
    // does not capture a friendly piece
    if ((islower(piecesArray[finRow][finCol]) != 0 && player == 1) || (isupper(piecesArray[finRow][finCol]) != 0 && player == -1))
        return 0;
    else if ((piecesArray[initRow][initCol] == 'n' || piecesArray[initRow][initCol] == 'N') && ((abs(finCol - initCol) == 1 && abs(finRow - initRow) == 2) || (abs(finCol - initCol) == 2 && abs(finRow - initRow) == 1)) && ((strlen(moveString) == 4 ) || (strlen(moveString) == 5 && promotionflag == 1)))
        return 1;
    else
        return 0;
}

/// Bishop's function (queen is included)
int bishopMovement()
{
    // does not capture a friendly piece
    if ((islower(piecesArray[finRow][finCol]) != 0 && player == 1) || (isupper(piecesArray[finRow][finCol]) != 0 && player == -1))
        return 0;
    // moving diagonally while checking if one of the 4 cases of movements, the bishop will leap over any piece
    else if ((piecesArray[initRow][initCol] == 'b' || piecesArray[initRow][initCol] == 'B' || piecesArray[initRow][initCol] == 'q' || piecesArray[initRow][initCol] == 'Q') && (abs(finRow - initRow) == abs(finCol - initCol)) && ((strlen(moveString) == 4 ) || (strlen(moveString) == 5 && promotionflag == 1)))
    {
        // case 1
        if (initCol < finCol && initRow > finRow)
        {
            int countRow = initRow - 1, countCol = initCol + 1;
            while (countRow > finRow && countCol < finCol)
            {
                if (piecesArray[countRow][countCol] != ' ')
                    return 0;
                countRow--;
                countCol++;
            }
        }

        // case 2
        if (initCol > finCol && initRow < finRow)
        {
            int countRow = initRow + 1, countCol = initCol - 1;
            while (countRow < finRow && countCol > finCol)
            {
                if (piecesArray[countRow][countCol] != ' ')
                {
                    return 0;
                }
                countRow++;
                countCol--;
            }
        }

        // case 3
        if (initCol > finCol && initRow > finRow)
        {
            int countRow = initRow - 1, countCol = initCol - 1;
            while (countRow > finRow && countCol > finCol)
            {
                if (piecesArray[countRow][countCol] != ' ')
                    return 0;
                countRow--;
                countCol--;
            }
        }

        // case 4
        if (initCol < finCol && initRow < finRow)
        {
            int countRow = initRow + 1, countCol = initCol + 1;
            while (countRow < finRow && countCol < finCol)
            {
                if (piecesArray[countRow][countCol] != ' ')
                    return 0;
                countRow++;
                countCol++;
            }
        }
        return 1;
    }
    else
        return 0;
}

/// King's function
int kingMovement()
{
    // does not capture a friendly piece
    if ((islower(piecesArray[finRow][finCol]) != 0 && player == 1) || (isupper(piecesArray[finRow][finCol]) != 0 && player == -1))
        return 0;
    // moves horizontally, diagonally or vertically one step.
    if ((piecesArray[initRow][initCol] == 'k' || piecesArray[initRow][initCol] == 'K') && ((initCol == finCol && abs(initRow - finRow) == 1) || (initRow == finRow && abs(finCol - initCol) == 1) || (abs(initCol - finCol) == 1 && abs(initRow - finRow)  == 1)) && strlen(moveString) == 4)
    {

        return 1;
    }
    else
        return 0;
}

///Checking danger for player1
int checkForPlayer1()
{
    int pieceinit = piecesArray[initRow][initCol];
    int piecefin = piecesArray[finRow][finCol];
    int kRowPostemp = kRowPos;
    int kColPostemp = kColPos;
    if(piecesArray[initRow][initCol] == 'k' )
    {
        piecesArray[finRow][finCol] = piecesArray[initRow][initCol];
        piecesArray[initRow][initCol] = ' ';
        kRowPos = finRow;
        kColPos = finCol;
    }
    else
    {
        piecesArray[finRow][finCol] = piecesArray[initRow][initCol];
        piecesArray[initRow][initCol] = ' ';
    }
    int dangertok = 0;
    int tempinitRow = initRow;
    int tempfinRow = finRow;
    int tempinitCol = initCol;
    int tempfinCol = finCol;
    int i , j , p = player;
    for(i=1; i<9; i++)
    {
        for(j=1; j<9; j++)
        {
            if(isupper(piecesArray[i][j]))
            {
                if(player == 1)
                    player *= -1;
                initRow = i;
                initCol = j;
                finRow = kRowPos;
                finCol = kColPos;
                if(piecesArray[i][j] != 'P')//Special case
                    dangertok = check();
                else if(piecesArray[i][j] == 'P' && (abs(initCol-finCol) == 1 || abs(initCol - finCol) == 0) && (piecesArray[finRow][initCol+1] == 'k' || piecesArray[finRow][initCol-1] == 'k') && abs(initRow-finRow) == 1 && finRow>initRow)///
                {
                    dangertok = 1;
                }
                player *= -1;
                if(dangertok == 1)
                {
                    kRowPos = kRowPostemp;
                    kColPos = kColPostemp;
                    player = p;
                    initRow = tempinitRow ;
                    finRow = tempfinRow ;
                    initCol = tempinitCol ;
                    finCol = tempfinCol ;
                    piecesArray[initRow][initCol] = pieceinit;
                    piecesArray[finRow][finCol] = piecefin;
                    return 1;
                }
            }
        }
    }
    initRow = tempinitRow ;
    finRow = tempfinRow ;
    initCol = tempinitCol ;
    finCol = tempfinCol ;
    if(dangertok == 0)
    {
        kRowPos = kRowPostemp;
        kColPos = kColPostemp;
        player = p;
        piecesArray[initRow][initCol] = pieceinit;
        piecesArray[finRow][finCol] = piecefin;
        return 0;
    }
}

/// Checking danger for player2
int checkForPlayer2()
{
    int pieceinit = piecesArray[initRow][initCol];
    int piecefin = piecesArray[finRow][finCol];
    int KRowPostemp = KRowPos;
    int KColPostemp = KColPos;
    if(piecesArray[initRow][initCol] == 'K')
    {
        piecesArray[finRow][finCol] = piecesArray[initRow][initCol];
        piecesArray[initRow][initCol] = ' ';
        KRowPos = finRow;
        KColPos = finCol;
    }
    else
    {
        piecesArray[finRow][finCol] = piecesArray[initRow][initCol];
        piecesArray[initRow][initCol] = ' ';
    }
    int dangertoK = 0;
    int tempinitRow = initRow;
    int tempfinRow = finRow;
    int tempinitCol = initCol;
    int tempfinCol = finCol;
    int i , j , p = player;
    for(i=1; i<9; i++)
    {
        for(j=1; j<9; j++)
        {
            if(islower(piecesArray[i][j]))
            {
                if(player == -1)
                    player *= -1;
                initRow = i;
                initCol = j;
                finRow = KRowPos;
                finCol = KColPos;
                if(piecesArray[i][j] != 'p')// Special Case
                    dangertoK = check();
                else if(piecesArray[i][j] == 'p' && (abs(initCol-finCol) == 1 || abs(initCol - finCol) == 0) && (piecesArray[finRow][initCol+1] == 'K' || piecesArray[finRow][initCol-1] == 'K') && abs(initRow-finRow) == 1 && finRow<initRow)///
                {
                    dangertoK = 1;
                }
                player *= -1;
                if(dangertoK == 1)
                {
                    KRowPos = KRowPostemp;
                    KColPos = KColPostemp;
                    initRow = tempinitRow ;
                    finRow = tempfinRow ;
                    initCol = tempinitCol ;
                    finCol = tempfinCol ;
                    player = p;
                    piecesArray[initRow][initCol] = pieceinit;
                    piecesArray[finRow][finCol] = piecefin;
                    return 1;
                }
            }
        }
    }
    initRow = tempinitRow ;
    finRow = tempfinRow ;
    initCol = tempinitCol ;
    finCol = tempfinCol ;
    if(dangertoK == 0)
    {
        KRowPos = KRowPostemp;
        KColPos = KColPostemp;
        player = p;
        piecesArray[initRow][initCol] = pieceinit;
        piecesArray[finRow][finCol] = piecefin;
        return 0;
    }
}

/// Validating that king1 can get out of the check state by escaping himself
int escapeKingRoute()
{
    int kingMove = 0;
    int placeDanger = 0;
    int abilityToMove = 8;

    // storing initial positions
    int tempinitRow = initRow;
    int tempinitCol = initCol;

    if (player == 1)
    {
        initRow = KRowPos;
        initCol = KColPos;

        int i;
        for (i = 0; i < 8; i++)
        {
            switch (i)
            {
            case 0:
                finRow = KRowPos;
                finCol = KColPos + 1;
                break;
            case 1:
                finRow = KRowPos + 1;
                finCol = KColPos;
                break;
            case 2:
                finRow = KRowPos;
                finCol = KColPos - 1;
                break;
            case 3:
                finRow = KRowPos - 1;
                finCol = KColPos;
                break;
            case 4:
                finRow = KRowPos + 1;
                finCol = KColPos + 1;
                break;
            case 5:
                finRow = KRowPos + 1;
                finCol = KColPos - 1;
                break;
            case 6:
                finRow = KRowPos - 1;
                finCol = KColPos + 1;
                break;
            case 7:
                finRow = KRowPos - 1;
                finCol = KColPos - 1;
                break;
            }

            if (finRow >= 1 && finRow <= 8 && finCol >= 1 && finCol <= 8)
            {
                if (isupper(piecesArray[finRow][finCol])) // momken yeb2a fee corner case hena!!
                    kingMove = 0;
                else
                    kingMove = 1;

                placeDanger = checkForPlayer2(); /// check for the destination if there is a check
            }
            else
            {
                kingMove = 0;
                placeDanger = 0;
            }

            if (kingMove == 0 || placeDanger == 1)
                abilityToMove--;
        }

        if (abilityToMove == 0)
        {
            // here goes the pawn flag
            initRow = tempinitRow;
            initCol = tempinitCol;
            return 1;
        }
    }


    else if (player == -1) // Then I am trying to make 'k' escape.
    {
        initRow = kRowPos;
        initCol = kColPos;
        // loop to check the 8 places.
        int i;
        for(i=0; i<8; i++)
        {
            switch(i)
            {
            case 0:
                finRow = kRowPos;
                finCol = kColPos + 1;
                break;
            case 1:
                finRow = kRowPos + 1;
                finCol = kColPos;
                break;
            case 2:
                finRow = kRowPos;
                finCol = kColPos - 1;
                break;
            case 3:
                finRow = kRowPos - 1;
                finCol = kColPos;
                break;
            case 4:
                finRow = kRowPos + 1;
                finCol = kColPos + 1;
                break;
            case 5:
                finRow = kRowPos + 1;
                finCol = kColPos - 1;
                break;
            case 6:
                finRow = kRowPos - 1;
                finCol = kColPos + 1;
                break;
            case 7:
                finRow = kRowPos - 1;
                finCol = kColPos - 1;
                break;
            }

            //As long as we remain inside the borders of the board.
            if (finRow >= 1 && finRow <= 8 && finCol >= 1 && finCol <= 8)
            {
                if (islower(piecesArray[finRow][finCol]))
                    kingMove = 0;
                else
                    kingMove = 1;

                placeDanger = checkForPlayer1(); /// check for the destination if there is a check
            }
            else
            {
                kingMove = 0;
                placeDanger = 0;
            }

            if (kingMove == 0 || placeDanger == 1)
                abilityToMove--;

        }

        // after the end of the loop, if AbilityToMove == 0,checkmate.
        if(abilityToMove == 0)
        {
            //pawnFlag = 0;
            initRow = tempinitRow;
            initCol = tempinitCol;
            return 1;
        }
    }
    //pawnFlag = 0;
    initRow = tempinitRow;
    initCol = tempinitCol;
    return 0;
}

int promotion()
{
    promotionflag = 0;
    /// Constructing Promotion for player 1
    if(moveString[4] == 'r' && player == 1 && finRow == 1 && piecesArray[initRow][initCol] == 'p')
    {
        system("cls");
        piecesArray[finRow][finCol] = 'r';
        piecesArray[initRow][initCol] = ' ';
        promotionflag = 1;
        return 1;
    }
    else if(moveString[4] == 'b' && player == 1 && finRow == 1 && piecesArray[initRow][initCol] == 'p')
    {
        system("cls");
        piecesArray[finRow][finCol] = 'b';
        piecesArray[initRow][initCol] = ' ';
        promotionflag = 1;
        return 1;
    }
    else if(moveString[4] == 'q' && player == 1 && finRow == 1 && piecesArray[initRow][initCol] == 'p')
    {
        system("cls");
        piecesArray[finRow][finCol] = 'q';
        piecesArray[initRow][initCol] = ' ';
        promotionflag = 1;
        return 1;
    }
    else if(moveString[4] == 'n' && player == 1 && finRow == 1 && piecesArray[initRow][initCol] == 'p')
    {
        system("cls");
        piecesArray[finRow][finCol] = 'n';
        piecesArray[initRow][initCol] = ' ';
        promotionflag = 1;
        return 1;
    }


    //For player 2....Promotion ^_^
    else if(moveString[4] == 'R' && player == -1 && finRow == 8 && piecesArray[initRow][initCol] == 'P')
    {
        system("cls");
        piecesArray[finRow][finCol] = 'R';
        piecesArray[initRow][initCol] = ' ';
        promotionflag = 1;
        return 1;
    }
    else if(moveString[4] == 'B' && player == -1 && finRow == 8 && piecesArray[initRow][initCol] == 'P')
    {
        system("cls");
        piecesArray[finRow][finCol] = 'B';
        piecesArray[initRow][initCol] = ' ';
        promotionflag = 1;
        return 1;
    }
    else if(moveString[4] == 'Q' && player == -1 && finRow == 8 && piecesArray[initRow][initCol] == 'P')
    {
        system("cls");
        piecesArray[finRow][finCol] = 'Q';
        piecesArray[initRow][initCol] = ' ';
        promotionflag = 1;
        return 1;
    }
    else if(moveString[4] == 'N' && player == -1 && finRow == 8 && piecesArray[initRow][initCol] == 'P')
    {
        system("cls");
        piecesArray[finRow][finCol] = 'N';
        piecesArray[initRow][initCol] = ' ';
        promotionflag = 1;
        return 1;
    }
    else
    {
        promotionflag = 0;
        return 0;
    }
}

//Checking after promotion if there is a check.
int checkAfterPromotion()
{
    if(promotionflag == 1 && player == 1)
    {
        initRow = finRow;
        initCol = finCol;
        finRow = KRowPos;
        finCol = KColPos;
        if(check() == 1)
            return 1;
        else
            return 0;
    }
    else if(promotionflag == 1 && player == -1)
    {
        initRow = finRow;
        initCol = finCol;
        finRow = kRowPos;
        finCol = kColPos;
        if(check() == 1)
            return 1;
        else
            return 0;
    }
    else
        return 0;   /// changed ^_^.
}

//Saving function
void save()
{
    FILE *log1;
    FILE *log2;
    char z;
    int i , j ;
    log1 = fopen("Pieces.txt","w");
    log2 = fopen("Captured.txt", "w");
    for(i=0; i<10; i++)
    {
        for(j=0; j<10; j++)
        {
            z = piecesArray[i][j];
            fputc(z,log1);
        }
    }

    // saving Global Variables
    fputc(player + 48, log1);
    fputc(kRowPos + 48, log1);
    fputc(kColPos + 48, log1);
    fputc(KRowPos + 48, log1);
    fputc(KColPos + 48, log1);
    fputc(promotionflag + 48, log1);
    fputc(numOfChecks + 48, log1);
    fputc(initRow + 48, log1);
    fputc(initCol + 48, log1);
    fputc(finRow + 48, log1);
    fputc(finCol + 48, log1);
    fputc(capturedPiecesArrayRowCount1 + 48, log1);
    fputc(capturedPiecesArrayColCount1 + 48, log1);
    fputc(capturedPiecesArrayRowCount2 + 48, log1);
    fputc(capturedPiecesArrayColCount2 + 48, log1);


    for(i=0; i<9; i++)
    {
        for(j=0; j<4; j++)
        {
            fputc(capturedPieces[i][j],log2);
        }
    }

    fclose(log1);
    fclose(log2);
}
//Loading function
void load()
{
    FILE *log1;
    FILE *log2;
    char z;
    int i , j;
    log1 = fopen("Pieces.txt","r");
    log2 = fopen("Captured.txt", "r");
    if (log1 == NULL)
    {
        printf("ERROR : Couldn't open the file.\n");
    }
    else
    {
        for(i=0; i<10; i++)
        {
            for(j=0; j<10; j++)
            {
                z = fgetc(log1);
                piecesArray[i][j] = z;

            }
        }

        // Loading Global Variables
        player = fgetc(log1);
        if (player == 49)
            player = 1;
        else if (player == 47)
            player == -1;

        // All other variables
        kRowPos = fgetc(log1) - 48;
        kColPos = fgetc(log1) - 48;
        KRowPos = fgetc(log1) - 48;
        KColPos = fgetc(log1) - 48;
        promotionflag = fgetc(log1) - 48;
        numOfChecks = fgetc(log1) - 48;
        initRow = fgetc(log1) - 48;
        initCol = fgetc(log1) - 48;
        finRow = fgetc(log1) - 48;
        finCol = fgetc(log1) - 48;
        capturedPiecesArrayRowCount1 = fgetc(log1) - 48;
        capturedPiecesArrayColCount1 = fgetc(log1) - 48;
        capturedPiecesArrayRowCount2 = fgetc(log1) - 48;
        capturedPiecesArrayColCount2 = fgetc(log1) - 48;

        for(i=0; i<9; i++)
        {
            for(j=0; j<4; j++)
            {
                z = fgetc(log2);
                capturedPieces[i][j] = z;
            }
        }
    }
    fclose(log1);
    fclose(log2);
    system("cls");
    boardPrinting();
}

//Checking for defending Check mate.
int checkForAllyKing() // hte3melaha call fel main .. ba3d lama yeb2a fee check 3la king .. hashoof lw dah checkmate wla la2.
{
    int i ,j ,m ,n ;
    for(i=1; i<9; i++) // iterating for every piece in board.
    {
        for(j=1; j<9; j++)
        {
            if(player == 1 && isupper(piecesArray[i][j]))
            {
                initRow = i;
                initCol = j;
                for(m=1; m<9; m++)
                {
                    for(n=1; n<9; n++)
                    {
                        finRow = m;
                        finCol = n;
                        player = -1;
                        if(check() && !checkForPlayer2())
                        {
                            player = 1;
                            return 1;// keda ba3d ama a7arakha msh hyb2a fee check
                        }
                        player = 1;
                    }
                }
            }
            else if(player == -1 && islower(piecesArray[i][j]))
            {
                initRow = i;
                initCol = j;
                for(m=1; m<9; m++)
                {
                    for(n=1; n<9; n++)
                    {
                        finRow = m;
                        finCol = n;
                        player = 1;
                        if(check() && !checkForPlayer1())
                        {
                            player = -1;
                            return 1;
                        }
                        player = -1;
                    }
                }
            }
        }
    }
    return 0;
}

/// Stalemate Function.
int checkForStalemate()
{
    char pieceinit = piecesArray[initRow][initCol];
    char piecefin = piecesArray[finRow][finCol];
    int tempinitRow = initRow;
    int tempfinRow = finRow;
    int tempinitCol = initCol;
    int tempfinCol = finCol;
    int p = player;
    int i ,j ,m ,n;

    for(i=1; i<9; i++) // iterating for every piece in board.
    {
        for(j=1; j<9; j++)
        {
            if(player == 1 && isupper(piecesArray[i][j]))
            {
                initRow = i;
                initCol = j;
                for(m=1; m<9; m++)
                {
                    for(n=1; n<9; n++)
                    {
                        finRow = m;
                        finCol = n;
                        player = -1;
                        if(!checkForPlayer2() && check() && (KRowPos != finRow && KColPos != finCol)    )//We must Cover that a piece cant move if it will make a check for its king.
                        {
                            initRow = tempinitRow ;
                            finRow = tempfinRow ;
                            initCol = tempinitCol ;
                            finCol = tempfinCol ;
                            piecesArray[initRow][initCol] = pieceinit;
                            piecesArray[finRow][finCol] = piecefin;
                            player = p;
                            return 1;// it will return one if there is a ---->valid move .
                        }
                        player = p;
                    }
                    player = p;
                }
                player = p;
            }
            else if(player == -1 && islower(piecesArray[i][j]))
            {
                initRow = i;
                initCol = j;
                for(m=1; m<9; m++)
                {
                    for(n=1; n<9; n++)
                    {
                        finRow = m;
                        finCol = n;
                        player = 1;
                        if(check() && !checkForPlayer1() && (kRowPos != finRow && kColPos != finCol))
                        {
                            initRow = tempinitRow ;
                            finRow = tempfinRow ;
                            initCol = tempinitCol ;
                            finCol = tempfinCol ;
                            piecesArray[initRow][initCol] = pieceinit;
                            piecesArray[finRow][finCol] = piecefin;
                            player = p;
                            return 1;
                        }
                        player = p;
                    }
                    player = p;
                }
            }
        }
    }

    initRow = tempinitRow ;
    finRow = tempfinRow ;
    initCol = tempinitCol ;
    finCol = tempfinCol ;
    piecesArray[initRow][initCol] = pieceinit;
    piecesArray[finRow][finCol] = piecefin;
    player = p;
    return 0;
}
// Fill the 2d array of moves.
void recordingMoves()
{
    movesOfGame[turnsCounterRow][0] = moveString[0];
    movesOfGame[turnsCounterRow][1] = moveString[1];
    movesOfGame[turnsCounterRow][2] = moveString[2];
    movesOfGame[turnsCounterRow][3] = moveString[3];
    movesOfGame[turnsCounterRow][4] = moveString[4];
}
// Undo-ing
void undo()
{
    // Flags
    turns--;
    checkAfterUndo = 0;
    undoFlag++;

    turnsCounterRow--;

    moveString[0] = movesOfGame[turnsCounterRow][0];
    moveString[1] = movesOfGame[turnsCounterRow][1];
    moveString[2] = movesOfGame[turnsCounterRow][2];
    moveString[3] = movesOfGame[turnsCounterRow][3];
    moveString[4] = movesOfGame[turnsCounterRow][4];

    // swapping initial position with final position
    finCol = moveString[0];
    initCol = moveString[2];

    if (initCol >= 'A' && initCol <= 'Z' && finCol >= 'A' && finCol <= 'Z')
    {
        finCol = moveString[0] - 'A' + 1;
        initCol = moveString[2] - 'A' + 1;
    }
    else if (initCol >= 'a' && initCol <= 'z' && finCol >= 'a' && finCol <= 'z')
    {
        finCol = moveString[0] - 'a' + 1;
        initCol = moveString[2] - 'a' + 1;
    }
    // swapping initial position with final position
    finRow = 9 - (moveString[1] - 48);
    initRow = 9 - (moveString[3] - 48);

    if(moveString[4] != '\0')
    {
        if(isupper(moveString[4]))
        {
            piecesArray[finRow][finCol] = 'P';
            piecesArray[initRow][initCol] = ' ';

            // returning captured pieces
            if(isalpha(piecesForUndo[turns]))
            {
                piecesArray[initRow][initCol] = piecesForUndo[turns];

                /// REMOVING THE PIECE FROM THE CAPTURED PIECES
                if (isupper(piecesForUndo[turns]))
                {
                    capturedPieces[capturedPiecesArrayRowCount2 - 1][capturedPiecesArrayColCount2] = ' ';
                    capturedPiecesArrayRowCount2--;
                }

                else if (islower(piecesForUndo[turns]))
                {
                    capturedPieces[capturedPiecesArrayRowCount1 - 1][capturedPiecesArrayColCount1] = ' ';
                    capturedPiecesArrayRowCount1--;
                }
                piecesForUndo[turns] = '\0';
            }
        }
        else if(islower(moveString[4]))
        {
            piecesArray[finRow][finCol] = 'p';
            piecesArray[initRow][initCol] = ' ';
            // returning captured pieces
            if(isalpha(piecesForUndo[turns]))
            {
                /// WE SHOULD DO SOMETHING TO PRINT CHECK WHEN UNDOED
                piecesArray[initRow][initCol] = piecesForUndo[turns];

                /// REMOVING THE PIECE FROM THE CAPTURED PIECES
                if (isupper(piecesForUndo[turns]))
                {
                    capturedPieces[capturedPiecesArrayRowCount2 - 1][capturedPiecesArrayColCount2] = ' ';
                    capturedPiecesArrayRowCount2--;
                }

                else if (islower(piecesForUndo[turns]))
                {
                    capturedPieces[capturedPiecesArrayRowCount1 - 1][capturedPiecesArrayColCount1] = ' ';
                    capturedPiecesArrayRowCount1--;
                }
                piecesForUndo[turns] = '\0';
            }
        }
    }
    else
    {
        // updating king's position if he was moved then Undo-ed.
        if (piecesArray[initRow][initCol] == 'k')
        {
            kRowPos = finRow;
            kColPos = finCol;
        }
        else if(piecesArray[initRow][initCol] == 'K')
        {
            KRowPos = finRow;
            KColPos = finCol;
        }

        if(checkForPlayer1() || checkForPlayer2())
            checkAfterUndo = 1;

        piecesArray[finRow][finCol] = piecesArray[initRow][initCol];
        piecesArray[initRow][initCol] = ' ';

        // returning captured pieces
        if(isalpha(piecesForUndo[turns]))
        {
            /// WE SHOULD DO SOMETHING TO PRINT CHECK WHEN UNDOED
            piecesArray[initRow][initCol] = piecesForUndo[turns];

            /// REMOVING THE PIECE FROM THE CAPTURED PIECES
            if (isupper(piecesForUndo[turns]))
            {
                capturedPieces[capturedPiecesArrayRowCount2 - 1][capturedPiecesArrayColCount2] = ' ';
                capturedPiecesArrayRowCount2--;
            }

            else if (islower(piecesForUndo[turns]))
            {
                capturedPieces[capturedPiecesArrayRowCount1 - 1][capturedPiecesArrayColCount1] = ' ';
                capturedPiecesArrayRowCount1--;
            }
            piecesForUndo[turns] = '\0';
        }
    }

    /*int temp1 = finRow;
    int temp2 = finCol;
    finRow = initRow;
    finCol = initCol;
    initRow = temp1;
    initCol = temp2;*/
    turnsCounterRow++;
    player *= -1;
    system("cls");
    boardPrinting();
}

// Redoing.
void redo()
{
    redoFlag++;
    turnsCounterRow--;

    // Assigning values
    moveString[0] = movesOfGame[turnsCounterRow][0];
    moveString[1] = movesOfGame[turnsCounterRow][1];
    moveString[2] = movesOfGame[turnsCounterRow][2];
    moveString[3] = movesOfGame[turnsCounterRow][3];
    moveString[4] = movesOfGame[turnsCounterRow][4];

    // Doing the move as it was
    initCol = moveString[0];
    finCol = moveString[2];
    if (initCol >= 'A' && initCol <= 'Z' && finCol >= 'A' && finCol <= 'Z')
    {
        initCol = moveString[0] - 'A' + 1;
        finCol = moveString[2] - 'A' + 1;
    }
    else if (initCol >= 'a' && initCol <= 'z' && finCol >= 'a' && finCol <= 'z')
    {
        initCol = moveString[0] - 'a' + 1;
        finCol = moveString[2] - 'a' + 1;
    }
    initRow = 9 - (moveString[1] - 48);
    finRow = 9 - (moveString[3] - 48);

    // Checking for promotion
    if(moveString[4] != '\0')
    {
        if(isupper(moveString[4]))
        {
            piecesArray[initRow][initCol] = ' ';
            piecesArray[finRow][finCol] = moveString[4];
        }
        else if(islower(moveString[4]))
        {
            piecesArray[initRow][initCol] = ' ';
            piecesArray[finRow][finCol] = moveString[4];
        }
    }
    else
    {
        // updating king's position if he was moved then Undo-ed.
        if (piecesArray[initRow][initCol] == 'k')
        {
            kRowPos = initRow;
            kColPos = initCol;
        }
        else if(piecesArray[initRow][initCol] == 'K')
        {
            KRowPos = initRow;
            KColPos = initCol;
        }

        if(checkForPlayer1() || checkForPlayer2())
            checkAfterUndo = 1;
    }

    if (isalpha(piecesArray[finRow][finCol]))
    {
        // IF A PIECE WAS CAPTURED AFTER UNDOING, THEN SAVE IT IN THE ARRAY
        piecesForUndo[turns] = piecesArray[finRow][finCol];

        if (player == -1)
        {
            capturedPieces[capturedPiecesArrayRowCount1][capturedPiecesArrayColCount1] = piecesArray[finRow][finCol];
            capturedPiecesArrayRowCount1++;
        }
        else if (player == 1)
        {
            capturedPieces[capturedPiecesArrayRowCount2][capturedPiecesArrayColCount2] = piecesArray[finRow][finCol];
            capturedPiecesArrayRowCount2++;
        }
    }

    // DOING THE ACTUAL MOVE
    piecesArray[finRow][finCol] = piecesArray[initRow][initCol];
    piecesArray[initRow][initCol] = ' ';

    turns++;
    turnsCounterRow++;
    player *= -1;
    system("cls");
    boardPrinting();
}
