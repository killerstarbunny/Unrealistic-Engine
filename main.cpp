#include <iostream>
#include <stdlib.h>
#include <string>
#include <windows.h>
#include <fstream>
#include <math.h>
#include <conio.h>
#include <sstream>
#include <vector>
#include <ostream>

using namespace std;

const float PI = 3.14159265f;
const float FOV = 60; //30 on both sides of the y axis
const float RAD_TO_DEG = 180/PI;
const float CUBE_WIDTH = 1; //is needed for calculating width
const float WIDTH_ADJUST= 1.5f;

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
* RENDERING SYSTEM HAS BEEN AJUSTED TO TAKE INTO ACCOUNT GRAPHICS, BUT THINGS NEED TO BE FIXED:
*   THE SYSTEM WORKED UNDER THE ASUMPTION THAT ALL BLOCKS WILL HAVE THE SAME SIDES SO THERE'S SOME OVERWRITING ON THE GRAPHICS
*   GRAPHICS IN SPRITE MODE SHOULD ALLOW THE BOXES TO RENDER ON SIDES BECAUSE OF THE ALPHA LAYER
* RESIZE DOESN'T QUITE WORK, LOOK AT 4.txt
* COMMENT!!!!!!!!!!!!
* CURRENT METHOD ONLY GETS HIGHT FOR THE CENTER, IN PRACTICE THIS DOESN'T WORK (SEE WORLD 1) FIX THAT
*   NO IDEA WHAT THIS MEANS
*       I THINK THIS IS WHEN I THOUGHT HIGHT WAS BASED ON DISTANCE, BUT IT'S NOT
* MOVE ALL WORLD RENDERING STUFF TO VOID TO MAKE IT ACTUALLY LIKE AN ENGINE
* RPG ELEMENTS
*   I REALLY WANT TO PUT EMPHASIS ON BATTLE OUTCOMES BLEEDING, BROKEN BONES, ACTUALL EFFECTS
*   HEAVY ATTACKS VS LIGHT ATTACKS
*   HEAVY (EASIER TO DODGE):
*       BREAK BONES (DECREASED SPEED, DEFENSE/ACCURACY (DEPENDING ON WHERE HIT))
*   LIGHT (MAKES YOURSELF MORE VUNURABLE):
*       BLEEDING (DECREASED SPEED, DEFENSE/ACCURACY (DEPENDING ON WHERE HIT))
*   POISION ATTACKS (WOULD FALL UNDER LIGHT PROBABLY OR JUST NORMAL)
*************/

/************
* VERSION 1:
* GIVE BLOCKS A STRUCT
*   WHY?
* CLEAN UP CLUTTER
* MAKE SURE floatToInt IS ALLWAYS USED
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
*************/

/************
* OTHER IDEAS:
* INTERACTION SCRIPTS
*   IF YOU PRESS E TEXT POPS UP
*   UPDATABLE GRAPHICS
*   GAME VARIBLES THAT INTERACTION SCRIPTS CAN USE
* DEFAULT BLOCK TEXTURE CHANGE
* ALPHA CHARACTER FOR TEXTURES
* LOWERCASED LETTERS ONLY ARE TEXTURED ON FRONT (AKA SPRITES)
*   MAYBE MAKE THEM RENDERED IN THE MIDDLE
* DEPTH FOG?
* CHANGE CHARS TO USE FULL ASSCI?
* INTERPERATE WORLD BASED ON DIRECTION?
*************/

/********
blockPos[wide][q]
loop thru y
loop thru x

length=y
width=x
********/


    ofstream out("output.txt");

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
    a=(int)f;
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

struct graphic
{
    int width;
    int length;
    char id;
    int imageWidth;
    int imageLength;
    bool isSprite;
    vector< vector<char> > data;//never actually resized, not really modified
    void resizeGraphic()
    {
        data.resize(width, vector<char>(length));
    }
    void resizeGraphic(int w, int l)
    {
        width=w;
        length=l;
        data.resize(width, vector<char>(length));
    }
    vector< vector<char> > image;//this is modified
    void resizeImage(int iw, int il)
    {
        imageWidth=iw;
        imageLength=il;
        image.resize(imageWidth, vector<char>(imageLength));
        int resizeCounterY=1;
        float ratioWidth=(float)width/imageWidth;
        float ratioLength=(float)length/imageLength;
        for(int yCounter=0; yCounter<imageLength; yCounter++)
        {
            int yIndex;
            yIndex=floatToInt(ratioLength*yCounter);
            for(int xCounter=0; xCounter<imageWidth; xCounter++)
            {
                int xIndex=floatToInt(xCounter*ratioWidth);
                cout<<convertInt(xIndex)<<" "<<convertInt(yIndex)<<endl;
                cout<<"pushing "<<data[xIndex][yIndex]<<" at x: "<<convertInt(xCounter)<<" and y:"<<convertInt(yCounter)<<endl;
                image[xCounter][yCounter]=data[xIndex][yIndex];
            }
        }
        image.resize(imageWidth, vector<char>(imageLength));
    }
    void resizeImage(char world[], float m1, float m2, float b1, float b2, int leftRange, int rightRange)//b1 is top b2 is bottom
    {
        int xCounter=0;
        //cout<<"starting x loop, leftx: "<<convertInt(leftRange)<<" rightx: "<<convertInt(rightRange)<<endl;
        for(int x=leftRange; x<=rightRange; x++)
        {
            int iw=rightRange-leftRange+1;
            float ratioWidth=(float)width/iw;
            int xIndex=floatToInt(xCounter*ratioWidth);
            if(xIndex==width)//sometimes floatToInt will break the limits of the array, so knock it back
            {
                xIndex--;
            }
            int yCounter=0;
            //cout<<"starting y loop"<<endl;
            for(int y=floatToInt(x*m1+b1); y<=floatToInt(x*m2+b2); y++)//y=slope of top checking if y<=slope of bottom
            {
                int il=floatToInt(x*m2+b2)-floatToInt(x*m1+b1)+1;
                float ratioLength=(float)length/il;
                int yIndex=floatToInt(ratioLength*yCounter);
                if(yIndex==length)//sometimes floatToInt will break the limits of the array, so knock it back
                {
                    yIndex--;
                }
                if(isValid(x,y))
                {
                    //out<<"xIndex: "<<xIndex<<" yIndex: "<<yIndex<<" il: "<<il<<" m2: "<<m2<<" m1: "<<m1<<endl;
                    //cout<<"Pushing "<<data[xIndex][yIndex]<<" at x: "<<convertInt(x)<<" and y: "<<convertInt(y)<<endl;
                    world[x+80*y]=data[xIndex][yIndex];
                }
                yCounter++;
            }
            xCounter++;
        }/**
        imageWidth=iw;
        imageLength=il;
        image.resize(imageWidth, vector<char>(imageLength));
        int resizeCounterY=1;
        float ratioWidth=(float)width/imageWidth;
        float ratioLength=(float)length/imageLength;
        for(int yCounter=0; yCounter<imageLength; yCounter++)
        {
            int yIndex;
            yIndex=ratioLength*yCounter;
            for(int xCounter=0; xCounter<imageWidth; xCounter++)
            {
                int xIndex=xCounter*ratioWidth;
                cout<<convertInt(xIndex)<<" "<<convertInt(yIndex)<<endl;
                cout<<"pushing "<<data[xIndex][yIndex]<<" at x: "<<convertInt(xCounter)<<" and y:"<<convertInt(yCounter)<<endl;
                image[xCounter][yCounter]=data[xIndex][yIndex];
            }
        }
        image.resize(imageWidth, vector<char>(imageLength));*/
    }
    /*void resizeImage(int iw, int il)
    {
        imageWidth=iw;
        imageLength=il;
        image.resize(imageWidth, vector<char>(imageLength));
        int resizeCounterY=1;
        for(int yCounter=1; yCounter<=length; yCounter++)
        {
            if((imageLength*yCounter)/(resizeCounterY*length))
            {
                int resizeCounterX=1;
                for(int xCounter=1; xCounter<=width; xCounter++)
                {
                    if((imageWidth*xCounter)/(resizeCounterX*width))
                    {
                        //cout<<"pushing "<<data[xCounter-1][yCounter-1]<<" at x: "<<convertInt(resizeCounterX-1)<<" and y:"<<convertInt(resizeCounterY-1)<<endl;
                        image[resizeCounterX-1][resizeCounterY-1]=data[xCounter-1][yCounter-1];
                        resizeCounterX++;
                    }
                }
                resizeCounterY++;
            }
        }
        image.resize(imageWidth, vector<char>(imageLength));
    }*/
    void printData()
    {
       for(int y2=0;y2<length;y2++)
        {
            for(int x2=0;x2<width; x2++)
            {
                cout<<data[x2][y2];
            }
            cout<<endl;
        }
    }
    void printImage()
    {
        for(int y2=0;y2<imageLength;y2++)
        {
            for(int x2=0;x2<imageWidth; x2++)
            {
                cout<<image[x2][y2];
            }
            cout<<endl;
        }
    }
};

int findGraphic(vector< graphic>graphicList, char id)
{
    for(int x=0; x<graphicList.size(); x++)
    {
        if(graphicList[x].id==id)
        {
            return x;
        }
    }
    return -1;
}


int main(int argc, char* argv[])
{
    vector< graphic>graphicList;
    /*graphic newGraphic;
    newGraphic.length=13;
    newGraphic.width=3;
    newGraphic.resizeGraphic();
    for(int x=0; x<newGraphic.width; x++)
    {
        for(int y=0; y<newGraphic.length; y++)
        {
            newGraphic.data[x][y]='1';
        }
    }
    for(int x=0; x<newGraphic.width; x++)
    {
        for(int y=0; y<newGraphic.length; y++)
        {
            cout<<newGraphic.data[x][y];
        }
        cout<<endl;
    }*/
    if(argc>1)
    {
        string command=argv[1];
        if(command=="-random")
        {
            makeRandomWorld();
            return 0;
        }
    }
    char world[1921];//declare screen (this is what is printed), for some reason has to be 1+size. if it isn't strange chars pop up has no overall effect if it's the wrong size
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
    string currentLine;
    size_t tempT;
    int l,w;
    getline(layout, currentLine);
    tempT=currentLine.find("x"); //getting dimensions of world
    w=stoi(currentLine.substr(0,tempT));//w=width
    l=stoi(currentLine.substr(tempT+1,currentLine.size()));//l=length
    cout <<"w: "<<w<<" l:  "<<l<<endl; //w is x, l is y
    vector< vector<char> > blockPos(w, vector<char>(l));
    int y=0;
    //cout<<"World layout:"<<endl;//0=floor, 1=wall, 2=spawn
    int playerX,playerY;//declare player's cords
    while(getline(layout, currentLine)) //get line (width)
    {
        if(currentLine.size()==0)
        {
            break;
        }
        for(int x=0;x<currentLine.size();x++) //put current line in array for world loading
        {
            blockPos[x][y]=currentLine[x];//put it in
            if(blockPos[x][y]=='2')//check if it's the player
            {
                playerX=x;
                playerY=y;
                //blockPos[x][y]='0';
                cout<<"Found Player x: "<<x<<" y: "<<y<<endl;
            }
            else if(blockPos[x][y]!='1'&&blockPos[x][y]!='0')
            {
                //cout<<"_BAD_"<<endl;
                graphic newGraphic;
                newGraphic.isSprite=false;
                newGraphic.id=blockPos[x][y];
                ifstream fileThing(fileLoc.c_str());
                string graphicFinder;
                string toSearch=":";
                toSearch=blockPos[x][y]+toSearch;
                //cout<<toSearch<<endl;
                while(getline(fileThing, graphicFinder))
                {
                    //cout<<graphicFinder<<endl;
                    if(graphicFinder==toSearch)
                    {
                        //cout<<"FOUND THE SEARCH"<<endl;
                        getline(fileThing, graphicFinder);
                        size_t tempT2=graphicFinder.find("x"); //getting dimensions of world
                        newGraphic.width=stoi(graphicFinder.substr(0,tempT2));//w=width
                        newGraphic.length=stoi(graphicFinder.substr(tempT2+1,graphicFinder.size()));//l=length
                        //cout<<
                        newGraphic.resizeGraphic();
                        //cout<<"Resize YES"<<endl;
                        int graphicY=0;
                        while(getline(fileThing, graphicFinder))
                        {

                            if(graphicFinder.size()==0)
                            {
                                //cout<<"BREAKING DOWN"<<endl;
                                break;
                            }
                            //cout<<endl;
                            for(int graphicX=0;graphicX<graphicFinder.size();graphicX++) //put current line in array for world loading
                            {
                                //cout<<graphicFinder[graphicX];
                                newGraphic.data[graphicX][graphicY]=graphicFinder[graphicX];
                            }
                            graphicY++;
                            if(graphicY==newGraphic.length)
                            {
                                while(getline(fileThing, graphicFinder))
                                {
                                    string dataCheck=blockPos[x][y]+"_data:";
                                    if(graphicFinder==dataCheck)
                                    {
                                        while(getline(fileThing, graphicFinder))
                                        {
                                            if(graphicFinder=="end_data")
                                            {
                                                break;
                                            }
                                            if(graphicFinder=="spritemode")
                                            {
                                                newGraphic.isSprite=true;
                                            }
                                        }
                                        break;
                                    }
                                    break;
                                }
                            }
                        }
                        graphicList.push_back(newGraphic);
                        break;
                    }
                }
                fileThing.close();
                cout<<"Doin things"<<endl;
                for(int index=0; index<graphicList.size(); index++)
                {
                    cout<<"index: "<<graphicList[index].id<<endl;
                    cout<<"width: "<<convertInt(graphicList[index].width)<<endl;
                    cout<<"length: "<<convertInt(graphicList[index].length)<<endl;
                    for(int y2=0; y2<graphicList[index].length; y2++)
                    {
                        for(int x2=0; x2<graphicList[index].width; x2++)
                        {
                            cout<<graphicList[index].data[x2][y2];
                        }
                        cout<<endl;
                    }
                }
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
    _getch();
    int direction=1;//1=north, 2=east, 3=south, 4=west. This will likely be changed
    for(;;)//start on north side
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
            bool leave=false;
            for(int x=0;x<w;x++)
            {
                if(blockPos[x][y]!='0'&&blockPos[x][y]!='2')//if it's a block we gotta render
                {
                    //THINGS GONNA GET BAD WE NEED TO CHANGE EVERYTHING  TO [top right, top left, bottom left, bottom right]
                    int screenLocations[2][4][2];//[front,back] [right (middle), right (top), left (middle), left (bottom)] [x,y]
                    //int screenPos[3];
                    float distances[2][4][2];//[front,back] [right, top right, left, top left] [x,y] (y can sometimes be a z (vert), but for atan2 it's still technically a y
                    //everything seems to work
                    //when we move to multi directional cube width stuff will have to be added depending on direction since the player is always the farthest backwards.
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
                                        int te=floatToInt(40-worldF);
                                        te=floatToInt(te*WIDTH_ADJUST);
                                        worldX=40-te;
                                    }
                                    else
                                    {
                                        int te=floatToInt(worldF-40);
                                        te=floatToInt(te*WIDTH_ADJUST);
                                        worldX=te+40;
                                    }
                                    screenLocations[z][i][0]=worldX;
                                    screenLocations[z][i][1]=12;
                                }
                            }
                        }
                    }
                    /***************
                    *     FRONT
                    *       |
                    *RIGHT--o----LEFT
                    *       |
                    *       |
                    ***************/
                    if(x>playerX&&x>0&&(blockPos[x-1][y]=='2'||blockPos[x-1][y]=='0'))//should left side be rendered
                    {
                        //m=slope
                        float m1=((float)screenLocations[0][1][1]-(float)screenLocations[1][1][1])/((float)screenLocations[0][2][0]-(float)screenLocations[1][2][0]);//(front top-back top)/(front left-back left) Had to make it negitive because with this setup higher in the screen is actually lower in the y
                        float m2=((float)screenLocations[0][3][1]-(float)screenLocations[1][3][1])/((float)screenLocations[0][2][0]-(float)screenLocations[1][2][0]);//(front bottom-back bottom)/(front left-back left)
                        float b1=-((float)screenLocations[0][2][0]*m1)+((float)screenLocations[0][1][1]);//b1=(-x1*m1)+y1
                        float b2=-((float)screenLocations[0][2][0]*m2)+((float)screenLocations[0][3][1]);//b1=(-x1*m1)+y1
                        if(blockPos[x][y]!='1')
                        {
                            if(y<l-1&&blockPos[x][y+1]=='1')
                            {
                                graphicList.at(findGraphic(graphicList,blockPos[x][y])).resizeImage(world, m1, m2, b1, b2, screenLocations[1][2][0]+1, screenLocations[0][2][0]-1);
                            }
                            else
                            {
                                graphicList.at(findGraphic(graphicList,blockPos[x][y])).resizeImage(world, m1, m2, b1, b2, screenLocations[1][2][0], screenLocations[0][2][0]);
                            }
                        }
                        else
                        {
                            for(int screenX=screenLocations[1][2][0]; screenX<=screenLocations[0][2][0]; screenX++)
                            {
                                for(int screenY=floatToInt(screenX*m1+b1); screenY<=floatToInt(screenX*m2+b2); screenY++)//screenY=slope of top checking if screenY<=slope of bottom
                                {
                                    int tempa, tempb;
                                    tempa=floatToInt(screenX*m1+b1);
                                    tempb=floatToInt(screenX*m2+b2);
                                    if(screenX==screenLocations[1][2][0]||screenX==screenLocations[0][2][0])
                                    {
                                        if((screenY==floatToInt(screenX*m1+b1)||screenY==floatToInt(screenX*m2+b2))&isValid(screenX,screenY))
                                        {
                                            world[screenX+80*screenY]='*';//corners
                                        }
                                        else if(isValid(screenX,screenY))
                                        {
                                            world[screenX+80*screenY]='|';//edge
                                        }
                                    }
                                    else if((screenY==tempa||screenY==tempb)&isValid(screenX,screenY))
                                    {
                                        world[screenX+80*screenY]='-';//top or bottom
                                    }
                                    else if(isValid(screenX,screenY))
                                    {
                                        world[screenX+80*screenY]='.';
                                    }
                                }
                            }
                        }
                    }
                    if(y<playerY&&y<l&&(blockPos[x][y+1]=='2'||blockPos[x][y+1]=='0'))//should the front be rendered
                    {
                        if(blockPos[x][y]!='1')
                        {
                            /********************
                            * THIS LOOKS REALLY DERPY, BUT I NEED IT.
                            * THE ENGINE LOOKS AT ALL THE BLOCKS AND SAYS
                            * "HEY WE CAN OVERWRITE EDGES BECAUSE EVERY BLOCK HAS THE SAME EDGES"
                            * WELL THAT DOESN'T WORK FOR GRAPHICS, SO WE HAVE TO DO THIS.
                            * EVERY TIME THERE'S A
                            ********************/
                            if(x>0&&blockPos[x-1][y]=='1')
                            {
                                graphicList.at(findGraphic(graphicList,blockPos[x][y])).resizeImage(world, 0, 0, (float)screenLocations[0][1][1], (float)screenLocations[0][3][1], screenLocations[0][2][0]+1, screenLocations[0][0][0]-1);
                            }
                            else
                            {
                                graphicList.at(findGraphic(graphicList,blockPos[x][y])).resizeImage(world, 0, 0, (float)screenLocations[0][1][1], (float)screenLocations[0][3][1], screenLocations[0][2][0], screenLocations[0][0][0]);
                            }

                        }
                        else
                        {
                            for(int y=screenLocations[0][1][1]; y<=screenLocations[0][3][1]; y++)//top<bottom
                            {
                                for(int x=screenLocations[0][2][0]; x<=screenLocations[0][0][0]&&isValid(1,y);x++)//far left<far right
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
                                if(isValid(1,y)&&(y==screenLocations[0][3][1]||y==screenLocations[0][1][1]))
                                {
                                    if(isValid(screenLocations[0][2][0],0))
                                    {
                                        world[screenLocations[0][2][0]+80*y]='*';
                                    }
                                    if(isValid(screenLocations[0][0][0],0))
                                    {
                                        world[screenLocations[0][0][0]+80*y]='*';
                                    }
                                }
                                else if(isValid(1,y))
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
                    }
                    if(x<playerX&&x<w&&(blockPos[x+1][y]=='2'||blockPos[x+1][y]=='0'))//should right side be rendered
                    {
                        //m=slope
                        float m1=((float)screenLocations[0][1][1]-(float)screenLocations[1][1][1])/((float)screenLocations[0][0][0]-(float)screenLocations[1][0][0]);//(front top-back top)/(front right-back right)
                        float m2=((float)screenLocations[0][3][1]-(float)screenLocations[1][3][1])/((float)screenLocations[0][0][0]-(float)screenLocations[1][0][0]);//(front bottom-back bottom)/(front right-back right)
                        float b1=-((float)screenLocations[0][0][0]*m1)+((float)screenLocations[0][1][1]);//b1=(-x1*m1)+y1
                        float b2=-((float)screenLocations[0][0][0]*m2)+((float)screenLocations[0][3][1]);//b1=(-x1*m1)+y1
                        if(blockPos[x][y]!='1')
                        {
                            if(y>0&&blockPos[x][y-1]!='0')
                            {
                                graphicList.at(findGraphic(graphicList,blockPos[x][y])).resizeImage(world, m1, m2, b1, b2, screenLocations[0][0][0]+1, screenLocations[1][0][0]-1);
                            }
                            else
                            {
                                graphicList.at(findGraphic(graphicList,blockPos[x][y])).resizeImage(world, m1, m2, b1, b2, screenLocations[0][0][0], screenLocations[1][0][0]);
                            }

                            //screenLocations[1][3][1] is way too big, but it seems to be tampered with. it's fine at line 623
                        }
                        else
                        {
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
                }
            }
            if(playerY==y)//this would mean that the objects being rendered are behind the player, which is pointless to mess with
            {
                for(int a=0;a<24;a++)//prep the world with the empty chars, likely will be changed so the roof and the ground are different chars
                {
                    for(int b=0;b<80;b++)
                    {
                        out<<world[80*a+b];
                    }
                    out<<endl;
                }
                printf(world);
                /*for(int a=0;a<24;a++)//prep the world with the empty chars, likely will be changed so the roof and the ground are different chars
                {
                    for(int b=0;b<80;b++)
                    {
                        cout<<world[80*a+b];
                    }
                }*/
                //Q=113
                //W=119
                //E=101
                //A=97
                //S=115
                //D=100
                int oldPlayerX=playerX;
                int oldPlayerY=playerY;
                for(;;)
                {
                    int c=_getch();
                    if(c==27)
                    {
                        return 0;
                    }
                    else if(c==119&&playerY>0&&(blockPos[playerX][playerY-1]=='0'||blockPos[playerX][playerY-1]=='2'))//w
                    {
                        playerY--;
                        leave=true;
                        break;
                    }
                    else if(c==97)//a
                    {
                        vector< vector<char> > backupBlockPos(w, vector<char>(l));
                        for(int y=0; y<l; y++)
                        {
                            for(int x=0; x<w; x++)
                            {
                                backupBlockPos[x][y]=blockPos[x][y];
                            }
                        }
                        blockPos.clear();
                        blockPos.resize(l, vector<char>(w));
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
                        l=tempW;
                        leave=true;
                        break;
                    }
                    else if(c==115&&playerY<l&&(blockPos[playerX][playerY+1]=='0'||blockPos[playerX][playerY+1]=='2'))//s
                    {
                        playerY++;
                        leave=true;
                        break;
                    }
                    else if(c==100)//d, clockwise (0,0) becomes (l,0)
                    {
                        vector< vector<char> > backupBlockPos(w, vector<char>(l));
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
                        l=tempW;
                        leave=true;
                        break;
                    }
                    else if(c==113&&playerX>0&&(blockPos[playerX-1][playerY]=='0'||blockPos[playerX-1][playerY]=='2'))//q
                    {
                        playerX--;
                        leave=true;
                        break;
                    }
                    else if(c==101&&playerX<w-1&&(blockPos[playerX+1][playerY]=='0'||blockPos[playerX+1][playerY]=='2'))//e
                    {
                        playerX++;
                        leave=true;
                        break;
                    }
                }
                if (leave==true)
                {
                    blockPos[oldPlayerX][oldPlayerY]='0';
                    blockPos[playerX][playerY]='2';
                    break;
                }
            }
        }
    }
    return 0;
}
