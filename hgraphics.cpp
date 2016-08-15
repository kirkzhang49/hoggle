/*
  File: hgraphics.cpp
  ------------------
  This file implements the hgraphics.h interface, which provides
  the graphics support for the Hoggle program.

 NOTE: In general you should not need to read anything in this file,
 put feel free to change these if you get inspired and are attempting
 an extension for extra credit.

 */


#include "hgraphics.h"
#include "lpclib.h"
#include "vector.h"
using namespace std;

/*
 * Constants
 * ---------
 * These constants define the graphical parameters of the display.
 * The purpose of each constant should be clear from its name, except
 * for Dimension. Dimension indicates the length of the longest row
 * (the center of the hexagon)
 */

#define ScreenWidth     800
#define Dimension       5  // Dimension must be an odd number
#define Middle			(Dimension / 2)
#define HexSize         36
#define MinorUnit		(HexSize * 0.86602540378)
#define FullSpoke       (HexSize)
#define HalfSpoke       (HexSize / 2.0)
#define BoardWidth      (2 * Dimension * MinorUnit)
#define BoardHeight		(HexSize * (1 + 1.5 * Dimension))
#define BorderWidth     15
#define MessageWidth    600
#define MessageSize     24
#define MessageRows     5
#define MessageLeft     (ScreenWidth - MessageWidth) / 2

#define HEXES_PER_ROW(x) (Dimension - (x > Middle ? (x - Middle) : (Middle - x)))

#define BoardX          200
#define BoardTopY       30
#define DividerLineY    (BoardTopY + 5)
#define ScoreY          (DividerLineY - ScoreSize)
#define WordY           (BoardTopY + 5)

#define HumanX          15
#define ComputerX       (BoardX + BoardWidth + BorderWidth)

#define WordHeight      13
#define LetterSize      32
#define ScoreSize       24

#define ColumnWidth     85
#define HumanColumns    2
#define ComputerColumns 3

enum vertexOrder { vTopmost=0, vTopRight, vBottomRight, vBottommost, vBottomLeft, vTopLeft };

struct point
{
    int x;
    int y;
    point(int xx=0, int yy=0): x(xx), y(yy) { }
};

point vertexGrid[Dimension][Dimension][6];

struct colorEntryT
{
    string colorName;
    double red, green, blue;
};

static colorEntryT colorTable[] = {
    {"Background", 255, 255, 255 },
    {"BoardColor",  0.8*255, 0.8*255, 0.8*255},     /* gray  */
    {"HexColor",    0.60*255, 0.80*255, 0.70*255},     /* almost white    */
    {"LetterColor", 0.01*255, 0.70*255, 0.85*255},     /* dark blue       */
    {"WordColor",   0.01*255, 0.30*255, 0.30*255},     /* med green-blue  */
    {"LabelColor",  0.8*255, 0.45*255, 0.25*255},     /* dark green-blue */
    {"MessageColor",  0.51*255, 0.85*255, 0.05*255},
};

static int nColors = sizeof colorTable / sizeof colorTable[0];
static int numWords[2];
static int scores[2];
static char letters[Dimension][Dimension];
static bool initialized = false;
static Vector<string> messageLines;
static int messageRow;

static void Init(void);
static void DrawDice(void);
static void DrawDie(int row, int col, char ch, bool highlight);
static void DrawCenteredChar(string color, int row, int col, char ch);
static void DrawScoreboard(void);
static void DrawBorder(void);
static void UpdateScore(int pointsToAdd, playerT player);
static void DrawOneScore(playerT player, int value);
static void CheckBounds(int row, int col);
static void DrawHex(int r, int c, string color);
//void DrawHex2 (int r, int c, string color);

void DrawBoard()
{
    if (!initialized) Init();
    initialized = true;
    scores[Human] = scores[Computer] = 0;
    numWords[Human] = numWords[Computer] = 0;
    DrawRectangle(0, 0, ScreenWidth, BoardTopY + BoardHeight, "Background", true);
    DrawBorder();
    DrawDice();
    DrawScoreboard();
    UpdateDisplay();
}

void LabelHex(int row, int col, char letter)
{
    if (!initialized) Init();
	CheckBounds(row, col);
    letters[row][col] = letter;
    DrawDie(row, col, letter, false);
}

void HighlightHex(int row, int col, bool flag)
{
    if (!initialized) Init();
    CheckBounds(row, col);
//    DrawHex2(row,col,"HexColor");
    DrawDie(row, col, letters[row][col], flag);
    UpdateDisplay();
}

void RecordNewWord(string word, playerT player)
{
    int row, col, nColumns;
    double x;

    switch (player) {
      case Human:
        nColumns = HumanColumns; x = HumanX; break;
      case Computer:
        nColumns = ComputerColumns; x = ComputerX; break;
    }
    row = numWords[player] / nColumns;
    col = numWords[player] % nColumns;
    DrawString(word, x + col * ColumnWidth, WordY + row * WordHeight, "WordColor");
    UpdateScore(word.length() >= 3 ? ((int) word.length() - 2) : 0, player);
    numWords[player]++;
    UpdateDisplay();
}

static void Init(void)
{
    for (int i = 0; i < nColors; i++) {
        DefineColor(colorTable[i].colorName,
                    colorTable[i].red,
                    colorTable[i].green,
                    colorTable[i].blue);
    }

    GraphicsWindow(ScreenWidth, BoardTopY + BoardHeight + MessageRows*MessageSize, "Background");

    double x, y;

    for (int r = 0; r < Dimension; r++)
    {
        for (int c = 0; c < HEXES_PER_ROW(r); c++)
        {
            x = BoardX +
                MinorUnit * (r > Middle ? ( r - Middle ) : ( Middle - r ) ) +
                MinorUnit * ( 1 + 2 * c);
            y = BoardTopY + HexSize * (1 + 1.5 * r);

            vertexGrid[r][c][vTopmost] = point(x, y-FullSpoke);
            vertexGrid[r][c][vTopLeft] = point(x-MinorUnit, y-HalfSpoke);
            vertexGrid[r][c][vTopRight] = point(x+MinorUnit, y-HalfSpoke);
            vertexGrid[r][c][vBottommost] = point(x, y+FullSpoke);
            vertexGrid[r][c][vBottomLeft] = point(x-MinorUnit, y+HalfSpoke);
            vertexGrid[r][c][vBottomRight] = point(x+MinorUnit, y+HalfSpoke);
        }
    }

    for (int i = 0; i < MessageRows; i++)
        messageLines.add(string(""));
    messageRow = 0;
}

static void DrawDice(void)
{
    if (!initialized) Init();
    int row, col;
        for (row = 0; row < Dimension; row++) {
			for (col = 0; col < HEXES_PER_ROW(row); col++) {
				DrawDie(row, col, ' ', false);
        }
    }
}

static void DrawDie(int row, int col, char ch, bool highlight)
{
    string hexcolor = (highlight) ? "LetterColor" : "HexColor";
    string lettercolor = (highlight) ? "red" : "LetterColor";
    DrawHex(row, col, hexcolor);
    DrawCenteredChar(lettercolor, row, col, ch);
}

static void DrawCenteredChar(string color, int row, int col, char ch)
{
    double centerX = BoardX +
        MinorUnit * ( row > Middle ? ( row - Middle ) : ( Middle - row ) ) +
        MinorUnit * ( 1 + 2 * col);
    double centerY = BoardTopY + HexSize * (1 + 1.5 * row);

    if (!initialized) Init();
    char buffer[2];
    buffer[0] = ch;
    buffer[1] = '\0';
    string str = buffer;
    int w = StringWidth(str, LetterSize);
    DrawString(str, centerX-w/2+1, centerY-LetterSize/2+1, color, LetterSize);
}


static void DrawScoreboard(void)
{
    DrawLine(HumanX, DividerLineY, BoardX - BorderWidth, DividerLineY, "LabelColor");
    DrawLine(ComputerX, DividerLineY, GetWidth()-HumanX, DividerLineY, "LabelColor");

    DrawString("Human", HumanX, ScoreY, "LabelColor", ScoreSize);
    DrawString("Computer", ComputerX, ScoreY, "LabelColor", ScoreSize);

    DrawOneScore(Human, scores[Human]);
    DrawOneScore(Computer, scores[Human]);
}


static void DrawBorder(void)
{
//    DrawRectangle(BoardX - BorderWidth/2, BoardTopY - BorderWidth/2,
//                  BoardWidth + BorderWidth, BoardHeight, "BoardColor", true);
    DrawImage("ball.bmp",BoardX - BorderWidth/2,BoardTopY - BorderWidth/2,BoardWidth + BorderWidth,BoardHeight);
                  ///ball.bmp
}


static void UpdateScore(int pointsToAdd, playerT player)
{
   // SetEraseMode(true);
    //DrawOneScore(player, scores[player]);
    //SetEraseMode(false);
    scores[player] += pointsToAdd;
    DrawOneScore(player, scores[player]);
}

static void DrawOneScore(playerT player, int value)
{
    char buffer[8];
    sprintf(buffer, "%d", value);

    DrawRectangle( ((player == Human) ? (BoardX - 50) : (GetWidth() - 50)), ScoreY,
    35, ScoreSize-1, "white", true);

    DrawString(string(buffer), ((player == Human) ? (BoardX - 50) : (GetWidth() - 50)), ScoreY, "LabelColor", ScoreSize);

    UpdateDisplay();
}

void DrawHex(int r, int c, string color)
{
    CheckBounds(r, c);
    Vector<int> vertices;
    for (int k = 0; k < 6; k++)
    {
        vertices.add(vertexGrid[r][c][k].x);
        vertices.add(vertexGrid[r][c][k].y);
    }

//    DrawPolygon(vertices, color);

    for (int k = 0; k < 6; k++)
    {
        DrawLine(vertexGrid[r][c][k].x, vertexGrid[r][c][k].y,
                 vertexGrid[r][c][(k+1)%6].x, vertexGrid[r][c][(k+1)%6].y,
                 "black");
    }
}
//void DrawHex2 (int r, int c, string color)
//{
//    CheckBounds(r, c);
//    Vector<int> vertices;
//    for (int k = 0; k < 6; k++)
//    {
//        vertices.add(vertexGrid[r][c][k].x);
//        vertices.add(vertexGrid[r][c][k].y);
//    }
//
//    DrawPolygon(vertices, color);
//}

static void CheckBounds(int row, int col)
{

	if (row < 0 || row >= Dimension)
		Error("Row out of bounds: " + IntToString(row));
	if (col < 0 || col >= HEXES_PER_ROW(row))
	{
		string msg = "Column out of bounds: ";
		msg += IntToString(col);
		msg += " (in row ";
		msg += IntToString(row);
		msg += ")";
		Error(msg);
	}
}

int DiceInRow(int row)
{
	return (HEXES_PER_ROW(row));
}

char LetterAt(int row, int col)
{
    CheckBounds(row, col);
	return letters[row][col];
}

bool PointIsInHex(int x, int y, int row, int col)
{
    CheckBounds(row, col);
    // bounds test for convex polygon
    // all cross-products should be positive
    for (int k = 0; k < 6; k++)
    {
        point sideVector = point(vertexGrid[row][col][(k+1)%6].x-
                                 vertexGrid[row][col][(k)%6].x,
                                 vertexGrid[row][col][(k+1)%6].y-
                                 vertexGrid[row][col][(k)%6].y);
        point pointVector = point(x - vertexGrid[row][col][(k)%6].x,
                                  y - vertexGrid[row][col][(k)%6].y);
        if ( (sideVector.x*pointVector.y - sideVector.y*pointVector.x) < 0)
            return false;
    }
    return true;
}

static void RenderMessageRow(int r)
{
    DrawRectangle(MessageLeft, BoardTopY + BoardHeight + r*MessageSize,
                  ScreenWidth, MessageSize, "Background", true);
    DrawString(messageLines[r], MessageLeft, BoardTopY + BoardHeight + r*MessageSize, "MessageColor", MessageSize);
    DrawRectangle(MessageLeft + MessageWidth, BoardTopY + BoardHeight,
                  (ScreenWidth - MessageWidth) / 2, MessageSize * MessageRows, "Background", true);
}

void PrintMessage(string msg)
{
    char c;
    for (int i = 0; i < msg.length(); i++)
    {
        c = msg[i];
        if (c == '\n')
        {
            if (messageRow < (MessageRows - 1))
            {
                RenderMessageRow(messageRow++);
            }
            else
            {
                messageLines.removeAt(0);
                messageLines.add("");
                for (int i = 0; i < MessageRows; i++)
                    RenderMessageRow(i);
            }
        }
        else
        {
            messageLines[messageRow] += c;
        }
    }
    RenderMessageRow(messageRow);
    UpdateDisplay();
};

string BoardGetLine()
{
    char c;
    PrintMessage("|");
    string s = "";
    while (1)
    {
        c = GetChar();
        messageLines[messageRow].erase(messageLines[messageRow].end()-1);
        if (c != 8)
        {
            PrintMessage(string("") + c);
            if (c == '\n')
                break;
            s += c;
        }
        else if (s != "")
        {
            s.erase(s.end()-1);
            messageLines[messageRow].erase(messageLines[messageRow].end()-1);
        }
        PrintMessage("|");
    }
    return s;
}

