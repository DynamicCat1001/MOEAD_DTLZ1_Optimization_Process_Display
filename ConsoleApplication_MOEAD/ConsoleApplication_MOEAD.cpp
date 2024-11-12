
#include <windows.h>
#include <stdio.h>
#include <glut.h>


#include <iostream>
#include <fstream>
#include "gnuplot-iostream.h"


#include <conio.h>
#include <stdarg.h>

#include <vector>
#include <Eigen/Dense>
#include "DTLZ1_Para.h"
#include "DTLZ1_function.h"
#include "MOEAD_function.h"
#include "Read_Ref_Pt.h"
#include "plot_EigenMatToVec.h"
#include<numeric>


using namespace std;


GLvoid* font_style = GLUT_BITMAP_TIMES_ROMAN_10;

/**---------------- function pre-define -------------------*/
void init(void);

void command_reshape(int width, int height);
void command_display();


void reshape_2w(int w, int h);
//void reshape(int w, int h);
//void keyboard(unsigned char key, int x, int y);

void mouse(int button, int state, int x, int y);
void mouse_button(int button, int state, int x, int y);
void motion(int x, int y);
void button_display(void);
void renderBitmapString(float x, float y, void* font, string str);
void display_main();//Eigen::MatrixXf);
void display_2w();
void animate(int value);
void drawCoordinates();//Eigen::MatrixXf);


Eigen::MatrixXf DTLZ1_EX(void);


/**---------------- variable define -------------------*/

int mx, my; //position of mouse;
float x_angle = 30, y_angle = 30; //angle of eye

/**-----------------animation parameter----------*/
int currentPeriod = 0;


#define GAP  5
GLuint main_window, screen, command;
GLuint sub_width = 500, sub_height = 500;
GLint selection = 0;

int Set_iter = 300;//1000
int Set_iter_step;//Set_iter/10;
Eigen::Matrix<float, Eigen::Dynamic, 3> Matrix_Cost;
vector<Eigen::Matrix<float, Eigen::Dynamic, 3>> Storage_Cost;
Eigen::MatrixXf ref_pt_matrix(2500, 3);


#define MAX_COMMAND_LENGTH 100
char commandBuffer[MAX_COMMAND_LENGTH];
int commandLength = 0;

/**---------------- button struct  -------------------*/
//global
float g_fWidth = sub_width;//250
float g_fHeight = sub_height;//500
float g_fDepth = 100;
float g_fAngle = .0;//angle of cube

struct Button
{
    float m_fPosX;		//表示在正交投影坐标系(左下角为坐标原点)的坐标&#xff0c;
    float m_fPosY;
    float m_fWidth;		//屏幕像素单位
    float m_fHeight;


    bool m_bPressed;

    Button() :m_fPosX(0), m_fPosY(0), m_fWidth(0), m_fHeight(0), m_bPressed(false) {};

    void Render()
    {
        glPushMatrix();
        {
            //将中心位于原点的button cube移动到使cube左下角坐标为m_fPosX,m_fPosY的位置
            //必须考虑cube的自身长宽
            glTranslatef(m_fPosX + m_fWidth / 2, m_fPosY + m_fHeight / 2, -2.0);	//-2.0只是为了按钮可见
            if (m_bPressed)
            {
                //double scaleAmt &#61; 10.0 * sin( (double)rand() );
                //double scaleAmt &#61; sin( (double)rand() );
                glScalef(0.9, 0.9, 1.0);

                //                Set_iter=1000;//can't directly change data, the memory go wrong

            }
            //cube中心位于原点
            glScalef(m_fWidth, m_fHeight, 5.0);
            glutSolidCube(1.0);
        }
        glPopMatrix();
    }
    bool OnMouseDown(int mousex, int mousey)
    {
        //鼠标的位置:mousex,mousey坐标系是原点位于左上角
        //必须将mousey变换到原点位于左下角的坐标系中
        mousey = g_fHeight - mousey;
        if (mousex > m_fPosX && mousex < m_fPosX + m_fWidth && mousey > m_fPosY && mousey < m_fPosY + m_fHeight)
        {
            printf("button is pressed .... \n");
            m_bPressed = true;

            return true;
        }
        return false;
    }
    void OnMouseUp()
    {
        m_bPressed = false;
    }
};

Button* pBtn;//pre define to be global use

/**---------------- main function  -------------------*/

int main(int argc, char** argv)
{

    //    DTLZ1_EX();
    //    Matrix_Cost=Storage_Cost[0];
    //    ref_pt_matrix=Read_file(2500,3);
    
    cout << "input Max interation, 10~1000:" << endl;
    cin >> Set_iter;
    if (Set_iter > 1000) {
        cout << "value exceed 1000, optimize with iter=1000." << endl;
        Set_iter = 1000;
    }
    else {
        cout << "Max iteration: " << Set_iter << endl;
    }
    Set_iter_step = Set_iter / 10;
    
    DTLZ1_EX();
    Matrix_Cost = Storage_Cost[0];
    ref_pt_matrix = Read_file(2500, 3);
    
    


    
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(1000, 500);
    glutInitWindowPosition(0, 0);
    main_window = glutCreateWindow("Coordinate_Display");
    init();

    glutDisplayFunc(display_main);
    glutReshapeFunc(reshape_2w);

    command = glutCreateSubWindow(main_window, 500, 5, 240, 490);
    glutReshapeFunc(command_reshape);
    glutDisplayFunc(button_display);
    glutMouseFunc(mouse_button);
    //    glutMainLoop();
        // get iteration first

    DTLZ1_EX();
    Matrix_Cost = Storage_Cost[0];
    ref_pt_matrix = Read_file(2500, 3);

    screen = glutCreateSubWindow(main_window, 5, 5, 490, 490);
    glutDisplayFunc(display_2w);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);

    glutTimerFunc(1000, animate, 0);

    glutMainLoop();
    
    return 0;
}

void init(void)
{
    glClearColor(0.1, 0.1, 0.1, 0.1);//視窗框線
    glShadeModel(GL_SMOOTH);

    pBtn = new Button;
    pBtn->m_bPressed = false;
    pBtn->m_fPosX = 150;//g_fWidth/2
    pBtn->m_fPosY = 300;//490/2-25         g_fHeight*0.9
    pBtn->m_fWidth = 40;
    pBtn->m_fHeight = 25;

    std::cout << "button pos:" << pBtn->m_fPosX << " , " << pBtn->m_fPosY << endl;
}



void command_reshape(int w, int h)
{
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60, 1.0, 1.5, 20);
    glMatrixMode(GL_MODELVIEW);
    //    glClearColor(0.2, 0.2, 0.0, 0.0);//subwindow background color
}



void reshape_2w(int w, int h)
{
    glViewport(0, 0, w, h);
    //    glMatrixMode(GL_PROJECTION);//後續矩陣作業目標的矩陣堆疊，套用至投影矩陣堆疊
    //    glLoadIdentity();
    //    gluOrtho2D(0, w, h, 0);
    //    glMatrixMode(GL_MODELVIEW);
    //    glLoadIdentity();
    sub_width = (w - GAP * 3) / 2.0;
    sub_height = h - GAP * 3;//(h-GAP*3)/2.0;


    if (w > 50)
    {
        sub_width = (w - 3 * GAP) / 2;
    }
    else
    {
        sub_width = 10;
    }
    if (h > 50)
    {
        sub_height = h - GAP * 3;//(h - 3 * GAP) / 2;
    }
    else
    {
        sub_height = 10;
    }



    glutSetWindow(screen);
    glutPositionWindow(GAP, GAP);
    glutReshapeWindow(sub_width, sub_height);

    glutSetWindow(command);
    glutPositionWindow(GAP * 2 + sub_width, GAP);
    glutReshapeWindow(sub_width, sub_height);

}


void mouse_button(int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON)
        switch (state)
        {
        case GLUT_DOWN:
            //左键按下:
            printf("Mouse pos : %d\t%d\n", x, 500 - y);
            if (pBtn->OnMouseDown(x, y))
            {
                g_fAngle += 2.0;
                if (g_fAngle > 360)
                {
                    g_fAngle -= 360;
                }
            }
            break;

        case GLUT_UP:
            pBtn->OnMouseUp();
            break;
        }
    glutPostRedisplay();
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

    y_angle += dx * 0.1f;
    x_angle += dy * 0.1f;

    mx = x;
    my = y;

    glutPostRedisplay();
}

void button_display(void)
{
    glClear(GL_COLOR_BUFFER_BIT);

    // print buttom & cube
    glColor3f(0, 0.5, 0.7);
    glMatrixMode(GL_PROJECTION);
    {
        glLoadIdentity();
        glOrtho(0, g_fWidth, 0, g_fHeight, 0, g_fDepth);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        pBtn->Render();
    }

    // 绘制cube物体,

    glColor3f(1, 0.5, 0.5);
    glMatrixMode(GL_PROJECTION);		//回复原有的设置
    {
        glLoadIdentity();
        gluPerspective(60, 1.0, 1.5, 20);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        // viewing transformation
        gluLookAt(0.0, 0.0, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
        glRotatef(g_fAngle, 0.0, 1.0, 0.0);
        glScalef(1.0, 2.0, 1.0);      // modeling transformation
        glutWireCube(1.0);
    }


    //print buttom text

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, glutGet(GLUT_WINDOW_WIDTH), 0, glutGet(GLUT_WINDOW_HEIGHT)); // 使用正交投影
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glColor3f(1.0f, 1.0f, 1.0f);
    std::string str_curr = "current Max iteration " + std::to_string(Set_iter);
    std::vector<string> str_iter = { "iteration option", "1000","100","10" };
    //    const char *str_a=str_curr.c_str();

    renderBitmapString(g_fWidth / 10, g_fHeight - 50, GLUT_BITMAP_HELVETICA_18, str_curr);
    for (int i = 0; i < (str_iter.end() - str_iter.begin()); ++i) {
        renderBitmapString(g_fWidth / 10, g_fHeight - (50 + 50 * (i + 1)), GLUT_BITMAP_HELVETICA_18, str_iter[i]);
    }
    //    renderBitmapString(g_fWidth/10, g_fHeight-70,GLUT_BITMAP_HELVETICA_18,str_iter[0]);
    //    renderBitmapString(g_fWidth/10, g_fHeight-100,GLUT_BITMAP_HELVETICA_18,str_iter[1]);
    //



    glutSwapBuffers();
}

void command_display(void)
{
    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, glutGet(GLUT_WINDOW_WIDTH), 0, glutGet(GLUT_WINDOW_HEIGHT));
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Draw command window
//    glColor3f(0.0f, 0.0f, 0.0f);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glRasterPos2i(10, glutGet(GLUT_WINDOW_HEIGHT) - 20);


    glutSwapBuffers();


}

void display_main()//Eigen::MatrixXf Matrix_Cost)
{
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(1.0f, 1.0f, 1.0f);
    glutSwapBuffers();
}

void display_2w()//Eigen::MatrixXf Matrix_Cost)
{
    int rect[4];
    float w, h;

    glGetIntegerv(GL_VIEWPORT, rect);
    w = rect[2];
    h = rect[3];

    glClearColor(0.1f, 0.1f, 0.1f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    float U_window_size = 100.0f;
    float L_window_size = U_window_size;

    if (w > h)
        glOrtho(-L_window_size, U_window_size, -L_window_size, U_window_size, -L_window_size, U_window_size);//(-w / h, w / h, -1.0f, 1.0f, -1.0f, 1.0f);
    else
        glOrtho(-L_window_size, U_window_size, -L_window_size, U_window_size, -L_window_size, U_window_size);//(-1.0f, 1.0f, -h / w, h / w, -1.0f, 1.0f);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glRotatef(x_angle, 1.0f, 0.0f, 0.0f);
    glRotatef(y_angle, 0.0f, 1.0f, 0.0f);

    drawCoordinates();//Matrix_Cost);


    // print current iteration.
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, glutGet(GLUT_WINDOW_WIDTH), 0, glutGet(GLUT_WINDOW_HEIGHT)); // 使用正交投影
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();


    glColor3f(1.0f, 1.0f, 1.0f);
    std::string str_2 = "current iteration/ Max iteration: " + std::to_string(currentPeriod) + "/" + std::to_string(Set_iter);
    const char* str_a = str_2.c_str();

    renderBitmapString(g_fWidth / 10, g_fHeight - 50, GLUT_BITMAP_HELVETICA_18, str_a);


    glFlush();
    glutSwapBuffers();
}

void animate(int value)
{

    //    int numPoints = 450;
    int numPeriods = Set_iter;
    // 在下一个时期
    currentPeriod = (currentPeriod + Set_iter_step) % numPeriods;//0 20 40 60 100

    Matrix_Cost = Storage_Cost[currentPeriod];
    // 触发重新绘制 trigger re-drawing
    glutPostRedisplay();

    // 重新设置定时器 timer reset
    glutTimerFunc(1000 / 60, animate, 0); // 1000/60 = 60 fps
}



void drawCoordinates()//Eigen::MatrixXf Matrix_Cost);
{

    vector<float> Max_val_dim = { Matrix_Cost.col(0).maxCoeff(),
                              Matrix_Cost.col(1).maxCoeff(),
                              Matrix_Cost.col(2).maxCoeff()
    };
    std::cout << Max_val_dim[0] << ", " << Max_val_dim[1] << ", " << Max_val_dim[2] << endl;

    glPointSize(4);
    glBegin(GL_POINTS);
    glColor3f(1, 0.5, 1);//reference points
    for (int i = 0; i < 2500; ++i)
    {
        glVertex3f(ref_pt_matrix.row(i)[0], ref_pt_matrix.row(i)[1], ref_pt_matrix.row(i)[2]);
    }
    glColor3f(0.5, 0.0, 0.5);//origin
    glVertex3f(0.5, 0.5, 0.5);
    glVertex3f(0, 0, 0);
    glEnd();


    glPointSize(3);
    glBegin(GL_POINTS);
    glColor3f(0.9, 0.5f, 0.0f);//Cost of repository
    for (int i = 0; i < Matrix_Cost.rows(); ++i)
    {
        glVertex3f(Matrix_Cost.row(i)[0], Matrix_Cost.row(i)[1], Matrix_Cost.row(i)[2]);
        std::cout << i << "-x_" << Matrix_Cost.row(i)[0] << " y_" << Matrix_Cost.row(i)[1] << " z_ " << Matrix_Cost.row(i)[2] << endl;
    }
    glEnd();


    int axis_max = 100;
    glLineWidth(3.0f);
    glColor3f(0.8f, 0.0f, 0.0f); //red x axis
    glBegin(GL_LINES);
    glVertex3f(0.0f, 0.0f, 0.0f);//draw points
    glVertex3f(axis_max, 0.0f, 0.0f);//200 Max_val_dim[0]
    glEnd();

    glColor3f(0.0, 0.8, 0.0); //green y axis
    glBegin(GL_LINES);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, axis_max, 0.0f);//200 Max_val_dim[1]
    glEnd();
    glColor3f(0.0, 0.0, 0.8); //blue z axis
    glBegin(GL_LINES);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, axis_max);//200 Max_val_dim[2]
    glEnd();


    int scale_ii = 0;
    glPointSize(3);
    glBegin(GL_POINTS);
    glColor3f(0.0, 0.0, 0.0); //mark for every scales
    for (int ii = 1; ii < 10; ii++) {
        scale_ii = ii * 10.0f;
        glVertex3f(scale_ii, 0.0f, 0.0f);
        glVertex3i(0.0f, scale_ii, 0.0f);
        glVertex3i(0.0f, 0.0f, scale_ii);
    }
    glEnd();



}

void renderBitmapString(float x, float y, void* font, string str)
{
    glRasterPos2f(x, y);
    for (string::iterator c = (&str)->begin(); c != (&str)->end(); ++c)
    {
        glutBitmapCharacter(font, *c);
    }
}

Eigen::MatrixXf DTLZ1_EX(void)
{

    DTLZ1_Para_F dtlz1_para;
    //    int objfun_dim = dtlz1_para.objfun_dim;//7
    //    int nobjfun = dtlz1_para.nobjfun;//3
    MatrixXf searchspaceUB = dtlz1_para.searchspaceUB;
    MatrixXf searchspaceLB = dtlz1_para.searchspaceLB;


    /**Algorithm Parameters*/
    dtlz1_para.MaxIt = Set_iter;//300
    dtlz1_para.nPop = 100;//Population Size 100
    dtlz1_para.nbox = 25;
    dtlz1_para.nRep = round(1.5 * (pow(dtlz1_para.nbox, dtlz1_para.nobjfun - 1)) / (factorial(dtlz1_para.nobjfun - 1)));// 400 Repository Size factorial:5!=120


    dtlz1_para.Position = MatrixXf::Constant(dtlz1_para.nPop, dtlz1_para.objfun_dim, 0);

    Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic> obtained_cost_MOEAD;
    Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic> obtained_position_MOEAD;

    //    for (int i=0; i<dtlz1_para.MaxIt; ++i)//dtlz1_para.MaxIt
    //    {


    for (int j = 0; j < dtlz1_para.nPop; ++j)//dtlz1_para.nPop
    {
        dtlz1_para.Position.row(j) = dtlz1_para.searchspaceLB.array() + (dtlz1_para.searchspaceUB.array() - dtlz1_para.searchspaceLB.array()) * unifrnd(0, 1, dtlz1_para.objfun_dim).array();

        //            cout<<j<<"_"<<dtlz1_para.Position.row(i)<<endl;
    }


    /**Algorithm execution*/

//        std::vector<empty_individual_class> rep;
//        rep=MOEAD_function(dtlz1_para);
    Storage_Cost = MOEAD_function(dtlz1_para);

    /*
    int rep_iter_size=Storage_Cost[dtlz1_para.MaxIt].

                      //cout<< rep.size()<<endl;
                      obtained_cost_MOEAD.resize(rep_iter_size,nobjfun);
    obtained_position_MOEAD.resize(rep_iter_size, objfun_dim);
    for(int i_rep=0; i_rep<rep_iter_size; ++i_rep)
    {
        obtained_cost_MOEAD.row(i_rep)=rep[i_rep].Cost;
    //            obtained_position_MOEAD.row(i_rep)=rep[i_rep].Position;
    //            cout<<"pos_"<<obtained_position_MOEAD.row(i_rep)<<"_cost_"<<obtained_cost_MOEAD.row(i_rep)[0]<<obtained_cost_MOEAD.row(i_rep)[1]<<obtained_cost_MOEAD.row(i_rep)[2]<<endl;
    }
    */
    //        Storage_Cost.push_back(obtained_cost_MOEAD);

    //    }

    return obtained_cost_MOEAD;

}
