#include <iostream>
#include <string>
#include <windows.h>
#include <fstream>
#include <math.h>
#include <conio.h>
#include <sstream>

using namespace std;

const float PI = 3.14159265;
const float FOV = 60; //60 on both sides of the y axis
const float RAD_TO_DEG = 180/PI;
const float CUBE_WIDTH = 1; //is needed for calculating width

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
*   (SCREEN_WIDTH(=80)*ANGLE(=REAL_ANGLE+(FOV)))/MAX_ANGLE(=FOV*2)=WORLD TO SCREEN X.
* ELSE:
*   FIND EDGE THAT IS
*   blaaa
**********/

/*************
*
* TODO:
* STRECH X
* SIDE RENDER
* COMMENT!!!!!!!!!!!!
* BACK UP/CLEAN UP
*
*************/
void ShowConsoleCursor(bool showFlag) //No idea what this does, i just need it to hide the cursor
{
    HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO     cursorInfo;
    GetConsoleCursorInfo(out, &cursorInfo);
    cursorInfo.bVisible = showFlag; // set the cursor visibility
    SetConsoleCursorInfo(out, &cursorInfo);
}

string convertInt(int number)
{
    stringstream ss;//create a stringstream
    ss << number;//add number to the stream
    return ss.str();//return a string with the contents of the stream
}

int main()
{
    //cout<<atan2(1.0,1.0);// y/x
    char world[1921];
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
    cout <<"w: "<<w<<" l:  "<<l<<endl; //w is x, l is y
    char **blockPos = new char*[w];
    for (int i = 0; i < l; i++) //
    {
        blockPos[i] = new char[l];
    }
    int y=0;
    cout<<"World layout:"<<endl;//0=floor, 1=wall, 2=spawn
    int playerx,playery;
    while(getline(layout, currentLine)) //get line (width)
    {
        for(int x=0;x<currentLine.size();x++) //put current line in array for world loading
        {
            blockPos[x][y]=currentLine[x];
            if(blockPos[x][y]=='2')
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
            cout<<blockPos[wide][q];
        }
        cout<<endl;
    }
    int direction=1;//1=north, 2=east, 3=south, 4=west
    if(direction==1)//start on north side
    {
        int counter=0;
        for(int a=0;a<24;a++)
        {
            for(int b=0;b<80;b++)
            {
                world[80*a+b]='_';
                counter++;
            }
        }
        cout<<convertInt(counter)<<endl;
        //printf(world);
        for(int y=0;y<l;y++)
        {
            for(int x=0;x<w;x++)
            {
                if(blockPos[x][y]=='1')
                {
                    cout<<"x: "<<x<<" y: "<<y<<endl;
                    cout<<"playerX: "<<playerx<<" playerY: "<<playery<<endl;
                    //find the three needed edges of a single cube
                    //float distances[4];
                    int screenLocations[2][4][2];//[front,back] [right, top right, left, top left] [x,y]
                    int screenPos[3];
                    float distances[2][4][2];//[front,back] [right, top right, left, top left] [x,y] (y can sometimes be a z (vert), but for atan2 it's still technically a y
                    //everything seems to work
                    distances[0][0][0]=playerx-(x+(CUBE_WIDTH*.5));//front right x
                    distances[0][0][1]=playery-(y+(CUBE_WIDTH*.5));//front right y
                    distances[0][1][0]=sqrt(((playery-(y+(CUBE_WIDTH*.5)))*(playery-(y+(CUBE_WIDTH*.5))))+((playerx-(x))*(playerx-(x))));//front top x
                    distances[0][1][1]=CUBE_WIDTH*.5;//front top y (technically a z), since player z=0 this is all that is needed
                    distances[0][2][0]=playerx-(x-(CUBE_WIDTH*.5));//front left x
                    distances[0][2][1]=playery-(y+(CUBE_WIDTH*.5));//front left y
                    distances[0][3][0]=sqrt(((playery-(y+(CUBE_WIDTH*.5)))*(playery-(y+(CUBE_WIDTH*.5))))+((playerx-(x))*(playerx-(x))));//front bottom x
                    distances[0][3][1]=CUBE_WIDTH*.5;
                    distances[1][0][0]=playerx-(x+(CUBE_WIDTH*.5));//back right x;
                    distances[1][0][1]=playery-(y-(CUBE_WIDTH*.5));//back right y;
                    distances[1][1][0]=sqrt(((playery-(y-(CUBE_WIDTH*.5)))*(playery-(y-(CUBE_WIDTH*.5))))+((playerx-(x))*(playerx-(x))));//back top x;
                    distances[1][1][1]=CUBE_WIDTH*.5;//back top y (technically a z), since player z=0 this is all that is needed;
                    distances[1][2][0]=playerx-(x-(CUBE_WIDTH*.5));//back left x;
                    distances[1][2][1]=playery-(y-(CUBE_WIDTH*.5));//back left y;
                    distances[1][3][0]=sqrt(((playery-(y-(CUBE_WIDTH*.5)))*(playery-(y-(CUBE_WIDTH*.5))))+((playerx-(x))*(playerx-(x))));//back bottom x;
                    distances[1][3][1]=CUBE_WIDTH*.5;//back bottom y (technically a z), but since player z=0 this is all that is needed;
                    _getch();

                    for(int z=0; z<2; z++)
                    {
                        for(int i=0; i<4; i++)
                        {
                            int worldChar;
                            int worldX = 0;
                            //still don't know which atan2 to use. I guess it will come down to how the angle to screen point works out
                            cout<<"Distance: y="<<distances[z][i][1]<< ", x="<<distances[z][i][0]<<" Degree: ";
                            float degree=atan2(distances[z][i][1],distances[z][i][0]); //x = 66 - 113 (middle 90) y = 8 - 351 (middle 0) (on world 2)
                            //float degree=atan2(distances[0][i][0],distances[0][i][1]); //x = 23 - 336 (middle 0) y = 81 - 98 (middle 90) (on world 2)
                            degree*=RAD_TO_DEG;
                            if(i==1)
                            {
                                degree = sqrt(degree*degree);
                                worldX=(80*degree)/(FOV*2);
                                screenLocations[z][i][0]=screenLocations[z][i-1][0];
                                screenLocations[z][i][1]=12-worldX;
                                cout<<degree<<" WorldX: "<<worldX<<endl;
                                //_getch();
                                worldChar=z*4+i;
                                char blaa[20];
                                itoa(worldChar,blaa,10);
                                //world[screenLocations[z][i][0]+screenLocations[z][i][1]*80]=blaa[0];
                            }
                            else if(i==3)
                            {
                                degree = sqrt(degree*degree);
                                worldX=(80*degree)/(FOV*2);
                                screenLocations[z][i][0]=screenLocations[z][i-1][0];
                                screenLocations[z][i][1]=12+worldX;
                                cout<<degree<<" WorldX: "<<worldX<<endl;
                                //_getch();
                                worldChar=z*4+i;
                                char blaa[20];
                                itoa(worldChar,blaa,10);
                                //world[screenLocations[z][i][0]+screenLocations[z][i][1]*80]=blaa[0];
                            }
                            else
                            {
                                if(degree<0)
                                {
                                    degree+=360;
                                    cout<<degree<<" TOO BIG"<<endl;
                                }
                                if(degree>FOV+90||degree<FOV-90)
                                {
                                    cout<<degree<<" BAD"<<endl;
                                    //NOT IN MY HOUSE
                                    //do stuff
                                }
                                else
                                {
                                    cout<<degree<<" Adjusted Deg: ";
                                    degree-=(90-FOV);
                                    worldX=(((80*degree)/(FOV*2)));
                                    if(worldX<0)
                                    {
                                        worldX=0;
                                        cout<<" WorldX set to 0 ";
                                    }
                                    if(worldX>80)
                                    {
                                        worldX=80;
                                        cout<<" WorldX set to 80 ";
                                    }
                                    screenLocations[z][i][0]=worldX;
                                    screenLocations[z][i][1]=12;
                                    worldChar=z*4+i;
                                    char blaa[20];
                                    itoa(worldChar,blaa,10);
                                    //world[screenLocations[z][i][0]+screenLocations[z][i][1]*80]=blaa[0];
                                    cout<<degree<<" WorldX: "<<worldX<<endl;
                                }
                            }

                            //_getch();
                        }
                    }
                    for(int z=1; z>=0; z--)
                    {
                        for(int y=screenLocations[z][1][1]; y<=screenLocations[z][3][1]; y++)//top<bottom
                        {
                            for(int x=screenLocations[z][2][0]; x<=screenLocations[z][0][0];x++)//far left<far right
                            {
                                //cout<<"a"<<endl;
                                if(y==screenLocations[z][3][1]||y==screenLocations[z][1][1])
                                {
                                    world[x+80*y]='-';
                                    world[x+80*y]='-';
                                }
                                else
                                {
                                    world[x+80*y]='.';
                                }
                            }
                            if(y==screenLocations[z][3][1]||y==screenLocations[z][1][1])
                            {
                                world[screenLocations[z][2][0]+80*y]='*';
                                world[screenLocations[z][0][0]+80*y]='*';
                            }
                            else
                            {
                                world[screenLocations[z][2][0]+80*y]='|';
                                world[screenLocations[z][0][0]+80*y]='|';
                            }
                        }
                    }
                    printf(world);
/*
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
                        locations[temp][0]=x+(.5*sin((PI*(1+((temp-1)*2)))/2));
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
                 /*   for(int temp1=0;temp1<3;temp1++)
                    {
                        cout<<"Pos "<<temp1+1<<": ";
                        for(int temp2=0;temp2<2;temp2++)
                        {
                            cout<<locations[temp1][temp2]<<" ";
                        }
                        cout<<endl;
                    system("pause");
                    }*/
                }
            }
            if(playery==y)
            {
                break;
            }
        }
    }
    ShowConsoleCursor(false);
    return 0;
}
