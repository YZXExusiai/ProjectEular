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
//������ڴ�С�����仯������Ӧ�ص���
void ChangeSize(int w, int h)
{
	if (h == 0) h = 1;

	glViewport(0, 0, w, h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	GLfloat Range = 150.0f;
	//ʹ��glOrtho�������Խ���ǰ�Ŀ��ӿռ�����Ϊ��ͶӰ�ռ�
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
	yRot += 360 * (GLfloat)deltax/ (GLfloat)1080;//������Ļ����껬���ľ�����������ת�ĽǶ�
	xRot += 360 * (GLfloat)deltay/ (GLfloat)800;
	oldx = x;
	oldy = y;
	glutPostRedisplay();
	//object.motion(x, y);
}
//�����ݽṹ��ȡŷ�������������岢�γɿ�����Ⱦ��ʵ��
void show(Solid* solid)
{
	Face* face = solid->face;
	int index = 0;
	//���ζ�ȡ���е���
	while (face)
	{
		//���ö������������ͼ�εıߺ��������Ⱦ
		gluTessBeginPolygon(obj, NULL);
		Loop* temploop = face->loop;
		unsigned char red = color_faces[index][0];
		unsigned char green = color_faces[index][1];
		unsigned char blue = color_faces[index][2];
		//���ζ�ȡ���еĻ�
		while (temploop)
		{
			glColor3b(red, green, blue);
			HalfEdges* halfedge = temploop->halfedge;
			Vertex* start = halfedge->ver;
			gluTessBeginContour(obj);
			gluTessVertex(obj, start->coordinate, start->coordinate);
			Vertex* tmp = halfedge->next->ver;
			halfedge = halfedge->next;
			//���ζ�ȡ��
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

	//��תͼ��
	object.apply_transform();
	glPushMatrix();
	glRotatef(xRot, 1.0f, 0.0f, 0.0f);
	glRotatef(yRot, 0.0f, 1.0f, 0.0f);
	glRotatef(zRot, 0.0f, 0.0f, 1.0f);
	glFrontFace(GL_CCW);
	//չʾģ��
	show(mySolid);
	glPopMatrix();
	glFlush();
	//ˢ���������
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
//������������һ�������壬�������е���������ŷ������ֱ������
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
//ʹ��ŷ����������������ڻ�����ͨ��ɨ�ɹ�����ͨ�׵�������
Solid* SweepHole() {
	//�⻷�������ڻ�������
	double points_out[][3] = { { 0.0, 0.0, 0.0 }, { 100.0, 0.0, 0.0 }, { 100.0, 100.0, 0.0 }, { 0.0, 100.0, 0.0 } };
	double points_in[][3] = { { 20.0, 20.0, 0.0 }, { 40.0, 20.0, 0.0 }, { 40.0, 40.0, 0.0 }, { 20.0, 40.0, 0.0 } };
	double points_in2[][3] = { { 60.0, 60.0, 0.0 }, { 60.0, 80.0, 0.0 }, { 80.0, 80.0, 0.0 }, { 80.0, 60.0, 0.0 } };

	Solid* pSolid = mvfs(points_out[0]);
	Loop* pLoop = pSolid->face->loop;
	Loop* outloop = pLoop;
	//�������
	mev(points_out[0], points_out[1], pLoop);
	mev(points_out[1], points_out[2], pLoop);
	mev(points_out[2], points_out[3], pLoop);
	mef(points_out[3], points_out[0], pLoop);
	//���ɵ�һ����
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
	//���ɵڶ�����
	pLoop = pSolid->face->next->loop;
	mev(points_out[0], points_in2[0], pLoop);
	pLoop = kemr(points_out[0], points_in2[0], pLoop);
	mev(points_in2[0], points_in2[1], pLoop);
	mev(points_in2[1], points_in2[2], pLoop);
	mev(points_in2[2], points_in2[3], pLoop);
	Loop* inloop2 = mef(points_in2[3], points_in2[0], pLoop);
	kfmrh(outloop, inloop2);
	//����������ɨ�ɿ��
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
			//����׷��
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
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_STENCIL);//��˫���棬��ɫ���棬��Ȼ���
	glutInitWindowPosition(100, 100);//����λ��
	glutInitWindowSize(1080, 800);//���ڴ�С
	glutCreateWindow("��άCAD��ģ");//���ڱ���
	initial();
	object.configure_buttons(1);
	//ȷ�������ת���ĵ�
	object.dolly.dolly[0] = 0;
	object.dolly.dolly[1] = 0;
	object.dolly.dolly[2] = 0;
	//object.trackball.incr = rotationf(vec3f(1, 1, 0), 0.05);
	glutDisplayFunc(RenderScene);//�����ͼ
	glutMouseFunc(mouse);//��������¼�
	glutMotionFunc(motion);//�����ƶ������ͼ�α任
	glutIdleFunc(idle_fem);//ÿ֡����������Ӧѡ�����
	glutReshapeFunc(ChangeSize);//�����ڸı�
	glutSpecialFunc(Keycontrol);//���̿���
	initMenu();//�Ҽ��˵�
	initSetting();
	glutMainLoop();
	system("pause");
	return 0;
}