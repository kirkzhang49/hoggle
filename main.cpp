#include "lpclib.h"
#include "hgraphics.h"
#include "fstream"
#include "lexicon.h"
#include "grid.h"
#include "vector.h"
#include "vector"
#include "set.h"
using namespace std;
static Lexicon lex("lexicon.dat");
bool islegal(string word);
bool islegalhelp(string Tword,string word,int r,int c,Vector<int> row,Vector<int> col);
void computerpower(Vector <string> words);
void computerpowerhelp(Set <string> &checker,int r,int c,Vector<int> row,Vector<int> col,string letters);
void drawsomewords(string word);

// NOTE: This main() is set up to demonstrate a few different
// parts of the code provided for you. It is NOT meant
// as a suggestion for what your actual main program should look like.

int main()
{
    // Use DrawBoard() to open the graphics window
    // instead of GraphicsWindow()


    // Here is a double for loop that iterates through all dice
    // on the board
    string xx="@";
    while (1)
    {
         DrawBoard();
 DrawImage("pika.bmp",20,50,150,270);
 DrawImage("jieni.bmp",530,50,250,270);

         ifstream fin;
    fin.open("dice.txt");
   string Letter[19];
    for (int n=0;n<19;n++)
    {
        string L;
        getline(fin,L);
        Letter[n]=L;
    }
    for (int r = 0; r < 5; r++)
    {
        for (int c = 0; c < DiceInRow(r); c++)
        {
            char cc;
            int a=RandomInt(0,18);
            int b=RandomInt(0,5);
            cc=Letter[a][b];
            LabelHex(r, c, cc);
        }
    }

    UpdateDisplay();
    PrintMessage("Welcome to Hoggle\n");
   PrintMessage("Human Player's Turn:\n");



   Vector<string> userenter;
      string words="@";
      while (words!="")
      {

           PrintMessage("enter a English word on screen:\n");
             words=BoardGetLine();
                bool exist=false;
               foreach(string k in userenter)
       {
           if (k==words)
            exist=true;
       }
           if (words.length()<3&&words!="")
      {
          PrintMessage("The word length is less than three!!!\n");
      }
    else if (exist)
    {
       PrintMessage("The word already exist on the screen!!!\n");
    }
     else if (!lex.containsWord(words)&&words!="")
     {
       PrintMessage("The word is not a real word.\n");
     }
     else
     {
         if (!islegal(words)&&words!="")
         {
             PrintMessage("The word can not formate on the board\n");
         }
         else
         {
             RecordNewWord(words,Human);
              userenter.add(words);
         }
     }
      }
   PrintMessage("computer Player's Turn:\n");
   {
       computerpower(userenter);
   }

    PrintMessage("Do you want to play again?(Y/N)");
    xx=BoardGetLine();


    while (xx!="Y"&&xx!="N")
    {
         PrintMessage("You have to enter:(Y/N)");
    xx=BoardGetLine();
    }
    if (xx=="N")
  break;
    }
    return 0;

}
bool islegal(string word)
{
    Vector <int> row;
    Vector <int> col;
char    letter=word[0];
   for (int r = 0; r < 5; r++)
    {
        for (int c = 0; c < DiceInRow(r); c++)
        {
            if (LetterAt(r,c)==letter)
            {
                row.add(r);
                col.add(c);
                 if (islegalhelp(word,word.substr(1),row[0],col[0],row,col))
     {
         return true;
     }
     row.clear();
     col.clear();
            }
        }
    }
    return false;
}


bool islegalhelp(string Tword,string word,int r,int c,Vector<int> row,Vector<int> col)/// up row col number can -1, down row col number can +1
{
    ///after row 2 is negative 1, before row 2 is positive one

    char letter=word[0];
//    bool exist=false;
    if (word.size()==0)///base case
    {
          for (int x=0;x<row.size();x++)
        {
             HighlightHex(row[x], col[x], true); // highlighting on
            Pause(0.06);
        }
        drawsomewords(Tword);
        Pause(0.12);
        for (int s=0;s<row.size();s++)
        {
           HighlightHex(row[s], col[s], false);
        }

        return true;
    }

if (r<2)
{
 for (int d = 0; d < 6; d++)
 {
     bool exist=false;

     int  deltaR[6] =  {0, 0,-1,-1,1,1};
     int  deltaC[6] =  {-1,1, 0,-1,0,1};

        int nextPosRow = r + deltaR[d];
        int nextPosCol = c + deltaC[d];
        if (nextPosRow<0)
            nextPosRow++;
        if(nextPosRow>4)
            nextPosRow--;
        if(nextPosCol<0)
            nextPosCol++;
         while(nextPosCol>DiceInRow(nextPosRow)-1)
            nextPosCol--;

            if (LetterAt(nextPosRow,nextPosCol)==letter)
            {
                for (int s=0; s<row.size();s++)
                {
                    if(nextPosRow==row[s]&&nextPosCol==col[s])
                             exist=true;
                }
               if (!exist)
                {
                    row.add(nextPosRow);
                col.add(nextPosCol);
                if (islegalhelp(Tword,word.substr(1),nextPosRow,nextPosCol,row,col))
                    return true;
                row.removeAt(row.size()-1);
                col.removeAt(col.size()-1);
                }


            }
 }

}
///  Row 0:    0 1 2
///  Row 1:   0 1 2 3
///  Row 2:  0 1 2 3 4
///  Row 3:   0 1 2 3
///  Row 4:    0 1 2
if (r==2)
{
   for (int d = 0; d < 6; d++)
 {
     bool exist=false;

     int  deltaR[6] =  {0, 0,-1,-1,1,1};
     int  deltaC[6] =  {-1,1, 0,-1,0,-1};

        int nextPosRow = r + deltaR[d];
        int nextPosCol = c + deltaC[d];
        if (nextPosRow<0)
            nextPosRow++;
        if(nextPosRow>4)
            nextPosRow--;
        if(nextPosCol<0)
            nextPosCol++;
         while(nextPosCol>DiceInRow(nextPosRow)-1)
            nextPosCol--;

            if (LetterAt(nextPosRow,nextPosCol)==letter)
            {
                for (int s=0; s<row.size();s++)
                {
                    if(nextPosRow==row[s]&&nextPosCol==col[s])
                             exist=true;
                }
                if (!exist)
                {
                    row.add(nextPosRow);
                col.add(nextPosCol);
                if (islegalhelp(Tword,word.substr(1),nextPosRow,nextPosCol,row,col))
                    return true;
                row.removeAt(row.size()-1);
                col.removeAt(col.size()-1);
                }


            }
 }
}
if (r>2)
{
    for (int d = 0; d < 6; d++)
 {
     bool exist=false;
     int   deltaR[6] = {0,0,-1,-1,1, 1};
     int deltaC[6] =   {-1,1,0, 1,0,-1};

        int nextPosRow = r + deltaR[d];
        int nextPosCol = c + deltaC[d];
        if (nextPosRow<0)
            nextPosRow++;
        if(nextPosRow>4)
            nextPosRow--;
        if(nextPosCol<0)
            nextPosCol++;
         while(nextPosCol>DiceInRow(nextPosRow)-1)
            nextPosCol--;

            if (LetterAt(nextPosRow,nextPosCol)==letter)
            {
                for (int s=0; s<row.size();s++)
                {
                    if(nextPosRow==row[s]&&nextPosCol==col[s])
                             exist=true;
                }
                if (!exist)
                {
                    row.add(nextPosRow);
                col.add(nextPosCol);
                if (islegalhelp(Tword,word.substr(1),nextPosRow,nextPosCol,row,col))
                    return true;
                row.removeAt(row.size()-1);
                col.removeAt(col.size()-1);
                }

            }
}
}
return false;
}

void computerpower(Vector <string> words)
{
    Vector <int> row;
    Vector <int> col;
    string letters;
 Set <string> Checker;
    for (int z=0;z<words.size();z++)
    {
        Checker.add(words[z]);
    }
   for (int r = 0; r < 5; r++)
    {
        for (int c = 0; c < DiceInRow(r); c++)
        {
            row.add(r);
                col.add(c);
                char l=LetterAt(r,c);
                letters+=l;
          computerpowerhelp(Checker,r,c,row,col,letters);
             row.clear();
     col.clear();
     letters="";
        }
    }
}
void drawsomewords(string word)
{
    if (word.size()==3)
    {
        DrawImage("good.bmp",520,340,190,40);
        UpdateDisplay();
    }
    if (word.size()==4)
    {
         DrawImage("awesome.bmp",520,340,190,40);
        UpdateDisplay();
    }
    if (word.size()==5)
    {

         DrawImage("excellent.bmp",520,340,190,40);
        UpdateDisplay();
    }
    if (word.size()>=6)
    {
      DrawImage("wonderful.bmp",520,340,190,40);
        UpdateDisplay();
    }
}
void computerpowerhelp(Set <string> &checker,int r,int c,Vector<int> row,Vector<int> col,string letters)
{

//    bool exist=false;
    if (letters.size()>=3&&lex.containsWord(letters)&&!checker.contains(letters))///base case
    {
//            for (int x=0;x<row.size();x++)
//        {
//             HighlightHex(row[x], col[x], true); // highlighting on
//            Pause(0.1);
//        }
//        Pause(0.1);
//        for (int s=0;s<row.size();s++)
//        {
//           HighlightHex(row[s], col[s], false);
//        }
    if (islegal(letters)==true)
    {
        RecordNewWord(letters,Computer);
            checker.add(letters);
    }

        }

if (r<2)
{
 for (int d = 0; d < 6; d++)
 {
     bool exist=false;

     int  deltaR[6] =  {0, 0,-1,-1,1,1};
     int  deltaC[6] =  {-1,1, 0,-1,0,1};
        int nextPosRow = r + deltaR[d];
        int nextPosCol = c + deltaC[d];
           if (nextPosRow<0)
            nextPosRow++;
        if(nextPosRow>4)
            nextPosRow--;
        if(nextPosCol<0)
            nextPosCol++;
        while(nextPosCol>DiceInRow(nextPosRow)-1)
            nextPosCol--;

                for (int s=0; s<row.size();s++)
                {
                    if(nextPosRow==row[s]&&nextPosCol==col[s])
                             exist=true;
                }
                if (!exist)
                {
                    row.add(nextPosRow);
                    col.add(nextPosCol);
computerpowerhelp(checker,nextPosRow,nextPosCol,row,col,letters+LetterAt(nextPosRow,nextPosCol));


                }

}
 }
 if (r==2)
{
   for (int d = 0; d < 6; d++)
 {
     bool exist=false;

     int  deltaR[6] =  {0, 0,-1,-1,1,1};
     int  deltaC[6] =  {-1,1, 0,-1,0,-1};

        int nextPosRow = r + deltaR[d];
        int nextPosCol = c + deltaC[d];
        if (nextPosRow<0)
            nextPosRow++;
        if(nextPosRow>4)
            nextPosRow--;
        if(nextPosCol<0)
            nextPosCol++;
         while(nextPosCol>DiceInRow(nextPosRow)-1)
            nextPosCol--;
             for (int s=0; s<row.size();s++)
                {
                    if(nextPosRow==row[s]&&nextPosCol==col[s])
                             exist=true;
                }
                if (!exist)
                {
                    row.add(nextPosRow);
                    col.add(nextPosCol);
computerpowerhelp(checker,nextPosRow,nextPosCol,row,col,letters+LetterAt(nextPosRow,nextPosCol));

                }

 }
}
if (r>2)
{
    for (int d = 0; d < 6; d++)
 {
     bool exist=false;
     int   deltaR[6] = {0,0,-1,-1,1, 1};
     int deltaC[6] =   {-1,1,0, 1,0,-1};

        int nextPosRow = r + deltaR[d];
        int nextPosCol = c + deltaC[d];
        if (nextPosRow<0)
            nextPosRow++;
        if(nextPosRow>4)
            nextPosRow--;
        if(nextPosCol<0)
            nextPosCol++;
         while(nextPosCol>DiceInRow(nextPosRow)-1)
            nextPosCol--;
            for (int s=0; s<row.size();s++)
                {
                    if(nextPosRow==row[s]&&nextPosCol==col[s])
                             exist=true;
                }
                if (!exist)
                {
                    row.add(nextPosRow);
                    col.add(nextPosCol);
computerpowerhelp(checker,nextPosRow,nextPosCol,row,col,letters+LetterAt(nextPosRow,nextPosCol));

                }

 }

 }

}
///  Row 0:    0 1 2
///  Row 1:   0 1 2 3
///  Row 2:  0 1 2 3 4
///  Row 3:   0 1 2 3
///  Row 4:    0 1 2
