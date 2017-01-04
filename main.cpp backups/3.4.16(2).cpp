#include <iostream>
#include <string>
#include <windows.h>
#include <fstream>
#include <math.h>
#include <conio.h>
#include <sstream>

using namespace std;

const float PI = 3.14159265;
const float FOV = 60; //30 on both sides of the y axis
const float RAD_TO_DEG = 180/PI;
const float CUBE_WIDTH = 1; //is needed for calculating width
const float WIDTH_ADJUST= 1.5;

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
* TODO:
* COMMENT!!!!!!!!!!!!
* FIX RENDERING ON EDGE OF SCREEN
* CHANGE FOV TO 30, TURNS OUT MY UNDERSTANDING OF WHAT FOV=60 IS WAS WRONG. THIS WILL FIX THE WHOLE STRECH ISSUE
* FIX "//WHAT DOES THIS DO I DON'T UNDERSTAND!!!!!!!!"
*   SEEMS TO THROW A FIT ON BLOCKS THAT SOULDN'T BE RENDERED. MESSAGE MIGHT HAVE BEEN WRITTEN WHEN I WAS STILL FIGURING OUT THE MATH
* STILL UNSURE WHEN IT COMES TO 79 OR 80 FOR DEGREE
* CURRENT METHOD ONLY GETS HIGHT FOR THE CENTER, IN PRACTICE THIS DOESN'T WORK (SEE WORLD 1) FIX THAT
*************/

/************
* VERSION 1:
* GIVE BLOCKS A STRUCT
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

string convertInt(int number)
{
    stringstream ss;//create a stringstream
    ss << number;//add number to the stream
    return ss.str();//return a string with the contents of the stream
}

int floatToInt(float f)
{
    int a;
    a=f;
    f-=a;//get the decimal points
    if(f>0.5)
    {
        a++;//round up
    }
    return a;
}

bool isValid(int x,int y)
{
    if(x>=0&&x<80&&y>=0&&y<24)
    {
        return true;
    }
    return false;
}

int main()
{
    //cout<<atan2(1.0,1.0);// y/x
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
    //ShowConsoleCursor(false); //will be needed when thing isn't so debugy
    string currentLine;
    size_t tempT;
    int l,w;
    getline(layout, currentLine);
    tempT=currentLine.find("x"); //getting dimensions of world
    w=stoi(currentLine.substr(0,tempT));//w=width
    l=stoi(currentLine.substr(tempT+1,currentLine.size()));//l=length
    cout <<"w: "<<w<<" l:  "<<l<<endl; //w is x, l is y
    char **blockPos = new char*[w];//declare array for all ze boxes
    for (int i = 0; i < l; i++) //
    {
        blockPos[i] = new char[l];//gotta do this nonsense
    }
    int y=0;
    cout<<"World layout:"<<endl;//0=floor, 1=wall, 2=spawn
    int playerx,playery;//declare player's cords
    while(getline(layout, currentLine)) //get line (width)
    {
        for(int x=0;x<currentLine.size();x++) //put current line in array for world loading
        {
            blockPos[x][y]=currentLine[x];//put it in
            if(blockPos[x][y]=='2')//check if it's the player
            {
                playerx=x;
                playery=y;
                cout<<"Found Player x: "<<x<<" y: "<<y<<endl;
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
    int direction=1;//1=north, 2=east, 3=south, 4=west. This will likely be changed
    if(direction==1)//start on north side
    {
        for(int a=0;a<24;a++)//prep the world with the empty chars, likely will be changed so the roof and the ground are different chars
        {
            for(int b=0;b<80;b++)
            {
                world[80*a+b]=' ';
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
        for(int y=0;y<l;y++)
        {
            for(int x=0;x<w;x++)
            {
                if(blockPos[x][y]=='1')//if it's a block we gotta render
                {
                    //cout<<"x: "<<x<<" y: "<<y<<endl;//for debug to double check math
                    //cout<<"playerX: "<<playerx<<" playerY: "<<playery<<endl;//for debug to double check math
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
                    //_getch();//pause so we can check math
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
                            //cout<<"Distance: y="<<distances[z][i][1]<< ", x="<<distances[z][i][0]<<" Degree: ";
                            float degree=atan2(distances[z][i][1],distances[z][i][0]); //x = 66 - 113 (middle 90) y = 8 - 351 (middle 0) (on world 2), this one is easier to work with
                            //float degree=atan2(distances[0][i][0],distances[0][i][1]); //x = 23 - 336 (middle 0) y = 81 - 98 (middle 90) (on world 2)
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
                                worldX=(80*degree)/(FOV*2);//when fov stuff is fixed this might need to be kept like this, or maybe this will be needed to help fix the vertical stretch. It's possible the stretch is simply because chars aren't square
                                screenLocations[z][i][0]=screenLocations[z][i-1][0];//this x is the same as as the last one, technically this doesn't need to be assigned, but I like it
                                screenLocations[z][i][1]=12-worldX;//this is the real important piece subtracting half the height of the block from 12 puts it in the proper y location
                                //cout<<degree<<" WorldX: "<<worldX<<endl;
                                //world[screenLocations[z][i][0]+screenLocations[z][i][1]*80]=blaa[0];
                            }
                            else if(i==3)
                            {
                                degree = sqrt(degree*degree);
                                worldX=(80*degree)/(FOV*2);
                                screenLocations[z][i][0]=screenLocations[z][i-1][0];
                                screenLocations[z][i][1]=12+worldX;//everything is the same as i==1 except instead of subtracting you add
                                //cout<<degree<<" WorldX: "<<worldX<<endl;
                                //world[screenLocations[z][i][0]+screenLocations[z][i][1]*80]=blaa[0];
                            }
                            else
                            {
                                if(degree<0)
                                {
                                    degree+=360;
                                    //cout<<degree<<" TOO BIG"<<endl;//not really sure what to do with this stuff
                                }
                                if(degree>FOV+90||degree<FOV-90)
                                {
                                    //WHAT DOES THIS DO I DON'T UNDERSTAND!!!!!!!!
                                    cout<<degree<<" BAD"<<endl;
                                    //NOT IN MY HOUSE
                                    //do stuff
                                }
                                else
                                {
                                    //cout<<degree<<" Adjusted Deg: ";
                                    degree-=(90-FOV);
                                    float worldF=(((80*degree)/(FOV*2)));
                                    //worldX=(((80*degree)/(FOV*2)));
                                    if(worldF<40)
                                    {
                                        int te=40-worldF;
                                        te*=WIDTH_ADJUST;
                                        worldX=40-te;
                                    }
                                    else
                                    {
                                        int te=worldF-40;
                                        te*=WIDTH_ADJUST;
                                        worldX=te+40;
                                    }
                                    /*if(worldX<0)
                                    {
                                        worldX=0;
                                        cout<<" WorldX set to 0 ";
                                    }
                                    if(worldX>80)
                                    {
                                        worldX=80;
                                        cout<<" WorldX set to 80 ";
                                    }*/
                                    screenLocations[z][i][0]=worldX;
                                    screenLocations[z][i][1]=12;
                                    //world[screenLocations[z][i][0]+screenLocations[z][i][1]*80]=blaa[0];
                                    //cout<<degree<<" WorldX: "<<worldX<<" World: "<<worldF<<endl;
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
                    if(x>playerx&&x>0&&blockPos[x-1][y]!='1')//should left side be rendered
                    {
                        //cout<<"render left"<<endl;
                        render[0]=true;
                    }
                    if(y<playery&&y<l&&blockPos[x][y+1]!='1')//should the front be rendered
                    {
                        //cout<<"render front"<<endl;
                        render[1]=true;
                    }
                    if(x<playerx&&x<w&&blockPos[x+1][y]!='1')//should right side be rendered
                    {
                        //cout<<"render right"<<endl;
                        render[2]=true;
                    }
                    if(render[0]==true)
                    {
                        //m=slope
                        //cout<<screenLocations[0][1][1]<<" "<<screenLocations[1][1][1]<<" "<<screenLocations[0][2][0]<<" "<<screenLocations[1][2][0]<<endl;
                        //cout<<screenLocations[0][3][1]<<" "<<screenLocations[1][3][1]<<" "<<screenLocations[0][2][0]<<" "<<screenLocations[1][2][0]<<endl;
                        float m1=((float)screenLocations[0][1][1]-(float)screenLocations[1][1][1])/((float)screenLocations[0][2][0]-(float)screenLocations[1][2][0]);//(front top-back top)/(front left-back left) Had to make it negitive because with this setup higher in the screen is actually lower in the y
                        float m2=((float)screenLocations[0][3][1]-(float)screenLocations[1][3][1])/((float)screenLocations[0][2][0]-(float)screenLocations[1][2][0]);//(front bottom-back bottom)/(front left-back left)
                        //cout<<screenLocations[0][2][0]<<" "<<m1<<" "<<screenLocations[0][1][1]<<endl;
                        //cout<<screenLocations[0][2][0]<<" "<<m1<<" "<<screenLocations[0][3][1]<<endl;
                        float b1=-((float)screenLocations[0][2][0]*m1)+((float)screenLocations[0][1][1]);//b1=(-x1*m1)+y1
                        float b2=-((float)screenLocations[0][2][0]*m2)+((float)screenLocations[0][3][1]);//b1=(-x1*m1)+y1
                        //cout<<"y1=x*"<<m1<<"+"<<b1<<" y2=x*"<<m2<<"+"<<b2<<endl;
                        for(int x=screenLocations[1][2][0]; x<=screenLocations[0][2][0]; x++)
                        {
                            for(int y=floatToInt(x*m1+b1); y<=floatToInt(x*m2+b2); y++)//y=slope of top checking if y<=slope of bottom
                            {
                                //cout<<"y: "<<y<<endl;
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
                                //cout<<"a"<<endl;
                                if((y==screenLocations[0][3][1]||y==screenLocations[0][1][1])&isValid(x,y))
                                {
                                    world[x+80*y]='-';//top
                                    //world[x+80*y]='-';
                                }
                                else if(isValid(x,y))
                                {
                                    world[x+80*y]='.';//middle
                                }
                            }
                            if(y==screenLocations[0][3][1]||y==screenLocations[0][1][1])
                            {
                                //world[x+80*y]='*';//the corners
                                if(isValid(screenLocations[0][2][0], 0))
                                {
                                    world[screenLocations[0][2][0]+80*y]='*';
                                }
                                if(isValid(screenLocations[0][0][0],0))
                                {
                                    world[screenLocations[0][0][0]+80*y]='*';
                                }
                                //world[screenLocations[0][2][0]+80*y]='*';//the corners
                                //world[screenLocations[0][0][0]+80*y]='*';
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
                                //world[x+80*y]='|';//the edges
                                //world[screenLocations[0][2][0]+80*y]='|';//the edges
                                //world[screenLocations[0][0][0]+80*y]='|';
                        }
                    }
                    if(render[2]==true)
                    {
                        //m=slope
                        cout<<screenLocations[0][1][1]<<" "<<screenLocations[1][1][1]<<" "<<screenLocations[0][0][0]<<" "<<screenLocations[1][0][0]<<endl;
                        cout<<screenLocations[0][3][1]<<" "<<screenLocations[1][3][1]<<" "<<screenLocations[0][0][0]<<" "<<screenLocations[1][0][0]<<endl;
                        float m1=((float)screenLocations[0][1][1]-(float)screenLocations[1][1][1])/((float)screenLocations[0][0][0]-(float)screenLocations[1][0][0]);//(front top-back top)/(front right-back right)
                        float m2=((float)screenLocations[0][3][1]-(float)screenLocations[1][3][1])/((float)screenLocations[0][0][0]-(float)screenLocations[1][0][0]);//(front bottom-back bottom)/(front right-back right)
                        cout<<screenLocations[0][0][0]<<" "<<m1<<" "<<screenLocations[0][1][1]<<endl;
                        cout<<screenLocations[0][0][0]<<" "<<m1<<" "<<screenLocations[0][3][1]<<endl;
                        float b1=-((float)screenLocations[0][0][0]*m1)+((float)screenLocations[0][1][1]);//b1=(-x1*m1)+y1
                        float b2=-((float)screenLocations[0][0][0]*m2)+((float)screenLocations[0][3][1]);//b1=(-x1*m1)+y1
                        cout<<"y1=x*"<<m1<<"+"<<b1<<" y2=x*"<<m2<<"+"<<b2<<endl;
                        for(int x=screenLocations[0][0][0]; x<=screenLocations[1][0][0]; x++)//check this
                        {
                            for(int y=floatToInt(x*m1+b1); y<=floatToInt(x*m2+b2); y++)//y=slope of top checking if y<=slope of bottom
                            {
                                //cout<<"y: "<<y<<endl;
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
                    cout<<endl;
                    printf(world);
                    //cout<<" WUIADS"<<endl;
                }
            }
            if(playery==y)//this would mean that the objects being rendered are behind the player, which is pointless to mess with
            {
                break;
            }
        }
    }
    return 0;
}
