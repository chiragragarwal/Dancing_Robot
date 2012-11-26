/*  
    Author : Chirag R. Agarwal (2009B4A7606G)        
    Date : 25/11/2012
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include <windows.h>
#include <mmsystem.h>

#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define TORSO_HEIGHT 5.0
#define TORSO_RADIUS 1.3

#define HEAD_HEIGHT 1.7
#define HEAD_RADIUS 1.3

#define UPPER_ARM_HEIGHT 3.0
#define LOWER_ARM_HEIGHT 2.0

#define UPPER_ARM_RADIUS  0.65
#define LOWER_ARM_RADIUS  0.5

#define LOWER_LEG_HEIGHT 3.0
#define UPPER_LEG_HEIGHT 3.0

#define UPPER_LEG_RADIUS  0.65
#define LOWER_LEG_RADIUS  0.5

#define SHOULDER_RADIUS 0.85
#define JOINT_RADIUS 0.85

int light_flag = 0;

typedef float point[3];
GLfloat seconds;
GLuint texture[4]; //the array for our texture

static GLfloat theta[11] = {0.0,0.0,0.0,0.0,0.0,0.0,0.0,
            180.0,0.0,180.0,0.0}; /* initial joint angles */

GLfloat theta_min[11];
GLfloat theta_max[11]; 
GLfloat theta_freq[11];
GLfloat knees_y, knees_z;
GLfloat mat_x, mat_y, mat_z;

GLfloat mat_specular[]={0.0, 0.0, 1.0, 1.0};
GLfloat mat_diffuse[]={0.0, 0.0, 1.0, 1.0};
GLfloat mat_ambient[]={0.0, 0.0, 1.0, 1.0};
GLfloat mat_shininess={100.0};
        
static GLint angle = 3;
int flag_dance = 0;

GLUquadricObj *t, *gl, *h, *lua, *lla, *rua, *rla, *lll, *rll, *rul, *lul;
GLUquadricObj *rs, *ls, *rleg, *lleg, *elbow; 
GLUquadricObj *disk;

double size=1.0;
int i;

GLuint LoadTexture( const char * filename, int width, int height )
{
    GLuint texture;
    unsigned char * data;
    FILE * file;

    //The following code will read in our RAW file
    file = fopen( filename, "rb" );
    if ( file == NULL ) return 0;
    data = (unsigned char *)malloc( width * height * 3 );
    fread( data, width * height * 3, 1, file );
    fclose( file );

    glGenTextures( 1, &texture ); //generate the texture with the loaded data
    glBindTexture( GL_TEXTURE_2D, texture ); //bind the texture to it’s array
    glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE ); //set texture environment parameters

    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );

    //Generate the texture
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

    free( data ); //free the texture
    return texture; //return whether it was successfull
}

void FreeTexture( GLuint texture )
{
  glDeleteTextures( 1, &texture );
}

void square (void) {
     glPushMatrix();
     
    glBindTexture( GL_TEXTURE_2D, texture[0] ); //bind our texture to our shape
   
    glBegin (GL_QUADS);
          glTexCoord2f(0.0, 0.0); glVertex3f(-5, -5, -010.0);
          glTexCoord2f(0.0, 1.0); glVertex3f(-5, 5, -10.0);
          glTexCoord2f(1.0, 1.0); glVertex3f(5, 5, -10.0);
          glTexCoord2f(1.0, 0.0); glVertex3f(5, -5, -10.0);
    glEnd();

     glBindTexture( GL_TEXTURE_2D, texture[2] ); //bind our texture to our shape
     
    glBegin (GL_QUADS);
        glTexCoord2f(0.0, 0.0); glVertex3f(-5.0,-5.0, -10.0);
          glTexCoord2f(0.0, 1.0); glVertex3f(-5, 5, -10.0);
          glTexCoord2f(1.0, 1.0); glVertex3f(-10, 10, 0.0);
          glTexCoord2f(1.0, 0.0); glVertex3f(-10, -10, 0.0);
    glEnd();
    
     glBindTexture( GL_TEXTURE_2D, texture[3] ); //bind our texture to our shape
     glBegin (GL_QUADS);
        glTexCoord2f(0.0, 0.0); glVertex3f(5.0,-5.0, -10.0);
          glTexCoord2f(0.0, 1.0); glVertex3f(5, 5, -10.0);
          glTexCoord2f(1.0, 1.0); glVertex3f(10, 10, 0.0);
          glTexCoord2f(1.0, 0.0); glVertex3f(10, -10, 0.0);
    glEnd();
    
     glBindTexture( GL_TEXTURE_2D, texture[1] ); //bind our texture to our shape
     glBegin (GL_QUADS);
         glTexCoord2f(0.0, 0.0); glVertex3f(-5.0,-5.0, -10.0);
          glTexCoord2f(0.0, 1.0); glVertex3f(5, -5, -10.0);
          glTexCoord2f(1.0, 1.0); glVertex3f(10, -10, 0.0);
          glTexCoord2f(1.0, 0.0); glVertex3f(-10, -10, 0.0);  
         
    glEnd();
    
    glPopMatrix();
}

void torso()
{
   glPushMatrix();
   glRotatef(-90.0, 1.0, 0.0, 0.0);
   gluCylinder(t,TORSO_RADIUS, TORSO_RADIUS*1.5, TORSO_HEIGHT,10,10);	//(*obj, base, top, height, slices, stacks)
   glPopMatrix();
}

void head()
{
   glPushMatrix();
   glTranslatef(0.0, 0.5*HEAD_HEIGHT,0.0);
   glScalef(HEAD_RADIUS, HEAD_HEIGHT, HEAD_RADIUS);
   gluSphere(h,1.0,10,10);
   glPopMatrix();
}

void glass_bot()
{
   glPushMatrix();
   glTranslatef(0.0, 0.5*HEAD_HEIGHT,0.075);
   glRotatef(-90.0, 1.0, 0.0, 0.0);
   
   gluCylinder(gl,HEAD_RADIUS, HEAD_RADIUS, HEAD_HEIGHT/2,10,10);
   glPopMatrix();
}

void shoulder_joints()
{
	glPushMatrix();
   	glScalef(SHOULDER_RADIUS, SHOULDER_RADIUS, SHOULDER_RADIUS);
   	gluSphere(h,1.0,10,10);
	glPopMatrix();
}

void elbow_joints()
{
	glPushMatrix();
   	glScalef(SHOULDER_RADIUS/1.2, SHOULDER_RADIUS/1.2, SHOULDER_RADIUS/1.2);
   	gluSphere(h,1.0,10,10);
	glPopMatrix();
}

void palms()
{
	glPushMatrix();
   	glScalef(SHOULDER_RADIUS/1.3, SHOULDER_RADIUS/1.3, SHOULDER_RADIUS/1.3);
   	gluSphere(h,1.0,10,10);
	glPopMatrix();
}

void leg_joints()
{
	glPushMatrix();
   	glScalef(JOINT_RADIUS, JOINT_RADIUS, JOINT_RADIUS);
   	gluSphere(h,1.0,10,10);
	glPopMatrix();
}

void knee_joints()
{
	glPushMatrix();
   	glScalef(JOINT_RADIUS, JOINT_RADIUS, JOINT_RADIUS);
   	gluSphere(h,1.0,10,10);
	glPopMatrix();
}

void torso_disk()
{
	glPushMatrix();
   	glScalef(1.5*TORSO_RADIUS, 0.1, 1.5*TORSO_RADIUS);
   	gluSphere(h,1.0,10,10);
	glPopMatrix();
}

void left_upper_arm()
{
   glPushMatrix();
   gluCylinder(lua,UPPER_ARM_RADIUS*1.2, UPPER_ARM_RADIUS, UPPER_ARM_HEIGHT,10,10);
   glPopMatrix();
}

void left_lower_arm()
{
   glPushMatrix();
   gluCylinder(lla,LOWER_ARM_RADIUS*1.1, LOWER_ARM_RADIUS, LOWER_ARM_HEIGHT,10,10);
   glPopMatrix();
}

void right_upper_arm()
{
   glPushMatrix();
   
   glRotatef(-90.0, 1.0, 0.0, 0.0);
   gluCylinder(rua,UPPER_ARM_RADIUS*1.2, UPPER_ARM_RADIUS, UPPER_ARM_HEIGHT,10,10);
   glPopMatrix();
}

void right_lower_arm()
{
   glPushMatrix();
   glRotatef(-90.0, 1.0, 0.0, 0.0);
   gluCylinder(rla,LOWER_ARM_RADIUS*1.1, LOWER_ARM_RADIUS, LOWER_ARM_HEIGHT,10,10);
   glPopMatrix();
}

void left_upper_leg()
{
   glColor3f(1.0,0.0,1.0);
   glPushMatrix();
   glRotatef(-120.0, 1.0, 0.0, 0.0);
   gluCylinder(lul,UPPER_LEG_RADIUS*1.2, UPPER_LEG_RADIUS, UPPER_LEG_HEIGHT,10,10);
   glPopMatrix();
}

void left_lower_leg()
{
   glColor3f(1.0,0.0,0.0);
   glPushMatrix();
   glTranslatef(0.0,-0.25,-UPPER_LEG_HEIGHT/2);
   glRotatef(-70.0, 1.0, 0.0, 0.0);
   gluCylinder(lll,LOWER_LEG_RADIUS, LOWER_LEG_RADIUS*1.5, LOWER_LEG_HEIGHT,10,10);
   glPopMatrix();
}

void right_upper_leg()
{
   glColor3f(1.0f,0.0f,1.0f);
   glPushMatrix();
   glRotatef(-120.0, 1.0, 0.0, 0.0);
   gluCylinder(rul,UPPER_LEG_RADIUS*1.2, UPPER_LEG_RADIUS, UPPER_LEG_HEIGHT,10,10);
   glPopMatrix();
}

void right_lower_leg()
{
   glColor3f(1.0,0.0,0.0);
   glPushMatrix();
   glTranslatef(0.0,-0.25,-UPPER_LEG_HEIGHT/2);
   glRotatef(-70.0, 1.0, 0.0, 0.0);
   gluCylinder(rll,LOWER_LEG_RADIUS, LOWER_LEG_RADIUS*1.5, LOWER_LEG_HEIGHT,10,10);
   glPopMatrix();
}

void display(void)
{
     GLfloat rot_x=0.0, rot_y=0.0;
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    glColor3f(0.0, 0.0, 0.0);
    
        glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
        glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
        glMaterialf(GL_FRONT, GL_SHININESS, mat_shininess);
        
    glEnable( GL_TEXTURE_2D );
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
    
    gluLookAt(0.0,0.0,8.0,0.0,0.0,0.0,0.0,1.0,0.0);
    glTranslatef(0.0, 0.0, 10);
    square();
     
    glDisable(GL_TEXTURE_2D);
     
    glTranslatef(0.0, -3.0, 0.0); 
    glRotatef(theta[0], 0.0, 1.0, 0.0);
    torso();
    
    glPushMatrix();
    glTranslatef(0.0, TORSO_HEIGHT+0.5*HEAD_HEIGHT, 0.0);
    glRotatef(theta[1], 1.0, 0.0, 0.0);
    glRotatef(theta[2], 0.0, 1.0, 0.0);
    glTranslatef(0.0, -0.5*HEAD_HEIGHT, 0.0);
    head();
    glass_bot();
    glPopMatrix();
    
    //right arm
    if(flag_dance==1)
    {
                  rot_x=1.0;
                  rot_y=0.0;              
    }
    if(flag_dance==2)
    {
                  rot_x=0.0;
                  rot_y=1.0;              
    }
    glPushMatrix();
    glTranslatef(-(TORSO_RADIUS+UPPER_ARM_RADIUS), 0.9*TORSO_HEIGHT, 0.0);
    glRotatef(theta[3], rot_x, rot_y, 0.0);
    left_upper_arm();
    
    //glTranslatef(0.0, UPPER_ARM_HEIGHT, 0.0);
    glTranslatef(0.0, 0.0, UPPER_ARM_HEIGHT);
    elbow_joints();
    glRotatef(theta[4], 1.0, 0.0, 0.0);
    left_lower_arm();
    
    glTranslatef(0.0, 0.0,LOWER_ARM_HEIGHT);
    palms(); //left hand
    
    glPopMatrix();
    
    //torso_disk
    glPushMatrix();
    glTranslatef(0.0, TORSO_HEIGHT, 0.0);
    torso_disk();
    glPopMatrix();
    
    //shoulder_joints
    glPushMatrix();
    glTranslatef(1.5*TORSO_RADIUS, 0.9*TORSO_HEIGHT, 0.0);
    shoulder_joints();
    
    glTranslatef(-3.0*TORSO_RADIUS, 0.0, 0.0);
    shoulder_joints();
    glPopMatrix();
    
    //leg_joints
    glPushMatrix();
    glTranslatef(1.1*TORSO_RADIUS, 0.0, 0.0);
    leg_joints();
    
    glTranslatef(-2.2*TORSO_RADIUS, 0.0, 0.0);
    shoulder_joints();
    glPopMatrix();
    
    //left arm
    glPushMatrix();
    glTranslatef(TORSO_RADIUS+UPPER_ARM_RADIUS, 0.9*TORSO_HEIGHT, 0.0);
    glRotatef(theta[5], 1.0, 0.0, 0.0);
    right_upper_arm();

    glTranslatef(0.0, UPPER_ARM_HEIGHT, 0.0);
    elbow_joints();
    glRotatef(theta[6], 1.0, 0.0, 0.0);
    glColor3f(1.0,1.0,1.0);
    right_lower_arm();
    
    glTranslatef(0.0, LOWER_ARM_HEIGHT, 0.0);
    palms(); //left hand
    glPopMatrix();

    glPushMatrix(); 
  	glTranslatef(-(TORSO_RADIUS), 0.1*UPPER_LEG_HEIGHT, 0.0);
    glRotatef(theta[7], 1.0, 0.0, 0.0);
    left_upper_leg();
    
    glTranslatef(0.0, UPPER_LEG_HEIGHT, -1.5);
    knee_joints();
    glTranslatef(0.0, 0.0, 1.5); 
     
    glRotatef(theta[8], 1.0, 0.0, 0.0);
    left_lower_leg();
    glPopMatrix();

    glPushMatrix();
    //glTranslatef(TORSO_RADIUS+UPPER_LEG_RADIUS, 0.1*UPPER_LEG_HEIGHT, 0.0);
    glTranslatef((TORSO_RADIUS), 0.1*UPPER_LEG_HEIGHT, 0.0);
    glRotatef(theta[9], 1.0, 0.0, 0.0);
    right_upper_leg();

    glTranslatef(0.0, UPPER_LEG_HEIGHT, -1.5);
     knee_joints();
     glTranslatef(0.0, 0.0, 1.5); 
     
    glRotatef(theta[10], 1.0, 0.0, 0.0);
    right_lower_leg();
    glPopMatrix();
	
    glFlush();
    glutSwapBuffers();
   // glDisable(GL_TEXTURE_2D);
}

void menu(int id)
{
   if(id <11 ) 
   		angle=id;
   if(id ==11 ) 
   		exit(0);
}

float getAngle(float freq, float min, float max, float t){
  return (max - min)*sin(freq*M_PI*t) + 0.5*(min + max);
}

static void idle(void){
  GLfloat seconds = glutGet(GLUT_ELAPSED_TIME)/1000.0;
  if(seconds >= 3.6) flag_dance =1;
  if(seconds >= 13.6) flag_dance =2;
  if(seconds >= 18.6) flag_dance =1;
  if(seconds>=3.6 && seconds<=9.6 || seconds > 13.6)
  {
  theta[3] = getAngle(theta_freq[3], theta_min[3],theta_max[3], seconds);
  theta[5] = getAngle(theta_freq[5], theta_min[5],theta_max[5], seconds);

  theta[7] = getAngle(theta_freq[7], theta_min[7],theta_max[7], seconds);  
  theta[9] = getAngle(theta_freq[9], theta_min[9],theta_max[9], seconds);
  }
 theta[1] = getAngle(theta_freq[1], theta_min[1],theta_max[1], seconds);
  
theta[4] = getAngle(theta_freq[4], theta_min[4],theta_max[4], seconds);
  theta[6] = getAngle(theta_freq[6], theta_min[6],theta_max[6], seconds);
  theta[0] = getAngle(theta_freq[0], theta_min[0],theta_max[0], seconds);
 
  srand(time(NULL));
  mat_x = rand()%2;
  mat_y = rand()%2;
  mat_z = rand()%2;
 
  light_flag = rand()%4;
  if(light_flag <1)
  {
        glEnable(GL_LIGHT0);
        glDisable(GL_LIGHT1);
        glDisable(GL_LIGHT2);
        glDisable(GL_LIGHT3);       
  }
  
  if(light_flag <2 && light_flag >=1)
  {
        glEnable(GL_LIGHT1);
        glDisable(GL_LIGHT0);
        glDisable(GL_LIGHT2);
        glDisable(GL_LIGHT3);       
  }
  
  if(light_flag <3 && light_flag >=2)
  {
        glEnable(GL_LIGHT2);
        glDisable(GL_LIGHT1);
        glDisable(GL_LIGHT0);
        glDisable(GL_LIGHT3);       
  }
  
  if(light_flag <4 && light_flag >=3)
  {
        glEnable(GL_LIGHT3);
        glDisable(GL_LIGHT1);
        glDisable(GL_LIGHT2);
        glDisable(GL_LIGHT0);       
  }
  
  mat_diffuse[0] = mat_x;
  mat_diffuse[1] = mat_y;
  mat_diffuse[2] = mat_z;
  
  //printf("\ntheta_max[3] = %f ; theta_max[5] = %f\n",theta_max[3],theta_max[5]);
  //printf("seconds = %f; theta_3 = %f ; theta_5 = %f\n",seconds, theta[3],theta[5]);
  
  glutPostRedisplay();
}

void myReshape(int w, int h)
{
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    
    glLoadIdentity();
    
    if (w <= h)
        glOrtho(-10.0, 10.0, -10.0 * (GLfloat) h / (GLfloat) w,
            10.0 * (GLfloat) h / (GLfloat) w, -10.0, 10.0);
    else
        glOrtho(-10.0 * (GLfloat) w / (GLfloat) h,
            10.0 * (GLfloat) w / (GLfloat) h, -10.0, 10.0, -10.0, 10.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
}

void myinit()
{
        //light 1
        GLfloat light_ambient1[]={0.0, 0.0, 0.0, 1.0};
        GLfloat light_diffuse1[]={1.0, 1.0, 1.0, 1.0};
        GLfloat light_specular1[]={1.0, 1.0, 1.0, 1.0};
        GLfloat light_position1[]={10.0, 10.0, 10.0, 0.0};

        glLightfv(GL_LIGHT0, GL_POSITION, light_position1);
        glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient1);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse1);
        glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular1);
        
        //light 2
        GLfloat light_ambient2[]={0.0, 0.0, 0.0, 1.0};
        GLfloat light_diffuse2[]={1.0, 1.0, 1.0, 1.0};
        GLfloat light_specular2[]={1.0, 1.0, 1.0, 1.0};
        GLfloat light_position2[]={10.0, 0.0, 10.0, 0.0};

        glLightfv(GL_LIGHT1, GL_POSITION, light_position2);
        glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambient2);
        glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse2);
        glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular2);

        //light 3
        GLfloat light_ambient3[]={0.0, 0.0, 0.0, 1.0};
        GLfloat light_diffuse3[]={1.0, 1.0, 1.0, 1.0};
        GLfloat light_specular3[]={1.0, 1.0, 1.0, 1.0};
        GLfloat light_position3[]={0.0, -10.0, 10.0, 0.0};

        glLightfv(GL_LIGHT2, GL_POSITION, light_position3);
        glLightfv(GL_LIGHT2, GL_AMBIENT, light_ambient3);
        glLightfv(GL_LIGHT2, GL_DIFFUSE, light_diffuse3);
        glLightfv(GL_LIGHT2, GL_SPECULAR, light_specular3);
        
        //light 4
        GLfloat light_ambient4[]={0.0, 0.0, 0.0, 1.0};
        GLfloat light_diffuse4[]={1.0, 1.0, 1.0, 1.0};
        GLfloat light_specular4[]={1.0, 1.0, 1.0, 1.0};
        GLfloat light_position4[]={-10.0, 10.0, 10.0, 0.0};

        glLightfv(GL_LIGHT3, GL_POSITION, light_position4);
        glLightfv(GL_LIGHT3, GL_AMBIENT, light_ambient4);
        glLightfv(GL_LIGHT3, GL_DIFFUSE, light_diffuse4);
        glLightfv(GL_LIGHT3, GL_SPECULAR, light_specular4);
        
        
        glShadeModel(GL_SMOOTH);
        glEnable(GL_LIGHTING);

        glDepthFunc(GL_LEQUAL);
        glEnable(GL_DEPTH_TEST);

        //glClearColor(0.75, 0.75, 0.75, 1.0);
        glClearColor(0.0, 0.0, 0.0, 1.0);
        glColor3f(0.0, 0.0, 0.0);

/*	Setting the min, max and frequency for body parts*/
		for(i=0;i<11;i++)
		{
			theta_min[i] = 0.0;
			theta_max[i] = 0.0;
			theta_freq[i] = 0.0;
		}
	
        theta_freq[1] = 3.0;
        theta_max[1] = 15.0; theta_min[1] = -5.0;
        
		theta_freq[3] = 3.0; theta_freq[5] = 3.0;
 	    theta_max[3] = -90.0; theta_max[5] = 90.0;
 	    
 	    theta_freq[4] = 2.0; theta_freq[6] = 2.0;
 	    theta_max[4] = -35.0; theta_max[6] = -35.0;
 	    theta_min[4] = -10.0; theta_min[6] = -10.0;	    
	
		theta_freq[7] = 3.0; theta_freq[9] = 3.0;
		
		theta_max[7] = 200.0; theta_max[9] = 160.0;
		theta_min[7] = 160.0; theta_min[9] = 200.0;
		
		theta_min[0] = -30; theta_freq[0] = 0.5;
		theta_max[0] = 30;
		
/* allocate quadrics with filled drawing style */

        h=gluNewQuadric();
        gluQuadricDrawStyle(h, GLU_FILL);
        gluQuadricNormals(h, GLU_SMOOTH);
        
        t=gluNewQuadric();
        gluQuadricDrawStyle(t, GLU_FILL);
        gluQuadricNormals(t, GLU_SMOOTH);
        
        gl=gluNewQuadric();
        gluQuadricDrawStyle(gl, GLU_FILL);
        gluQuadricNormals(gl, GLU_SMOOTH);
        
        lua=gluNewQuadric();
        gluQuadricDrawStyle(lua, GLU_FILL);
        gluQuadricNormals(lua, GLU_SMOOTH);
        
        lla=gluNewQuadric();
        gluQuadricDrawStyle(lla, GLU_FILL);
        gluQuadricNormals(lla, GLU_SMOOTH);
        
        rua=gluNewQuadric();
        gluQuadricDrawStyle(rua, GLU_FILL);
        gluQuadricNormals(rua, GLU_SMOOTH);
        
        rla=gluNewQuadric();
        gluQuadricDrawStyle(rla, GLU_FILL);
        gluQuadricNormals(rla, GLU_SMOOTH);
        
        lul=gluNewQuadric();
        gluQuadricDrawStyle(lul, GLU_FILL);
        gluQuadricNormals(lul, GLU_SMOOTH);
        
        lll=gluNewQuadric();
        gluQuadricDrawStyle(lll, GLU_FILL);
        gluQuadricNormals(lll, GLU_SMOOTH);
        
        rul=gluNewQuadric();
        gluQuadricDrawStyle(rul, GLU_FILL);
        gluQuadricNormals(rul, GLU_SMOOTH);
        
        rll=gluNewQuadric();
        gluQuadricDrawStyle(rll, GLU_FILL);
        gluQuadricNormals(rll, GLU_SMOOTH);
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(500, 500);
    glutCreateWindow("robot");
    PlaySound("bird_FINAL.wav", NULL, SND_ASYNC|SND_FILENAME|SND_LOOP );
       
    myinit();
    glutReshapeFunc(myReshape);
    glutDisplayFunc(display);
	glutIdleFunc(idle);
	
    glutCreateMenu(menu);
    glutAddMenuEntry("torso", 0);
    glutAddMenuEntry("head1", 1);
    glutAddMenuEntry("head2", 2);
    glutAddMenuEntry("right_upper_arm", 3);
    glutAddMenuEntry("right_lower_arm", 4);
    glutAddMenuEntry("left_upper_arm", 5);
    glutAddMenuEntry("left_lower_arm", 6);
    glutAddMenuEntry("right_upper_leg", 7);
    glutAddMenuEntry("right_lower_leg", 8);
    glutAddMenuEntry("left_upper_leg", 9);
    glutAddMenuEntry("left_lower_leg", 10);
    glutAddMenuEntry("quit", 11);
    glutAttachMenu(GLUT_MIDDLE_BUTTON);

    //Load our texture
    texture[3] = LoadTexture( "tex3df.bmp", 640, 480 );      //back
    texture[2] = LoadTexture( "tex3df.bmp", 640, 480 );      //floor
    texture[1] = LoadTexture( "books.bmp", 422, 220 );      //left
    texture[0] = LoadTexture( "MJ.bmp", 400, 316 );      //right
    
    glutMainLoop();
    return 0;
}
