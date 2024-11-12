#include <windows.h>
#include <stdio.h>
#include <glut.h>

#include <iostream>
#include <fstream>
#include <conio.h>
using namespace std;

/**
https://zouzhongliang.com/index.php/2019/09/19/openglhuasanweizuobiaozhou/
**/
void init(void);
void reshape(int w, int h);
void mouse(int button, int state, int x, int y);
void motion(int x, int y);
void display(void);
void drawCoordinates(void);
void Read_Ref_Pt(float* p, int col, int row);

int mx, my; //position of mouse;
float x_angle, y_angle; //angle of eye

void init(void)
{
}

void reshape(int w, int h)
{
    glViewport(0, 0, w, h);
}

void mouse(int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
    {
        mx = x;
        my = y;
    }
}

void motion(int x, int y)
{
    int dx, dy; //offset of mouse;

    dx = x - mx;
    dy = y - my;

    y_angle += dx * 0.01f;
    x_angle += dy * 0.01f;

    mx = x;
    my = y;

    glutPostRedisplay();
}

void display(void)
{
    int rect[4];
    float w, h;

    glGetIntegerv(GL_VIEWPORT, rect);
    w = rect[2];
    h = rect[3];

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    if (w > h)
        glOrtho(-w / h, w / h, -1.0f, 1.0f, -1.0f, 1.0f);
    else
        glOrtho(-1.0f, 1.0f, -h / w, h / w, -1.0f, 1.0f);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glRotatef(x_angle, 1.0f, 0.0f, 0.0f);
    glRotatef(y_angle, 0.0f, 1.0f, 0.0f);

    drawCoordinates();

    glFlush();
    glutSwapBuffers();
}

void drawCoordinates(void)
{


    float ref_matrix[2500][3];
    float* p = &ref_matrix[0][0];
    Read_Ref_Pt(p, 3, 2500);//read reference pt.


    //    glPointSize(5);
    glBegin(GL_POINTS);
    glColor3f(0.5, 0.5, 0.0); //extra pt
    for (int i = 0; i < 2500; ++i) {

        glVertex3f(ref_matrix[i][0], ref_matrix[i][1], ref_matrix[i][2]);
    }
    glColor3f(0.5, 0.0, 0.5);
    glVertex3f(0.5, 0.5, 0.5);
    glEnd();


    glLineWidth(3.0f);
    glColor3f(1.0f, 0.0f, 0.0f); //red x axis
    glBegin(GL_LINES);
    glVertex3f(0.0f, 0.0f, 0.0f);//draw points
    glVertex3f(1.0f, 0.0f, 0.0f);
    glEnd();

    glColor3f(0.0, 1.0, 0.0); //green y axis
    glBegin(GL_LINES);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 1.0f, 0.0f);
    glEnd();
    glColor3f(0.0, 0.0, 1.0); //blue z axis
    glBegin(GL_LINES);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, 1.0f);
    glEnd();

}




void Read_Ref_Pt(float* p, int col, int row)
{
    ifstream file2("DTLZ1ReferencePoints.dat");
    if (!file2)
    {
        cout << "file can't open" << endl;
    }
    else
    {
        for (int i = 0; i < row; ++i)
        {
            for (int j = 0; j < col; ++j)
            {
                file2 >> *(p + (i * col) + j);
            }
            //            printf("%d : %f, %f, %f  \n",i,*(p+(i*col)), *(p+(i*col)+1), *(p+(i*col)+2) );
        }

        //            cout<<ref_matrix[i][0]<<","<<ref_matrix[i][1]<<","<<ref_matrix[i][2]<<endl;


        cout << "data read finished" << endl;

        file2.close();
    }
}





int main(int argc, char** argv)
{
    drawCoordinates();
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(500, 500);
    glutInitWindowPosition(0, 0);
    glutCreateWindow("gl_1_2");
    init();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glutMainLoop();

    return 0;
}
