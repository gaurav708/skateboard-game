#include <windows.h>
#include <GL/glut.h>
#include <SOIL.h>
#include <iostream>
#include <cstring>
#include <cmath>
#include <ctime>
#include <sstream>
#include <fstream>

using namespace std;

// Min and Max distance b/w blocks
#define MIN_DST 150
#define MAX_DST 250

//Min and Max heights of player
#define MIN_HEIGHT 55						// Height of platform : 30
#define MAX_HEIGHT 135

double a=50.0, b, velocityBlock, velocityPlayer, maxVelocity ;
double block[30], timediff ;
double separator;
bool finished ;
int finalscore=0, lastBlock ;
int stoneCount ;
int hs ;
clock_t oldTime, newTime ;
bool flag = false ;

// Converts int to string
string tostring(int j)
{
  stringstream ss;
  string s;
  ss<<j;
  ss>>s;
  return s;       
}

// Load all the images into OpenGL textures
GLuint bgd, stone, runner, go;
void loadTextures(){

bgd = SOIL_load_OGL_texture
	(
		"images/bgd.png",
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
	);

stone = SOIL_load_OGL_texture
	(
		"images/stone.png",
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
	);

runner = SOIL_load_OGL_texture
	(
		"images/skater.png",
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
	);

go = SOIL_load_OGL_texture
	(
		"images/go.png",
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
	);
}

// Initialize the parameters of the game
void initialize()
{
	oldTime = clock() ;
	block[0] = 700 ;
	velocityPlayer = 0 ;
	velocityBlock = 240 ;
	maxVelocity = 240;
	flag = false ;
	stoneCount = 0 ;
	lastBlock = 0 ;
	separator = 640 ;
	b = MIN_HEIGHT ;
	finished = 0 ;
}

// Initialize the window
void myInit()
{
  glClearColor(1.0,4.0,6.0,0.0);
  glColor3f(0.0f, 0.0f, 0.0f);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0.0, 640.0, 0.0, 480.0);
  loadTextures();
  initialize();
}

// Create Background Scene
void createBgd(){
	separator-= velocityBlock * timediff ;
	if(separator<=0)
		separator+= 640 ;

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, bgd);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glColor4f(1.0,1.0,1.0,1.0);
	glBegin(GL_POLYGON);
		glTexCoord2f(0.0f, 0.0f);
		glVertex2f(separator-640,0);
		
		glTexCoord2f(1.0f, 0.0f);
		glVertex2f(separator,0);
		
		glTexCoord2f(1.0f, 1.0f);
		glVertex2f(separator,480);
		
		glTexCoord2f(0.0f, 1.0f);
		glVertex2f(separator-640,480);
	glEnd();

	glBegin(GL_POLYGON);
		glTexCoord2f(0.0f, 0.0f);
		glVertex2f(separator,0);
		
		glTexCoord2f(1.0f, 0.0f);
		glVertex2f(separator+640,0);
		
		glTexCoord2f(1.0f, 1.0f);
		glVertex2f(separator+640,480);
		
		glTexCoord2f(0.0f, 1.0f);
		glVertex2f(separator,480);
	glEnd();
	glDisable(GL_TEXTURE_2D);
}

// Adds a new block at the end
void newblock()
{
	block[lastBlock+1] = block[lastBlock] + ( ((double)rand()/RAND_MAX)*(MAX_DST-MIN_DST) ) + MIN_DST ;
	lastBlock++ ;
}

// Detects collision b/w player and stones
void detect()
{
  if( (abs(a-block[0])<25 && b<75) || (abs(a-block[1])<25 && b<75) )
  {
		finished=true;
    finalscore=stoneCount;
    PlaySound(TEXT("sounds/crash.wav"), NULL, SND_SYNC|SND_FILENAME);
		
		ifstream fin("data/highscore.txt");
    fin>>hs;
    fin.close();

    hs=max(hs,finalscore);
    
    ofstream fout("data/highscore.txt");
    fout<<hs<<"\n";
    fout.close();

    PlaySound(TEXT("sounds/go.wav"), NULL, SND_ASYNC|SND_FILENAME);
  }
}

// Display callback function
void disp()
{
  glClear(GL_COLOR_BUFFER_BIT);

  if(!finished)
  {	
	newTime = clock() ;
	timediff = ( (double)(newTime-oldTime)/CLOCKS_PER_SEC ) ;
	oldTime = newTime ; 

	createBgd();

  b+= velocityPlayer * timediff ;
	if(b>=MAX_HEIGHT){
		velocityPlayer = -1 * maxVelocity ;
		flag = false ;
	}
	if(b<=MIN_HEIGHT) {
		b = MIN_HEIGHT ;
		if(flag) {
			velocityPlayer = maxVelocity ;
			PlaySound(TEXT("sounds/jump.wav"), NULL, SND_ASYNC|SND_FILENAME);
		}
		else
			velocityPlayer = 0 ;
	}

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, runner);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glMatrixMode(GL_MODELVIEW);

	glBegin(GL_POLYGON);
	  glTexCoord2d(0.0, 0.0);
	  glVertex2d(a-25, b-25);

	  glTexCoord2d(0.0, 1.0);
      glVertex2d(a-25, b+25);

	  glTexCoord2d(1.0, 1.0);
      glVertex2d(a+25, b+25);

	  glTexCoord2d(1.0, 0.0);
      glVertex2d(a+25, b-25);
    glEnd();

	double changePos = velocityBlock * timediff ;

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, stone);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	for(int i=0; i<=lastBlock; i++) {	
		block[i]-= changePos ;
		double pos = block[i] ;
        
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		
		glMatrixMode(GL_MODELVIEW);
		
		
		glLoadIdentity();
		glBegin(GL_POLYGON);
		  glColor4f(1.0,1.0,1.0,1.0);

      glTexCoord2d(0.0, 0.0);
		  glVertex2d(pos, 30);

		  glTexCoord2d(0.0, 1.0);
      glVertex2d(pos, 50);

		  glTexCoord2d(1.0, 1.0);
      glVertex2d(pos+20, 50);

		  glTexCoord2d(1.0, 0.0);
      glVertex2d(pos+20, 30);
     glEnd();
    }    
		glDisable(GL_TEXTURE_2D);  

		detect();

    glRasterPos2f(500, 400);
    string score="Score: ";
    score+=tostring(stoneCount);
    int len, i;
    len = score.size();
    for (i = 0; i < len; i++)
    {
      glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18 , score[i]);
    }
    
    glFlush();
    
    if(block[lastBlock]<640)
    {
      newblock();
    }

		if(block[0]<-21) {
			for(int i=0 ; i<lastBlock ; i++) 
				block[i] = block[i+1] ;
			lastBlock-- ;
			stoneCount++ ;
			if(stoneCount%5 == 0) {
				velocityBlock += 10;
				maxVelocity += 15;
			}
		}
  }
  else
	{
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, go);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glBegin(GL_POLYGON);
			glTexCoord2f(0.0f, 0.0f);
			glVertex2f(0,0);
		
			glTexCoord2f(1.0f, 0.0f);
			glVertex2f(640,0);
		
			glTexCoord2f(1.0f, 1.0f);
			glVertex2f(640,480);
		
			glTexCoord2f(0.0f, 1.0f);
			glVertex2f(0,480);
		glEnd();
		glDisable(GL_TEXTURE_2D);

		int len, i;
    
    string SC="Score: ";
    SC+=tostring(finalscore);
    len = SC.size();
    glRasterPos2f(280-len/2, 260);

    for (i = 0; i < len; i++)
    {
      glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18 , SC[i]);
    }
    
    string HC="High Score: ";
    HC+=tostring(hs);
    len = HC.size();
    glRasterPos2f(280-len/2, 240);
    
    for (i = 0; i < len; i++)
    {
      glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18 , HC[i]);
    }
    
    string NG="Press N for new game";
    len = NG.size();
    glRasterPos2f(280-len/2, 220);

    for (i = 0; i < len; i++)
    {
      glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18 , NG[i]);
    }
  }
	glFlush();
}

// Callback function for handling arrow key events
void specialKeyFunc(int k, int x, int y)
{
	if(!finished && k==GLUT_KEY_UP)
		flag = true ;
}

// Callback function for handling alphanumeric key events
void keyFunc(unsigned char k, int x, int y)
{
  if(k=='n' || k=='N')
  {
    initialize();
		glutPostRedisplay();
  }
}

// Redisplays the contents of the window
void idleFunc()
{
	glutPostRedisplay() ;
}

// Main Function
int main(int argc, char **argv)
{
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
  glutInitWindowSize(900, 675);
  glutInitWindowPosition(0, 0);           
  glutCreateWindow("Rural Skateboarder");
  glutDisplayFunc(disp);
  glutIdleFunc(idleFunc) ;
  glutSpecialFunc(specialKeyFunc);
  glutKeyboardFunc(keyFunc);
  myInit();
  glutMainLoop();
  return 0;
}
