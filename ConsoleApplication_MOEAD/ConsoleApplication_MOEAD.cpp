#include <windows.h>
#include <stdio.h>
#include <glut.h>
#include <GLFW/glfw3.h>

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
void reshape_main(int w, int h);
void display_main();//Eigen::MatrixXf);
void display_Optimization_animation();
void display_final_EP(void);
void animate(int value);
void mouse(int button, int state, int x, int y);
void motion(int x, int y);
void renderBitmapString(float x, float y, void* font, string str);
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

int Set_iter =10;//1000
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

/*
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
*/

/**---------------- main function  -------------------*/

int main(int argc, char** argv)
{
  
    // get iteration first
    cout << "input Max interation, 10~1000:" << endl;
    cin >> Set_iter;
    if (Set_iter > 1000) {
        cout << "value exceed 1000, optimize with iter=1000." << endl;
        Set_iter = 1000;
    }
    else if (Set_iter < 10) {
        cout << "value less than 10, optimize with iter=10." << endl;
        Set_iter = 10;
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
    main_window = glutCreateWindow("MOEAD_DTLZ1_Display");
    init();
    
    glutDisplayFunc(display_main);
    glutReshapeFunc(reshape_main);

    command = glutCreateSubWindow(main_window, 500, 5, 240, 490);
    glutReshapeFunc(command_reshape);
    glutDisplayFunc(display_final_EP);// display_final_EP);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
        

    screen = glutCreateSubWindow(main_window, 5, 5, 490, 490);
    glutDisplayFunc(display_Optimization_animation);
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

}


void command_reshape(int w, int h)
{
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60, 1.0, 1.5, 20);
    glMatrixMode(GL_MODELVIEW);
   
}


void reshape_main(int w, int h)
{
    glViewport(0, 0, w, h);
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

class Draw_solution_base {
public:

    virtual void draw_solutions_points() = 0;

    void draw_everything() {
            draw_solutions_points();
            draw_coordinate();
            draw_reference_points();
    }
    void show_sol() {
        vector<float> Max_val_dim = { Matrix_Cost.col(0).maxCoeff(),
                                Matrix_Cost.col(1).maxCoeff(),
                                Matrix_Cost.col(2).maxCoeff() };
        std::cout << Max_val_dim[0] << ", " << Max_val_dim[1] << ", " << Max_val_dim[2] << endl;
    }

    void draw_coordinate() {
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

        glColor3f(0.9, 0.9, 0.9); //frame & grid
        glLineWidth(1.0f);
        glBegin(GL_LINES);
        for (int i = 10; i <= axis_max; i += 10) {
            glVertex3f(i, 0.0f, 0.0f);
            glVertex3f(i, 0.0f, axis_max);//200 Max_val_dim[2]
        }
        for (int i = 10; i <= axis_max; i += 10) {
            glVertex3f(i, 0.0f, 0.0f);
            glVertex3f(i, axis_max, 0.0f);//200 Max_val_dim[2]
        }
        for (int i = 10; i <= axis_max; i += 10) {
            glVertex3f(0.0f, i, 0.0f);
            glVertex3f(0.0f, i, axis_max);//200 Max_val_dim[2]
        }
        for (int i = 10; i <= axis_max; i += 10) {
            glVertex3f(0.0f, i, 0.0f);
            glVertex3f(axis_max, i, 0.0f);//200 Max_val_dim[2]
        }
        for (int i = 10; i <= axis_max; i += 10) {
            glVertex3f(0.0f, 0.0f, i);
            glVertex3f(axis_max, 0.0f, i);//200 Max_val_dim[2]
        }
        for (int i = 10; i <= axis_max; i += 10) {
            glVertex3f(0.0f, 0.0f, i);
            glVertex3f(0.0f, axis_max, i);//200 Max_val_dim[2]
        }
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

    void draw_reference_points() {
        glPointSize(4);
        glBegin(GL_POINTS);
        glColor3f(1, 0.5, 1);
        for (int i = 0; i < 2500; ++i)
        {
            glVertex3f(ref_pt_matrix.row(i)[0], ref_pt_matrix.row(i)[1], ref_pt_matrix.row(i)[2]);
        }
        glColor3f(0.5, 0.0, 0.5);//origin
        glVertex3f(0.5, 0.5, 0.5);
        glVertex3f(0, 0, 0);
        glEnd();
    }
};

class Draw_solution_final: public Draw_solution_base {
public:
    
    void draw_solutions_points() override {
        Eigen::Matrix<float, Eigen::Dynamic, 3> Final_Cost;
        Final_Cost = Storage_Cost[Set_iter-1];

        glPointSize(5);
        glBegin(GL_POINTS);
        glColor3f(0.9, 0.5f, 0.0f);
        for (int i = 0; i < Final_Cost.rows(); ++i)
        {
            glVertex3f(Final_Cost.row(i)[0], Final_Cost.row(i)[1], Final_Cost.row(i)[2]);
            std::cout << i << "-x_" << Final_Cost.row(i)[0] << " y_" << Final_Cost.row(i)[1] << " z_ " << Final_Cost.row(i)[2] << endl;
        }
        glEnd();

    }
};

class Draw_solution_loop : public Draw_solution_base {
    void draw_solutions_points() override {
        //ObjectiveValue of every iteration of Elite Population
        glPointSize(5);
        glBegin(GL_POINTS);
        glColor3f(0.9, 0.5f, 0.0f);
        for (int i = 0; i < Matrix_Cost.rows(); ++i)
        {
            glVertex3f(Matrix_Cost.row(i)[0], Matrix_Cost.row(i)[1], Matrix_Cost.row(i)[2]);
            std::cout << i << "-x_" << Matrix_Cost.row(i)[0] << " y_" << Matrix_Cost.row(i)[1] << " z_ " << Matrix_Cost.row(i)[2] << endl;
        }
        glEnd();
    }


};

class Display_Base {
public:

    void Window_Set_Base() {
        glClear(GL_COLOR_BUFFER_BIT);

        int rect[4];
        float w, h;

        glGetIntegerv(GL_VIEWPORT, rect);//get width and height of window
        w = rect[2];
        h = rect[3];

        glClearColor(0.7f, 0.7f, 0.7f, 0.0f);
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
    }

    void Text_Print(float X_position, float Y_position , void *font ,std::string TargetText) {
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        gluOrtho2D(0, glutGet(GLUT_WINDOW_WIDTH), 0, glutGet(GLUT_WINDOW_HEIGHT)); // 使用正交投影
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();

        glColor3f(1.0f, 1.0f, 1.0f);
        //std::string str_curr = TargetText + std::to_string(Set_iter);

        renderBitmapString(X_position, Y_position, font, TargetText);
    }
};

void display_main()//Eigen::MatrixXf Matrix_Cost)
{
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(0.7f, 0.7f, 0.7f);
    glutSwapBuffers();
}

void display_Optimization_animation() {
    Display_Base Display;
    Display.Window_Set_Base();//3D projection for solutions drawing

    Draw_solution_loop draw_sol_loop;
    draw_sol_loop.draw_everything();

    std::string str_current_iter = "current iteration/ Max iteration: " + std::to_string(currentPeriod) + "/" + std::to_string(Set_iter);
    Display.Text_Print(g_fWidth / 10, g_fHeight - 50, GLUT_BITMAP_HELVETICA_18, str_current_iter);//2D projection & text drawing

    glFlush();
    glutSwapBuffers();
}

void display_final_EP() {
    Display_Base Display;
    Display.Window_Set_Base();

    Draw_solution_final draw_sol_final;
    draw_sol_final.draw_everything();

    std::string str_final = "Best solutions in Max iteration: " + std::to_string(Set_iter);
    std::string str_final2 = "Total solutions: " + std::to_string(Storage_Cost[Set_iter - 1].rows());

    Display.Text_Print(g_fWidth / 10, g_fHeight - 50, GLUT_BITMAP_HELVETICA_18, str_final);
    Display.Text_Print(g_fWidth / 10, g_fHeight - 80, GLUT_BITMAP_HELVETICA_18, str_final2);

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

    ofstream file2;
    file2.open("BestEP_MOEAD.csv", ios::out | ios::trunc);
    //file2.trunc;
    if (file2) {

        for (int ww = 0; ww < Storage_Cost[Set_iter-1].rows(); ++ww) {
            for (int qq = 0; qq < Storage_Cost[Set_iter - 1].cols(); ++qq) {
                file2 << Storage_Cost[Set_iter - 1].row(ww).col(qq) << ",";
            }
            file2 << endl;
        }
    }
    file2.close();
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
