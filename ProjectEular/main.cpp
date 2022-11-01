#include<iostream>
#include<windows.h>
#include <stdio.h>
#include<GL/gl.h>
#include<GL/glu.h>
#include<GL/glut.h>
#include"EularStructure.h"
#include"eular.h"
#include"../GL/glh_convenience.h"
#include "../GL/glh_glut.h"
using namespace glh;
glut_simple_mouse_interactor object;

using namespace std;
Solid* mySolid;
static GLfloat xRot = 0.0f;
static GLfloat yRot = 0.0f;
static GLfloat zRot = 0.0f;
static bool isopen = false;
GLfloat oldx, oldy;
int color_faces[1000][3];
int ids = 0;
bool b[256];
static int level = 1;
float lightpos[4] = { 13, 10.2, 3.2, 0 };
void initSetting()
{
	b['9'] = false;
}
void CALLBACK beginCallback(GLenum which)
{
	glBegin(which);
}
void CALLBACK errorCallback(GLenum errorCode)
{
	const GLubyte* estring;
	estring = gluErrorString(errorCode);
	fprintf(stderr, "Tessellation Error: %s\n", estring);
	exit(0);
}
void CALLBACK endCallback(void)
{
	glEnd();
}
void CALLBACK vertexCallback(GLvoid* vertex)
{
	const GLdouble* pointer;
	pointer = (GLdouble*)vertex;
	glVertex3dv(pointer);
}
//如果窗口大小发生变化就自适应地调整
void ChangeSize(int w, int h)
{
	if (h == 0) h = 1;

	glViewport(0, 0, w, h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	GLfloat Range = 150.0f;
	//使用glOrtho函数可以将当前的可视空间设置为正投影空间
	if (w <= h)
		glOrtho(-Range, Range, -Range * h / w, Range * h / w, -Range, Range);
	else
		glOrtho(-Range * w / h, Range * w / h, -Range, Range, -Range, Range);

	object.reshape(w, h);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}
void key_fem(unsigned char k, int x, int y)
{
	b[k] = !b[k];
	object.keyboard(k, x, y);
	glutPostRedisplay();
}
GLUtesselator* obj = gluNewTess();
void mouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		oldx = x;
		oldy = y;
	}
	//object.mouse(button, state, x, y);
}
void motion(int x, int y)
{
	GLint deltax = oldx - x;
	GLint deltay = oldy - y;
	yRot += 360 * (GLfloat)deltax/ (GLfloat)1080;//根据屏幕上鼠标滑动的距离来设置旋转的角度
	xRot += 360 * (GLfloat)deltay/ (GLfloat)800;
	oldx = x;
	oldy = y;
	glutPostRedisplay();
	//object.motion(x, y);
}
//按数据结构读取欧拉操作产生的体并形成可以渲染的实体
void show(Solid* solid)
{
	Face* face = solid->face;
	int index = 0;
	//依次读取体中的面
	while (face)
	{
		//利用多面体绘制来对图形的边和面进行渲染
		gluTessBeginPolygon(obj, NULL);
		Loop* temploop = face->loop;
		unsigned char red = color_faces[index][0];
		unsigned char green = color_faces[index][1];
		unsigned char blue = color_faces[index][2];
		//依次读取面中的环
		while (temploop)
		{
			glColor3b(red, green, blue);
			HalfEdges* halfedge = temploop->halfedge;
			Vertex* start = halfedge->ver;
			gluTessBeginContour(obj);
			gluTessVertex(obj, start->coordinate, start->coordinate);
			Vertex* tmp = halfedge->next->ver;
			halfedge = halfedge->next;
			//依次读取点
			while (tmp != start)
			{
				gluTessVertex(obj, tmp->coordinate, tmp->coordinate);
				halfedge = halfedge->next;
				tmp = halfedge->ver;
			}
			gluTessEndContour(obj);
			temploop = temploop->next;
		}
		gluTessEndPolygon(obj);
		face = face->next;
		index++;
	}
}
void RenderScene(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//旋转图形
	object.apply_transform();
	glPushMatrix();
	glRotatef(xRot, 1.0f, 0.0f, 0.0f);
	glRotatef(yRot, 0.0f, 1.0f, 0.0f);
	glRotatef(zRot, 0.0f, 0.0f, 1.0f);
	glFrontFace(GL_CCW);
	//展示模型
	show(mySolid);
	glPopMatrix();
	glFlush();
	//刷新命令缓冲区
	glutSwapBuffers();
}
void Keycontrol(int key, int x, int y)
{
	if (key == GLUT_KEY_UP) xRot -= 7.0f;
	if (key == GLUT_KEY_DOWN) xRot += 7.0f;
	if (key == GLUT_KEY_LEFT) yRot -= 7.0f;
	if (key == GLUT_KEY_RIGHT) yRot += 7.0f;
	if (xRot > 354.0f) xRot = 0.0f;
	if (xRot < -1.0f) xRot = 355.0f;
	if (yRot > 354.0f) yRot = 0.0f;
	if (yRot < -1.0f) yRot = 355.0f;
	glutPostRedisplay();
}
void initial()
{
	glClearColor(1.0, 1.0, 0.8, 1.0);
	glClearDepth(1.0);
	glShadeModel(GL_SMOOTH);
	glDepthFunc(GL_DEPTH_TEST);
	glEnable(GL_DEPTH_TEST);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	gluTessCallback(obj, GLU_TESS_VERTEX, (void (CALLBACK*)())vertexCallback);
	gluTessCallback(obj, GLU_TESS_BEGIN, (void (CALLBACK*)())beginCallback);
	gluTessCallback(obj, GLU_TESS_END, (void (CALLBACK*)())endCallback);
	gluTessCallback(obj, GLU_TESS_ERROR, (void (CALLBACK*)())errorCallback);
}
//测试用于生成一个立方体，设置所有点坐标运用欧拉操作直接生成
Solid* MakeSolid() {
	double points[][3] = { { 0.0, 0.0, 0.0 }, { 65.0, 0.0, 0.0 }, { 65.0, 65.0, 0.0 }, { 0.0, 65.0, 0.0 },
			{ 0.0, 0.0, 65.0 }, { 65.0, 0.0, 65.0 }, { 65.0, 65.0, 65.0 }, { 0.0, 65.0, 65.0 } };
	Loop* loops;
	Solid* solid = mvfs(points[0]);
	loops = solid->face->loop;
	mev(points[0],points[1],loops);
	mev(points[1], points[2], loops);
	mev(points[2], points[3], loops);
	mef(points[3], points[0], loops);
	mev(points[0], points[4], loops);
	mev(points[1], points[5], loops);
	mef(points[4], points[5], loops);
	mev(points[2], points[6], loops);
	mef(points[5], points[6], loops);
	mev(points[3], points[7], loops);
	mef(points[6], points[7], loops);
	mef(points[7], points[4], loops);
	return solid;
}
//使用欧拉操作构建地面和内环，再通过扫成构建带通孔的立方体
Solid* SweepHole() {
	//外环和两个内环的坐标
	double points_out[][3] = { { 0.0, 0.0, 0.0 }, { 100.0, 0.0, 0.0 }, { 100.0, 100.0, 0.0 }, { 0.0, 100.0, 0.0 } };
	double points_in[][3] = { { 20.0, 20.0, 0.0 }, { 40.0, 20.0, 0.0 }, { 40.0, 40.0, 0.0 }, { 20.0, 40.0, 0.0 } };
	double points_in2[][3] = { { 60.0, 60.0, 0.0 }, { 60.0, 80.0, 0.0 }, { 80.0, 80.0, 0.0 }, { 80.0, 60.0, 0.0 } };

	Solid* pSolid = mvfs(points_out[0]);
	Loop* pLoop = pSolid->face->loop;
	Loop* outloop = pLoop;
	//生成外壳
	mev(points_out[0], points_out[1], pLoop);
	mev(points_out[1], points_out[2], pLoop);
	mev(points_out[2], points_out[3], pLoop);
	mef(points_out[3], points_out[0], pLoop);
	//生成第一个孔
	pLoop = pSolid->face->next->loop;
	mev(points_out[0], points_in[0], pLoop);
	//cout << "degug";
	pLoop = kemr(points_out[0], points_in[0], pLoop);
	cout << "degug";
	mev(points_in[0], points_in[1], pLoop);
	mev(points_in[1], points_in[2], pLoop);
	mev(points_in[2], points_in[3], pLoop);
	Loop* inloop = mef(points_in[3], points_in[0], pLoop);
	kfmrh(outloop, inloop);
	//生成第二个孔
	pLoop = pSolid->face->next->loop;
	mev(points_out[0], points_in2[0], pLoop);
	pLoop = kemr(points_out[0], points_in2[0], pLoop);
	mev(points_in2[0], points_in2[1], pLoop);
	mev(points_in2[1], points_in2[2], pLoop);
	mev(points_in2[2], points_in2[3], pLoop);
	Loop* inloop2 = mef(points_in2[3], points_in2[0], pLoop);
	kfmrh(outloop, inloop2);
	//这个坐标决定扫成宽度
	double pos[3] = {0.0,0.0,65.0};
	sweep(pSolid->face, pos);

	return pSolid;
}
void idle_fem()
{
	if (b['a']) {
		cout << "Cube Model";
		mySolid = MakeSolid();
		b['a'] = false;
	}
	else if (b['b']) {
		cout << "Hole Model";
		mySolid = SweepHole();
		b['b'] = false;
	}
	else if (b['c']) {
		if (!isopen) {
			isopen = true;
			//光线追踪
			GLfloat lightPos[] = { 10.0, 10.0, 10.0, 0.0 };
			GLfloat lightAmb[4] = { 0.0, 0.0, 0.0, 1.0 };
			GLfloat lightDiff[4] = { 1.0, 1.0, 1.0, 1.0 };
			GLfloat lightSpec[4] = { 1.0, 1.0, 1.0, 1.0 };

			glLightfv(GL_LIGHT0, GL_POSITION, &lightpos[0]);
			glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmb);
			glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiff);
			glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpec);

			GLfloat black[] = { 0.0, 0.0, 0.0, 1.0 };
			glLightModelfv(GL_LIGHT_MODEL_AMBIENT, black);
			glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);

			glEnable(GL_LIGHT0);
			glEnable(GL_LIGHTING);
			glEnable(GL_DEPTH_TEST);
		}
		else {
			isopen = false;
			glDisable(GL_LIGHTING);
		}
		b['c'] = false;
		/*
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_LIGHTING);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glShadeModel(GL_SMOOTH);
		*/
	}
	glutPostRedisplay();
}
void main_menu(int i)
{
	key_fem((unsigned char)i, 0, 0);
}
void initMenu() {
	glutCreateMenu(main_menu);
	glutAddMenuEntry("Basic Solid", 'a');
	glutAddMenuEntry("Sweep Hole Solid", 'b');
	glutAddMenuEntry("Light Trace", 'c');
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}
int main(int argc, char* argv[]) {
	mySolid = MakeSolid();
	//mySolid = SweepHole();
	srand(time(NULL));
	for (int i = 0; i < 1000; i++)
	{
		unsigned char red = rand() % 255;
		unsigned char green = rand() % 255;
		unsigned char blue = rand() % 255;
		color_faces[ids][0] = red;
		color_faces[ids][1] = green;
		color_faces[ids][2] = blue;
		ids++;
	}
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_STENCIL);//打开双缓存，颜色缓存，深度缓存
	glutInitWindowPosition(100, 100);//窗口位置
	glutInitWindowSize(1080, 800);//窗口大小
	glutCreateWindow("三维CAD建模");//窗口标题
	initial();
	object.configure_buttons(1);
	//确立鼠标旋转中心点
	object.dolly.dolly[0] = 0;
	object.dolly.dolly[1] = 0;
	object.dolly.dolly[2] = 0;
	//object.trackball.incr = rotationf(vec3f(1, 1, 0), 0.05);
	glutDisplayFunc(RenderScene);//处理绘图
	glutMouseFunc(mouse);//处理鼠标事件
	glutMotionFunc(motion);//处理移动光标后的图形变换
	glutIdleFunc(idle_fem);//每帧处理，用作相应选项操作
	glutReshapeFunc(ChangeSize);//处理窗口改变
	glutSpecialFunc(Keycontrol);//键盘控制
	initMenu();//右键菜单
	initSetting();
	glutMainLoop();
	system("pause");
	return 0;
}