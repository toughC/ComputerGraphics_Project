//#include<glut.h>
//#include<gl.h>
//#include<glu.h>
#include "person.h"		//���
#include "particle.h"	//��ƼŬ
#include "treeCyl.h"	//����Ż Ʈ��
#include "car.h"		//�ڵ���

using namespace std;

static GLfloat MyPlane[] = { 1.0, 0.0, 0.0, 0.0 };//����2 ������

const int MAX_ANINUM = 1;				//������ �ִϸ��̼� �Ѱ���
const int MAX_PGENERATOR = 10;

TreeCyl leafs[100000];//����Ż �̿��� �׸� �迭

float temp1 = 0.1, temp2 = 0.1;
float lifeDot = 0.818 / 1000.0;	//ü��1�� ǥ�õ�Ʈ

double width;	//������ ũ�� �ޱ����� ����
double height;
double radius;	//����-������ �Ÿ�(ȸ����)

int timer2 = 0;
//int sleepSpeed = 14;	//�� ������ �����ð�(�ʴ� 70������)
int sleepSpeed = 17;	//�� ������ �����ð�(�ʴ� 60������)
short keyDoubleTouchTime = 20;	//�� �̳��� Ű�� �ι������� �����Է����� �����ΰ�
short up1, down1, left1, right1;		//�Է� ������
short up2, down2, left2, right2;	//������ġ ������

bool isFogDraw = true;								// �հ� �Ȱ�ȿ�� on/off��
GLuint fogMode[] = { GL_EXP, GL_EXP2, GL_LINEAR };	// ������ �Ȱ�����
GLuint fogFilter = 2;								// ����� �Ȱ�����
GLfloat fogColor[4] = { 0.5f, 0.5f, 0.5f, 1.0f };      // �Ȱ�����

int FlatShaded = 0;// flatshading�� wireframe�� ��۸��� ���� �ο� ����
int Wireframed = 0;

double TableX = 5.0;	//���̺� ũ��
double TableY = 8.0;
double TableHeight = 2.0;//���̺� ����

bool isBgmPlay = true;	//bgm �������
bool isPaused = false;	//�Ͻ����� ����
bool isAtDraw = false;	//����,������ǥ�� ���� �׸���
bool isHelpDraw = false;	//���� ���̳�
bool isLifeBarDraw = false;	//�������� �׸���
bool isCanMouse = true;	//���콺 ��Ʈ�� �����ϳ� 
bool isParticleDraw = true;		//��ƼŬ �׸���
bool isCarDraw = false;		//�� �׸���
bool isCyren = false;		//���̷�ȿ��
short treeDrawMode = 0;	//Ʈ�� ���   0 : �Ǹ���   1: ����Ż Ʈ��
short mountainDrawMode = 0;	//�� ���   0 : �ܼ�   1: �ؽ�ó

int ViewX = 450;// ���콺 �����ӿ� ���� ������ �ٲٱ� ���� ����
int ViewY = 170;//-25;

int oldX;	//���콺Ŀ����ġ
int oldY;

double plusX;	//���Ұ���
double plusY;

//float eyeAngle[3];	//��������

GLUquadricObj* cyl;	//�Ǹ��� ��ü

int TableListID;	//����Ʈ�� ID. ���̺�
int netID;			//����Ʈ�� ID. ���̺� ��Ʈ
int listID[20];			//����Ʈ�� ID

float ball[3];  //����ġ

float RacketColor[3] = { 200 / 255.0, 50 / 255.0, 50 / 255.0 };		//���ϻ�

objects objs;						//��ü��(����迭)
Car* car = new Car(&objs);

HANDLE Thread[MAX_ANINUM];				//�ִϸ��̼ǿ� ������ (���������� �ϱ� ����)
UINT ThreadState[MAX_ANINUM];			//������ ���� ���� ����(���� �̻��. ���� ���Ῡ�� �˼�����)
UINT WINAPI AniIntro(void* arg);		//������ ȣ���Լ� - ��Ʈ��
UINT WINAPI AniEnding(void* arg);	//������ ȣ���Լ� - ����

string currSound;	//���� ������� ȿ����. (���� ����)

//-----------------------------------------
// ��ƼŬ ������ �迭
ParticleGenerator pGenerators[MAX_PGENERATOR];

//----- �� �� ----------------------------
GLfloat ambi0[] = { 0.4f, 0.4f, 0.4f, 1.0f };//�ֺ��� 	//-----�޺� 
GLfloat diff0[] = { 0.7f, 0.7f, 0.7f, 1.0f };//Ȯ�걤
//GLfloat spec0[] = {1.0f, 1.0f, 1.0f, 1.0f};		//�ݻ籤
GLfloat spec0[] = { 0.9, 0.9, 0.9, 1.0 };		//�ݻ籤
GLfloat posi0[] = { 400.0, 800.0, -300.0, 1.0 };	//������ġ. x,y,z,�Ÿ�

GLfloat ambi1[] = { 0.4f, 0.4f, 0.4f, 1.0f };//�ֺ���		//���
GLfloat diff1[] = { 1.0f, 1.0f, 1.0f, 1.0f };//Ȯ�걤
GLfloat spec1[] = { 1.0f, 1.0f, 1.0f, 1.0f };		//�ݻ籤
GLfloat posi1[] = { 0.0, 10.0, -3.0, 1.0 };	//������ġ. x,y,z,�Ÿ�

GLfloat ambi5[] = { 1.0, 0.0, 0.0, 1.0 };				//������
GLfloat diff5[] = { 1.0, 0.0, 0.0, 1.0 };
//GLfloat spec5[] = {1.0, 1.0, 1.0, 1.0};
GLfloat spec5[] = { 1.0, 0.0, 0.0, 1.0 };
GLfloat posi5[] = { 3.0, 5.0, -1.0, 1.0 };
GLfloat spotAngle5[] = { 10.0 };
GLfloat angleX5 = 0;
GLfloat dir5[] = { 0,0,0 };
GLfloat dir6[] = { 0,0,0 };				//������2

GLfloat mAmbi[] = { 0.5, 0.5, 0.5, 1 };	//��üƯ��
GLfloat mDiff[] = { 0.5, 0.5, 0.5, 1 };
GLfloat mSpec[] = { 0.5, 0.5, 0.5, 1.0 };
GLfloat mShin[] = { 5.0 };

//----------------  �� �� ----------------------------------------
void setupGL();
void setupParticle();

void InitLight();				//�����ʱ�ȭ
void InitPosition();			//�����ġ,�����ʱ�ȭ
void MyMainMenu(int entryID);	//��Ŭ���޴�
void MySubMenu(int entryID);	//����޴�
void Create_TableList();		//���÷��̸���Ʈ ����
void MyMouseMove(int button, int state, GLint X, GLint Y);	//���콺 Ŭ���̺�Ʈ �ν��ϴ� �Լ�
void MyMotion(GLint X, GLint Y);					//���콺 Ŭ���̵� �̺�Ʈ �ν��ϴ� �Լ�
void MyMouseMove2(int button, int state, GLint X, GLint Y);	//���콺 ����Ŭ��
void MyMotion2(GLint X, GLint Y);					//���콺 ����Ŭ���̵�
void EyePosChange(GLint angleX, GLint angleY);		//�����̵�(MyMotion���� �и�. �ִϿ��� ��Ʈ�Ѱ����� �ϱ�����)
void MyPassiveMotion(GLint X, GLint Y);	//���콺 Ŭ������ �̵������� �̺�Ʈ�޴� �Լ�(�̻��)
void CalculateEyeXY();		//������ǥ ���ϴ� �Լ�
void CalculateRadius();		//����-������ �Ÿ�/��������
void MyKeyboard(unsigned char key, int x, int y);	//Ű���� �ݹ�
void Specialkeys(int key, int x, int y);		//����Ű �� �ݹ�
void DrawTable();//(�̻��)
void MyDisplay();								//���÷��� �ݹ�
void MyReshape(int w, int h);					//ȭ������ �ݹ�
void MyTimer(int Value);						//Ÿ�̸� �ݹ�
//void DrawRacket();								//���� �׸���
//void DrawBall();								//�� �׸���
void MyKeyboardUp(unsigned char key, int x, int y);	//Ű���� ��
void p1ActionChange();								//���ΰ� �������ۿ� ���� �ൿ�� ��Ű�� �Լ�
void DrawLifeBar();							//�������� ǥ��
void DrawCylTree();							//�Ǹ��� Ʈ�� ���
void DrawFractalTree();							//����Ż Ʈ�� ���
void RenderText(char* text, float posX, float posY);	//�������
void RenderHelp();							//���� ���
void RenderEnd();							//�� ���

void drawLightPoll();  //���ε� �����
void drawLightPoll()

{

	GLUquadricObj* cyl;
	cyl = gluNewQuadric();

	glRotatef(-90, 1, 0, 0);
	glScalef(1, 1, 1);
	glColor3f(1, 1, 1);
	gluCylinder(cyl, 0.6, 0.4, 8, 100, 100);
	glTranslatef(0, 0, 9);
	glColor3f(1, 1, 0);
	gluSphere(cyl, 1, 100, 100);
}

//---------------------------------------------------------------------
//���� �ؽ�ó�ε��Լ�(jpg,png����)
// Function load a image, turn it into a texture, and return the texture ID as a GLuint for use
GLuint loadImage(const char* theFileName)
{
	ILuint imageID;    // Create an image ID as a ULuint
	GLuint textureID;   // Create a texture ID as a GLuint
	ILboolean success;   // Create a flag to keep track of success/failure
	ILenum error;    // Create a flag to keep track of the IL error state
	ilGenImages(1, &imageID);   // Generate the image ID
	ilBindImage(imageID);    // Bind the image
	success = ilLoadImage(theFileName);  // Load the image file

	// If we managed to load the image, then we can start to do things with it...
	if (success)
	{
		// If the image is flipped (i.e. upside-down and mirrored, flip it the right way up!)
		ILinfo ImageInfo;
		iluGetImageInfo(&ImageInfo);
		if (ImageInfo.Origin == IL_ORIGIN_UPPER_LEFT)
		{
			iluFlipImage();
		}

		// Convert the image into a suitable format to work with
		// NOTE: If your image contains alpha channel you can replace IL_RGB with IL_RGBA
		success = ilConvertImage(IL_RGB, IL_UNSIGNED_BYTE);

		// Quit out if we failed the conversion
		if (!success)
		{
			error = ilGetError();
			std::cout << "Image conversion failed - IL reports error: " << error << " - " << iluErrorString(error) << std::endl;
			//			exit(-1);	//�̹��� ��� ����ȵǰ� ����
			return NULL;
		}

		// Generate a new texture
		glGenTextures(1, &textureID);

		// Bind the texture to a name
		glBindTexture(GL_TEXTURE_2D, textureID);

		// Set texture clamping method
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

		// Set texture interpolation method to use linear interpolation (no MIPMAPS)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);//�ӸʺҰ��ѵ�

				// Specify the texture specification	���� �Ӹ� �ƴҽ�
		glTexImage2D(GL_TEXTURE_2D,     // Type of texture
			0,    // Pyramid level (for mip-mapping) - 0 is the top level
			ilGetInteger(IL_IMAGE_BPP), // Image colour depth
			ilGetInteger(IL_IMAGE_WIDTH), // Image width
			ilGetInteger(IL_IMAGE_HEIGHT), // Image height
			0,    // Border width in pixels (can either be 1 or 0)
			ilGetInteger(IL_IMAGE_FORMAT), // Image format (i.e. RGB, RGBA, BGR etc.)
			GL_UNSIGNED_BYTE,  // Image data type
			ilGetData());   // The actual image data itself

		//�Ӹ��Ͻ�
		//gluBuild2DMipmaps(GL_TEXTURE_2D, 3, TextureImage->sizeX, TextureImage[0]->sizeY, GL_RGB, GL_UNSIGNED_BYTE, TextureImage[0]->data);
	}
	else // If we failed to open the image file in the first place...
	{
		error = ilGetError();
		std::cout << "Image load failed - IL reports error: " << error << " - " << iluErrorString(error) << std::endl;
		//		exit(-1); //�������� ����ȵǰ�
		return NULL;
	}

	ilDeleteImages(1, &imageID); // Because we have already copied image data into texture data we can release memory used by image.

	std::cout << "Texture creation successful." << std::endl;

	return textureID; // Return the GLuint to the texture so you can use it!
}

//�����ġ, ���� �ʱ�ȭ
void InitPosition()
{
	sndPlaySound(NULL, SND_ASYNC);//�Ҹ���� ����

	DWORD nExitCode = NULL;
	for (int i = 0; i < MAX_ANINUM; i++) {
		GetExitCodeThread(Thread[i], &nExitCode);	//
		TerminateThread(Thread[i], nExitCode);		//������ ��������
		CloseHandle(Thread[i]);					//������ ����
	}

	for (unsigned int i = 0; i < objs.people.size(); i++) {	//����迭 ����
		objs.people.at(i)->InitPosition();				//��ǥ,���� �ʱ�ȭ
		objs.people.at(i)->InitColor();				//���ʱ�ȭ		
	}

	double tempAtX = objs.AtX;	//������ȭ ���̰���
	double tempAtY = objs.AtY;
	double tempAtZ = objs.AtZ;
	objs.AtX = 3;		//����
	objs.AtY = 14;
	objs.AtZ = 2;
	objs.EyeX -= tempAtX - objs.AtX;	//������ �����̵�ġ ��ŭ �̵�
	objs.EyeY -= tempAtY - objs.AtY;
	objs.EyeZ -= tempAtZ - objs.AtZ;
}

//�����׸��� ť��
GLvoid glDrawCube()
{
	glBegin(GL_QUADS);
	// Front Face
	glNormal3f(0.0f, 0.0f, 0.5f);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f, 1.0f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(1.0f, -1.0f, 1.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(1.0f, 1.0f, 1.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f, 1.0f, 1.0f);
	// Back Face
	glNormal3f(0.0f, 0.0f, -0.5f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f, 1.0f, -1.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(1.0f, 1.0f, -1.0f);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(1.0f, -1.0f, -1.0f);
	// Top Face
	glNormal3f(0.0f, 0.5f, 0.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f, 1.0f, -1.0f);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, 1.0f, 1.0f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(1.0f, 1.0f, 1.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(1.0f, 1.0f, -1.0f);
	// Bottom Face
	glNormal3f(0.0f, -0.5f, 0.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(1.0f, -1.0f, -1.0f);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(1.0f, -1.0f, 1.0f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f, 1.0f);
	// Right Face
	glNormal3f(0.5f, 0.0f, 0.0f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(1.0f, -1.0f, -1.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(1.0f, 1.0f, -1.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(1.0f, 1.0f, 1.0f);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(1.0f, -1.0f, 1.0f);
	// Left Face
	glNormal3f(-0.5f, 0.0f, 0.0f);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f, 1.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f, 1.0f, 1.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f, 1.0f, -1.0f);
	glEnd();
}

//����&�Ȱ�����
void InitLight() {
	glClearColor(0.9f, 0.9f, 0.9f, 1.0f);            // �Ȱ� ������ ȭ���� ����
	glFogi(GL_FOG_MODE, fogMode[fogFilter]);        // �Ȱ���� 0~2. exp, exp2, linear
	glFogfv(GL_FOG_COLOR, fogColor);            // �Ȱ� ���� ����
	glFogf(GL_FOG_DENSITY, 0.1f);                // �Ȱ��� �е�(exp��)
//	glHint(GL_FOG_HINT, GL_DONT_CARE);            // �Ȱ� ��Ʈ ��. - opengl�ڵ�
	glHint(GL_FOG_HINT, GL_FASTEST);            // �Ȱ� ��Ʈ ��.	������ ����
//	glHint(GL_FOG_HINT, GL_NICEST);            // �Ȱ� ��Ʈ ��.	�ȼ��� ����
	glFogf(GL_FOG_START, -50.0f);                // �Ȱ� ���� ����
	glFogf(GL_FOG_END, 400.0f);                // �Ȱ� �� ����
	if (isFogDraw == true) {
		glEnable(GL_FOG);                    // �Ȱ� ��

	}

	glEnable(GL_DEPTH_TEST);	//��������
//	glEnable(GL_NORMALIZE);		//����ȭ
	glEnable(GL_SMOOTH);		//�������� �ε巴�� ����. ���μ��̵�
	glEnable(GL_LIGHTING);	//���޴� ������ ���� ����ȭ
//	glDepthMask(GL_TRUE);	//�ڿ��ִ°� ������
//	glEnable(GL_CULL_FACE);//�ĸ�����
//	glCullFace(GL_BACK);		

	glLightfv(GL_LIGHT0, GL_AMBIENT, ambi0);	//����0���� ����
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diff0);
	glLightfv(GL_LIGHT0, GL_SPECULAR, spec0);
	glLightfv(GL_LIGHT0, GL_POSITION, posi0);

	glLightfv(GL_LIGHT1, GL_AMBIENT, ambi1);	//����1���� ����
	glLightfv(GL_LIGHT1, GL_DIFFUSE, diff1);
	glLightfv(GL_LIGHT1, GL_SPECULAR, spec1);
	glLightfv(GL_LIGHT1, GL_POSITION, posi1);

	glLightfv(GL_LIGHT5, GL_AMBIENT, ambi5);	//����5���� ����
	glLightfv(GL_LIGHT5, GL_DIFFUSE, diff5);
	glLightfv(GL_LIGHT5, GL_SPECULAR, spec5);
	glLightfv(GL_LIGHT5, GL_POSITION, posi5);

	glLightfv(GL_LIGHT6, GL_AMBIENT, ambi5);	//����6���� ����
	glLightfv(GL_LIGHT6, GL_DIFFUSE, diff5);
	glLightfv(GL_LIGHT6, GL_SPECULAR, spec5);
	glLightfv(GL_LIGHT6, GL_POSITION, posi5);

	glEnable(GL_LIGHT0);			//�����ȣ0Ȱ��ȭ
//	glEnable(GL_LIGHT1);			//�����ȣ1Ȱ��ȭ

	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
	glMateriali(GL_FRONT, GL_SHININESS, 10);		//�ݻ����� ����
	glEnable(GL_COLOR_MATERIAL);

	//�ؽ�Ʈ ��� �ϱ�� ��������
	GLfloat ambientLight7[] = { 0.5f, 0.5f, 0.5f, 1.0f };//�ֺ���
	GLfloat diffuseLight7[] = { 0.5f, 0.5f, 0.5f, 1.0f };//Ȯ�걤
//	GLfloat specular7[] = {1.0f, 1.0f, 1.0f, 1.0f};		//�ݻ籤	
	GLfloat specular7[] = { 0.1f, 0.1f, 0.1f, 0.1f };		//�ݻ籤
	GLfloat position7[] = { 10.0, 50.0, 50.0, 1.0 };	//������ġ. x,y,z,�Ÿ�
	glLightfv(GL_LIGHT7, GL_AMBIENT, ambientLight7);	//�������� ����. �ؽ�Ʈ ��� ���̰� �ϱ�����
	glLightfv(GL_LIGHT7, GL_DIFFUSE, diffuseLight7);
	glLightfv(GL_LIGHT7, GL_SPECULAR, specular7);
	glLightfv(GL_LIGHT7, GL_POSITION, position7);

	glEnable(GL_POINT_SMOOTH);//�� ��������
//    glEnable(GL_BLEND);		//������
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);	//���� �ɼ�

	glEnable(GL_NORMALIZE);//���� ����ȭ
}

//���÷��� �ݹ�
void MyDisplay() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	width = glutGet(GLUT_WINDOW_WIDTH);
	height = glutGet(GLUT_WINDOW_HEIGHT);
	gluPerspective(60.0, (GLfloat)width / height, 0.01, 500.0);

	// ���콺 �����ӿ� ���� ������ȭ
	gluLookAt(objs.EyeX, objs.EyeY, objs.EyeZ, objs.AtX, objs.AtY, objs.AtZ, 0.0, 1.0, 0.0);
	//	printf("eyex = %f , eyey = %f , eyez = %f \n",(float)(ViewX - width/2.0)/width*10, (float)(height/2 - ViewY)/height*10, (float)ViewZ);

	if (isHelpDraw == true) {//���� ǥ��. �������� ��� ���
		RenderHelp();
	}
	if (objs.gameState == 4) {//���� ǥ��. �������� ��� ���
		RenderEnd();
	}

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	//	glDisable(GL_COLOR_MATERIAL);

		//�ϴ� �� �׸�
	glDisable(GL_DEPTH_TEST);	//���̹��� ��
	glDisable(GL_CULL_FACE);//�ĸ����� ��
	glCallList(listID[2]);			//�ϴ� ��(�������� ������ �Ѱ� �׷���)
//	glEnable(GL_CULL_FACE);//�ĸ�����
//	glEnable(GL_DEPTH_TEST);

	if (mountainDrawMode == 0) {
		glCallList(listID[4]);			//��
	}

	glEnable(GL_DEPTH_TEST);	//���̹��� ��

	//	glCallList(listID[0]);			//ť��ٴ�
	glCallList(listID[1]);			//�ؽ�ó �ٴ�


	if (treeDrawMode == 0) {//�Ǹ��� ������ �׸���
		DrawCylTree();
	}
	else if (treeDrawMode == 1) {//����Ż ������ �׸���
		DrawFractalTree();
	}

	if (isCarDraw == true) {		//�� �׸���
		glPushMatrix();
		glTranslatef(car->pos.x, car->pos.y, car->pos.z);
		glRotatef(car->angles.y + car->dAngle, 0, 1, 0);
		glCallList(listID[10]);	//�ܼ� ��(ť��2��)
		glPopMatrix();
	}




	//	glutWireSphere(0.0, 5, 5);	//�ݻ�ü.

	if (isAtDraw == TRUE) {//��,���� ǥ��
		glColor3f(255 / 255.0, 152 / 255.0, 10 / 255.0);
		glutWireSphere(0.5, 20, 16);	//WCS������ ���� �� �׸�. ������, �漱, ����

		glPushMatrix();
		glTranslatef(objs.AtX, objs.AtY, objs.AtZ);
		glColor3f(0.5, 0.9, 0.5);		//������. �ʷϻ�
		glutWireSphere(0.5, 20, 16);	//WCS������ ���� �� �׸�. ������, �漱, ����
		glPopMatrix();
	}

	//��ƼŬ �׸�
	glEnable(GL_BLEND);		//���� Ȱ��
	for (int i = 0; i < MAX_PGENERATOR; i++) {
		if (pGenerators[i].alive == true) {
			pGenerators[i].DrawParticle();
		}
	}
	glDisable(GL_BLEND);		//���� ��Ȱ��


	glDisable(GL_FOG);
	if (isLifeBarDraw == true) {
		//		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);	//�ٸ��� ����ȹް�
		DrawLifeBar();			//�������� ǥ��
//		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	}
	if (isFogDraw == true) {
		glEnable(GL_FOG);
	}

	if (isPaused == false) {
		angleX5 += 5;//glutGet(GLUT_ELAPSED_TIME)%360;
		dir5[0] = cos(angleX5 * 3.14 / 180.0);	//����5�� ����
		dir5[2] = sin(angleX5 * 3.14 / 180.0);
		dir6[0] = -dir5[0];	//����6 ����
		dir6[2] = -dir5[2];
	}


	//	GLfloat Emit[] = {1.0, 0.0, 0.0, 1.0};
//	glMaterialfv(GL_FRONT, GL_EMISSION, Emit);		//��üƯ�� ����
	//glMaterialfv(GL_FRONT, GL_AMBIENT, mAmbi);		//��üƯ�� ����
	//glMaterialfv(GL_FRONT, GL_DIFFUSE, mDiff);
	//glMaterialfv(GL_FRONT, GL_SPECULAR, mSpec);
	//glMaterialfv(GL_FRONT, GL_SPOT_DIRECTION, dir5);
	//glMaterialfv(GL_FRONT, GL_SPOT_CUTOFF, spotAngle5);
	//glMaterialf(GL_FRONT, GL_SPOT_EXPONENT, 1.0f);
	//glMaterialfv(GL_FRONT, GL_SHININESS, mShin);	
	//glMaterialfv(GL_EMISSION, GL_SHININESS, diff5);


//	glLightfv(GL_LIGHT1, GL_POSITION, posi1);	//����1��ġ��ȭ ��

	if (isCarDraw == true && isCyren == true) {
		glEnable(GL_LIGHT5);			//�����ȣ5Ȱ��ȭ
		glEnable(GL_LIGHT6);			//�����ȣ5Ȱ��ȭ
		glPushMatrix();				//����������(��������)
		glColor3f(1, 0.3, 0.3);
		glTranslatef(posi5[0], posi5[1] - 0.5, posi5[2]);
		glScalef(1, 2, 1);
		glutSolidSphere(0.3, 10, 10);
		glPopMatrix();

		posi5[0] = car->pos.x;
		posi5[1] = car->pos.y + 4.5;
		posi5[2] = car->pos.z;
		glLightfv(GL_LIGHT5, GL_POSITION, posi5);//����5 ����
		glLightfv(GL_LIGHT5, GL_SPOT_DIRECTION, dir5);//���� ����
		glLightfv(GL_LIGHT5, GL_SPOT_CUTOFF, spotAngle5);
		glLightf(GL_LIGHT5, GL_SPOT_EXPONENT, 1.0f);
		glLightfv(GL_LIGHT6, GL_POSITION, posi5);//����6 ����
		glLightfv(GL_LIGHT6, GL_SPOT_DIRECTION, dir6);//���� ����
		glLightfv(GL_LIGHT6, GL_SPOT_CUTOFF, spotAngle5);
		glLightf(GL_LIGHT6, GL_SPOT_EXPONENT, 1.0f);

		//glLightfv(GL_LIGHT6, GL_POSITION, posi5);//���� ����
		//glLightfv(GL_LIGHT6, GL_SPOT_DIRECTION, dir5);//���� ����
		//glLightfv(GL_LIGHT6, GL_SPOT_CUTOFF, spotAngle5);
		//glLightf(GL_LIGHT6, GL_SPOT_EXPONENT, 1.0f);

		glColor4f(1.0, 0, 0, 0.1);
		cyl = gluNewQuadric();//����
		glPushMatrix();
		glTranslatef(posi5[0], posi5[1] - 0.1, posi5[2]);
		glRotatef(-angleX5 + 90, 0, 1, 0);		//������1
		glRotatef(0, 1, 0, 0);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);	//���� �ɼ�

		gluCylinder(cyl, 0, 10, 100, 10, 10);

		glRotatef(180, 0, 1, 0);				//������2
		gluCylinder(cyl, 0, 10, 100, 10, 10);

		glDisable(GL_BLEND);
		glPopMatrix();
	}
	else {
		glDisable(GL_LIGHT5);			//�����ȣ5��Ȱ��ȭ
		glDisable(GL_LIGHT6);			//�����ȣ6��Ȱ��ȭ
	}

	//	glFlush();
	glutSwapBuffers();	//������۸�
}

//���÷��̸���Ʈ
void Create_List() {
	//���̺� �׸�
	TableListID = glGenLists(1);
	glNewList(TableListID, GL_COMPILE);
	glPushMatrix();
	glColor3f(0.5, 0.25, 0.0);
	cyl = gluNewQuadric();
	glRotatef(-90, 1.0, 0.0, 0.0);
	gluCylinder(cyl, 0.2, 0.2, TableHeight, 10, 10); //���1
	glPushMatrix();
	cyl = gluNewQuadric();
	glTranslatef(TableX, 0.0, 0.0);
	gluCylinder(cyl, 0.2, 0.2, TableHeight, 10, 10); //2
	glPushMatrix();
	cyl = gluNewQuadric();
	glTranslatef(0.0, TableY, 0.0);
	gluCylinder(cyl, 0.2, 0.2, TableHeight, 10, 10); //3
	glPushMatrix();
	cyl = gluNewQuadric();
	glTranslatef(-TableX, 0.0, 0.0);
	gluCylinder(cyl, 0.2, 0.2, TableHeight, 10, 10); //4
	glPushMatrix();
	glTranslatef(TableX / 2.0, -TableY / 2, TableHeight);
	glScalef(TableX + 0.5, TableY + 0.5, 0.5);
	glutSolidCube(1);							//���� �׸���(��3�ٺ���)						
	//glTranslatef(0.0, 0.0, TableHeight);//������ ä��簢������ �׸���
	//glBegin(GL_QUADS);					
	//	glVertex3f(-0.5, 0, 0);
	//	glVertex3f(TableX+0.5, 0, 0);
	//	glVertex3f(TableX+0.5, -TableY-0.5, 0);
	//	glVertex3f(-0.5, -TableY-0.5, 0);
	//glEnd();
	glPopMatrix();

	glPushMatrix();							//�ﰢ��1
	glTranslatef(0, -TableY / 2, TableHeight);
	glBegin(GL_TRIANGLES);
	glColor3f(0.8, 0.9, 0.5);
	glVertex3f(TableY / 16.0, 0, TableY / 8.0);//1
	glVertex3f(0, TableY / 8.0, 0);
	glVertex3f(0, -TableY / 8.0, 0);
	glVertex3f(TableY / 16.0, 0, TableY / 8.0);//2
	glVertex3f(0, -TableY / 8.0, 0);
	glVertex3f(TableY / 16.0, -TableY / 8.0, 0);
	glVertex3f(TableY / 16.0, 0, TableY / 8.0);//3
	glVertex3f(TableY / 16.0, -TableY / 8.0, 0);
	glVertex3f(TableY / 16.0, TableY / 8.0, 0);
	glVertex3f(TableY / 16.0, 0, TableY / 8.0);//4
	glVertex3f(TableY / 16.0, TableY / 8.0, 0);
	glVertex3f(0, TableY / 8.0, 0);
	glEnd();
	glPushMatrix();						//�ﰢ��2
	glTranslatef(TableX - TableY / 8.0, 0, 0);
	glBegin(GL_TRIANGLES);
	glColor3f(1.0, 0.0, 0.0);
	glVertex3f(TableY / 16.0, 0, TableY / 8.0);//1
	glColor3f(0.0, 1.0, 0.0);
	glVertex3f(TableY / 16.0, TableY / 8.0, 0);
	glColor3f(0.0, 0.0, 1.0);
	glVertex3f(TableY / 16.0, -TableY / 8.0, 0);
	glColor3f(1.0, 0.0, 0.0);
	glVertex3f(TableY / 16.0, 0, TableY / 8.0);//2
	glColor3f(0.0, 1.0, 0.0);
	glVertex3f(TableY / 16.0, -TableY / 8.0, 0);
	glColor3f(0.0, 0.0, 1.0);
	glVertex3f(TableY / 8.0, -TableY / 8.0, 0);
	glColor3f(1.0, 0.0, 0.0);
	glVertex3f(TableY / 16.0, 0, TableY / 8.0);//3
	glColor3f(0.0, 1.0, 0.0);
	glVertex3f(TableY / 8.0, -TableY / 8.0, 0);
	glColor3f(0.0, 0.0, 1.0);
	glVertex3f(TableY / 8.0, TableY / 8.0, 0);
	glColor3f(1.0, 0.0, 0.0);
	glVertex3f(TableY / 16.0, 0, TableY / 8.0);//4
	glColor3f(0.0, 1.0, 0.0);
	glVertex3f(TableY / 8.0, TableY / 8.0, 0);
	glColor3f(0.0, 0.0, 1.0);
	glVertex3f(TableY / 16.0, TableY / 8.0, 0);
	glEnd();
	glPopMatrix();

	glPopMatrix();
	glPopMatrix();
	glPopMatrix();
	glPopMatrix();
	glPopMatrix();
	glEndList();

	//ť�� �ٴ�
	listID[0] = glGenLists(1);
	glNewList(listID[0], GL_COMPILE);
	glPushMatrix();
	glTranslatef(0, -3.1, 0);
	glScalef(250, 6, 500);
	glColor3f(0.0, 0.5, 0.5);
	//			glutSolidCube(1);
	glDrawCube();
	glColor3f(1, 1, 1);
	glPopMatrix();
	glEndList();

	//�ؽ�ó�ٴ�
	listID[1] = glGenLists(1);
	glNewList(listID[1], GL_COMPILE);
	//	    glEnable(GL_BLEND);		//������
	//		glBlendFunc(GL_SRC_ALPHA, GL_ONE);	//���� �ɼ�
	glPushMatrix();
	glBindTexture(GL_TEXTURE_2D, objs.textures[0]);	//����

	glColor4f(1, 1, 1, 1.0);
	glTranslatef(0, 0, -300);
	for (int i = 0; i < 11; i++) {
		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-20.0f, 0.0f, 30.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(20.0f, 0.0f, 30.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(20.0f, 0.0f, -30.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-20.0f, 0.0f, -30.0f);
		glEnd();
		glTranslatef(0, 0, 60);
	}
	glPopMatrix();

	glPushMatrix();
	glBindTexture(GL_TEXTURE_2D, objs.textures[20]);	//��Ǯ��

	glTranslatef(-120, 0, -200);
	for (int i = 0; i < 3; i++) {
		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-100.0f, 0.0f, 100.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(100.0f, 0.0f, 100.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(100.0f, 0.0f, -100.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-100.0f, 0.0f, -100.0f);
		glEnd();
		glTranslatef(0, 0, 200);
	}
	glPopMatrix();

	glPushMatrix();
	glBindTexture(GL_TEXTURE_2D, objs.textures[20]);	//����Ǯ��

	glTranslatef(120, 0, -200);
	for (int i = 0; i < 3; i++) {
		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-100.0f, 0.0f, 100.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(100.0f, 0.0f, 100.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(100.0f, 0.0f, -100.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-100.0f, 0.0f, -100.0f);
		glEnd();
		glTranslatef(0, 0, 200);
	}
	glPopMatrix();

	glDisable(GL_BLEND);		//���� ��Ȱ��
	glEndList();

	//�ϴñ�
	listID[2] = glGenLists(1);
	glNewList(listID[2], GL_COMPILE);
	glPushMatrix();

	glColor3f(1, 1, 1);
	glRotatef(180, 0, 0, 1);

	glBindTexture(GL_TEXTURE_2D, objs.textures[10]);
	//		glTexGeni(GL_Q, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);

	cyl = gluNewQuadric();
	gluQuadricTexture(cyl, 2);

	gluSphere(cyl, 300, 50, 50);	//��
	glPopMatrix();
	glEndList();

	//����Ż ���� 1��
	listID[3] = glGenLists(1);
	glNewList(listID[3], GL_COMPILE);
	int fsrc = 0;		//����Ż �迭���� �׸���ġ
	int fdest = 1;	//����Ż �迭���� ������ġ
//		Vec3<float> tempPos;
//		float mat[16];
	cyl = gluNewQuadric();

	glPushMatrix();
	leafs[0].pos.set(0.0f, 0.0f, 0.0f);		//ù ���� ����
	leafs[0].angle.set(0.0f, 0.0f, 0.0f);
	leafs[0].height = 4.0f;
	leafs[0].radius = 0.5f;
	glRotatef(-90, 1, 0, 0);				//���� �׷�����
	glScalef(2, 2, 2);				//�ø�

	glColor3f(236 / 255.0, 77 / 255.0, 0 / 255.0);//����
	gluCylinder(cyl, leafs[0].radius, leafs[0].radius, leafs[0].height, 10, 5);//ù ���� �׸�
	glTranslatef(0, 0, 2);				//������ �׷�����

	for (int i = 0; i < 10000; i++) {
		glPushMatrix();
		glTranslatef(leafs[fsrc].pos.x, leafs[fsrc].pos.y, leafs[fsrc].pos.z);
		glRotatef(leafs[fsrc].angle.z, 0, 0, 1);
		glRotatef(leafs[fsrc].angle.y, 0, 1, 0);
		glRotatef(leafs[fsrc].angle.x, 1, 0, 0);//�ҽ� ��ġ�� �̵�/ȸ��

		for (int j = 0; j < 3; j++) {
			glPushMatrix();
			leafs[fdest].angle.set(leafs[fsrc].angle.x + random(-50.0, 50.0), leafs[fsrc].angle.y + random(-50.0, 50.0), leafs[fsrc].angle.z);//�ҽ����� ����������
			leafs[fdest].pos.set(0, 0, leafs[fsrc].height * 2 / 3);
			leafs[fdest].height = leafs[fsrc].height * 2.5 / 3.0;	//���̺�ȭ
			leafs[fdest].radius = leafs[fsrc].radius * 2 / 3.0;	//ũ�⺯ȭ

			glTranslatef(leafs[fdest].pos.x, leafs[fdest].pos.y, leafs[fdest].pos.z);
			glRotatef(leafs[fdest].angle.z, 0, 0, 1);
			glRotatef(leafs[fdest].angle.y, 0, 1, 0);
			glRotatef(leafs[fdest].angle.x, 1, 0, 0);//�ҽ� ��ġ�� �̵�/ȸ��	
			glColor3f(100 / 255.0, 200 / 255.0, 100 / 255.0);//�ʷϻ�

			if (leafs[fdest].radius < 0.1) {	//�� ũ�� ����. ������ �ȱ׸�
				gluCylinder(cyl, leafs[fdest].radius, leafs[fdest].radius, leafs[fdest].height, 3, 3);//ù ���� �׸�
			}

			fdest++;
			glPopMatrix();
		}
		fsrc++;
		glPopMatrix();
	}
	glPopMatrix();
	glEndList();

	//�ܼ� ��
	listID[4] = glGenLists(1);
	glNewList(listID[4], GL_COMPILE);
	float tcos;
	float tsin;
	float ran;
	glColor3f(0.3, 1, 0.3);
	for (int i = 0; i < 360; i += 15 + ran) {
		ran = rand() % 10;		//����
		tcos = cos(i * pi / 180.0);
		tsin = sin(i * pi / 180.0);

		glPushMatrix();
		glTranslatef(tsin * -250, 0, tcos * -250);
		glScalef(40 + ran * 6, 40 + ran * 6, 40 + ran * 6);
		//				glutSolidDodecahedron();	//12��
		glutSolidIcosahedron();	//20��
		glPopMatrix();
	}

	glEndList();

	//�Ǹ��� ���� 1��
	listID[5] = glGenLists(1);
	glNewList(listID[5], GL_COMPILE);
	cyl = gluNewQuadric();
	glColor3f(236 / 255.0, 77 / 255.0, 0 / 255.0);//����

	glBindTexture(GL_TEXTURE_2D, objs.textures[20]);			//��1
	//	glTexGeni(GL_Q, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	gluQuadricTexture(cyl, 2);

	glPushMatrix();
	glRotatef(-90, 1, 0, 0);
	gluCylinder(cyl, 0.5, 0.5, 6, 5, 5);	//���		

	glColor3f(100 / 255.0, 200 / 255.0, 100 / 255.0);//�ʷϻ�
	glTranslatef(0, 0, 3);
	gluCylinder(cyl, 3, 0.2, 6.0, 8, 8);	//��
	glTranslatef(0, 0, 2);
	gluCylinder(cyl, 3, 0.2, 6.0, 8, 8);	//��
	glTranslatef(0, 0, 2);
	gluCylinder(cyl, 3, 0.2, 6.0, 8, 8);	//��
	glPopMatrix();
	glBindTexture(GL_TEXTURE_2D, 0);			//�ؽ�ó ����
	glEndList();

	//�ܼ� ��
	listID[10] = glGenLists(1);
	glNewList(listID[10], GL_COMPILE);
	glBindTexture(GL_TEXTURE_2D, 0);	//�ؽ�ó ����
	glColor4f(0.5, 0.5, 0.5, 0.5);
	glPushMatrix();
	glTranslatef(0, 1.5, 0);

	//			glScalef(4,2,12);
	//			glutSolidCube(1);
	glScalef(2.5, 1, 6);
	glDrawCube();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0, 3.0, 0);
	//			glScalef(4,2,4);
	//			glutSolidCube(1);
	glScalef(2.5, 1, 2);
	glDrawCube();
	glPopMatrix();
	glEndList();
}

//����Ż Ʈ���� �׸���
void DrawFractalTree() {
	glPushMatrix();
	glTranslatef(-30, 0, 0);
	for (int i = 0; i < 7; i++) {
		glCallList(listID[3]);			//����Ż ����
		glTranslatef(0, 0, -10);
	}
	glPopMatrix();

	glPushMatrix();
	glTranslatef(30, 0, 0);
	for (int i = 0; i < 7; i++) {
		glCallList(listID[3]);			//����Ż ����
		glTranslatef(0, 0, -10);
	}
	glPopMatrix();
}

//�Ǹ��� Ʈ���� �׸���
void DrawCylTree() {
	glPushMatrix();				//������
	glTranslatef(30, 0, 300);
	for (int i = 0; i < 20; i++) {
		glCallList(listID[5]);//�Ǹ��� ����
		glTranslatef(0, 0, -30);
	}
	glPopMatrix();
	glPushMatrix();				//������2
	glTranslatef(35, 0, 297.5);
	for (int i = 0; i < 33; i++) {
		glCallList(listID[5]);//�Ǹ��� ����
		glTranslatef(0, 0, -16);
	}
	glPopMatrix();
	glPushMatrix();				//������3
	glTranslatef(40, 0, 295);
	for (int i = 0; i < 33; i++) {
		glCallList(listID[5]);//�Ǹ��� ����
		glTranslatef(0, 0, -8);
	}
	glPopMatrix();
	glPushMatrix();				//������4
	glTranslatef(45, 0, 292.5);
	for (int i = 0; i < 20; i++) {
		glCallList(listID[5]);//�Ǹ��� ����
		glTranslatef(0, 0, -27);
	}
	glPopMatrix();
	glPushMatrix();				//������5
	glTranslatef(50, 0, 290);
	for (int i = 0; i < 20; i++) {
		glCallList(listID[5]);//�Ǹ��� ����
		glTranslatef(0, 0, -22);
	}
	glPopMatrix();
	glPushMatrix();				//������6
	glTranslatef(55, 0, 287.5);
	for (int i = 0; i < 20; i++) {
		glCallList(listID[5]);//�Ǹ��� ����
		glTranslatef(0, 0, -18);
	}
	glPopMatrix();
	glPopMatrix();
	glPushMatrix();				//������7
	glTranslatef(60, 0, 285);
	for (int i = 0; i < 20; i++) {
		glCallList(listID[5]);//�Ǹ��� ����
		glTranslatef(0, 0, -30);
	}
	glPopMatrix();
	glPopMatrix();

	glPushMatrix();				//������9
	glTranslatef(70, 0, 280);
	for (int i = 0; i < 20; i++) {
		glCallList(listID[5]);//�Ǹ��� ����
		glTranslatef(0, 0, -8);
	}
	glPopMatrix();
	glPopMatrix();
	glPushMatrix();				//������10
	glTranslatef(75, 0, 277.5);
	for (int i = 0; i < 20; i++) {
		glCallList(listID[5]);//�Ǹ��� ����
		glTranslatef(0, 0, -27);
	}
	glPopMatrix();
	glPopMatrix();

	glPushMatrix();				//������12
	glTranslatef(85, 0, 272.5);
	for (int i = 0; i < 20; i++) {
		glCallList(listID[5]);//�Ǹ��� ����
		glTranslatef(0, 0, -18);
	}
	glPopMatrix();
	glPushMatrix();				//������13
	glTranslatef(90, 0, 270);
	for (int i = 0; i < 20; i++) {
		glCallList(listID[5]);//�Ǹ��� ����
		glTranslatef(0, 0, -30);
	}
	glPopMatrix();
	glPushMatrix();				//������14
	glTranslatef(95, 0, 267.5);
	for (int i = 0; i < 20; i++) {
		glCallList(listID[5]);//�Ǹ��� ����
		glTranslatef(0, 0, -16);
	}
	glPopMatrix();
	glPushMatrix();				//������15
	glTranslatef(100, 0, 265);
	for (int i = 0; i < 20; i++) {
		glCallList(listID[5]);//�Ǹ��� ����
		glTranslatef(0, 0, -8);
	}
	glPopMatrix();

	glPushMatrix();				//������17
	glTranslatef(110, 0, 260);
	for (int i = 0; i < 20; i++) {
		glCallList(listID[5]);//�Ǹ��� ����
		glTranslatef(0, 0, -22);
	}
	glPopMatrix();
	glPushMatrix();				//������18
	glTranslatef(115, 0, 257.5);
	for (int i = 0; i < 18; i++) {
		glCallList(listID[5]);//�Ǹ��� ����
		glTranslatef(0, 0, -18);
	}
	glPopMatrix();
	glPushMatrix();				//������19
	glTranslatef(120, 0, 255);
	for (int i = 0; i < 18; i++) {
		glCallList(listID[5]);//�Ǹ��� ����
		glTranslatef(0, 0, -30);
	}
	glPopMatrix();
	glPopMatrix();
	glPushMatrix();				//������20
	glTranslatef(130, 0, 250);
	for (int i = 0; i < 18; i++) {
		glCallList(listID[5]);//�Ǹ��� ����
		glTranslatef(0, 0, -16);
	}
	glPopMatrix();
	glPopMatrix();
	glPushMatrix();				//������21
	glTranslatef(135, 0, 247.5);
	for (int i = 0; i < 18; i++) {
		glCallList(listID[5]);//�Ǹ��� ����
		glTranslatef(0, 0, -8);
	}
	glPopMatrix();
	glPopMatrix();
	glPushMatrix();				//������22
	glTranslatef(140, 0, 245);
	for (int i = 0; i < 20; i++) {
		glCallList(listID[5]);//�Ǹ��� ����
		glTranslatef(0, 0, -22);
	}
	glPopMatrix();
	glPopMatrix();
	glPushMatrix();				//������23
	glTranslatef(145, 0, 242.5);
	for (int i = 0; i < 18; i++) {
		glCallList(listID[5]);//�Ǹ��� ����
		glTranslatef(0, 0, -18);
	}
	glPopMatrix();
	glPopMatrix();
	glPushMatrix();				//������24
	glTranslatef(150, 0, 240);
	for (int i = 0; i < 18; i++) {
		glCallList(listID[5]);//�Ǹ��� ����
		glTranslatef(0, 0, -30);
	}
	glPopMatrix();
	glPopMatrix();
	glPushMatrix();				//������25
	glTranslatef(160, 0, 237.5);
	for (int i = 0; i < 18; i++) {
		glCallList(listID[5]);//�Ǹ��� ����
		glTranslatef(0, 0, -16);
	}
	glPopMatrix();
	glPopMatrix();
	glPushMatrix();				//������26
	glTranslatef(165, 0, 235);
	for (int i = 0; i < 18; i++) {
		glCallList(listID[5]);//�Ǹ��� ����
		glTranslatef(0, 0, -8);
	}
	glPopMatrix();
	glPopMatrix();
	glPushMatrix();				//������27
	glTranslatef(170, 0, 232.5);
	for (int i = 0; i < 20; i++) {
		glCallList(listID[5]);//�Ǹ��� ����
		glTranslatef(0, 0, -27);
	}
	glPopMatrix();
	glPopMatrix();
	glPushMatrix();				//������28
	glTranslatef(175, 0, 230);
	for (int i = 0; i < 20; i++) {
		glCallList(listID[5]);//�Ǹ��� ����
		glTranslatef(0, 0, -22);
	}
	glPopMatrix();
	glPopMatrix();
	glPushMatrix();				//������29
	glTranslatef(180, 0, 227.5);
	for (int i = 0; i < 18; i++) {
		glCallList(listID[5]);//�Ǹ��� ����
		glTranslatef(0, 0, -18);
	}
	glPopMatrix();
	glPopMatrix();
	glPushMatrix();				//������30
	glTranslatef(190, 0, 225);
	for (int i = 0; i < 18; i++) {
		glCallList(listID[5]);//�Ǹ��� ����
		glTranslatef(0, 0, -30);
	}
	glPopMatrix();
	glPushMatrix();				//������31
	glTranslatef(195, 0, 222.5);
	for (int i = 0; i < 18; i++) {
		glCallList(listID[5]);//�Ǹ��� ����
		glTranslatef(0, 0, -16);
	}
	glPopMatrix();
	glPushMatrix();				//������32
	glTranslatef(200, 0, 220);
	for (int i = 0; i < 18; i++) {
		glCallList(listID[5]);//�Ǹ��� ����
		glTranslatef(0, 0, -8);
	}
	glPopMatrix();
	glPushMatrix();				//������33
	glTranslatef(205, 0, 217.5);
	for (int i = 0; i < 18; i++) {
		glCallList(listID[5]);//�Ǹ��� ����
		glTranslatef(0, 0, -27);
	}
	glPopMatrix();
	glPushMatrix();				//������34
	glTranslatef(205, 0, 215.5);
	for (int i = 0; i < 18; i++) {
		glCallList(listID[5]);//�Ǹ��� ����
		glTranslatef(0, 0, -30);
	}
	glPopMatrix();

	glPushMatrix();				//����
	glTranslatef(-30, 0, 300);
	for (int i = 0; i < 20; i++) {
		glCallList(listID[5]);//�Ǹ��� ����
		glTranslatef(0, 0, -30);
	}
	glPopMatrix();
	glPushMatrix();				//����2
	glTranslatef(-35, 0, 297.5);
	for (int i = 0; i < 33; i++) {
		glCallList(listID[5]);//�Ǹ��� ����
		glTranslatef(0, 0, -35);
	}
	glPopMatrix();
	glPushMatrix();				//����3
	glTranslatef(-40, 0, 295);
	for (int i = 0; i < 33; i++) {
		glCallList(listID[5]);//�Ǹ��� ����
		glTranslatef(0, 0, -8);
	}
	glPopMatrix();
	glPushMatrix();				//����4
	glTranslatef(-45, 0, 292.5);
	for (int i = 0; i < 20; i++) {
		glCallList(listID[5]);//�Ǹ��� ����
		glTranslatef(0, 0, -27);
	}
	glPopMatrix();
	glPushMatrix();				//����5
	glTranslatef(-50, 0, 290);
	for (int i = 0; i < 20; i++) {
		glCallList(listID[5]);//�Ǹ��� ����
		glTranslatef(0, 0, -22);
	}
	glPopMatrix();
	glPushMatrix();				//����6
	glTranslatef(-55, 0, 287.5);
	for (int i = 0; i < 20; i++) {
		glCallList(listID[5]);//�Ǹ��� ����
		glTranslatef(0, 0, -18);
	}
	glPopMatrix();
	glPopMatrix();
	glPushMatrix();				//����7
	glTranslatef(-60, 0, 285);
	for (int i = 0; i < 20; i++) {
		glCallList(listID[5]);//�Ǹ��� ����
		glTranslatef(0, 0, -30);
	}
	glPopMatrix();
	glPopMatrix();

	glPushMatrix();				//����9
	glTranslatef(-70, 0, 280);
	for (int i = 0; i < 20; i++) {
		glCallList(listID[5]);//�Ǹ��� ����
		glTranslatef(0, 0, -8);
	}
	glPopMatrix();
	glPopMatrix();
	glPushMatrix();				//����10
	glTranslatef(-75, 0, 277.5);
	for (int i = 0; i < 20; i++) {
		glCallList(listID[5]);//�Ǹ��� ����
		glTranslatef(0, 0, -27);
	}
	glPopMatrix();
	glPopMatrix();

	glPushMatrix();				//����12
	glTranslatef(-85, 0, 272.5);
	for (int i = 0; i < 20; i++) {
		glCallList(listID[5]);//�Ǹ��� ����
		glTranslatef(0, 0, -18);
	}
	glPopMatrix();
	glPushMatrix();				//����13
	glTranslatef(-90, 0, 270);
	for (int i = 0; i < 20; i++) {
		glCallList(listID[5]);//�Ǹ��� ����
		glTranslatef(0, 0, -30);
	}
	glPopMatrix();
	glPushMatrix();				//����14
	glTranslatef(-95, 0, 267.5);
	for (int i = 0; i < 20; i++) {
		glCallList(listID[5]);//�Ǹ��� ����
		glTranslatef(0, 0, -16);
	}
	glPopMatrix();
	glPushMatrix();				//����15
	glTranslatef(-100, 0, 265);
	for (int i = 0; i < 20; i++) {
		glCallList(listID[5]);//�Ǹ��� ����
		glTranslatef(0, 0, -8);
	}
	glPopMatrix();

	glPushMatrix();				//����17
	glTranslatef(-110, 0, 260);
	for (int i = 0; i < 20; i++) {
		glCallList(listID[5]);//�Ǹ��� ����
		glTranslatef(0, 0, -22);
	}
	glPopMatrix();
	glPushMatrix();				//����18
	glTranslatef(-115, 0, 257.5);
	for (int i = 0; i < 18; i++) {
		glCallList(listID[5]);//�Ǹ��� ����
		glTranslatef(0, 0, -18);
	}
	glPopMatrix();
	glPushMatrix();				//����19
	glTranslatef(-120, 0, 255);
	for (int i = 0; i < 18; i++) {
		glCallList(listID[5]);//�Ǹ��� ����
		glTranslatef(0, 0, -30);
	}
	glPopMatrix();
	glPopMatrix();
	glPushMatrix();				//����20
	glTranslatef(-130, 0, 250);
	for (int i = 0; i < 18; i++) {
		glCallList(listID[5]);//�Ǹ��� ����
		glTranslatef(0, 0, -16);
	}
	glPopMatrix();
	glPopMatrix();
	glPushMatrix();				//����21
	glTranslatef(-135, 0, 247.5);
	for (int i = 0; i < 18; i++) {
		glCallList(listID[5]);//�Ǹ��� ����
		glTranslatef(0, 0, -8);
	}
	glPopMatrix();
	glPopMatrix();
	glPushMatrix();				//����22
	glTranslatef(-140, 0, 245);
	for (int i = 0; i < 20; i++) {
		glCallList(listID[5]);//�Ǹ��� ����
		glTranslatef(0, 0, -20);
	}
	glPopMatrix();
	glPopMatrix();
	glPushMatrix();				//����23
	glTranslatef(-145, 0, 242.5);
	for (int i = 0; i < 18; i++) {
		glCallList(listID[5]);//�Ǹ��� ����
		glTranslatef(0, 0, -10);
	}
	glPopMatrix();
	glPopMatrix();
	glPushMatrix();				//����24
	glTranslatef(-150, 0, 240);
	for (int i = 0; i < 18; i++) {
		glCallList(listID[5]);//�Ǹ��� ����
		glTranslatef(0, 0, -15);
	}
	glPopMatrix();
	glPopMatrix();
	glPushMatrix();				//����25
	glTranslatef(-160, 0, 237.5);
	for (int i = 0; i < 18; i++) {
		glCallList(listID[5]);//�Ǹ��� ����
		glTranslatef(0, 0, -20);
	}
	glPopMatrix();
	glPopMatrix();
	glPushMatrix();				//����26
	glTranslatef(-165, 0, 235);
	for (int i = 0; i < 18; i++) {
		glCallList(listID[5]);//�Ǹ��� ����
		glTranslatef(0, 0, -8);
	}
	glPopMatrix();
	glPopMatrix();
	glPushMatrix();				//����27
	glTranslatef(-170, 0, 232.5);
	for (int i = 0; i < 20; i++) {
		glCallList(listID[5]);//�Ǹ��� ����
		glTranslatef(0, 0, -27);
	}
	glPopMatrix();
	glPopMatrix();
	glPushMatrix();				//����28
	glTranslatef(-175, 0, 230);
	for (int i = 0; i < 20; i++) {
		glCallList(listID[5]);//�Ǹ��� ����
		glTranslatef(0, 0, -22);
	}
	glPopMatrix();
	glPopMatrix();
	glPushMatrix();				//����29
	glTranslatef(-180, 0, 227.5);
	for (int i = 0; i < 18; i++) {
		glCallList(listID[5]);//�Ǹ��� ����
		glTranslatef(0, 0, -18);
	}
	glPopMatrix();
	glPopMatrix();
	glPushMatrix();				//����30
	glTranslatef(-190, 0, 225);
	for (int i = 0; i < 18; i++) {
		glCallList(listID[5]);//�Ǹ��� ����
		glTranslatef(0, 0, -20);
	}
	glPopMatrix();
	glPushMatrix();				//����31
	glTranslatef(-195, 0, 222.5);
	for (int i = 0; i < 18; i++) {
		glCallList(listID[5]);//�Ǹ��� ����
		glTranslatef(0, 0, -16);
	}
	glPopMatrix();
	glPushMatrix();				//����32
	glTranslatef(-200, 0, 220);
	for (int i = 0; i < 18; i++) {
		glCallList(listID[5]);//�Ǹ��� ����
		glTranslatef(0, 0, -10);
	}
	glPopMatrix();
	glPushMatrix();				//����33
	glTranslatef(-205, 0, 217.5);
	for (int i = 0; i < 18; i++) {
		glCallList(listID[5]);//�Ǹ��� ����
		glTranslatef(0, 0, -27);
	}
	glPopMatrix();
	glPushMatrix();				//����34
	glTranslatef(-205, 0, 215.5);
	for (int i = 0; i < 18; i++) {
		glCallList(listID[5]);//�Ǹ��� ����
		glTranslatef(0, 0, -30);
	}
	glPopMatrix();

}

void DrawBoards() {
	glPushMatrix();

	glPopMatrix();
}

//Ű�����ݹ�
void MyKeyboard(unsigned char key, int x, int y)
{
	//	printf("key = %d \n",key);

	switch (key) {
	case 'q': case 'Q': case '\033':
		exit(0);
		break;
	case 't': case 'T':		//�÷����̵�, ���ν��̵� ��۸�
		if (FlatShaded) {
			FlatShaded = 0;
			glShadeModel(GL_SMOOTH);
		}
		else {
			FlatShaded = 1;
			glShadeModel(GL_FLAT);
		}
		break;
	case 'r': case 'R':	//���̾������� ��۸�
		if (Wireframed) {
			Wireframed = 0;
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
		else {
			Wireframed = 1;//			
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}
		break;
	case 52:		//pad ��
		objs.AtX -= .5;
		break;
	case 56:		//pad ��
		objs.AtY += .5;
		break;
	case 54:		//pad ��
		objs.AtX += .5;
		break;
	case 50:		//pad ��
		objs.AtY -= .5;
		break;
	case 43:		//pad +
		objs.AtZ += .5;
		break;
	case 45:		//pad -
		objs.AtZ -= .5;
		break;
	case 48:		//pad 0
		objs.EyeZ += 1;
		break;
	case 46:		//pad .
		objs.EyeZ -= 1;
		break;
	case 'z': case 'Z':					//����
		objs.EyeX = objs.EyeX + 0.1 * (objs.AtX - objs.EyeX);
		objs.EyeY = objs.EyeY + 0.1 * (objs.AtY - objs.EyeY);
		objs.EyeZ = objs.EyeZ + 0.1 * (objs.AtZ - objs.EyeZ);
		break;
	case 'x': case 'X':					//�ܾƿ�
		objs.EyeX = objs.EyeX - 0.1 * (objs.AtX - objs.EyeX);
		objs.EyeY = objs.EyeY - 0.1 * (objs.AtY - objs.EyeY);
		objs.EyeZ = objs.EyeZ - 0.1 * (objs.AtZ - objs.EyeZ);
		break;
	case 'f': case 'F':					//�Ȱ���庯��
		if (isFogDraw == false) {
			isFogDraw = true;
			glEnable(GL_FOG);		//�Ȱ� ��
		}
		else {
			isFogDraw = false;
			glDisable(GL_FOG);	//�Ȱ� ��
		}
		//fogFilter++;
		//if(fogFilter == 1){
		//	glDisable(GL_FOG);	//�Ȱ� ��
		//}else if(fogFilter > 1){
		//	fogFilter = 0;
		//	glFogi (GL_FOG_MODE, fogMode[fogFilter]);	//�Ȱ�����
		//	glEnable(GL_FOG);		//�Ȱ� ��
		//}

		break;
	case 'g': case 'G':					//Ʈ�� �׸��� ��� ���
		treeDrawMode = ++treeDrawMode % 3;
		break;
	case 'v': case 'V':					//�� �׸��� ��� 0:�ܼ�   1:�ȱ׸�
		mountainDrawMode = ++mountainDrawMode % 2;
		break;
	case 'c': case 'C':					//��ƼŬ �׸��� ���
		if (isParticleDraw == false) {
			isParticleDraw = true;
		}
		else {
			isParticleDraw = false;
		}
		break;
	case 'n': case 'N':					//������ ������ ���������
		objs.EyeX = objs.EyeX + 0.1 * (-objs.EyeX);
		objs.EyeY = objs.EyeY + 0.1 * (-objs.EyeY);
		objs.EyeZ = objs.EyeZ + 0.1 * (-objs.EyeZ);
		break;
	case 'm': case 'M':					//������ �������� �־�����
		objs.EyeX = objs.EyeX - 0.1 * (-objs.EyeX);
		objs.EyeY = objs.EyeY - 0.1 * (-objs.EyeY);
		objs.EyeZ = objs.EyeZ - 0.1 * (-objs.EyeZ);
		break;
	case 'p': case 'P':					//�Ͻ����� ���� ��Ʈ��
		if (isPaused == FALSE) {
			isPaused = TRUE;
			for (int i = 0; i < MAX_ANINUM; i++) {	//������ �Ͻ�����
				SuspendThread(Thread[i]);
			}
		}
		else {
			isPaused = FALSE;
			for (int i = 0; i < MAX_ANINUM; i++) {	//������ ����
				ResumeThread(Thread[i]);
			}
		}
		break;
	case 'e': case 'E':					//����, ������ǥ�� ���� �׸��� ���
		if (isAtDraw == FALSE) {
			isAtDraw = TRUE;
		}
		else {
			isAtDraw = FALSE;
		}
		break;
	case 'h': case 'H':					//���� ON/OFF
		if (isHelpDraw == false) {
			isHelpDraw = true;
			glDisable(GL_FOG);		//�Ȱ� ��
		}
		else {
			isHelpDraw = false;
			if (isFogDraw == true) {
				glEnable(GL_FOG);		//�Ȱ� ��
			}
		}
		break;
	case 93:						//] Ű ������, ������ �����ð� ����(�ִϸ��̼� ������)
		sleepSpeed -= 17;
		if (sleepSpeed < 17) {
			sleepSpeed = 17;
		}
		break;
	case 91:						//[ Ű ������, ������ �����ð� ����(�ִϸ��̼� ������)
		sleepSpeed += 17;
		if (sleepSpeed > 200) {
			sleepSpeed = 200;
		}
		break;


	case 'a': case 'A':
		objs.people.at(0)->keyHold[1] = 1;
		if (objs.people.at(0)->keyDoubleTouchLevel[1] % 2 == 0) {
			objs.people.at(0)->keyDoubleTouch[1] = keyDoubleTouchTime;	//Ű �����Է� ������ �迭 ����
			objs.people.at(0)->keyDoubleTouchLevel[1]++;
		}
		break;
	case 's': case 'S':
		objs.people.at(0)->keyHold[2] = 1;
		if (objs.people.at(0)->keyDoubleTouchLevel[2] % 2 == 0) {
			objs.people.at(0)->keyDoubleTouch[2] = keyDoubleTouchTime;	//Ű �����Է� ������ �迭 ����
			objs.people.at(0)->keyDoubleTouchLevel[2]++;
		}
		break;
	case 'd': case 'D':
		objs.people.at(0)->keyHold[3] = 1;
		if (objs.people.at(0)->keyDoubleTouchLevel[3] % 2 == 0) {
			objs.people.at(0)->keyDoubleTouch[3] = keyDoubleTouchTime;	//Ű �����Է� ������ �迭 ����
			objs.people.at(0)->keyDoubleTouchLevel[3]++;
		}
		break;
	case 'j': case 'J':			//��ġ
		objs.people.at(0)->AttKeyHold[0] = 1;	//Ű���� ����
		if (objs.people.at(0)->ctrl) {
			if (objs.people.at(0)->attackNum == 0) {
				objs.people.at(0)->actNo = RPUNCH;
				objs.people.at(0)->ctrl = 0;
			}
			else if (objs.people.at(0)->attackNum == 1) {
				objs.people.at(0)->actNo = LPUNCH;
				objs.people.at(0)->ctrl = 0;
			}
			else if (objs.people.at(0)->attackNum == 2) {
				objs.people.at(0)->actNo = RUPPER;
				objs.people.at(0)->ctrl = 0;
			}
		}
		break;
	case 'k': case 'K':
		objs.people.at(0)->AttKeyHold[1] = 1;	//Ű���� ����
		if (objs.people.at(0)->ctrl) {
			objs.people.at(0)->actNo = RUPPER;
			objs.people.at(0)->ctrl = 0;
		}

		break;
	case 'l': case 'L':
		objs.people.at(0)->AttKeyHold[2] = 1;	//Ű���� ����
		break;
	case 32:		//�����̽�
		break;
	}

	//if ( GetAsyncKeyState(VK_) & 0x8000 )  {
	//	cout<<"w \n";
	//}

//	p1ActionChange();	//p1�ൿ�ϰ�

	CalculateRadius();
	glutPostRedisplay();
}

//����Ű�� �ݹ�
void Specialkeys(int key, int x, int y)
{
	//	printf("Special Key = %d \n",key);

	switch (key) {
	case 1:		//F1		�� HP 1��
		if (objs.gameState == 1) {	//�������̸�
			for (unsigned int i = 1; i < objs.people.size(); i++) {	//���迭 ����
				objs.people.at(i)->HP = 1;				//ü�¼�
			}
		}
		break;
	case 2:		//F2		���ΰ� HP 1��		
		if (objs.gameState == 1) {
			objs.people.at(0)->HP = 1;				//ü�¼�		
		}
		break;
	case 3:		//F3		���ΰ� HP ä��
		if (objs.gameState == 1) {
			objs.people.at(0)->HP = 1000;				//ü�¼�	
			if (isBgmPlay == true) {
				mciSendString(("play " + soundBGM1 + " repeat").c_str(), 0, 0, 0);//play bgm
			}
		}
		break;
		//case 4:		//F4		�ӽ�(����ȣ��)
		//	Thread[0] = (HANDLE)_beginthreadex(NULL, 0, AniEnding, (void*)NULL, 0, &ThreadState[0]);	//������� �Լ�ȣ��
		//		objs.gameState = 3;
		//	break;
	case 100:		//��		//�����̵�
		objs.EyeX -= .5;
		//		temp1 -= 0.03;
		break;
	case 101:		//��
		objs.EyeY += .5;
		//		temp2 += 0.03;
		break;
	case 102:		//��
		objs.EyeX += .5;
		//		temp1 += 0.03;
		break;
	case 103:		//��
		objs.EyeY -= .5;
		//		temp2 -= 0.03;
		break;
	case 104:		//Pgup
		posi1[0] += 1;
		break;
	case 108:		//Insert
		posi1[0] -= 1;

		break;
	case 106:		//Home
		posi1[1] += 1;
		break;
	case 107:		//End
		posi1[1] -= 1;
		break;
	}

	CalculateRadius();
	glutPostRedisplay();	//ȭ�� ���ΰ�ħ
}

//Ű���� ��
void MyKeyboardUp(unsigned char key, int x, int y) {
	//	printf("keyUp = %d \n",key);

	switch (key) {


	case 'a': case 'A':
		objs.people.at(0)->keyHold[1] = 0;
		if (objs.people.at(0)->keyDoubleTouchLevel[1] == 1) {
			objs.people.at(0)->keyDoubleTouchLevel[1] = 2;
		}
		break;
	case 's': case 'S':
		objs.people.at(0)->keyHold[2] = 0;
		if (objs.people.at(0)->keyDoubleTouchLevel[2] == 1) {
			objs.people.at(0)->keyDoubleTouchLevel[2] = 2;
		}
		break;
	case 'd': case 'D':
		objs.people.at(0)->keyHold[3] = 0;
		if (objs.people.at(0)->keyDoubleTouchLevel[3] == 1) {
			objs.people.at(0)->keyDoubleTouchLevel[3] = 2;
		}
		break;
		//case 'j': case 'J':			//��ġ
		//	objs.people.at(0)->AttKeyHold[0] = 0;	//Ű�� ����
		//	break;
		//case 'k': case 'K':
		//	objs.people.at(0)->AttKeyHold[1] = 0;	//Ű���� ����
		//	break;
		//case 'l': case 'L':
		//	objs.people.at(0)->AttKeyHold[2] = 0;	//Ű���� ����
		//	break;
	}

	p1ActionChange();	//p1�ൿ�Լ� ȣ��
}

//���ΰ� �������ۿ� ���� �ൿ�� ��Ű�� �Լ�
void p1ActionChange() {
	up1 = objs.people.at(0)->keyHold[0];
	left1 = objs.people.at(0)->keyHold[1];
	down1 = objs.people.at(0)->keyHold[2];
	right1 = objs.people.at(0)->keyHold[3];

	up2 = (objs.people.at(0)->keyDoubleTouchLevel[0] == 3) ? 1 : 0;	//�����Է� �Ǿ��� ������
	left2 = (objs.people.at(0)->keyDoubleTouchLevel[1] == 3) ? 1 : 0;
	down2 = (objs.people.at(0)->keyDoubleTouchLevel[2] == 3) ? 1 : 0;
	right2 = (objs.people.at(0)->keyDoubleTouchLevel[3] == 3) ? 1 : 0;

	if (objs.people.at(0)->ctrl) {	//���۰����Ҷ��� ���⿡ ���� �̵�����

		if (up2 || left2 || right2 || down2) {		//�޸��� �˻�
			objs.people.at(0)->aniNo = ANI_RUN;
			for (int i = 0; i < 4; i++) {
				objs.people.at(0)->keyDoubleTouchLevel[i] = -1;
			}
		}

		if (up1 && !left1 && !right1) {			//�ȱ� �˻�. ����
			objs.people.at(0)->actNo = WALK_FRONT;
		}
		else if (up1 && left1) {						//��
			objs.people.at(0)->actNo = WALK_FRONT_LEFT;
		}
		else if (up1 && right1) {						//��
			objs.people.at(0)->actNo = WALK_FRONT_RIGHT;
		}
		else if (left1 && !right1 && !up1 && !down1) {	//����
			objs.people.at(0)->actNo = WALK_LEFT;
		}
		else if (!left1 && right1 && !up1 && !down1) {	//������
			objs.people.at(0)->actNo = WALK_RIGHT;
		}
		else if (down1 && !left1 && !right1) {			//�Ʒ�
			objs.people.at(0)->actNo = WALK_BACK;
		}
		else if (down1 && left1) {					//��
			objs.people.at(0)->actNo = WALK_BACK_LEFT;
		}
		else if (down1 && right1) {					//��
			objs.people.at(0)->actNo = WALK_BACK_RIGHT;
		}
		else {
			objs.people.at(0)->actNo = IDLE;
		}
	}

	//	printf("%d %d %d %d \n",up2,left2,right2,down2);
}

//��Ŭ�� ���θ޴�
void MyMainMenu(int entryID) {
	if (isCanMouse == false) {
		return;
	}

	switch (entryID) {
	case 50:			//��Ʈ�� �ִϸ��̼�
		InitPosition();	//��ü ��ġ�ʱ�ȭ

		//for(int i=0; i < AniNum; i++){			
		//	GetExitCodeThread(Thread[i], NULL);	//
		//	TerminateThread(Thread[i], NULL);		//������ ��������
		//	CloseHandle(Thread[i]);					//������ ����
		//}

		isLifeBarDraw = false;
		Thread[0] = (HANDLE)_beginthreadex(NULL, 0, AniIntro, (void*)NULL, 0, &ThreadState[0]);	//������� �Լ�ȣ��
		break;
	case 100:			//���� �ִϸ��̼�
		InitPosition();	//��ü ��ġ�ʱ�ȭ
		break;
	case 800:	//������Ʈ �׽�Ʈ (AI on)
		InitPosition();	//��ü ��ġ�ʱ�ȭ
		objs.gameState = 1;
		isLifeBarDraw = true;
		objs.isAniControl = false;//�ִ����� off
		if (isBgmPlay == true) {
			mciSendString(("play " + soundBGM1 + " repeat").c_str(), 0, 0, 0);//play bgm
		}

		objs.people.at(0)->x = -40.6;//�ӽ�
		objs.people.at(0)->z = -38;//
		objs.EyeX = objs.people.at(0)->x + 10;		//������
		objs.EyeZ = objs.people.at(0)->z; // + 5;
		objs.AtX = objs.people.at(0)->x;		//������
		objs.AtZ = objs.people.at(0)->z;



		objs.people.at(0)->actNo = IDLE;	//���ΰ� ������ �� �ְ�
		objs.people.at(0)->ctrl = 1;
		objs.people.at(0)->isDraw = true;		//�׸��� ����
		objs.people.at(0)->HP = 1000;

		break;
	case 850:	//������Ʈ �׽�Ʈ (AI off)
		InitPosition();	//��ü ��ġ�ʱ�ȭ
		objs.gameState = 1;
		isLifeBarDraw = true;
		objs.isAniControl = false;	//�ִ����� off
		if (isBgmPlay == true) {
			mciSendString(("play " + soundBGM1 + " repeat").c_str(), 0, 0, 0);//play bgm
		}

		objs.people.at(0)->x = -40.6;//�ӽ�
		objs.people.at(0)->z = -38;//
		objs.EyeX = objs.people.at(0)->x + 10;		//������
		objs.EyeZ = objs.people.at(0)->z; // + 5;
		objs.AtX = objs.people.at(0)->x;		//������
		objs.AtZ = objs.people.at(0)->z;


		objs.people.at(0)->actNo = IDLE;	//���ΰ� ������ �� �ְ�
		objs.people.at(0)->ctrl = 1;
		objs.people.at(0)->isDraw = true;		//�׸��� ����
		objs.people.at(0)->HP = 1000;

		break;
	case 900:		//���(�ʱ�ȭ)
		InitPosition();	//��ü ��ġ�ʱ�ȭ
		objs.gameState = 0;
		isLifeBarDraw = false;
		isCarDraw = false;		//���̷�ȿ��
		isCyren = false;		//���̷�ȿ��
		car->Init();
		objs.isAniControl = false;
		mciSendString(("stop " + soundBGM1).c_str(), 0, 0, 0);//stop bgm



		objs.gameState = 0;	//��Ȳ����0
		break;
	}
}

//��Ŭ�� ����޴�
void MySubMenu(int entryID) {
	switch (entryID) {
	case 4:
		isBgmPlay = true;		//BGM on
		if (objs.gameState == 1) {
			mciSendString(("play " + soundBGM1 + " repeat").c_str(), 0, 0, 0);//play bgm
		}
		break;
	case 5:
		mciSendString(("stop " + soundBGM1).c_str(), 0, 0, 0);//stop bgm
		isBgmPlay = false;		//BGM off
		break;
	}
}

//���콺 Ŭ���ݹ�(��������, ������)
void MyMouseMove(int button, int state, GLint X, GLint Y)
{
	if (isCanMouse == false) {
		return;
	}

	// ���콺 �������� X, Y�� ������ �Ҵ�
	if (state == 0 && button == 0) {	//��push��
		oldX = X;
		oldY = Y;
		plusX = 0;
		plusY = 0;

		//	CalculateEyeXY();
//		printf("ViewX = %d, ViewY = %d \n", ViewX, ViewY);

		//	glutPostRedisplay();
	}

	if (state == 1) {		//��ưrelease ��
		CalculateRadius();		//����-���� �Ÿ����� ����
	}
}
//���콺 ����Ŭ����
void MyMouseMove2(int button, int state, GLint X, GLint Y)
{
	if (state == 0 && button == 0) {	//��push��
		oldX = X;
		oldY = Y;
		plusX = 0;
		plusY = 0;
	}
	if (state == 1) {		//��ưrelease ��
		CalculateRadius();		//����-���� �Ÿ����� ����
	}
}

//���콺 Ŭ���̵��ݹ�
void MyMotion(GLint X, GLint Y)
{
	if (isCanMouse == false) {
		return;
	}

	int diffX = (X - oldX);		//��ǥ���� ������ ���� 4��
	int diffY = -(Y - oldY);
	oldX = X;
	oldY = Y;

	plusX += diffX * (180.0 / width);	//���Ұ��� ���
	plusY += diffY * (180.0 / height);

	double angleX = objs.eyeAngleX + plusX;	//������� ���
	double angleY = objs.eyeAngleY + plusY;

	EyePosChange(angleX, angleY);
	//printf("MViewX = %d, MViewY = %d \n", X, Y);
	//printf("eyeAngleX = %f \n",objs.eyeAngleX);
	//printf("eyeAngleY = %f \n",objs.eyeAngleY);
	//printf("AngleX = %f \n",angleX);
	//printf("AngleY = %f \n",angleY);
}
//���콺 ����Ŭ���̵���
void MyMotion2(GLint X, GLint Y)
{
	int diffX = (X - oldX);		//��ǥ���� ������ ���� 4��
	int diffY = -(Y - oldY);
	oldX = X;
	oldY = Y;

	plusX += diffX * (180.0 / width);	//���Ұ��� ���
	plusY += diffY * (180.0 / height);

	double angleX = objs.eyeAngleX + plusX;	//������� ���
	double angleY = objs.eyeAngleY + plusY;

	EyePosChange(angleX, angleY);
}

//�����̵�(MyMotion���� �и�. �ִϿ��� ��Ʈ�Ѱ����� �ϱ�����)
void EyePosChange(GLint angleX, GLint angleY) {
	if (angleY >= 89) {		//���� ���� ����
		angleY = 89;
	}
	else if (angleY <= 1) {
		angleY = 1;
	}

	double xPosX = radius * cos(angleX * pi / 180.0);		//���Ⱥ�ȯ�Ͽ� �������κ����� �¿� x���� ����
	double xPosY = radius * sin(angleX * pi / 180.0);		//���Ⱥ�ȯ�Ͽ� �������κ����� �¿� Y���� ����
	double yPosX = radius * cos(angleY * pi / 180.0);		//���Ⱥ�ȯ�Ͽ� �������κ����� ���� x���� ����
	double yPosY = radius * sin(angleY * pi / 180.0);		//���Ⱥ�ȯ�Ͽ� �������κ����� ���� Y���� ����

	double PosZ = yPosX * cos(angleX * pi / 180.0);		//�¿�,���� ���յ� ��ġ�κ��� Z�̵���ġ ���
	double PosX = PosZ * tan(angleX * pi / 180.0);		//�¿�,���� ���յ� ��ġ�κ��� X�̵���ġ ���

	objs.EyeZ = objs.AtZ + PosZ;			//���� ������ǥ����
	objs.EyeX = objs.AtX + PosX;//xPosY;
	objs.EyeY = objs.AtY + yPosY;
}

//���콺 �׳��̵��ݹ�(���� �̻��)
void MyPassiveMotion(GLint X, GLint Y) {

	double angleX = objs.eyeAngleX + (X - width / 2) / (width / 2 / 180.0);		//�¿찢������ -180 ~ 180
	double angleY = objs.eyeAngleY + -((Y - height / 2) / (height / 2 / 180.0));	//���ϰ�������

	double xPosX = radius * cos(angleX * pi / 180.0);		//���Ⱥ�ȯ�Ͽ� �������κ����� �¿� x���� ����
	double xPosY = radius * sin(angleX * pi / 180.0);		//���Ⱥ�ȯ�Ͽ� �������κ����� �¿� Y���� ����
	double yPosX = radius * cos(angleY * pi / 180.0);		//���Ⱥ�ȯ�Ͽ� �������κ����� ���� x���� ����
	double yPosY = radius * sin(angleY * pi / 180.0);		//���Ⱥ�ȯ�Ͽ� �������κ����� ���� Y���� ����

	double PosZ = yPosX * cos(angleX * pi / 180.0);		//�¿�,���� ���յ� ��ġ�κ��� Z�̵���ġ ���
	double PosX = PosZ * tan(angleX * pi / 180.0);		//�¿�,���� ���յ� ��ġ�κ��� X�̵���ġ ���

	//EyeZ = AtZ + xPosX;	//�¿�θ� �̵���
	//EyeX = AtX + xPosY;

	//EyeZ = AtZ + yPosX;	//���Ϸθ� �̵���
	//EyeY = AtY + yPosY;

	objs.EyeZ = objs.AtZ + PosZ;			//���� ������ǥ����
	objs.EyeX = objs.AtX + PosX;//xPosY;
	objs.EyeY = objs.AtY + yPosY;

	//printf("MouseX = %f, MouseY = %f Radius = %f \n", angleX, angleY, radius);

	//eyeAngle[0] = angleY;
	//eyeAngle[1] = angleX;

	//printf("eyeAngleX = %f \n",eyeAngleX);
	//printf("eyeAngleY = %f \n",eyeAngleY);
	//printf("AngleX = %f \n",angleX);
	//printf("AngleY = %f \n",angleY);

	glutPostRedisplay();
}

//EyeX, EyeY�� ViewX,Y�κ��� ���
void CalculateEyeXY() {
	width = glutGet(GLUT_WINDOW_WIDTH);	//������ ��ġ����
	height = glutGet(GLUT_WINDOW_HEIGHT);

	objs.EyeX = (ViewX - width / 2) / width * 20 + 2.5;			//�������
	objs.EyeY = (height / 2 - ViewY) / height * 20 + 2.5;
}

//����-������ �Ÿ�/���� ����
void CalculateRadius() {
	radius = sqrt(pow((objs.EyeX - objs.AtX), 2) + pow((objs.EyeY - objs.AtY), 2) + pow((objs.EyeZ - objs.AtZ), 2));//����-������ �Ÿ�����

	double XX = (objs.EyeZ - objs.AtZ);								//����-������ �� ������ ����
	double YX = sqrt(pow((objs.EyeX - objs.AtX), 2) + pow((objs.EyeZ - objs.AtZ), 2));

	objs.eyeAngleX = acos(XX / YX) * 180.0 / pi;			//�����̸� �̿��� ���ﰢ�Լ��� ����, ������ ��������.
	objs.eyeAngleY = acos(YX / radius) * 180.0 / pi;

	if (objs.EyeX < objs.AtX) {
		objs.eyeAngleX = -objs.eyeAngleX;
	}
	if (objs.EyeY < objs.AtY) {
		objs.eyeAngleY = -objs.eyeAngleY;
	}

	//printf("eyeAngleX = %f \n",objs.eyeAngleX);
	//printf("eyeAngleY = %f \n",objs.eyeAngleY);
}

//ȭ������ �ݹ�
void MyReshape(int w, int h) {
	//	printf("width = %d, height = %d \n", w, h);

	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);
}

//�������
void RenderHelp()
{
	glEnable(GL_LIGHT7);	//��� �ϱ�� ����

	const short textsMax = 26;	//���� ��
	int len[textsMax];
	string texts[textsMax];
	char* text[textsMax];

	float posX = -0.8, posY = 0.7;

	//texts[0] = "- ���콺��Ŭ�� : �޴�";
	//texts[1] = "- E : WCS������ ������ �� ǥ�� & �������� / �ǰ����� �� ǥ��";
	//texts[2] = "- R : ���̾�������/�ָ��巻���� ���";
	//texts[3] = "- T : �÷�/���� ���̵� ���";
	//texts[4] = "- Z, X : ����/�ܾƿ�";
	//texts[5] = "- F, G : ������ ������ ���� �̵�/�־�����";
	//texts[6] = "- ] : �ִϸ��̼� �ӵ� ������ (�����ð� ����)";
	//texts[7] = "- [ : �ִϸ��̼� �ӵ� ������ (�����ð� ����)";
	//texts[8] = "- P : �ִϸ��̼� �Ͻ�����/���";
	//texts[9] = "- ȭ��ǥŰ, Ű�е�0, Ű�е�Delete : �����̵�";
	//texts[10] = "- Ű�е��� ����Ű, Ű�е�+,- : �����̵�";
	//texts[11] = "- ���콺 Ŭ��-�巡�� : ����ȸ�� (�ﰢ�Լ� �̿�)";
	//texts[12] = "- Q : ����";
	//texts[13] = "";
	//texts[14] = "------------------------ ���ΰ� ���� ---------------------------------";
	//texts[15] = "- W,A,S,D (����Ű): �̵�";
	//texts[16] = "- �������� 2���Է� : �޸���";
	//texts[17] = "- J : ����";
	texts[0] = "- Right Click : Menu";
	texts[1] = "- E           : Draw Shperes at View/Focus point. Draw Collision Shperes";
	texts[2] = "- R           : WireFrame / Solid rendering Toggle";
	texts[3] = "- T           : Flat / Guro shading Toggle";
	texts[4] = "- Z, X        : Zoom In/Out";
	texts[5] = "- N, M        : View point move nearby / Far away from WCS original point";
	texts[6] = "- C           : Hit Particle on/off";
	texts[7] = "- V           : BG Mountain on/off";
	texts[8] = "- F           : Fog on/off ";
	texts[9] = "- G           : Change Trees draw mode - Cylinder/Fractal/Off ";
	texts[10] = "- [,]           : Slow/Fast ";
	texts[11] = "- P           : Pause / Resume";
	texts[12] = "- Left, Right, Up, Down, pad0, padDel : Move a Viewpoint";
	texts[13] = "- pad4, pad2, pad6, pad8, pad+,-     : Move a Focus point";
	texts[14] = "- Drag                               : Rotate Viewpoint ";
	texts[15] = "- H           : Help";
	texts[16] = "- Q           : Exit";
	texts[17] = "------------------------ Player Control ---------------------------------";
	texts[18] = "- W,A,S,D                  : Move";
	texts[19] = "- W,W or A,A or S,S,or D,D : Run";
	texts[20] = "- J                        : Attack";
	texts[21] = "- K                        : Upper cut";
	texts[22] = "";
	texts[23] = "- F1                        : Set Enemy's HP to 1";
	texts[24] = "- F2                        : Set Hero's HP to 1";
	texts[25] = "- F3                        : Set Hero's HP to Full";

	for (int i = 0; i < textsMax; i++) {	//����, char* ����
		text[i] = (char*)texts[i].c_str();
		len[i] = texts[i].length();
	}

	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	//	glColor3f(1.0,1.0,0.1);//���
	glColor3f(0, 0, 0);	//����

	for (int i = 0; i < textsMax; i++) {
		glRasterPos2f(posX, posY);
		for (int j = 0; j < len[i]; j++) {
			glutBitmapCharacter(GLUT_BITMAP_9_BY_15, text[i][j]);
			//				glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, text[i]);				
		}
		posY -= 0.05;
	}

	glFlush();
	glPopAttrib();

	glDisable(GL_LIGHT7);		//�ؽ�Ʈ ��� �ϱ�� ���� ��
}

//�������
void RenderText(char* text, float posX, float posY)
{
	glEnable(GL_LIGHT7);	//��� �ϱ�� ����

	int len = strlen(text);

	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glColor4f(1.0, 1.0, 0.0, 1.0);
	glRasterPos2f(posX, posY);

	for (int i = 0; i < len; i++) {
		//glutBitmapCharacter(GLUT_BITMAP_9_BY_15, text[i]);
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, text[i]);
	}

	glFlush();
	glPopAttrib();

	glDisable(GL_LIGHT7);		//�ؽ�Ʈ ��� �ϱ�� ���� ��
}

//�������� ǥ�� (�ļ�. ���� 3d��ǥ�� 2d��ǥ�� ��ȯ�� �׷����ϳ�)q
void DrawLifeBar() {
	//	float gap1 = objs.people.at(0)->displayHP - objs.people.at(0)->HP;	//ǥ��ü�°� ��ü�� ����

	//	printf("%f %f \n",temp1, temp2);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glPushMatrix();

	glTranslatef(0, 0, -0.1);
	//		glRasterPos2f(0.0,-10);

	glEnable(GL_LIGHT7);	//��� �ϱ�� ����

	glBegin(GL_LINE_LOOP);
	glColor4f(1.0, 1.0, 1.0, 1);
	glLineWidth(2);	//�ȸ���
	glVertex3f(-0.90, 0.90, 0);
	glVertex3f(-0.90, 0.80, 0);
	glVertex3f(-0.08, 0.80, 0);
	glVertex3f(-0.08, 0.90, 0);
	glEnd();

	//glMaterialfv(GL_FRONT, GL_AMBIENT, mAmbi);		//��üƯ�� ����
	//glMaterialfv(GL_FRONT, GL_DIFFUSE, mDiff);
	//glMaterialfv(GL_FRONT, GL_SPECULAR, mSpec);
	glMateriali(GL_FRONT, GL_SHININESS, 0);		//�ݻ����� ����

	glBegin(GL_QUADS);
	glColor4f(119 / 255.0, 213 / 255.0, 253 / 255.0, 1);	//�Ķ�
	glVertex3f(-0.899, 0.899, 0);
	glVertex3f(-0.899, 0.801, 0);
	glVertex3f(-0.899 + lifeDot * objs.people.at(0)->HP, 0.801, 0);
	glVertex3f(-0.899 + lifeDot * objs.people.at(0)->HP, 0.899, 0);

	glEnd();

	if (objs.people.at(0)->displayHP >= objs.people.at(0)->HP + 1) {
		glBegin(GL_QUADS);
		glColor3f(1.0, 0.0, 0.0);			//����
		glVertex3f(-0.899 + lifeDot * (objs.people.at(0)->HP + 1), 0.899, 0);
		glVertex3f(-0.899 + lifeDot * (objs.people.at(0)->HP + 1), 0.801, 0);
		glVertex3f(-0.899 + lifeDot * objs.people.at(0)->displayHP, 0.801, 0);
		glVertex3f(-0.899 + lifeDot * objs.people.at(0)->displayHP, 0.899, 0);
		glEnd();
	}

	glDisable(GL_LIGHT7);		//�ؽ�Ʈ ��� �ϱ�� ���� ��

//		printf("%f %f \n",temp1, temp2);

	if (objs.gametime < 60) {
		RenderText("START !", -0.1, 0.005);

	}
	if (objs.people.at(0)->HP <= 0) {
		if (isHelpDraw == false) {
			RenderText("SCENE OVER", -0.2, 0.005);
		}
		mciSendString(("stop " + soundBGM1).c_str(), 0, 0, 0);//stop bgm
	}
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}

//End���
void RenderEnd() {

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glPushMatrix();

	glTranslatef(0, 0, -0.1);
	//		glEnable(GL_LIGHT7);	//��� �ϱ�� ����
	//		glMateriali(GL_FRONT, GL_SHININESS, 255);		//�ݻ����� ����

	RenderText("  END", -0.1, 0.005);

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}

//Ÿ�̸� (��Ÿ�� üũ����)
void MyTimer(int Value) {
	//printf("p1x = %f p1y = %f p1z = %f \n", p1x, p1y, p1z);//��ġ����

	for (int i = 0; i < 4; i++) {
		if (objs.people.at(0)->keyDoubleTouch[i] > 0) {//Ű �����Է� �����ð� -1
			objs.people.at(0)->keyDoubleTouch[i]--;
		}
		if (objs.people.at(0)->keyDoubleTouch[i] <= 0 && objs.people.at(0)->keyDoubleTouchLevel[i] != -1) {	//�����ð� �ٵǸ� �������� -1��
			objs.people.at(0)->keyDoubleTouchLevel[i] = -1;
		}
		if (objs.people.at(0)->keyHold[i] == 0 && objs.people.at(0)->keyDoubleTouchLevel[i] == -1) {	//�������� -1�̸�, Ű�� ���¿��� 0����.
			objs.people.at(0)->keyDoubleTouchLevel[i] = 0;
		}

	}

	if (isPaused == false) {	//�Ͻ����� �ƴҽ�
		if (objs.people.at(0)->keyHold[0] || objs.people.at(0)->keyHold[1] || objs.people.at(0)->keyHold[2] || objs.people.at(0)->keyHold[3]) {	//����Ű�� �ϳ� ��������
			p1ActionChange();		//�ൿ��Ŵ
		}

		//	printf("%d %d %d %d \n", objs.people.at(0)->AttKeyHold[0], objs.people.at(0)->AttKeyHold[1], objs.people.at(0)->AttKeyHold[2], objs.people.at(0)->AttKeyHold[3]);

		for (unsigned int i = 0; i < objs.people.size(); i++) {	//����迭 ����
			objs.people.at(i)->AlwaysDo();			//�׻�����Լ� ȣ��			
		}

		for (int i = 0; i < 10; i++) {		//���ΰ� Ÿ�ݰ˻�
			if (objs.people.at(0)->attacks[i][3] != 0) {		//���ΰ� Ÿ��������
				float ax = objs.people.at(0)->attacks[i][0];	//�ӽú��� ����
				float ay = objs.people.at(0)->attacks[i][1];
				float az = objs.people.at(0)->attacks[i][2];
				float ar = objs.people.at(0)->attacks[i][3];

				for (unsigned int j = 1; j < objs.people.size(); j++) {//���迭 ����
					bool isHit = false;
					float hbx;	//�ӽú��� ����(��Ʈ�� ��ü��ġ)
					float hby;
					float hbz;
					for (int k = 0; k < 10; k++) {		//��ü�����˻�
						if (objs.people.at(j)->bodies[k][3] != 0) {	//��ü������
							float bx = objs.people.at(j)->bodies[k][0];	//�ӽú��� ����
							float by = objs.people.at(j)->bodies[k][1];
							float bz = objs.people.at(j)->bodies[k][2];
							float br = objs.people.at(j)->bodies[k][3];
							float dis = sqrt(pow(bx - ax, 2) + pow(by - ay, 2) + pow(bz - az, 2));//�Ÿ�

							if (dis <= ar + br) {			//��Ʈ
								isHit = true;
								hbx = bx;		//��Ʈ��ġ ���
								hby = by;
								hbz = bz;

								//								printf("%f %f %f %f : %f %f %f %f : %f \n", ax,ay,az,ar,bx,by,bz,br,dis);
								//								printf("hit\n");
								float angleSub = objs.people.at(0)->BodyAngle[2] - objs.people.at(j)->BodyAngle[2];
								if (angleSub >= 360) {
									angleSub -= 360;
								}
								else if (angleSub < 0) {
									angleSub += 360;
								}

								mciSendString(("stop " + currSound).c_str(), 0, 0, 0);//����ȿ���� ����

								attAttr st;					//�������� ����ü ����								
								st.attackerBodyAngle = objs.people.at(0)->BodyAngle[2];
								st.hitType = objs.people.at(0)->hitType;
								if (angleSub > 270 || angleSub < 90) {
									st.hitDirection = D_BACKWARD;
								}
								else {
									st.hitDirection = D_FORWARD;
								}
								st.damage = objs.people.at(0)->hitAttr.damage;
								st.backVel = objs.people.at(0)->hitAttr.backVel;
								st.backAirVel = objs.people.at(0)->hitAttr.backAirVel;
								st.upVel = objs.people.at(0)->hitAttr.upVel;
								st.yAccel = objs.people.at(0)->hitAttr.yAccel;

								currSound = objs.people.at(0)->hitAttr.hitSound;	//���� ȿ��������
								mciSendString(("play " + currSound).c_str(), 0, 0, 0);//play sound

								objs.people.at(j)->actNo = GETHIT;	//�´� ��������
								objs.people.at(j)->ctrl = 0;
								objs.people.at(j)->hitAttr = st;
							}
						}
					}

					//Ÿ��ȿ�� ��ƼŬ����
					if (isHit == true && isParticleDraw == true) {
						for (int i = 0; i < MAX_PGENERATOR; i++) {
							if (pGenerators[i].alive == false) {
								pGenerators[i].setPos((hbx + ax) / 2.0, (hby + ay) / 2.0, (hbz + az) / 2.0);
								break;
							}
						}
					}
				}
			}
		}

		bool isWin = true;	//�̰峪
		for (unsigned int j = 1; j < objs.people.size(); j++) {//�� Ÿ�ݰ˻�
			if (objs.people.at(j)->manType != 10 && (objs.people.at(j)->HP > 0 || objs.people.at(j)->aniNo != ANI_LIEDOWN)) {//�� �Ѹ��̶� ���������
				isWin = false;
			}

			for (int k = 0; k < 10; k++) {		//�� Ÿ�������迭
				if (objs.people.at(j)->attacks[k][3] != 0) {
					float ax = objs.people.at(j)->attacks[k][0];	//�ӽú��� ����
					float ay = objs.people.at(j)->attacks[k][1];
					float az = objs.people.at(j)->attacks[k][2];
					float ar = objs.people.at(j)->attacks[k][3];

					bool isHit = false;
					float hbx;	//�ӽú��� ����(��Ʈ�� ��ü��ġ)
					float hby;
					float hbz;
					for (int i = 0; i < 10; i++) {		//���ΰ� ��ü�����迭
						if (objs.people.at(0)->bodies[i][3] != 0) {		//���ΰ� ��ü������
							float bx = objs.people.at(0)->bodies[i][0];	//�ӽú��� ����
							float by = objs.people.at(0)->bodies[i][1];
							float bz = objs.people.at(0)->bodies[i][2];
							float br = objs.people.at(0)->bodies[i][3];
							float dis = sqrt(pow(bx - ax, 2) + pow(by - ay, 2) + pow(bz - az, 2));//�Ÿ�

							if (dis <= ar + br) {			//������ ��Ʈ
								isHit = true;
								hbx = bx;		//��Ʈ��ġ ���
								hby = by;
								hbz = bz;

								float angleSub = objs.people.at(j)->BodyAngle[2] - objs.people.at(0)->BodyAngle[2];
								if (angleSub >= 360) {
									angleSub -= 360;
								}
								else if (angleSub < 0) {
									angleSub += 360;
								}

								mciSendString(("stop " + currSound).c_str(), 0, 0, 0);//����ȿ���� ����

								attAttr st;					//�������� ����ü ����								
								st.attackerBodyAngle = objs.people.at(j)->BodyAngle[2];
								st.hitType = objs.people.at(j)->hitType;
								if (angleSub > 270 || angleSub < 90) {
									st.hitDirection = D_BACKWARD;
								}
								else {
									st.hitDirection = D_FORWARD;
								}
								st.damage = objs.people.at(j)->hitAttr.damage;
								st.backVel = objs.people.at(j)->hitAttr.backVel;
								st.backAirVel = objs.people.at(j)->hitAttr.backAirVel;
								st.upVel = objs.people.at(j)->hitAttr.upVel;
								st.yAccel = objs.people.at(j)->hitAttr.yAccel;

								currSound = objs.people.at(j)->hitAttr.hitSound;	//���� ȿ��������
								mciSendString(("play " + currSound).c_str(), 0, 0, 0);//play sound

								objs.people.at(0)->actNo = GETHIT;
								objs.people.at(0)->ctrl = 0;
								objs.people.at(0)->hitAttr = st;
							}
						}
					}

					//Ÿ��ȿ�� ��ƼŬ����
					if (isHit == true && isParticleDraw == true) {
						for (int i = 0; i < MAX_PGENERATOR; i++) {
							if (pGenerators[i].alive == false) {
								pGenerators[i].setPos((hbx + ax) / 2.0, (hby + ay) / 2.0, (hbz + az) / 2.0);
								break;
							}
						}
					}
				}
			}
		}

		if (objs.gameState == 1 && isWin == true) {	//���̰�����			
			objs.gameState = 2;			//�̱����
		}

		if (objs.gameState == 2 && objs.people.at(0)->actNo == IDLE) {	//���̰�����
			Thread[0] = (HANDLE)_beginthreadex(NULL, 0, AniEnding, (void*)NULL, 0, &ThreadState[0]);	//������� �Լ�ȣ��
			objs.gameState = 3;
		}


		objs.gametime++;
		//		printf("%f %f \n",objs.people.at(0)->x, objs.people.at(0)->z);
		//		printf("%d \n",glutGet(GLUT_ELAPSED_TIME));

				//	printf("%d %d %d %d \n", objs.people.at(0)->keyHold[0], objs.people.at(0)->keyHold[1], objs.people.at(0)->keyHold[2], objs.people.at(0)->keyHold[3]);
				//	printf("%d %d %d %d \n", objs.people.at(0)->keyDoubleTouch[0], objs.people.at(0)->keyDoubleTouch[1], objs.people.at(0)->keyDoubleTouch[2], objs.people.at(0)->keyDoubleTouch[3]);
				//	printf("%d %d %d %d \n", objs.people.at(0)->keyDoubleTouchLevel[0], objs.people.at(0)->keyDoubleTouchLevel[1], objs.people.at(0)->keyDoubleTouchLevel[2], objs.people.at(0)->keyDoubleTouchLevel[3]);

				//printf("%d %d \n",objs.people.at(1).prevActNo, objs.people.at(1).actNo);
				//printf("%d %d \n",objs.people.at(1).prevAniNo, objs.people.at(1).aniNo);
				//	printf("%d \n",objs.people.at(0)->timer);
	}

	//	printf("%f \n",eyeAngleX);

	glutPostRedisplay();

	//	glutTimerFunc(14, MyTimer, 1);//Ÿ�̸� ����. �ʴ� 70������
	glutTimerFunc(sleepSpeed, MyTimer, 1);//Ÿ�̸� ����. �ʴ� 60������
}

//��Ʈ�� �ִϸ��̼� =================================================================
UINT WINAPI AniIntro(void* arg) {
	objs.isAniControl = true;
	objs.gameState = 0;

	mciSendString(("stop " + soundBGM1).c_str(), 0, 0, 0);//stop bgm

	for (unsigned int i = 0; i < objs.people.size(); i++) {//����迭 ����
		objs.people.at(i)->actNo = MAIN_ANI;//������ ����� �� ���·� ����.

		if (objs.people.at(i)->manType != 10) {			//���� �ƴϸ�
			objs.people.at(i)->aniNo = ANI_MANSTANDING;	//���ִ� ��������
			objs.people.at(i)->AniHandler();				//���ۼ�
		}
	}

	Person* m = objs.people.at(0);	//���0 ����Ű�� ������
	Person* m2 = objs.people.at(1);	//���1 ����Ű�� ������
	Person* m3 = objs.people.at(2);	//���2 ����Ű�� ������
	Person* w = objs.people.at(3);	//���3 ����Ű�� ������

	if (isBgmPlay == true) {
	}

	//�ɾ------------------------------------------------------	
	timer2 = 0;
	m->timer = 0;
	m->aniNo = ANI_WALK;		//�ȴ� ����
	float tempX = 0;
	isCanMouse = false;		//ȸ���� ���콺 ����
	MyMouseMove2(0, 0, 0, 0);	//���� ���콺Ŭ��
	do {
		m->movePosFoward(m->walkSpeed);//������ �̵�
		m->AniHandler();				//�ִ� ��Ŵ

		if (timer2 < 60) {

			tempX += 6;
			//			EyePosChange(tempX, 0);		//������ȭ			
			//			CalculateRadius();	//���콺 �� ȿ��
			MyMotion2(tempX, 0);		//���� ���콺�̵�(����ȸ��)
		}
		if (timer2 == 60) {
			MyMouseMove2(0, 1, 0, 0);	//���� ���콺��
			isCanMouse = true;		//���콺 ����
		}

		timer2++;			//�ð�++
		Sleep(sleepSpeed);			//�ʴ� �������� ���߱� ���� ����
	} while (timer2 < 140);	//������ �ð�����(60 = 1��)

	//------���缶
	timer2 = 0;
	m->timer = 0;
	m->aniNo = ANI_MANSTANDING;
	mciSendString(("play " + soundVoice01).c_str(), 0, 0, 0);	//voice01
	do {
		m->AniHandler();				//�ִ� ��Ŵ

		if (timer2 < 70) {
			m->HeadAngle[2] += 1;	//������
		}

		timer2++;			//�ð�++
		Sleep(sleepSpeed);			//�ʴ� �������� ���߱� ���� ����
	} while (timer2 < 140);	//������ �ð�����

	//------�ٽ� �ȱ� ����
	timer2 = 0;
	m->timer = 0;
	m->aniNo = ANI_WALK;		//�ȴ� ����
	do {

		m->movePosFoward(m->walkSpeed);//������ �̵�
		m->AniHandler();				//�ִ� ��Ŵ

		if (timer2 < 35) {
			m->HeadAngle[2] -= 2;
			m->BodyAngle[2] += 2;
		}

		timer2++;			//�ð�++
		Sleep(sleepSpeed);			//�ʴ� �������� ���߱� ���� ����
	} while (timer2 < 35);	//������ �ð�����

	//------�޷���
	timer2 = 0;
	m->timer = 0;
	m->aniNo = ANI_RUN;		//�޸��� ����
	do {
		m->movePosFoward(m->runSpeed);//������ �̵�
		m->AniHandler();				//�ִ� ��Ŵ

		timer2++;			//�ð�++
		Sleep(sleepSpeed);			//�ʴ� �������� ���߱� ���� ����
	} while (timer2 < 145);	//������ �ð�����(60 = 1��)

	//------����
	objs.people.at(0)->x = -40.6;//�ӽ�
	objs.people.at(0)->z = -38;//
	objs.EyeX = objs.people.at(0)->x + 10;		//������
	objs.EyeZ = objs.people.at(0)->z; // + 5;
	objs.AtX = objs.people.at(0)->x;		//������
	objs.AtZ = objs.people.at(0)->z;
	m->BodyAngle[2] = 70;

	timer2 = 0;
	m->timer = 0;
	m->aniNo = IDLE;
	m->InitAngle();
	do {
		m->BodyAngle[2] += 1;

		timer2++;			//�ð�++
		Sleep(sleepSpeed);			//�ʴ� �������� ���߱� ���� ����
	} while (timer2 < 20);	//������ �ð�����(60 = 1��)

	timer2 = 0;
	mciSendString(("play " + soundVoice02).c_str(), 0, 0, 0);	//voice - ����鹹
	tempX = 0;
	isCanMouse = false;		//ȸ���� ���콺 ����
	MyMouseMove2(0, 0, 0, 0);	//���� ���콺Ŭ��
	do {
		if (((objs.eyeAngleX < 170 && objs.eyeAngleX > 10) || (objs.eyeAngleX > -170 && objs.eyeAngleX < -10)) && timer2 < 59) {
			tempX += 6;
			MyMotion2(tempX, 0);		//���� ���콺�̵�(����ȸ��)
		}
		if (timer2 == 59) {
			MyMouseMove2(0, 1, 0, 0);	//���� ���콺��
			isCanMouse = true;		//���콺 ����
		}

		timer2++;			//�ð�++
		Sleep(sleepSpeed);			//�ʴ� �������� ���߱� ���� ����		
	} while (timer2 < 70);	//������ �ð�����(60 = 1��)

	//------�� ���Ƽ�
	timer2 = 0;
	do {

		//		m->movePosFoward(m->runSpeed);//������ �̵�
		//		m->AniHandler();				//�ִ� ��Ŵ

		m2->BodyAngle[2] -= 5;
		m3->BodyAngle[2] += 7;

		timer2++;			//�ð�++
		Sleep(sleepSpeed);			//�ʴ� �������� ���߱� ���� ����
	} while (timer2 < 20);	//������ �ð�����(60 = 1��)

	timer2 = 0;
	mciSendString(("play " + soundVoice03).c_str(), 0, 0, 0);	//voice - �Ű沨
	do {
		timer2++;			//�ð�++
		Sleep(sleepSpeed);			//�ʴ� �������� ���߱� ���� ����
	} while (timer2 < 60);	//������ �ð�����(60 = 1��)

	timer2 = 0;
	mciSendString(("play " + soundVoice04).c_str(), 0, 0, 0);	//voice - �������̼�
	do {
		timer2++;			//�ð�++
		Sleep(sleepSpeed);			//�ʴ� �������� ���߱� ���� ����
	} while (timer2 < 60);	//������ �ð�����(60 = 1��)

	//------������
	timer2 = 0;
	do {
		if (timer2 >= 60 && timer2 < 80) {
			m->HeadAngle[2] += 1;
		}
		else if (timer2 >= 140 && timer2 < 160) {
			m->HeadAngle[2] -= 1;
		}

		timer2++;			//�ð�++
		Sleep(sleepSpeed);			//�ʴ� �������� ���߱� ���� ����
	} while (timer2 < 160);	//������ �ð�����(60 = 1��)

	timer2 = 0;
	mciSendString(("play " + soundVoice05).c_str(), 0, 0, 0);	//voice - ���� �ƴ�����
	do {
		if (timer2 == 120) {
			mciSendString(("play " + soundVoice06).c_str(), 0, 0, 0);	//voice
		}
		timer2++;			//�ð�++
		Sleep(sleepSpeed);			//�ʴ� �������� ���߱� ���� ����
	} while (timer2 < 200);	//������ �ð�����(60 = 1��)

	timer2 = 0;
	mciSendString(("play " + soundVoice07).c_str(), 0, 0, 0);	//voice
	do {
		timer2++;			//�ð�++
		Sleep(sleepSpeed);			//�ʴ� �������� ���߱� ���� ����
	} while (timer2 < 240);	//������ �ð�����(60 = 1��)

	timer2 = 0;
	mciSendString(("play " + soundVoice08).c_str(), 0, 0, 0);	//voice
	do {
		timer2++;			//�ð�++
		Sleep(sleepSpeed);			//�ʴ� �������� ���߱� ���� ����
	} while (timer2 < 120);	//������ �ð�����(60 = 1��)


	//------����
	objs.gametime = 0;
	isLifeBarDraw = true;
	if (isBgmPlay == true) {
		mciSendString(("play " + soundBGM1 + " repeat").c_str(), 0, 0, 0);//play bgm
	}

	for (unsigned int i = 1; i < objs.people.size(); i++) {//����迭 ����
		if (objs.people.at(i)->manType != 10) {
			objs.people.at(i)->actNo = IDLE;
			objs.people.at(i)->ctrl = 1;
			objs.people.at(i)->isAI = true;
			objs.people.at(i)->HP = 500;
		}
	}
	objs.people.at(0)->actNo = IDLE;	//���ΰ� ������ �� �ְ�
	objs.people.at(0)->ctrl = 1;
	objs.people.at(0)->HP = 1000;

	objs.gameState = 1;	//������Ʈ ���ΰ�
	objs.isAniControl = false;	//�ִ����� off
	return 0;
}//-------------------��Ʈ�� �ִ� ��

//���� �ִϸ��̼� =================================================================
UINT WINAPI AniEnding(void* arg) {
	float destPosX;			//�ӽ� ��ǥx
	float destPosZ;			//�ӽ� ��ǥz
	float timerDest[5];		//�ӽ� �ð� �迭
	float timerCut[5];		//�ӽ� �ð� �迭
	float angleDis;			//�ӽ� ��ǥ��
	float angleSub;			//�ӽ� ��������
	float posDis;			//�ӽ� ��ǥ����
	float tempX = 0;		//����ȸ���� �ӽú���
	float tempY = 0;

	isLifeBarDraw = false;
	objs.isAniControl = true;
	objs.gameState = 3;
	Person* m = objs.people.at(0);	//���ΰ� ������ ����
	Person* w = NULL;			//���� ������

	mciSendString(("stop " + soundBGM1).c_str(), 0, 0, 0);//stop bgm

	for (unsigned int i = 0; i < objs.people.size(); i++) {//����迭 ����
		objs.people.at(i)->actNo = MAIN_ANI;//������ ����� �� ���·� ����.
		objs.people.at(i)->ctrl = 0;
		if (objs.people.at(i)->manType == 10) {
			w = objs.people.at(i);	//���� ������ ����
		}
	}

	if (w == NULL) {	//���� ������ ����
		return 0;
	}
	else {
		w->isAI = false;
		w->ctrl = 0;
	}

	//���� ������ ȸ��------------------------------------------------------	
	timer2 = 0;
	m->timer = 0;
	m->aniNo = ANI_WALK;		//�ȴ� ����
//	tempX = 0;
//	isCanMouse = false;		//ȸ���� ���콺 ����
//	MyMouseMove2(0, 0, 0, 0);	//���� ���콺Ŭ��
	destPosX = w->x + 3;		//��ǥ��ġ
	destPosZ = w->z;
	do {
		angleDis = m->getP2AngleXY(destPosX, destPosZ);	//��ǥ�޾�, ���κ��� ���ڰ� ������� �ֳ� ����
		angleSub = m->BodyAngle[2] - angleDis;	//������
		posDis = sqrt(pow(destPosX - m->x, 2) + pow(destPosZ - m->z, 2));	//�Ÿ�����

		m->rotateCheck(angleDis, rotateSpeed);			//���� ���� �������� ȸ��

		if (posDis > 1) {						//�Ÿ� Ŭ���� �̵�
			m->movePosFoward(m->walkSpeed);//������ �̵�
		}
		m->AniHandler();				//�ִ� ��Ŵ

//		if(timer2 < 60){

//			tempX += 6;
//			MyMotion2(tempX, 0);		//���� ���콺�̵�(����ȸ��)
//		}
//		if(timer2 == 60){
//			MyMouseMove2(0, 1, 0, 0);	//���� ���콺��
//			isCanMouse = true;		//���콺 ����
//		}

		timer2++;			//�ð�++
		Sleep(sleepSpeed);			//�ʴ� �������� ���߱� ���� ����
	} while (timer2 < 30);	//������ �ð�����(60 = 1��)

	//------���� �޸�
	mciSendString(("stop " + soundBGM1).c_str(), 0, 0, 0);//stop bgm
	timer2 = 0;
	m->timer = 0;
	m->aniNo = ANI_RUN;
	do {
		angleDis = m->getP2AngleXY(destPosX, destPosZ);	//��ǥ�޾�, ���κ��� ���ڰ� ������� �ֳ� ����
		posDis = sqrt(pow(destPosX - m->x, 2) + pow(destPosZ - m->z, 2));	//�Ÿ�����

		m->rotateCheck(angleDis, rotateSpeed);			//�������� ȸ��

		m->movePosFoward(m->runSpeed);//������ �̵�
		m->AniHandler();				//�ִ� ��Ŵ

		timer2++;			//�ð�++
		Sleep(sleepSpeed);			//�ʴ� �������� ���߱� ���� ����
	} while (posDis > 0.5);	//���� ���Ǽ���

	//------���ھտ��� ����ȸ��
	mciSendString(("stop " + soundBGM1).c_str(), 0, 0, 0);//stop bgm
	timer2 = 0;
	m->x = destPosX;		//��ǥ��
	m->z = destPosZ;
	m->timer = 0;
	m->aniNo = ANI_MANSTANDING;
	do {
		angleDis = m->getP2AngleXY(w->x, w->z);	//��ǥ�޾�, ���κ��� ������ ������� �ֳ� ����
		angleSub = m->BodyAngle[2] - angleDis;

		m->rotateCheck(angleDis, rotateSpeed);			//�������� ȸ��

		m->AniHandler();				//�ִ� ��Ŵ

		timer2++;			//�ð�++
		Sleep(sleepSpeed);			//�ʴ� �������� ���߱� ���� ����
	} while (angleSub > rotateSpeed || angleSub < -rotateSpeed);	//���� ���Ǽ���

	/////////////////�ӽ�///////////////
	//m->isHandCuffed = true;
	//w->x = -49.1;		//��ǥ��
	//w->z = -36;	
	//float disX;
	//float disY;
	//destPosX = -47;		//��ǥ��ġ
	//destPosZ = -36;
	//objs.EyeX += destPosX - m->x;
	//objs.EyeZ += destPosZ - m->z;
	//objs.AtX += destPosX - m->x;
	//objs.AtZ += destPosZ - m->z;	
	//m->x = destPosX;		//��ǥ��
	//m->z = destPosZ;
	//m->BodyAngle[2] = 90;
	//m->aniNo = ANI_MANSTANDING;
	//m->AniHandler();				//�ִ� ��Ŵ	

	//------���ں��� ���
	timerDest[0] = 30;		//��ǥ Ÿ��
	timerCut[0] = timerDest[0];//�ð���
	timer2 = 0;
	m->BodyAngle[2] = 90;
	mciSendString(("play " + soundVoice09).c_str(), 0, 0, 0);	//voice
	tempX = 0;
	isCanMouse = false;		//ȸ���� ���콺 ����
	MyMouseMove2(0, 0, 0, 0);	//���� ���콺Ŭ��

	do {
		if (timer2 < timerDest[0]) {
			m->LshoulderAngle[0] += 45 / timerCut[0];	//����
			m->LelbowAngle[0] += 10 / timerCut[0];	//����		
			m->LelbowAngle[1] += 35 / timerCut[0];	//����					
		}

		if ((objs.eyeAngleX < 170 && objs.eyeAngleX > 10) || (objs.eyeAngleX > -170 && objs.eyeAngleX < -10)) {
			tempX += 6;
			MyMotion2(tempX, 0);		//���� ���콺�̵�(����ȸ��)
		}
		if (timer2 == 59) {
			MyMouseMove2(0, 1, 0, 0);	//���� ���콺��
			isCanMouse = true;		//���콺 ����
		}


		timer2++;			//�ð�++
		Sleep(sleepSpeed);			//�ʴ� �������� ���߱� ���� ����
	} while (timer2 < 60);	//���� ���Ǽ���

	//------���ڰɾ��
	timer2 = 0;
	w->aniNo = ANI_WALK;
	do {
		w->movePosFoward(0.03);//������ �̵�
		w->AniHandler();				//�ִ� ��Ŵ

		timer2++;			//�ð�++
		Sleep(sleepSpeed);			//�ʴ� �������� ���߱� ���� ����
	} while (timer2 < 30);	//���� ���Ǽ���


	/////////////////�ӽ�///////////////
	//m->isHandCuffed = true;
	//w->x = -49.1;		//��ǥ��
	//w->z = -36;	
	//float disX;
	//float disY;
	//destPosX = -47;		//��ǥ��ġ
	//destPosZ = -36;
	//objs.EyeX += destPosX - m->x;
	//objs.EyeZ += destPosZ - m->z;
	//objs.AtX += destPosX - m->x;
	//objs.AtZ += destPosZ - m->z;	
	//m->x = destPosX;		//��ǥ��
	//m->z = destPosZ;
	//m->BodyAngle[2] = 90;
	//m->aniNo = ANI_MANSTANDING;
	//m->AniHandler();				//�ִ� ��Ŵ


	//------���ڼ���
	timerDest[0] = 30;		//��ǥ Ÿ��
	timerDest[1] = 60;		//��ǥ Ÿ��
	timerCut[0] = timerDest[0];//�ð���
	timerCut[1] = timerDest[1] - timerDest[0];//�ð���
	timer2 = 0;
	w->aniNo = ANI_IDLE;
	w->InitAngle();
	do {
		if (timer2 < timerDest[0]) {
			w->WaistAngle[0] -= 45 / timerCut[0];	//�㸮 ����
			w->LshoulderAngle[0] += 80 / timerCut[0];	//����
			w->LelbowAngle[0] += 20 / timerCut[0];
			w->RelbowAngle[0] += 20 / timerCut[0];		//������
			w->RelbowAngle[1] -= 35 / timerCut[0];		//�� �ָӴϿ�
		}
		else if (timer2 < timerDest[1]) {
			w->WaistAngle[0] += 45 / timerCut[1];	//�㸮 ��
			w->LshoulderAngle[0] -= 60 / timerCut[1];
			w->RshoulderAngle[0] += 35 / timerCut[1];
			w->LelbowAngle[1] += 20 / timerCut[1];

			m->RshoulderAngle[0] += 45 / timerCut[0];
			m->RelbowAngle[0] += 10 / timerCut[0];
			m->RelbowAngle[1] -= 35 / timerCut[0];
		}

		timer2++;			//�ð�++
		Sleep(sleepSpeed);			//�ʴ� �������� ���߱� ���� ����
	} while (timer2 < 60);	//���� ���Ǽ���

	//------���� ä����
	m->isHandCuffed = true;
	timer2 = 0;
	do {
		timer2++;			//�ð�++
		Sleep(sleepSpeed);			//�ʴ� �������� ���߱� ���� ����
	} while (timer2 < 60);	//���� ���Ǽ���

	//------��ȭ	
	timer2 = 0;
	tempX = 3;	//�� �ӵ�
	mciSendString(("play " + soundVoice10).c_str(), 0, 0, 0);	//voice ����
	do {
		//	cout<<w->x<<" "<<w->z<<" "<<m->x<<" "<<m->z<<"\n";
		if (timer2 == 60) {
			mciSendString(("play " + soundVoice11).c_str(), 0, 0, 0);	//voice  ����
		}
		else if (timer2 == 120) {
			mciSendString(("play " + soundVoice12).c_str(), 0, 0, 0);	//voice  �ʸ�
		}
		else if (timer2 == 300) {
			mciSendString(("play " + soundVoice14).c_str(), 0, 0, 0);	//voice  �����
		}
		else if (timer2 == 330) {
			mciSendString(("play " + soundVoice13).c_str(), 0, 0, 0);	//voice  ������1
		}
		else if (timer2 == 420) {
			mciSendString(("play " + soundVoice15).c_str(), 0, 0, 0);	//voice  ������2
		}
		else if (timer2 == 540) {
			mciSendString(("play " + soundVoice16).c_str(), 0, 0, 0);	//voice  �̷����
		}
		else if (timer2 == 660) {
			mciSendString(("play " + soundVoice17).c_str(), 0, 0, 0);	//voice  ����
		}
		else if (timer2 == 770) {
			mciSendString(("play " + soundVoice18).c_str(), 0, 0, 0);	//voice  Ÿ
		}

		tempY = 500;	//�ӽú���
		//�� �̵���
		if (timer2 == 0 + tempY) {
			isCarDraw = true;
			isCyren = true;		//���̷�ȿ��
			car->pos.x = 0;
			car->pos.z = 300;
		}
		else if (timer2 < 80 + tempY) {
			car->movePosFoward(tempX);		//�� ����
		}
		else if (timer2 < 120 + tempY) {
			tempX -= 0.02;
			car->movePosFoward(tempX);		//�� �ӵ������ϸ� ����
		}
		else if (timer2 < 140 + tempY) {
			tempX -= 0.24;
			if (tempX > 0) {
				car->movePosFoward(tempX);		//�� �ӵ������ϸ� ����
			}
			else {
				car->movePosFoward(0.35);		//�� ����
			}
		}
		else if (timer2 < 150 + tempY) {
			tempX -= 0.04;
			if (tempX > 0) {
				car->movePosFoward(tempX);		//�� ����
			}
			else {
				//				car->movePosFoward(0.5);		//�� ����
			}
		}
		else if (timer2 > 150 + tempY && timer2 < 160 + tempY) {
			car->movePosFoward(-0.3);
		}

		//�� ȸ����
		if (timer2 >= 95 + tempY && timer2 < 140 + tempY) {
			car->angles.y += 2.5;				//�� ȸ��
		}
		else if (timer2 >= 140 + tempY && timer2 < 150 + tempY) {
			if (car->angles.y + car->dAngle < 180 + tempY) {
				car->dAngle += 7;				//�� �帮��Ʈ�ϸ� ȸ��
			}
		}
		else if (timer2 == 150 + tempY) {
			car->angles.y += car->dAngle;	//�� ����
			car->dAngle = 0;
		}

		if (timer2 == 65 + tempY) {
			mciSendString(("play " + soundCarStop).c_str(), 0, 0, 0);	//�����ߴ���
		}

		timer2++;			//�ð�++
		Sleep(sleepSpeed);			//�ʴ� �������� ���߱� ���� ����
	} while (timer2 < 830);	//���� �ð�����

	//------����
	timer2 = 0;
	timerDest[0] = 10;		//��ǥ Ÿ��
	timerCut[0] = timerDest[0];//�ð���
	do {
		w->LshoulderAngle[0] -= 20 / timerCut[0];	//����		//�� �������
		w->RshoulderAngle[0] -= 35 / timerCut[0];	//����
		w->LelbowAngle[0] -= 20 / timerCut[0];
		w->LelbowAngle[1] -= 20 / timerCut[1];
		w->RelbowAngle[0] -= 20 / timerCut[0];		//������
		w->RelbowAngle[1] += 35 / timerCut[0];		//�� �ָӴϿ�

		w->BodyAngle[0] += 45 / timerCut[0];
		w->WaistAngle[0] -= 20 / timerCut[0];	//�㸮 ����

		w->LlegAngle[0] -= 45 / timerCut[0];	//�޴ٸ�
		w->RlegAngle[0] += 40 / timerCut[0];	//�����ٸ�

		timer2++;			//�ð�++
		Sleep(sleepSpeed);			//�ʴ� �������� ���߱� ���� ����
	} while (timer2 < 10);	//���� �ð�����
	mciSendString(("play " + soundHit1).c_str(), 0, 0, 0);	//��Ʈ��

	//------���� ���ư�
	timer2 = 0;
	timerDest[0] = 30;		//��ǥ Ÿ��
	timerCut[0] = timerDest[0];//�ð���
	float velY = 0.1;		//�ӵ�
	float addY = -0.0075;		//���ӵ�
	do {
		m->BodyAngle[0] -= 90 / timerCut[0];
		m->BodyAngle[2] -= 180 / timerCut[0];

		m->LlegAngle[0] += 70 / timerCut[0];
		m->RlegAngle[0] += 70 / timerCut[0];
		m->LkneeAngle[0] -= 100 / timerCut[0];
		m->RkneeAngle[0] -= 100 / timerCut[0];

		m->x += 0.15;

		m->y += velY;
		velY += addY;

		timer2++;			//�ð�++
		Sleep(sleepSpeed);			//�ʴ� �������� ���߱� ���� ����
	} while (timer2 < 30);	//���� �ð�����
	m->isDraw = false;	//���� �ȱ׸�
	mciSendString(("play " + soundVoice19).c_str(), 0, 0, 0);	//voice ����

	//------�� �������
	timer2 = 0;
	timerDest[0] = 10;		//��ǥ Ÿ��
	timerCut[0] = timerDest[0];//�ð���
	do {
		w->BodyAngle[0] -= 45 / timerCut[0];
		w->WaistAngle[0] += 20 / timerCut[0];	//�㸮 ����

		w->LlegAngle[0] += 45 / timerCut[0];	//�޴ٸ�
		w->RlegAngle[0] -= 40 / timerCut[0];	//�����ٸ�

		timer2++;			//�ð�++
		Sleep(sleepSpeed);			//�ʴ� �������� ���߱� ���� ����
	} while (timer2 < 10);	//���� �ð�����

	//------���� �ɾ
	timer2 = 0;
	w->aniNo = ANI_WALK;
	do {
		w->movePosFoward(w->walkSpeed / 2);//������ �̵�
		w->AniHandler();				//�ִ� ��Ŵ
		objs.AtX += w->walkSpeed / 2;		//�����̵�

		timer2++;			//�ð�++
		Sleep(sleepSpeed);			//�ʴ� �������� ���߱� ���� ����
	} while (timer2 < 60);	//���� �ð�����

	//------���� ���� ���� Ž
	timer2 = 0;
	w->aniNo = IDLE;
	timerDest[0] = 30;		//��ǥ Ÿ��
	timerDest[1] = 60;		//��ǥ Ÿ��
	timerCut[0] = timerDest[0];//�ð���
	timerCut[1] = timerDest[1] - timerDest[0];//�ð���
	do {
		if (timer2 < timerDest[0]) {
			w->WaistAngle[0] -= 70 / timerCut[0];	//�㸮 ����
		}
		else if (timer2 < timerDest[1]) {
			w->movePosFoward(w->walkSpeed / 2);//������ �̵�
			objs.AtX += w->walkSpeed / 2;		//�����̵�

			w->LlegAngle[0] += 30 / timerCut[1];	//�޴ٸ�
			w->RlegAngle[0] -= 30 / timerCut[1];	//�����ٸ�
		}

		timer2++;			//�ð�++
		Sleep(sleepSpeed);			//�ʴ� �������� ���߱� ���� ����
	} while (timer2 < 60);	//���� �ð�����

	w->isDraw = false;	//���� �ȱ׸�

	//------�� �̵�
	timer2 = 0;
	tempX = 0;
	do {
		if (timer2 < 40) {
			tempX -= 0.005;
			car->movePosFoward(tempX);//�� �ڷ� 

			car->angles.y += 1;

		}
		else if (timer2 < 60) {
			tempX += 0.01;
			car->movePosFoward(tempX);//��������

			car->angles.y += 1;
		}
		else if (timer2 < 120) {
			tempX += 0.01;
			car->movePosFoward(tempX);//�� ������
		}
		else if (timer2 < 200) {
			if (car->angles.y > 180) {
				car->angles.y -= 1;
			}
			car->movePosFoward(tempX);//�� ������
		}
		else if (timer2 < 720) {
			car->movePosFoward(tempX);//�� ������
		}

		//���� ������
		if (timer2 == 30) {
			mciSendString(("play " + soundVoice20).c_str(), 0, 0, 0);	//voice �̶���
		}
		else if (timer2 == 90) {
			mciSendString(("play " + soundCarStart).c_str(), 0, 0, 0);	//����
		}
		else if (timer2 == 270) {
			mciSendString(("play " + soundVoice21).c_str(), 0, 0, 0);	//voice ����
		}
		else if (timer2 == 300) {
			mciSendString(("play " + soundVoice22).c_str(), 0, 0, 0);	//voice �Ű�
		}
		else if (timer2 == 360) {
			mciSendString(("play " + soundVoice23).c_str(), 0, 0, 0);	//voice �Ű�
		}
		else if (timer2 == 480) {
			mciSendString(("play " + soundHit1).c_str(), 0, 0, 0);	//voice �Ű�}
		}

		objs.AtX = car->pos.x;
		objs.AtZ = car->pos.z;

		if (timer2 > 120 && timer2 < 500) {
			objs.EyeY += 0.1;
		}

		timer2++;			//�ð�++
		Sleep(sleepSpeed);			//�ʴ� �������� ���߱� ���� ����
	} while (timer2 < 720);	//���� �ð�����

	isCarDraw = false;	//�� �ȱ׸�
	isCyren = false;	//���̷�ȿ�� ��
	objs.gameState = 4;

	objs.isAniControl = false;	//�ִ����� off
	return 0;
}//-------------------���� �ִ� ��

//�����Լ�
int main(int argc, char** argv) {
	glutInit(&argc, argv);
	//	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);//GLUT_DOUBLE�� ���Ƿ� GLUT_SINGLE�� �Ⱦ�
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_STENCIL | GLUT_MULTISAMPLE);
	glutInitWindowSize(800, 600);
	glutInitWindowPosition(200, 200);
	glutCreateWindow("OpenGL Project 3");
	glClearColor(0.6, 0.6, 0.6, 1.0);
	InitLight();

	srand((unsigned int)time(NULL));		//���������� ���� �̷��� ��. ���� �ð���(time)�̿�.

	//devil ���̺귯�� �ʱ�ȭ
	ilInit();
	iluInit();
	ilutInit();
	ilutRenderer(ILUT_OPENGL);
	ilutEnable(ILUT_OPENGL_CONV);

	//���뱸��ü �����ʱ�ȭ
	objs.manID = 0;	//��� ���̵� �ʱ�ȭ
	objs.EyeX; //������
	objs.EyeY;
	objs.EyeZ = 9;
	objs.AtX = TableX / 2;		//������
	objs.AtY = TableHeight;
	objs.AtZ = -TableY / 2;
	objs.isAniControl = false;	//���ο��� �ִ� ��Ʈ���ϳ�
	objs.gameState = 0;	//��Ȳ����0
	objs.gametime = 0; objs.gametime = 0;
	//	car->pos.z = 300;			//�� ��ġ ����	
	car->pos.z = 0;			//�� ��ġ ����

	//��ƼŬ������ �ʱ�ȭ(c++�� �ʱ�ȭ���ص�)
	for (int i = 0; i < MAX_PGENERATOR; i++) {
		pGenerators[i].alive = false;
	}

	//��� ���� ��(������ �̿�)
	Person* m = new Man(&objs);
	m->manType = 0;
	m->ctrl = 0;
	m->actNo = MAIN_ANI;
	objs.people.push_back(m);	//�迭�� ����.  ù��° ����� ���ΰ�

	Person* m2 = new Man(&objs);
	m2->manType = 1;
	m2->actNo = MAIN_ANI;
	objs.people.push_back(m2);	//�迭�� ����

	Person* m3 = new Man(&objs);
	m3->manType = 2;
	m3->actNo = MAIN_ANI;
	objs.people.push_back(m3);	//�迭�� ����

	Person* m4 = new Man(&objs);
	m4->manType = 3;
	m4->actNo = MAIN_ANI;
	objs.people.push_back(m4);	//�迭�� ������

	Person* w1 = new Woman(&objs);;		//���ڻ���
	w1->manType = 10;
	w1->actNo = MAIN_ANI;
	objs.people.push_back(w1);	//�迭�� ����.  ù��° ����� ���ΰ�

	objs.EyeX = objs.people.at(0)->x;		//������
	objs.EyeY = objs.people.at(0)->y + 7.5;// + 5;
	objs.EyeZ = objs.people.at(0)->z + 10; // + 5;

	objs.AtX = objs.people.at(0)->x;		//������
	objs.AtY = objs.people.at(0)->y + 2;
	objs.AtZ = objs.people.at(0)->z;

	InitPosition();
	//mciSendString("play pro2_runBgm.wav", 0, 0, 0);	//������Ʈ�Ӽ� ��Ƽ����Ʈ���
	//	mciSendString("play sound\\bgm01.mp3 repeat", 0, 0, 0);//play bgm
//	mciSendString("pause sound\\bgm01.mp3", 0, 0, 0);//pause bgm
//	mciSendString("resume sound\\bgm01.mp3", 0, 0, 0);//resume bgm
//	mciSendString("stop sound\\bgm01.mp3", 0, 0, 0);		//stop bgm
//	mciSendString("play sound\\hit1.mp3 repeat ", 0, 0, 0);	

	//�ؽ�ó �ε�
	objs.textures[0] = loadImage("images\\board256.jpg");
	objs.textures[1] = loadImage("images\\face01_2561.jpg");
	objs.textures[2] = loadImage("images\\face01_2562.jpg");
	objs.textures[3] = loadImage("images\\face01_2563.jpg");
	objs.textures[4] = loadImage("images\\face01_2564.jpg");
	objs.textures[10] = loadImage("images\\sky02.jpg");
	objs.textures[20] = loadImage("images\\grass.jpg");
	objs.textures[21] = loadImage("images\\leaf1.jpg");
	//glEnable(GL_TEXTURE_2D);
	//glShadeModel(GL_SMOOTH);
	//glClearDepth(1.0);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	Create_List();

	CalculateEyeXY();		//�������
	CalculateRadius();		//����-���� �Ÿ����

	glutDisplayFunc(MyDisplay);
	glutKeyboardFunc(MyKeyboard);
	glutSpecialFunc(Specialkeys);	//����Ű
	glutKeyboardUpFunc(MyKeyboardUp);	//Ű���� �� ����. �Ź��� glut.dll �ʿ�
	glutMouseFunc(MyMouseMove);
	glutMotionFunc(MyMotion);
	//	glutPassiveMotionFunc(MyPassiveMotion);	//���콺 �׳��̵� ���� �ݹ�
	glutReshapeFunc(MyReshape);

	glutTimerFunc(sleepSpeed, MyTimer, 1);		//�ʴ�70������. Ÿ�̸� ���


	GLint SubMenu2 = glutCreateMenu(MySubMenu);	//����޴�����2
	glutAddMenuEntry("On", 4);
	glutAddMenuEntry("Off", 5);

	GLint MyMainMenuID = glutCreateMenu(MyMainMenu);	//���θ޴����
	glutAddMenuEntry("��Ʈ�� ���� ", 50);
	glutAddMenuEntry("������Ʈ �׽�Ʈ (AI on)", 800);
	glutAddMenuEntry("������Ʈ �׽�Ʈ (AI off)", 850);
	glutAddSubMenu("BGM on/off", SubMenu2);		//����޴� ����
	glutAddMenuEntry("�ʱ�ȭ", 900);
	glutAttachMenu(GLUT_RIGHT_BUTTON);		//���θ޴� ����

	printf("--------------------------------------------------------------------- \n");
	printf("- E : WCS������ ������ �� ǥ�� & �������� / �ǰ����� �� ǥ�� on/off \n");
	printf("- R : ���̾������� on/off \n");
	printf("- T : �÷�/���μ��̵� ��� \n");
	printf("- Z, X : ����/�ܾƿ� \n");
	printf("- N, M : ������ ������ ���� �̵�/�־����� \n");
	printf("- C : Ÿ�� ��ƼŬ on/off \n");
	printf("- V : �� �׸���/�ȱ׸��� \n");
	printf("- F : �Ȱ� on/off \n");
	printf("- G : Ʈ�� �׸��� ��� ��ȯ - �Ǹ���/����Ż/Off \n");
	printf("- [,] : �ִϸ��̼� �ӵ� ���� \n");
	printf("- P : �ִϸ��̼� �Ͻ�����/��� \n");
	printf("- ȭ��ǥŰ, Ű�е�0, Ű�е�Delete : �����̵� \n");
	printf("- Ű�е��� ����Ű, Ű�е�+,- : �����̵� \n");
	printf("- ���콺 Ŭ��-�巡�� : ����ȸ�� (�ﰢ�Լ��� �̿��� ������ ���� ��ġ ���) \n");
	printf("- H : ���� \n");
	printf("- Q : ���� \n");
	printf("------------------------ ���ΰ� ���� --------------------------------- \n");
	printf("- W,A,S,D (����Ű): �̵� \n");
	printf("- �������� 2���Է� : �޸��� \n");
	printf("- J : ���� \n");
	printf("- K : ������ \n");
	printf("- F1 : ��ü��1��      - F2 :���ΰ�ü��1��   - F3 : ���ΰ�ü�� ä�� \n");
	printf("\n");

	glutMainLoop();
}