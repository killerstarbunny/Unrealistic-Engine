#include <iostream>
#include <stdlib.h>
#include <string>
#include <windows.h>
#include <fstream>
#include <math.h>
#include <conio.h>
#include <sstream>
#include <vector>

using namespace std;

const float PI = 3.14159265f;
const float FOV = 60; //30 on both sides of the y axis
const float RAD_TO_DEG = 180/PI;
const float CUBE_WIDTH = 1; //is needed for calculating width
const float WIDTH_ADJUST= 1.5;
const int RENDER_DISTANCE=200;

/**********
* LAYOUT OF PROGRAM
* FIND EXCLUDED VERTEX
* POSTION VERTEXES
* FIND VERTEX H
**********/

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
* TODO:
* COMMENT!!!!!!!!!!!!
* MAKE PLAN AN ACTUAL OVERVIEW, OR WRITE AN OVERVIEW AND INCLUDE IT IN PROJ
* RENAME l AND w TO worldLength AND worldWidth
*************/

/************
* VERSION 1:
* GIVE BLOCKS A STRUCT
* DON'T ROTATE ENTIRE WORLD
*   FOR ATAN2 ADD SOMETHING LIKE -direction*90
*   FOR LOOPS MAKE IT SO Y AND X ALLWAYS ARE INITIALIZED WITH A VARIABLE THAT direction HELPS DECIDE. ALSO A VAR FOR THE X< BIT
* CLEAN UP CLUTTER
*************/

/************
* PHASE II: GRAPHICS
* ADD GRAPHICS RECONINTION TO CODE (UPPER CASE LETTERS)
* IMPORT GRAPHICS FROM WORLD FILE
*   OPEN FILE WITH ANOTHER FSTREAM
*   FIND LABEL (LETTER+:)
*   GRAB DIMENSIONS
*   STICK IN ARRAY
* RESIZE ALGORTHIM
**************
* DEPTH FOG?
* CHANGE CHARS TO USE FULL ASSCI?
* MAKE PLAYER CORDS A FLOAT INTEAD OF AN INT SO YOU WALKING ISN'T SO CLUNKY?
*************/

//STRECH ISSUE STEMS FROM CHARS NOT HAVING A 1:1 RATIO
void ShowConsoleCursor(bool showFlag) //No idea what this does, i just need it to hide the cursor
{
    HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO     cursorInfo;
    GetConsoleCursorInfo(out, &cursorInfo);
    cursorInfo.bVisible = showFlag; // set the cursor visibility
    SetConsoleCursorInfo(out, &cursorInfo);
}

void makeRandomWorld()
{
    int width, height, probablity;
    string input;
    cout<<"Width: ";
    cin>>input;
    width=stoi(input);
    cout<<"Height: ";
    cin>>input;
    height=stoi(input);
    cout<<"Probablity: ";
    cin>>input;
    probablity=stoi(input);
    cout<<"Output name: ";
    cin>>input;
    ofstream file(input.c_str());
    file<<width<<"x"<<height<<endl;
    int playerX=width/2;
    int playerY=height/2;
    for(int x=0; x<width; x++)
    {
        for(int y=0; y<height; y++)
        {
            int temp=rand()%100;
            if(x==playerX&&y==playerY)
            {
                file<<"2";
            }
            else if(temp<probablity)
            {
                file<<"1";
            }
            else
            {
                file<<"0";
            }
        }
        if(x<width-1)
        {
            file<<endl;
        }
    }
}

string convertInt(int number)
{
    ostringstream ss;//create a stringstream
    ss << number;//add number to the stream
    return ss.str();//return a string with the contents of the stream
}

int normalize(int i,int maxNum)
{
    //0=North, 1=East, 2=South, 3=West
    if(i<0)
    {
        i+=maxNum;//-1+4=3
    }
    i%=maxNum;//will normalize it back down inside of the range. This also handles negative numbers. -1%4=3 West
    return i;
}

int floatToInt(float f)//this should be used at all times. Double check for this.
{
    int a;
    a=(int)f;
    f-=a;//set f to only the decimal points 4.93819193012-4=.93819193012
    if(f>0.5)
    {
        a++;//round up
    }
    return a;
}

bool isValid(int x,int y)
{
    /*********
    * check to see all of the points are within the screen range.
    * This can't use the x+80*y formula because a high x and a low
    * y would give a value inside the range
    *********/
    if(x>=0&&x<80&&y>=0&&y<24)
    {
        return true;
    }
    return false;
}

int main()
{
    if(false)
    {
        makeRandomWorld();
        return 1;
    }
    char world[1921];//declare 3D world, for some reason has to be 1+size. if it isn't strange chars pop up has no overall effect if it's the wrong size
    string fileLoc;//declare input for world location
    cout << "World loc: ";//ask
    cin >> fileLoc;//get
    size_t checkExtension = fileLoc.find("txt");//check for ext
    if(checkExtension==string::npos)//if it isn't there...
    {
        fileLoc+=".txt";//add it
    }
    ifstream layout(fileLoc.c_str());//open the world
    if(!layout)//if file doesn't exist
    {
        cout<<fileLoc;
        cout<<" doesn't exist!"<<endl;//say it's so
        return 5;
    }
    ShowConsoleCursor(false); //will be needed when thing isn't so debugy
    string currentLine; //temp variable for reading file
    size_t tempT; //temp variable for .find
    int l,w; //l=length w=width. THIS NEEDS TO BE CHANGE. I came up with these names when l and w were only being used in like 2 places
    getline(layout, currentLine);
    tempT=currentLine.find("x"); //getting dimensions of world
    w=stoi(currentLine.substr(0,tempT));//w=width
    l=stoi(currentLine.substr(tempT+1,currentLine.size()));//l=length
    cout <<"w: "<<w<<" l:  "<<l<<endl; //w is x, l is y
    vector< vector<char> > blockPos(w, vector<char>(l));
    int y=0;
    cout<<"World layout:"<<endl;//0=floor, 1=wall, 2=spawn lowercase letters will be used for walls with graphics
    int playerX,playerY;//declare player's cords
    while(getline(layout, currentLine)) //get line (width)
    {
        for(int x=0;x<currentLine.size();x++) //put current line in array for world loading
        {
            blockPos[x][y]=currentLine[x];//put it in
            if(blockPos[x][y]=='2')//check if it's the player
            {
                playerX=x;
                playerY=y;
                cout<<"Found Player x: "<<x<<" y: "<<y<<endl;
            }
        }
        y++;
    }
    layout.close();
    for(int q=0;q<l&&false;q++)//display world
    {
        for(int wide=0;wide<w;wide++)
        {
            //cout<<blockPos[wide][q];
        }
        //cout<<endl;
    }
    cout<<"Done"<<endl;
    _getch();
    int direction=0;//0=north, 1=east, 2=south, 3=west
    for(;;)//start on north side
    {
        for(int a=0;a<24;a++)//prep the world with the empty chars, likely will be changed so the roof and the ground are different chars
        {
            for(int b=0;b<80;b++)
            {
                if(a>13)
                {
                    world[80*a+b]='_';
                }
                else
                {
                    world[80*a+b]=' ';
                }
            }
        }
        /*****************
        * WORLD LOOKS LIKE THIS:
        *  Y
        * X+--------------
        *  |
        *  |
        *  |
        *  |
        * ORGIN IS +
        *****************/
        int startingY, startingX, endingY, endingX;
        bool addToY=false;
        bool addToX=false;
        if(direction==0)//north
        {
            addToX=true;
            addToY=true;
            startingY=playerY-RENDER_DISTANCE;
            endingY=playerY;
            if(startingY<0)
            {
                startingY=0;
            }
            startingX=playerX-RENDER_DISTANCE;
            endingX=startingX+2*RENDER_DISTANCE;
            if(startingX<0)
            {
                startingX=0;
            }
            if(endingX>w)
            {
                endingX=w-1;
            }
        }
        else if(direction==1)//east
        {
            addToX=false;
            addToY=true;
            startingY=playerY-RENDER_DISTANCE;
            endingY=playerY+RENDER_DISTANCE;
            if(startingY<0)
            {
                startingY=0;
            }
            if(endingY>l)
            {
                endingY=l-1;
            }
            startingX=playerX+RENDER_DISTANCE;
            endingX=playerX;
            if(startingX>w)
            {
                startingX=w-1;
            }
        }
        else if(direction==2)//south
        {
            addToX=false;
            addToY=false;
            startingY=playerY+RENDER_DISTANCE;
            endingY=playerY;
            if(startingY>l)
            {
                startingY=l-1;
            }
            startingX=playerX+RENDER_DISTANCE;
            endingX=startingX-2*RENDER_DISTANCE;
            if(startingX>w)
            {
                startingX=w-1;
            }
            if(endingX<0)
            {
                endingX=0;
            }
        }
        else if(direction==3)//west
        {
            addToX=false;
            addToY=true;
            startingY=playerX-RENDER_DISTANCE;
            endingY=playerX;
            if(startingY<0)
            {
                startingY=0;
            }
            startingX=playerY+RENDER_DISTANCE;
            endingX=startingY-2*RENDER_DISTANCE;
            if(startingX>l)
            {
                startingX=l-1;
            }
            if(endingX<0)
            {
                endingX=0;
            }
        }
        else
        {
            cout<<"DIRECTION ERROR!!!! D="<<direction<<endl;
            _getch();
        }
        cout<<"startingY: "<<convertInt(startingY)<<" endingY: "<<convertInt(endingY)<<" startingX: "<<convertInt(startingX)<<" endingX: "<<convertInt(endingX)<<endl;
        cout<<"Startin"<<endl;
        _getch();
        for(int y=startingY;;)
        {
            //bool leave=false;
            for(int x=startingX;;)
            {
                cout<<"x: "<<x<<" y: "<<y<<endl;
                if(blockPos[x][y]=='1')//if it's a block we gotta render
                {
                    int screenLocations[2][4][2];//[front,back] [right, top right, left, top left] [x,y]
                    //int screenPos[3];
                    float distances[2][4][2];//[front,back] [right, top right, left, top left] [x,y] (y can sometimes be a z (vert), but for atan2 it's still technically a y
                    //everything seems to work
                    //when we move to multi directional cube width stuff will have to be added depending on direction since the player is always the farthest backwards.
                    if(true)
                    {
                        cout<<"start ";
                        distances[0][0][0]=playerX-(x+(CUBE_WIDTH*.5f));//front right x
                        distances[0][0][1]=(playerY+(CUBE_WIDTH*.5f))-(y+(CUBE_WIDTH*.5f));//front right y
                        distances[0][1][0]=(playerY+(CUBE_WIDTH*.5f))-(y+(CUBE_WIDTH*.5f));//front top x
                        distances[0][1][1]=CUBE_WIDTH*.5f;//front top y (technically a z), since player z=0 this is all that is needed
                        distances[0][2][0]=playerX-(x-(CUBE_WIDTH*.5f));//front left x
                        distances[0][2][1]=(playerY+(CUBE_WIDTH*.5f))-(y+(CUBE_WIDTH*.5f));//front left y
                        distances[0][3][0]=(playerY+(CUBE_WIDTH*.5f))-(y+(CUBE_WIDTH*.5f));//front bottom x
                        distances[0][3][1]=CUBE_WIDTH*.5f;
                        distances[1][0][0]=playerX-(x+(CUBE_WIDTH*.5f));//back right x;
                        distances[1][0][1]=(playerY+(CUBE_WIDTH*.5f))-(y-(CUBE_WIDTH*.5f));//back right y;
                        distances[1][1][0]=(playerY+(CUBE_WIDTH*.5f))-(y-(CUBE_WIDTH*.5f));//back top x;
                        distances[1][1][1]=CUBE_WIDTH*.5f;//back top y (technically a z), since player z=0 this is all that is needed;
                        distances[1][2][0]=playerX-(x-(CUBE_WIDTH*.5f));//back left x;
                        distances[1][2][1]=(playerY+(CUBE_WIDTH*.5f))-(y-(CUBE_WIDTH*.5f));//back left y;
                        distances[1][3][0]=(playerY+(CUBE_WIDTH*.5f))-(y-(CUBE_WIDTH*.5f));//back bottom x;
                        distances[1][3][1]=CUBE_WIDTH*.5f;//back bottom y (technically a z), but since player z=0 this is all that is needed;
                        cout<<"MATHS"<<endl;
                        _getch();
                    }
                    /*else
                    {
                        distances[0][0][0]=playerX-(x+(CUBE_WIDTH*.5));//front right x
                        distances[0][0][1]=playerY-(y+(CUBE_WIDTH*.5));//front right y
                        distances[0][1][0]=sqrt(((playerY-(y+(CUBE_WIDTH*.5)))*(playerY-(y+(CUBE_WIDTH*.5))))+((playerX-(x))*(playerX-(x))));//front top x
                        distances[0][1][1]=CUBE_WIDTH*.5;//front top y (technically a z), since player z=0 this is all that is needed
                        distances[0][2][0]=playerX-(x-(CUBE_WIDTH*.5));//front left x
                        distances[0][2][1]=playerY-(y+(CUBE_WIDTH*.5));//front left y
                        distances[0][3][0]=sqrt(((playerY-(y+(CUBE_WIDTH*.5)))*(playerY-(y+(CUBE_WIDTH*.5))))+((playerX-(x))*(playerX-(x))));//front bottom x
                        distances[0][3][1]=CUBE_WIDTH*.5;
                        distances[1][0][0]=playerX-(x+(CUBE_WIDTH*.5));//back right x;
                        distances[1][0][1]=playerY-(y-(CUBE_WIDTH*.5));//back right y;
                        distances[1][1][0]=sqrt(((playerY-(y-(CUBE_WIDTH*.5)))*(playerY-(y-(CUBE_WIDTH*.5))))+((playerX-(x))*(playerX-(x))));//back top x;
                        distances[1][1][1]=CUBE_WIDTH*.5;//back top y (technically a z), since player z=0 this is all that is needed;
                        distances[1][2][0]=playerX-(x-(CUBE_WIDTH*.5));//back left x;
                        distances[1][2][1]=playerY-(y-(CUBE_WIDTH*.5));//back left y;
                        distances[1][3][0]=sqrt(((playerY-(y-(CUBE_WIDTH*.5)))*(playerY-(y-(CUBE_WIDTH*.5))))+((playerX-(x))*(playerX-(x))));//back bottom x;
                    }*/
                    for(int z=0; z<2; z++)//z switches between front and back
                    {
                        for(int i=0; i<4; i++)//cycles through [right, top right, left, top left]
                        {
                            int worldX = 0;
                            /**************************
                            * SO FOR THE LEFT AND RIGHT SIDE CALCULATIONS WORK LIKE THIS:
                            * X
                            * | <- Y DISTANCE
                            * |
                            * |
                            * |  X DISTANCE
                            * |      :
                            * +------------------P
                            * X BEING BOX AND P BEING PLAYER THIS IS USING AN AERIAL
                            * DEGREE = ARCTAN(Y/X)
                            * THE FORUMAL worldX=(80*degree)/(FOV*2) WORKS LIKE THIS:
                            *  SCREEN WIDTH          X
                            * -------------- = -------------
                            *    FOV * 2           ANGLE
                            * SINCE THE LEFT IS THE MOST EXTEREMES YOU CAN EASILY MOVE THINGS AROUND
                            ***************************/
                            //still don't know which atan2 to use. I guess it comes down to how the angle to screen point works out
                            float degree=atan2(distances[z][i][1],distances[z][i][0]); //x = 66 - 113 (middle 90) y = 8 - 351 (middle 0) (on world 2), this one is easier to work with
                            degree-=direction*90;
                            //cout<<degree<<endl;
                            //_getch();
                            degree*=RAD_TO_DEG;//atan2 is in rads which is a little bit more of a hassle to work with for this case
                            if(i==1)//if this is the top of the box
                            {
                                /*****************
                                * SINCE WE KNOW THE X WE CAN IGNORE EVERYTHING BESIDES HEIGHT OF BLOCK AND DISTANCE
                                * SO INSTEAD OF THIS:
                                * X
                                * | <- Y DISTANCE
                                * |
                                * |
                                * |  X DISTANCE
                                * |      :
                                * +------------------P
                                * IT'S JUST:
                                *   HYPOTENUSE
                                *        :
                                * P--------------C
                                * LOGICALY THIS MAKES SENSE TO ME, BUT I'M NOT SURE IF IT WORKS OUT
                                * THIS APROACH IS BASED OFF THE CLASSIC 'HOW TALL IS THE TREE' PROBLEM
                                *****************/
                                degree = sqrt(degree*degree);//it may be possible to merge i==3 with this one if this line is removed
                                worldX=floatToInt((80*degree)/(FOV*2));//when fov stuff is fixed this might need to be kept like this, or maybe this will be needed to help fix the vertical stretch. It's possible the stretch is simply because chars aren't square
                                screenLocations[z][i][0]=screenLocations[z][i-1][0];//this x is the same as as the last one, technically this doesn't need to be assigned, but I like it
                                screenLocations[z][i][1]=12-worldX;//this is the real important piece subtracting half the height of the block from 12 puts it in the proper y location
                            }
                            else if(i==3)
                            {
                                degree = sqrt(degree*degree);
                                worldX=floatToInt((80*degree)/(FOV*2));
                                screenLocations[z][i][0]=screenLocations[z][i-1][0];
                                screenLocations[z][i][1]=12+worldX;//everything is the same as i==1 except instead of subtracting you add
                            }
                            else
                            {
                                if(degree<0)
                                {
                                    degree+=360;
                                }
                                else
                                {
                                    degree-=(90-FOV);
                                    float worldF=(((80*degree)/(FOV*2)));
                                    if(worldF<40)
                                    {
                                        int te=floatToInt((40-worldF)*WIDTH_ADJUST);
                                        //te*=WIDTH_ADJUST;
                                        worldX=40-te;
                                    }
                                    else
                                    {
                                        int te=floatToInt((worldF-40)*WIDTH_ADJUST);
                                        //te*=WIDTH_ADJUST;
                                        worldX=te+40;
                                    }
                                    screenLocations[z][i][0]=worldX;
                                    screenLocations[z][i][1]=12;
                                }
                            }
                        }
                    }
                    bool render[3]={false,false,false};//[left, front, right]
                    /***************
                    *     FRONT
                    *       |
                    *RIGHT--o----LEFT
                    *       |
                    *       |
                    ***************/
                    if(x>playerX&&x>0&&blockPos[x-1][y]!='1')//should left side be rendered
                    {
                        render[0]=true;
                    }
                    if(y<playerY&&y<l&&blockPos[x][y+1]!='1')//should the front be rendered
                    {
                        render[1]=true;
                    }
                    if(x<playerX&&x<w&&blockPos[x+1][y]!='1')//should right side be rendered
                    {
                        render[2]=true;
                    }
                    if(render[0]==true)
                    {
                        //m=slope
                        float m1=((float)screenLocations[0][1][1]-(float)screenLocations[1][1][1])/((float)screenLocations[0][2][0]-(float)screenLocations[1][2][0]);//(front top-back top)/(front left-back left) Had to make it negitive because with this setup higher in the screen is actually lower in the y
                        float m2=((float)screenLocations[0][3][1]-(float)screenLocations[1][3][1])/((float)screenLocations[0][2][0]-(float)screenLocations[1][2][0]);//(front bottom-back bottom)/(front left-back left)
                        float b1=-((float)screenLocations[0][2][0]*m1)+((float)screenLocations[0][1][1]);//b1=(-x1*m1)+y1
                        float b2=-((float)screenLocations[0][2][0]*m2)+((float)screenLocations[0][3][1]);//b1=(-x1*m1)+y1
                        for(int x=screenLocations[1][2][0]; x<=screenLocations[0][2][0]; x++)
                        {
                            for(int y=floatToInt(x*m1+b1); y<=floatToInt(x*m2+b2); y++)//y=slope of top checking if y<=slope of bottom
                            {
                                int tempa, tempb;
                                tempa=floatToInt(x*m1+b1);
                                tempb=floatToInt(x*m2+b2);
                                if(x==screenLocations[1][2][0]||x==screenLocations[0][2][0])
                                {
                                    if((y==floatToInt(x*m1+b1)||y==floatToInt(x*m2+b2))&isValid(x,y))
                                    {
                                        world[x+80*y]='*';//corners
                                    }
                                    else if(isValid(x,y))
                                    {
                                        world[x+80*y]='|';//edge
                                    }
                                }
                                else if((y==tempa||y==tempb)&isValid(x,y))
                                {
                                    world[x+80*y]='-';//top or bottom
                                }
                                else if(isValid(x,y))
                                {
                                    world[x+80*y]='.';
                                }
                            }
                        }
                    }
                    if(render[1]==true)
                    {
                        for(int y=screenLocations[0][1][1]; y<=screenLocations[0][3][1]; y++)//top<bottom
                        {
                            for(int x=screenLocations[0][2][0]; x<=screenLocations[0][0][0];x++)//far left<far right
                            {
                                if((y==screenLocations[0][3][1]||y==screenLocations[0][1][1])&isValid(x,y))
                                {
                                    world[x+80*y]='-';//top
                                }
                                else if(isValid(x,y))
                                {
                                    world[x+80*y]='.';//middle
                                }
                            }
                            if(y==screenLocations[0][3][1]||y==screenLocations[0][1][1])
                            {
                                if(isValid(screenLocations[0][2][0], 0))
                                {
                                    world[screenLocations[0][2][0]+80*y]='*';
                                }
                                if(isValid(screenLocations[0][0][0],0))
                                {
                                    world[screenLocations[0][0][0]+80*y]='*';
                                }
                            }
                            else
                            {
                                if(isValid(screenLocations[0][2][0],0))
                                {
                                    world[screenLocations[0][2][0]+80*y]='|';//the edges
                                }
                                if(isValid(screenLocations[0][0][0],0))
                                {
                                    world[screenLocations[0][0][0]+80*y]='|';
                                }
                            }
                        }
                    }
                    if(render[2]==true)
                    {
                        //m=slope
                        float m1=((float)screenLocations[0][1][1]-(float)screenLocations[1][1][1])/((float)screenLocations[0][0][0]-(float)screenLocations[1][0][0]);//(front top-back top)/(front right-back right)
                        float m2=((float)screenLocations[0][3][1]-(float)screenLocations[1][3][1])/((float)screenLocations[0][0][0]-(float)screenLocations[1][0][0]);//(front bottom-back bottom)/(front right-back right)
                        float b1=-((float)screenLocations[0][0][0]*m1)+((float)screenLocations[0][1][1]);//b1=(-x1*m1)+y1
                        float b2=-((float)screenLocations[0][0][0]*m2)+((float)screenLocations[0][3][1]);//b1=(-x1*m1)+y1
                        for(int x=screenLocations[0][0][0]; x<=screenLocations[1][0][0]; x++)//check this
                        {
                            for(int y=floatToInt(x*m1+b1); y<=floatToInt(x*m2+b2); y++)//y=slope of top checking if y<=slope of bottom
                            {
                                int tempa, tempb;
                                tempa=floatToInt(x*m1+b1);
                                tempb=floatToInt(x*m2+b2);
                                if(x==screenLocations[1][0][0]||x==screenLocations[0][0][0])
                                {
                                    if((y==floatToInt(x*m1+b1)||y==floatToInt(x*m2+b2))&&isValid(x,y))
                                    {
                                        world[x+80*y]='*';//corners
                                    }
                                    else if(isValid(x,y))
                                    {
                                        world[x+80*y]='|';//edge
                                    }
                                }
                                else if((y==tempa||y==tempb)&&isValid(x,y))
                                {
                                    world[x+80*y]='-';//top or bottom
                                }
                                else if(isValid(x,y))
                                {
                                    world[x+80*y]='.';
                                }
                            }
                        }
                    }
                }
                if(addToX==true)
                {
                    x++;
                    if(x>endingX)
                    {
                        break;
                    }
                }
                else
                {
                    x--;
                    if(x<endingX)
                    {
                        break;
                    }
                }
            }
            /*if(playerY==y)//this would mean that the objects being rendered are behind the player, which is pointless to mess with
            {

                }
                if (leave==true)
                {
                    break;
                }
            }*/
            if(addToY==true)
            {
                y++;
                if(y>endingY)
                {
                    break;
                }
            }
            else
            {
                y--;
                if(y<endingY)
                {
                    break;
                }
            }
        }
        if(direction==0)
        {
            world[0]='N';
        }
        else if(direction==1)
        {
            world[0]='E';
        }
        else if(direction==2)
        {
            world[0]='S';
        }
        else if(direction==3)
        {
            world[0]='W';
        }
        world[1]=' ';
        string tempString=convertInt(playerX);
        //cout<<"hi"<<endl;
        int storeLength=2;
        for(int i=0; i<tempString.size(); i++)
        {
            world[storeLength]=tempString[i];
            storeLength++;
        }
        storeLength++;
        world[storeLength]=',';
        storeLength++;
        world[storeLength]=' ';
        tempString=convertInt(playerY);
        storeLength++;
        for(int i=0; i<tempString.size(); i++)
        {
            world[i+storeLength]=tempString[i];
        }
        cout<<endl;
        printf(world);
        //Q=113
        //W=119
        //E=101
        //A=97
        //S=115
        //D=100
        for(;;)
        {
            int c=_getch();
            if(c==119&&playerY>0&&(blockPos[playerX][playerY-1]=='0'||blockPos[playerX][playerY-1]=='2'))//w
            {
                playerY--;//move up
//                leave=true;
                break;
            }
            else if(c==97)//a this will all change
            {
                /*vector< vector<char> > backupBlockPos(w, vector<char>(l));//make a backup spot
                for(int y=0; y<l; y++)//do a backup
                {
                    for(int x=0; x<w; x++)
                    {
                        backupBlockPos[x][y]=blockPos[x][y];
                    }
                }
                blockPos.clear();//clear the old
                blockPos.resize(l, vector<char>(w));//resize it to the
                for(int x=0; x<w; x++)
                {
                    int xCounter=0;
                    for (int y=l-1; y>=0; y--)
                    {
                        blockPos[xCounter][x]=backupBlockPos[x][y];
                        xCounter++;
                    }
                }
                int tempW=playerY;
                playerY=playerX;
                playerX=l-tempW-1;
                tempW=w;
                w=l;
                l=tempW;*/
                //leave=true;
                direction--;
                direction=normalize(direction,4);
                break;
            }
            else if(c==115&&playerY<l&&(blockPos[playerX][playerY+1]=='0'||blockPos[playerX][playerY+1]=='2'))//s
            {
                playerY++;
                //leave=true;
                break;
            }
            else if(c==100)//d, clockwise (0,0) becomes (l,0)
            {
                /*vector< vector<char> > backupBlockPos(w, vector<char>(l));
                for(int y=0; y<l; y++)
                {
                    for(int x=0; x<w; x++)
                    {
                        backupBlockPos[x][y]=blockPos[x][y];
                    }
                }
                blockPos.clear();
                blockPos.resize(l, vector<char>(w));
                int yCounter=0;
                for (int y=0; y<l; y++)
                {
                    int xCounter=0;
                    for(int x=w-1; x>=0; x--)
                    {
                        blockPos[y][xCounter]=backupBlockPos[x][y];
                        xCounter++;
                    }
                }
                int tempW=playerX;
                playerX=playerY;
                playerY=w-tempW-1;
                tempW=w;
                w=l;
                l=tempW;*/
                //leave=true;
                direction++;
                direction=normalize(direction,4);
                break;
            }
            else if(c==113&&playerX>0&&(blockPos[playerX-1][playerY]=='0'||blockPos[playerX-1][playerY]=='2'))//q
            {
                playerX--;
                //leave=true;
                break;
            }
            else if(c==101&&playerX<w-1&&(blockPos[playerX+1][playerY]=='0'||blockPos[playerX+1][playerY]=='2'))//e
            {
                playerX++;
                //leave=true;
                break;
            }
        }
    }
    return 0;
}
