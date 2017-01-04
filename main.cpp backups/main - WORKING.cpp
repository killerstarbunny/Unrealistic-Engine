#include <iostream>
#include <string>
#include <windows.h>
#include <fstream>
#include <math.h>

using namespace std;

#define pi 3.14159265
#define FOV 60 //60 on both sides of the y axis
/**********
* LAYOUT OF PROGRAM
* FIND EXCLUDED VERTEX
* POSTION VERTEXES
* FIND VERTEX H
*/

/**********
* PLAN:
* LOAD WORLD
* MAKE DISTANCE AND HEIGHT GRAPH
* CHECK IF BOX (OR PART OF BOX) IS IN THE THE FOV
* IF ALL:
*   GET DISTANCE OF EACH BLOCK
*   DISTANCE TO HEIGHT
*   (120*ANGLE(=REAL_ANGLE-30))/SCREEN_WIDTH(=80)=WORLD TO SCREEN Y.
* ELSE:
*   FIND EDGE THAT IS
*   blaaa
**********/
void ShowConsoleCursor(bool showFlag) //No idea what this does, i just need it to hide the cursor
{
    HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO     cursorInfo;
    GetConsoleCursorInfo(out, &cursorInfo);
    cursorInfo.bVisible = showFlag; // set the cursor visibility
    SetConsoleCursorInfo(out, &cursorInfo);
}


int main()
{
    for(int x=1; x<=24; x++)
    {
        for(int x=1; x<=80; x++)
        {
            cout<<"_";
        }
    }
    cout<<endl;
    cout<<atan2(1.0,1.0);
    char world[24][80];
    string fileLoc;
    cout << "World loc: ";
    cin >> fileLoc;
    size_t checkExtension = fileLoc.find("txt");
    if(checkExtension==string::npos)
    {
        fileLoc+=".txt";
    }
    ifstream layout(fileLoc.c_str());
    if(!layout)
    {
        cout<<fileLoc;
        cout<<"File doesn't exist!"<<endl;
        return 5;
    }
    string currentLine;
    size_t tempT;
    int l,w;
    getline(layout, currentLine);
    tempT=currentLine.find("x"); //getting demensions of world
    w=stoi(currentLine.substr(0,tempT));
    l=stoi(currentLine.substr(tempT+1,currentLine.size()));
    cout <<"w: "<<w<<" l:  "<<l<<endl;
    char **blockPos = new char*[l];
    for (int i = 0; i < l; i++) //
    {
        blockPos[i] = new char[w];
    }
    int y=0;
    cout<<"World layout:"<<endl;//0=floor, 1=wall, 2=spawn
    int playerx,playery;
    while(getline(layout, currentLine)) //get line (width)
    {
        for(int x=0;x<currentLine.size();x++) //put current line in array for world loading
        {
            blockPos[y][x]=currentLine[x];
            if(blockPos[y][x]=='2')
            {
                playerx=x;
                playery=y;
                cout<<"Found Player"<<endl;
            }
        }
        y++;
    }
    for(int q=0;q<l;q++)//display world
    {
        for(int wide=0;wide<w;wide++)
        {
            cout<<blockPos[q][wide];
        }
        cout<<endl;
    }
    int direction=1;//1=north, 2=east, 3=south, 4=west
    if(direction==1)//start on north side
    {
        for(int y=0;y<l;y++)
        {
            int recheckX=-1;
            for(int x=0;x<w;x++)
            {
                if(blockPos[y][x]=='1')
                {
                    if(x==playerx||y==playery)//check if only two edges should be checked because of the rendering method this has to be done
                    {
                        recheckX=x;//might have to swap rendering method, currently it just proceeds right which may cause extra lines to appear. The best method might be to go right then turn around and come from the right and go left
                    }
                    else //checks if three need to be checked
                    {
                        //find the three needed edges of a single cube
                        //float distances[4];
                        float locations[3][2];
                        int screenPos[3];
                        float distances[4][2];
                        distances[0][0]=(float)sqrt((((x-.5)-playerx)*((x-.5)-playerx))+(((y+.5)-playery)*((y+.5)-playery)));//bottom right corner
                        distances[1][0]=(float)sqrt((((x+.5)-playerx)*((x+.5)-playerx))+(((y+.5)-playery)*((y+.5)-playery)));//bottom left
                        distances[2][0]=(float)sqrt((((x-.5)-playerx)*((x-.5)-playerx))+(((y-.5)-playery)*((y-.5)-playery)));//top right
                        distances[3][0]=(float)sqrt((((x+.5)-playerx)*((x+.5)-playerx))+(((y-.5)-playery)*((y-.5)-playery)));//top left
                        distances[0][1]=0;
                        distances[1][1]=1;
                        distances[2][1]=2;
                        distances[3][1]=3;
                        int excludedValue;
                        float currentMax=0;
                        int i = 1;
                        while(i<4)
                        {
                            for(int j=i;distances[j-1][0]>distances[j][0];j--)
                            {
                                float temp = distances[j-1][0];
                                distances[j-1][0] = distances[j][0];
                                distances[j][0] = temp;

                                float temp2 = distances[j-1][1];
                                distances[j-1][1] = distances[j][1];
                                distances[j][1] = temp2;
                            }
                            i++;
                        }
                        for(int temp=0;temp<4;temp++)
                        {
                            cout<<"distance: "<<distances[temp][0]<<" val: "<<distances[temp][1]<<endl;
                        }
                        bool foundExcluded=false;
                        int worldLoc[3][2];//[0]=horizontal, [1]=height
                        for(int temp=0;temp<3;temp++)
                        {
                            locations[temp][0]=x+(.5*sin((pi*(1+((temp-1)*2)))/2));
                            int tempSign=1;
                            if(distances[temp][1]>1)
                            {
                                tempSign*=-1;
                            }
                            locations[temp][1]=y+(.5*tempSign);
                        }
                        for(int temp=0; temp<3; temp++)
                        {
                            //worldLoc[temp][0]=((atan2(locations[temp][1]-playery,locations[temp][0]-playerx))/pi)*81;
                            //cout<<"World loc "<<temp+1<<": "<<worldLoc[temp][0]<<endl;
                        }

                        /*for(int temp=0;temp<4;temp++)
                        {
                            if(temp!=excludedValue)
                            {
                                if(foundExcluded==true)
                                {
                                    locations[temp-1][0]=x+(.5*cos((pi*temp*-1)));//stupid cos thing is used because it will go 1,-1,1,-1 which is the way that distances are messed with
                                    int tempSign=1;
                                    if(temp>1)
                                    {
                                        tempSign*=-1;
                                    }
                                    locations[temp-1][1]=y+(.5*tempSign);
                                    newDistance[temp]=distances[temp-1];
                                }
                                else
                                {
                                    locations[temp][0]=x+(.5*sin((pi*(1+((temp-1)*2)))/2));
                                    newDistance[temp]=distances[temp];
                                    int tempSign=1;
                                    if(temp>1)
                                    {
                                        tempSign*=-1;
                                    }
                                    locations[temp][1]=y+(.5*tempSign);
                                }
                            }
                            else
                            {
                                foundExcluded=true;
                            }
                        }*/
                        for(int temp1=0;temp1<3;temp1++)
                        {
                            cout<<"Pos "<<temp1+1<<": ";
                            for(int temp2=0;temp2<2;temp2++)
                            {
                                cout<<locations[temp1][temp2]<<" ";
                            }
                            cout<<endl;
                        }
                        system("pause");
                    }
                }
            }
            if(recheckX!=-1)//
            if(playery==y)
            {
                break;
            }
        }
    }
    ShowConsoleCursor(false);
    return 0;
}
