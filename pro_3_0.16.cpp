//#include<glut.h>
//#include<gl.h>
//#include<glu.h>
#include "person.h"		//사람
#include "particle.h"	//파티클
#include "treeCyl.h"	//프렉탈 트리
#include "car.h"		//자동차

using namespace std;

static GLfloat MyPlane[] = { 1.0, 0.0, 0.0, 0.0 };//나무2 생성용

const int MAX_ANINUM = 1;				//스레드 애니매이션 총갯수
const int MAX_PGENERATOR = 10;

TreeCyl leafs[100000];//프렉탈 이용해 그릴 배열

float temp1 = 0.1, temp2 = 0.1;
float lifeDot = 0.818 / 1000.0;	//체력1당 표시도트

double width;	//윈도우 크기 받기위한 변수
double height;
double radius;	//초점-시점간 거리(회전용)

int timer2 = 0;
//int sleepSpeed = 14;	//매 프레임 지연시간(초당 70프레임)
int sleepSpeed = 17;	//매 프레임 지연시간(초당 60프레임)
short keyDoubleTouchTime = 20;	//얼마 이내에 키를 두번누르면 더블입력으로 볼것인가
short up1, down1, left1, right1;		//입력 감지용
short up2, down2, left2, right2;	//더블터치 감지용

bool isFogDraw = true;								// 먼곳 안개효과 on/off용
GLuint fogMode[] = { GL_EXP, GL_EXP2, GL_LINEAR };	// 세가지 안개종류
GLuint fogFilter = 2;								// 사용할 안개종류
GLfloat fogColor[4] = { 0.5f, 0.5f, 0.5f, 1.0f };      // 안개색상

int FlatShaded = 0;// flatshading과 wireframe을 토글링을 위한 부울 변수
int Wireframed = 0;

double TableX = 5.0;	//테이블 크기
double TableY = 8.0;
double TableHeight = 2.0;//테이블 높이

bool isBgmPlay = true;	//bgm 재생여부
bool isPaused = false;	//일시정지 여부
bool isAtDraw = false;	//초점,전역좌표계 원점 그리나
bool isHelpDraw = false;	//도움말 보이나
bool isLifeBarDraw = false;	//라이프바 그리나
bool isCanMouse = true;	//마우스 컨트롤 가능하나 
bool isParticleDraw = true;		//파티클 그리나
bool isCarDraw = false;		//차 그리나
bool isCyren = false;		//사이렌효과
short treeDrawMode = 0;	//트리 모드   0 : 실린더   1: 프랙탈 트리
short mountainDrawMode = 0;	//산 모드   0 : 단순   1: 텍스처

int ViewX = 450;// 마우스 움직임에 따라 시점을 바꾸기 위한 변수
int ViewY = 170;//-25;

int oldX;	//마우스커서위치
int oldY;

double plusX;	//더할각도
double plusY;

//float eyeAngle[3];	//시점각도

GLUquadricObj* cyl;	//실린더 객체

int TableListID;	//리스트용 ID. 테이블
int netID;			//리스트용 ID. 테이블 네트
int listID[20];			//리스트용 ID

float ball[3];  //볼위치

float RacketColor[3] = { 200 / 255.0, 50 / 255.0, 50 / 255.0 };		//라켓색

objects objs;						//객체들(사람배열)
Car* car = new Car(&objs);

HANDLE Thread[MAX_ANINUM];				//애니매이션용 스레드 (중지가능케 하기 위함)
UINT ThreadState[MAX_ANINUM];			//스레드 상태 받을 변수(현재 미사용. 사용시 종료여부 알수있음)
UINT WINAPI AniIntro(void* arg);		//스레드 호출함수 - 인트로
UINT WINAPI AniEnding(void* arg);	//스레드 호출함수 - 핑퐁

string currSound;	//현재 재생중인 효과음. (끄기 위함)

//-----------------------------------------
// 파티클 생성기 배열
ParticleGenerator pGenerators[MAX_PGENERATOR];

//----- 광 원 ----------------------------
GLfloat ambi0[] = { 0.4f, 0.4f, 0.4f, 1.0f };//주변광 	//-----햇빛 
GLfloat diff0[] = { 0.7f, 0.7f, 0.7f, 1.0f };//확산광
//GLfloat spec0[] = {1.0f, 1.0f, 1.0f, 1.0f};		//반사광
GLfloat spec0[] = { 0.9, 0.9, 0.9, 1.0 };		//반사광
GLfloat posi0[] = { 400.0, 800.0, -300.0, 1.0 };	//광원위치. x,y,z,거리

GLfloat ambi1[] = { 0.4f, 0.4f, 0.4f, 1.0f };//주변광		//흰빛
GLfloat diff1[] = { 1.0f, 1.0f, 1.0f, 1.0f };//확산광
GLfloat spec1[] = { 1.0f, 1.0f, 1.0f, 1.0f };		//반사광
GLfloat posi1[] = { 0.0, 10.0, -3.0, 1.0 };	//광원위치. x,y,z,거리

GLfloat ambi5[] = { 1.0, 0.0, 0.0, 1.0 };				//붉은빛
GLfloat diff5[] = { 1.0, 0.0, 0.0, 1.0 };
//GLfloat spec5[] = {1.0, 1.0, 1.0, 1.0};
GLfloat spec5[] = { 1.0, 0.0, 0.0, 1.0 };
GLfloat posi5[] = { 3.0, 5.0, -1.0, 1.0 };
GLfloat spotAngle5[] = { 10.0 };
GLfloat angleX5 = 0;
GLfloat dir5[] = { 0,0,0 };
GLfloat dir6[] = { 0,0,0 };				//붉은빛2

GLfloat mAmbi[] = { 0.5, 0.5, 0.5, 1 };	//물체특성
GLfloat mDiff[] = { 0.5, 0.5, 0.5, 1 };
GLfloat mSpec[] = { 0.5, 0.5, 0.5, 1.0 };
GLfloat mShin[] = { 5.0 };

//----------------  함 수 ----------------------------------------
void setupGL();
void setupParticle();

void InitLight();				//광원초기화
void InitPosition();			//사람위치,각도초기화
void MyMainMenu(int entryID);	//우클릭메뉴
void MySubMenu(int entryID);	//서브메뉴
void Create_TableList();		//디스플레이리스트 생성
void MyMouseMove(int button, int state, GLint X, GLint Y);	//마우스 클릭이벤트 인식하는 함수
void MyMotion(GLint X, GLint Y);					//마우스 클릭이동 이벤트 인식하는 함수
void MyMouseMove2(int button, int state, GLint X, GLint Y);	//마우스 강제클릭
void MyMotion2(GLint X, GLint Y);					//마우스 강제클릭이동
void EyePosChange(GLint angleX, GLint angleY);		//시점이동(MyMotion에서 분리. 애니에서 컨트롤가능케 하기위함)
void MyPassiveMotion(GLint X, GLint Y);	//마우스 클릭없이 이동만으로 이벤트받는 함수(미사용)
void CalculateEyeXY();		//시점좌표 구하는 함수
void CalculateRadius();		//초점-시점간 거리/각도구함
void MyKeyboard(unsigned char key, int x, int y);	//키보드 콜백
void Specialkeys(int key, int x, int y);		//방향키 등 콜백
void DrawTable();//(미사용)
void MyDisplay();								//디스플레이 콜백
void MyReshape(int w, int h);					//화면조정 콜백
void MyTimer(int Value);						//타이머 콜백
//void DrawRacket();								//라켓 그리기
//void DrawBall();								//공 그리기
void MyKeyboardUp(unsigned char key, int x, int y);	//키보드 업
void p1ActionChange();								//주인공 방향조작에 따른 행동을 시키는 함수
void DrawLifeBar();							//라이프바 표시
void DrawCylTree();							//실린더 트리 출력
void DrawFractalTree();							//프랙탈 트리 출력
void RenderText(char* text, float posX, float posY);	//글자출력
void RenderHelp();							//도움말 출력
void RenderEnd();							//끝 출력

void drawLightPoll();  //가로등 만들기
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
//데몬 텍스처로드함수(jpg,png가능)
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
			//			exit(-1);	//이미지 없어도 종료안되게 설정
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
		//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);//밉맵불가한듯

				// Specify the texture specification	윗줄 밉맵 아닐시
		glTexImage2D(GL_TEXTURE_2D,     // Type of texture
			0,    // Pyramid level (for mip-mapping) - 0 is the top level
			ilGetInteger(IL_IMAGE_BPP), // Image colour depth
			ilGetInteger(IL_IMAGE_WIDTH), // Image width
			ilGetInteger(IL_IMAGE_HEIGHT), // Image height
			0,    // Border width in pixels (can either be 1 or 0)
			ilGetInteger(IL_IMAGE_FORMAT), // Image format (i.e. RGB, RGBA, BGR etc.)
			GL_UNSIGNED_BYTE,  // Image data type
			ilGetData());   // The actual image data itself

		//밉맵일시
		//gluBuild2DMipmaps(GL_TEXTURE_2D, 3, TextureImage->sizeX, TextureImage[0]->sizeY, GL_RGB, GL_UNSIGNED_BYTE, TextureImage[0]->data);
	}
	else // If we failed to open the image file in the first place...
	{
		error = ilGetError();
		std::cout << "Image load failed - IL reports error: " << error << " - " << iluErrorString(error) << std::endl;
		//		exit(-1); //에러나도 종료안되게
		return NULL;
	}

	ilDeleteImages(1, &imageID); // Because we have already copied image data into texture data we can release memory used by image.

	std::cout << "Texture creation successful." << std::endl;

	return textureID; // Return the GLuint to the texture so you can use it!
}

//사람위치, 각도 초기화
void InitPosition()
{
	sndPlaySound(NULL, SND_ASYNC);//소리재생 중지

	DWORD nExitCode = NULL;
	for (int i = 0; i < MAX_ANINUM; i++) {
		GetExitCodeThread(Thread[i], &nExitCode);	//
		TerminateThread(Thread[i], nExitCode);		//스레드 강제종료
		CloseHandle(Thread[i]);					//포인터 제거
	}

	for (unsigned int i = 0; i < objs.people.size(); i++) {	//사람배열 돌며
		objs.people.at(i)->InitPosition();				//좌표,각도 초기화
		objs.people.at(i)->InitColor();				//색초기화		
	}

	double tempAtX = objs.AtX;	//초점변화 차이계산용
	double tempAtY = objs.AtY;
	double tempAtZ = objs.AtZ;
	objs.AtX = 3;		//초점
	objs.AtY = 14;
	objs.AtZ = 2;
	objs.EyeX -= tempAtX - objs.AtX;	//시점도 초점이동치 만큼 이동
	objs.EyeY -= tempAtY - objs.AtY;
	objs.EyeZ -= tempAtZ - objs.AtZ;
}

//직접그리는 큐브
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

//광원&안개설정
void InitLight() {
	glClearColor(0.9f, 0.9f, 0.9f, 1.0f);            // 안개 색으로 화면을 지움
	glFogi(GL_FOG_MODE, fogMode[fogFilter]);        // 안개모드 0~2. exp, exp2, linear
	glFogfv(GL_FOG_COLOR, fogColor);            // 안개 색을 설정
	glFogf(GL_FOG_DENSITY, 0.1f);                // 안개의 밀도(exp만)
//	glHint(GL_FOG_HINT, GL_DONT_CARE);            // 안개 힌트 값. - opengl자동
	glHint(GL_FOG_HINT, GL_FASTEST);            // 안개 힌트 값.	정점당 생성
//	glHint(GL_FOG_HINT, GL_NICEST);            // 안개 힌트 값.	픽셀당 생성
	glFogf(GL_FOG_START, -50.0f);                // 안개 시작 깊이
	glFogf(GL_FOG_END, 400.0f);                // 안개 끝 깊이
	if (isFogDraw == true) {
		glEnable(GL_FOG);                    // 안개 켬

	}

	glEnable(GL_DEPTH_TEST);	//은면제거
//	glEnable(GL_NORMALIZE);		//정규화
	glEnable(GL_SMOOTH);		//정점색상 부드럽게 연결. 구로셰이딩
	glEnable(GL_LIGHTING);	//빛받는 각도에 따라 색상변화
//	glDepthMask(GL_TRUE);	//뒤에있는것 불투명
//	glEnable(GL_CULL_FACE);//후면제거
//	glCullFace(GL_BACK);		

	glLightfv(GL_LIGHT0, GL_AMBIENT, ambi0);	//광원0정보 셋팅
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diff0);
	glLightfv(GL_LIGHT0, GL_SPECULAR, spec0);
	glLightfv(GL_LIGHT0, GL_POSITION, posi0);

	glLightfv(GL_LIGHT1, GL_AMBIENT, ambi1);	//광원1정보 셋팅
	glLightfv(GL_LIGHT1, GL_DIFFUSE, diff1);
	glLightfv(GL_LIGHT1, GL_SPECULAR, spec1);
	glLightfv(GL_LIGHT1, GL_POSITION, posi1);

	glLightfv(GL_LIGHT5, GL_AMBIENT, ambi5);	//광원5정보 셋팅
	glLightfv(GL_LIGHT5, GL_DIFFUSE, diff5);
	glLightfv(GL_LIGHT5, GL_SPECULAR, spec5);
	glLightfv(GL_LIGHT5, GL_POSITION, posi5);

	glLightfv(GL_LIGHT6, GL_AMBIENT, ambi5);	//광원6정보 셋팅
	glLightfv(GL_LIGHT6, GL_DIFFUSE, diff5);
	glLightfv(GL_LIGHT6, GL_SPECULAR, spec5);
	glLightfv(GL_LIGHT6, GL_POSITION, posi5);

	glEnable(GL_LIGHT0);			//조명번호0활성화
//	glEnable(GL_LIGHT1);			//조명번호1활성화

	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
	glMateriali(GL_FRONT, GL_SHININESS, 10);		//반사정도 지정
	glEnable(GL_COLOR_MATERIAL);

	//텍스트 밝게 하기용 광원설정
	GLfloat ambientLight7[] = { 0.5f, 0.5f, 0.5f, 1.0f };//주변광
	GLfloat diffuseLight7[] = { 0.5f, 0.5f, 0.5f, 1.0f };//확산광
//	GLfloat specular7[] = {1.0f, 1.0f, 1.0f, 1.0f};		//반사광	
	GLfloat specular7[] = { 0.1f, 0.1f, 0.1f, 0.1f };		//반사광
	GLfloat position7[] = { 10.0, 50.0, 50.0, 1.0 };	//광원위치. x,y,z,거리
	glLightfv(GL_LIGHT7, GL_AMBIENT, ambientLight7);	//광원정보 셋팅. 텍스트 밝게 보이게 하기위함
	glLightfv(GL_LIGHT7, GL_DIFFUSE, diffuseLight7);
	glLightfv(GL_LIGHT7, GL_SPECULAR, specular7);
	glLightfv(GL_LIGHT7, GL_POSITION, position7);

	glEnable(GL_POINT_SMOOTH);//점 원형으로
//    glEnable(GL_BLEND);		//블렌드모드
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);	//블렌드 옵션

	glEnable(GL_NORMALIZE);//법선 정규화
}

//디스플레이 콜백
void MyDisplay() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	width = glutGet(GLUT_WINDOW_WIDTH);
	height = glutGet(GLUT_WINDOW_HEIGHT);
	gluPerspective(60.0, (GLfloat)width / height, 0.01, 500.0);

	// 마우스 움직임에 따라 시점변화
	gluLookAt(objs.EyeX, objs.EyeY, objs.EyeZ, objs.AtX, objs.AtY, objs.AtZ, 0.0, 1.0, 0.0);
	//	printf("eyex = %f , eyey = %f , eyez = %f \n",(float)(ViewX - width/2.0)/width*10, (float)(height/2 - ViewY)/height*10, (float)ViewZ);

	if (isHelpDraw == true) {//도움말 표시. 프로젝션 행렬 사용
		RenderHelp();
	}
	if (objs.gameState == 4) {//도움말 표시. 프로젝션 행렬 사용
		RenderEnd();
	}

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	//	glDisable(GL_COLOR_MATERIAL);

		//하늘 구 그림
	glDisable(GL_DEPTH_TEST);	//깊이버퍼 끔
	glDisable(GL_CULL_FACE);//후면제거 끔
	glCallList(listID[2]);			//하늘 구(라이프바 가리면 켜고 그려야)
//	glEnable(GL_CULL_FACE);//후면제거
//	glEnable(GL_DEPTH_TEST);

	if (mountainDrawMode == 0) {
		glCallList(listID[4]);			//산
	}

	glEnable(GL_DEPTH_TEST);	//깊이버퍼 켬

	//	glCallList(listID[0]);			//큐브바닥
	glCallList(listID[1]);			//텍스처 바닥


	if (treeDrawMode == 0) {//실린더 나무들 그리기
		DrawCylTree();
	}
	else if (treeDrawMode == 1) {//프랙탈 나무들 그리기
		DrawFractalTree();
	}

	if (isCarDraw == true) {		//차 그리기
		glPushMatrix();
		glTranslatef(car->pos.x, car->pos.y, car->pos.z);
		glRotatef(car->angles.y + car->dAngle, 0, 1, 0);
		glCallList(listID[10]);	//단순 차(큐브2개)
		glPopMatrix();
	}




	//	glutWireSphere(0.0, 5, 5);	//반사체.

	if (isAtDraw == TRUE) {//시,초점 표시
		glColor3f(255 / 255.0, 152 / 255.0, 10 / 255.0);
		glutWireSphere(0.5, 20, 16);	//WCS원점에 붉은 구 그림. 반지름, 경선, 위선

		glPushMatrix();
		glTranslatef(objs.AtX, objs.AtY, objs.AtZ);
		glColor3f(0.5, 0.9, 0.5);		//색설정. 초록색
		glutWireSphere(0.5, 20, 16);	//WCS원점에 붉은 구 그림. 반지름, 경선, 위선
		glPopMatrix();
	}

	//파티클 그림
	glEnable(GL_BLEND);		//블렌드 활성
	for (int i = 0; i < MAX_PGENERATOR; i++) {
		if (pGenerators[i].alive == true) {
			pGenerators[i].DrawParticle();
		}
	}
	glDisable(GL_BLEND);		//블렌드 비활성


	glDisable(GL_FOG);
	if (isLifeBarDraw == true) {
		//		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);	//다른색 영향안받게
		DrawLifeBar();			//라이프바 표시
//		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	}
	if (isFogDraw == true) {
		glEnable(GL_FOG);
	}

	if (isPaused == false) {
		angleX5 += 5;//glutGet(GLUT_ELAPSED_TIME)%360;
		dir5[0] = cos(angleX5 * 3.14 / 180.0);	//조명5용 방향
		dir5[2] = sin(angleX5 * 3.14 / 180.0);
		dir6[0] = -dir5[0];	//조명6 방향
		dir6[2] = -dir5[2];
	}


	//	GLfloat Emit[] = {1.0, 0.0, 0.0, 1.0};
//	glMaterialfv(GL_FRONT, GL_EMISSION, Emit);		//물체특성 셋팅
	//glMaterialfv(GL_FRONT, GL_AMBIENT, mAmbi);		//물체특성 셋팅
	//glMaterialfv(GL_FRONT, GL_DIFFUSE, mDiff);
	//glMaterialfv(GL_FRONT, GL_SPECULAR, mSpec);
	//glMaterialfv(GL_FRONT, GL_SPOT_DIRECTION, dir5);
	//glMaterialfv(GL_FRONT, GL_SPOT_CUTOFF, spotAngle5);
	//glMaterialf(GL_FRONT, GL_SPOT_EXPONENT, 1.0f);
	//glMaterialfv(GL_FRONT, GL_SHININESS, mShin);	
	//glMaterialfv(GL_EMISSION, GL_SHININESS, diff5);


//	glLightfv(GL_LIGHT1, GL_POSITION, posi1);	//조명1위치변화 셋

	if (isCarDraw == true && isCyren == true) {
		glEnable(GL_LIGHT5);			//조명번호5활성화
		glEnable(GL_LIGHT6);			//조명번호5활성화
		glPushMatrix();				//붉은광원구(경찰차위)
		glColor3f(1, 0.3, 0.3);
		glTranslatef(posi5[0], posi5[1] - 0.5, posi5[2]);
		glScalef(1, 2, 1);
		glutSolidSphere(0.3, 10, 10);
		glPopMatrix();

		posi5[0] = car->pos.x;
		posi5[1] = car->pos.y + 4.5;
		posi5[2] = car->pos.z;
		glLightfv(GL_LIGHT5, GL_POSITION, posi5);//광원5 설정
		glLightfv(GL_LIGHT5, GL_SPOT_DIRECTION, dir5);//광원 설정
		glLightfv(GL_LIGHT5, GL_SPOT_CUTOFF, spotAngle5);
		glLightf(GL_LIGHT5, GL_SPOT_EXPONENT, 1.0f);
		glLightfv(GL_LIGHT6, GL_POSITION, posi5);//광원6 설정
		glLightfv(GL_LIGHT6, GL_SPOT_DIRECTION, dir6);//광원 설정
		glLightfv(GL_LIGHT6, GL_SPOT_CUTOFF, spotAngle5);
		glLightf(GL_LIGHT6, GL_SPOT_EXPONENT, 1.0f);

		//glLightfv(GL_LIGHT6, GL_POSITION, posi5);//광원 설정
		//glLightfv(GL_LIGHT6, GL_SPOT_DIRECTION, dir5);//광원 설정
		//glLightfv(GL_LIGHT6, GL_SPOT_CUTOFF, spotAngle5);
		//glLightf(GL_LIGHT6, GL_SPOT_EXPONENT, 1.0f);

		glColor4f(1.0, 0, 0, 0.1);
		cyl = gluNewQuadric();//원뿔
		glPushMatrix();
		glTranslatef(posi5[0], posi5[1] - 0.1, posi5[2]);
		glRotatef(-angleX5 + 90, 0, 1, 0);		//붉은빛1
		glRotatef(0, 1, 0, 0);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);	//블렌드 옵션

		gluCylinder(cyl, 0, 10, 100, 10, 10);

		glRotatef(180, 0, 1, 0);				//붉은빛2
		gluCylinder(cyl, 0, 10, 100, 10, 10);

		glDisable(GL_BLEND);
		glPopMatrix();
	}
	else {
		glDisable(GL_LIGHT5);			//조명번호5비활성화
		glDisable(GL_LIGHT6);			//조명번호6비활성화
	}

	//	glFlush();
	glutSwapBuffers();	//더블버퍼링
}

//디스플레이리스트
void Create_List() {
	//테이블 그림
	TableListID = glGenLists(1);
	glNewList(TableListID, GL_COMPILE);
	glPushMatrix();
	glColor3f(0.5, 0.25, 0.0);
	cyl = gluNewQuadric();
	glRotatef(-90, 1.0, 0.0, 0.0);
	gluCylinder(cyl, 0.2, 0.2, TableHeight, 10, 10); //기둥1
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
	glutSolidCube(1);							//위판 그리기(위3줄부터)						
	//glTranslatef(0.0, 0.0, TableHeight);//위판을 채움사각형으로 그리기
	//glBegin(GL_QUADS);					
	//	glVertex3f(-0.5, 0, 0);
	//	glVertex3f(TableX+0.5, 0, 0);
	//	glVertex3f(TableX+0.5, -TableY-0.5, 0);
	//	glVertex3f(-0.5, -TableY-0.5, 0);
	//glEnd();
	glPopMatrix();

	glPushMatrix();							//삼각뿔1
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
	glPushMatrix();						//삼각뿔2
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

	//큐브 바닥
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

	//텍스처바닥
	listID[1] = glGenLists(1);
	glNewList(listID[1], GL_COMPILE);
	//	    glEnable(GL_BLEND);		//블렌드모드
	//		glBlendFunc(GL_SRC_ALPHA, GL_ONE);	//블렌드 옵션
	glPushMatrix();
	glBindTexture(GL_TEXTURE_2D, objs.textures[0]);	//돌길

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
	glBindTexture(GL_TEXTURE_2D, objs.textures[20]);	//왼풀길

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
	glBindTexture(GL_TEXTURE_2D, objs.textures[20]);	//오른풀길

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

	glDisable(GL_BLEND);		//블렌드 비활성
	glEndList();

	//하늘구
	listID[2] = glGenLists(1);
	glNewList(listID[2], GL_COMPILE);
	glPushMatrix();

	glColor3f(1, 1, 1);
	glRotatef(180, 0, 0, 1);

	glBindTexture(GL_TEXTURE_2D, objs.textures[10]);
	//		glTexGeni(GL_Q, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);

	cyl = gluNewQuadric();
	gluQuadricTexture(cyl, 2);

	gluSphere(cyl, 300, 50, 50);	//구
	glPopMatrix();
	glEndList();

	//프렉탈 나무 1개
	listID[3] = glGenLists(1);
	glNewList(listID[3], GL_COMPILE);
	int fsrc = 0;		//프렉탈 배열에서 그릴위치
	int fdest = 1;	//프렉탈 배열에서 넣을위치
//		Vec3<float> tempPos;
//		float mat[16];
	cyl = gluNewQuadric();

	glPushMatrix();
	leafs[0].pos.set(0.0f, 0.0f, 0.0f);		//첫 원통 셋팅
	leafs[0].angle.set(0.0f, 0.0f, 0.0f);
	leafs[0].height = 4.0f;
	leafs[0].radius = 0.5f;
	glRotatef(-90, 1, 0, 0);				//세워 그려지게
	glScalef(2, 2, 2);				//늘림

	glColor3f(236 / 255.0, 77 / 255.0, 0 / 255.0);//갈색
	gluCylinder(cyl, leafs[0].radius, leafs[0].radius, leafs[0].height, 10, 5);//첫 원통 그림
	glTranslatef(0, 0, 2);				//위에서 그려지게

	for (int i = 0; i < 10000; i++) {
		glPushMatrix();
		glTranslatef(leafs[fsrc].pos.x, leafs[fsrc].pos.y, leafs[fsrc].pos.z);
		glRotatef(leafs[fsrc].angle.z, 0, 0, 1);
		glRotatef(leafs[fsrc].angle.y, 0, 1, 0);
		glRotatef(leafs[fsrc].angle.x, 1, 0, 0);//소스 위치로 이동/회전

		for (int j = 0; j < 3; j++) {
			glPushMatrix();
			leafs[fdest].angle.set(leafs[fsrc].angle.x + random(-50.0, 50.0), leafs[fsrc].angle.y + random(-50.0, 50.0), leafs[fsrc].angle.z);//소스에서 각도랜덤셋
			leafs[fdest].pos.set(0, 0, leafs[fsrc].height * 2 / 3);
			leafs[fdest].height = leafs[fsrc].height * 2.5 / 3.0;	//높이변화
			leafs[fdest].radius = leafs[fsrc].radius * 2 / 3.0;	//크기변화

			glTranslatef(leafs[fdest].pos.x, leafs[fdest].pos.y, leafs[fdest].pos.z);
			glRotatef(leafs[fdest].angle.z, 0, 0, 1);
			glRotatef(leafs[fdest].angle.y, 0, 1, 0);
			glRotatef(leafs[fdest].angle.x, 1, 0, 0);//소스 위치로 이동/회전	
			glColor3f(100 / 255.0, 200 / 255.0, 100 / 255.0);//초록색

			if (leafs[fdest].radius < 0.1) {	//잎 크기 제한. 넘으면 안그림
				gluCylinder(cyl, leafs[fdest].radius, leafs[fdest].radius, leafs[fdest].height, 3, 3);//첫 원통 그림
			}

			fdest++;
			glPopMatrix();
		}
		fsrc++;
		glPopMatrix();
	}
	glPopMatrix();
	glEndList();

	//단순 산
	listID[4] = glGenLists(1);
	glNewList(listID[4], GL_COMPILE);
	float tcos;
	float tsin;
	float ran;
	glColor3f(0.3, 1, 0.3);
	for (int i = 0; i < 360; i += 15 + ran) {
		ran = rand() % 10;		//난수
		tcos = cos(i * pi / 180.0);
		tsin = sin(i * pi / 180.0);

		glPushMatrix();
		glTranslatef(tsin * -250, 0, tcos * -250);
		glScalef(40 + ran * 6, 40 + ran * 6, 40 + ran * 6);
		//				glutSolidDodecahedron();	//12면
		glutSolidIcosahedron();	//20면
		glPopMatrix();
	}

	glEndList();

	//실린더 나무 1개
	listID[5] = glGenLists(1);
	glNewList(listID[5], GL_COMPILE);
	cyl = gluNewQuadric();
	glColor3f(236 / 255.0, 77 / 255.0, 0 / 255.0);//갈색

	glBindTexture(GL_TEXTURE_2D, objs.textures[20]);			//잎1
	//	glTexGeni(GL_Q, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	gluQuadricTexture(cyl, 2);

	glPushMatrix();
	glRotatef(-90, 1, 0, 0);
	gluCylinder(cyl, 0.5, 0.5, 6, 5, 5);	//기둥		

	glColor3f(100 / 255.0, 200 / 255.0, 100 / 255.0);//초록색
	glTranslatef(0, 0, 3);
	gluCylinder(cyl, 3, 0.2, 6.0, 8, 8);	//잎
	glTranslatef(0, 0, 2);
	gluCylinder(cyl, 3, 0.2, 6.0, 8, 8);	//잎
	glTranslatef(0, 0, 2);
	gluCylinder(cyl, 3, 0.2, 6.0, 8, 8);	//잎
	glPopMatrix();
	glBindTexture(GL_TEXTURE_2D, 0);			//텍스처 없앰
	glEndList();

	//단순 차
	listID[10] = glGenLists(1);
	glNewList(listID[10], GL_COMPILE);
	glBindTexture(GL_TEXTURE_2D, 0);	//텍스처 없앰
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

//프랙탈 트리들 그리기
void DrawFractalTree() {
	glPushMatrix();
	glTranslatef(-30, 0, 0);
	for (int i = 0; i < 7; i++) {
		glCallList(listID[3]);			//프렉탈 나무
		glTranslatef(0, 0, -10);
	}
	glPopMatrix();

	glPushMatrix();
	glTranslatef(30, 0, 0);
	for (int i = 0; i < 7; i++) {
		glCallList(listID[3]);			//프렉탈 나무
		glTranslatef(0, 0, -10);
	}
	glPopMatrix();
}

//실린더 트리들 그리기
void DrawCylTree() {
	glPushMatrix();				//오른쪽
	glTranslatef(30, 0, 300);
	for (int i = 0; i < 20; i++) {
		glCallList(listID[5]);//실린더 나무
		glTranslatef(0, 0, -30);
	}
	glPopMatrix();
	glPushMatrix();				//오른쪽2
	glTranslatef(35, 0, 297.5);
	for (int i = 0; i < 33; i++) {
		glCallList(listID[5]);//실린더 나무
		glTranslatef(0, 0, -16);
	}
	glPopMatrix();
	glPushMatrix();				//오른쪽3
	glTranslatef(40, 0, 295);
	for (int i = 0; i < 33; i++) {
		glCallList(listID[5]);//실린더 나무
		glTranslatef(0, 0, -8);
	}
	glPopMatrix();
	glPushMatrix();				//오른쪽4
	glTranslatef(45, 0, 292.5);
	for (int i = 0; i < 20; i++) {
		glCallList(listID[5]);//실린더 나무
		glTranslatef(0, 0, -27);
	}
	glPopMatrix();
	glPushMatrix();				//오른쪽5
	glTranslatef(50, 0, 290);
	for (int i = 0; i < 20; i++) {
		glCallList(listID[5]);//실린더 나무
		glTranslatef(0, 0, -22);
	}
	glPopMatrix();
	glPushMatrix();				//오른쪽6
	glTranslatef(55, 0, 287.5);
	for (int i = 0; i < 20; i++) {
		glCallList(listID[5]);//실린더 나무
		glTranslatef(0, 0, -18);
	}
	glPopMatrix();
	glPopMatrix();
	glPushMatrix();				//오른쪽7
	glTranslatef(60, 0, 285);
	for (int i = 0; i < 20; i++) {
		glCallList(listID[5]);//실린더 나무
		glTranslatef(0, 0, -30);
	}
	glPopMatrix();
	glPopMatrix();

	glPushMatrix();				//오른쪽9
	glTranslatef(70, 0, 280);
	for (int i = 0; i < 20; i++) {
		glCallList(listID[5]);//실린더 나무
		glTranslatef(0, 0, -8);
	}
	glPopMatrix();
	glPopMatrix();
	glPushMatrix();				//오른쪽10
	glTranslatef(75, 0, 277.5);
	for (int i = 0; i < 20; i++) {
		glCallList(listID[5]);//실린더 나무
		glTranslatef(0, 0, -27);
	}
	glPopMatrix();
	glPopMatrix();

	glPushMatrix();				//오른쪽12
	glTranslatef(85, 0, 272.5);
	for (int i = 0; i < 20; i++) {
		glCallList(listID[5]);//실린더 나무
		glTranslatef(0, 0, -18);
	}
	glPopMatrix();
	glPushMatrix();				//오른쪽13
	glTranslatef(90, 0, 270);
	for (int i = 0; i < 20; i++) {
		glCallList(listID[5]);//실린더 나무
		glTranslatef(0, 0, -30);
	}
	glPopMatrix();
	glPushMatrix();				//오른쪽14
	glTranslatef(95, 0, 267.5);
	for (int i = 0; i < 20; i++) {
		glCallList(listID[5]);//실린더 나무
		glTranslatef(0, 0, -16);
	}
	glPopMatrix();
	glPushMatrix();				//오른쪽15
	glTranslatef(100, 0, 265);
	for (int i = 0; i < 20; i++) {
		glCallList(listID[5]);//실린더 나무
		glTranslatef(0, 0, -8);
	}
	glPopMatrix();

	glPushMatrix();				//오른쪽17
	glTranslatef(110, 0, 260);
	for (int i = 0; i < 20; i++) {
		glCallList(listID[5]);//실린더 나무
		glTranslatef(0, 0, -22);
	}
	glPopMatrix();
	glPushMatrix();				//오른쪽18
	glTranslatef(115, 0, 257.5);
	for (int i = 0; i < 18; i++) {
		glCallList(listID[5]);//실린더 나무
		glTranslatef(0, 0, -18);
	}
	glPopMatrix();
	glPushMatrix();				//오른쪽19
	glTranslatef(120, 0, 255);
	for (int i = 0; i < 18; i++) {
		glCallList(listID[5]);//실린더 나무
		glTranslatef(0, 0, -30);
	}
	glPopMatrix();
	glPopMatrix();
	glPushMatrix();				//오른쪽20
	glTranslatef(130, 0, 250);
	for (int i = 0; i < 18; i++) {
		glCallList(listID[5]);//실린더 나무
		glTranslatef(0, 0, -16);
	}
	glPopMatrix();
	glPopMatrix();
	glPushMatrix();				//오른쪽21
	glTranslatef(135, 0, 247.5);
	for (int i = 0; i < 18; i++) {
		glCallList(listID[5]);//실린더 나무
		glTranslatef(0, 0, -8);
	}
	glPopMatrix();
	glPopMatrix();
	glPushMatrix();				//오른쪽22
	glTranslatef(140, 0, 245);
	for (int i = 0; i < 20; i++) {
		glCallList(listID[5]);//실린더 나무
		glTranslatef(0, 0, -22);
	}
	glPopMatrix();
	glPopMatrix();
	glPushMatrix();				//오른쪽23
	glTranslatef(145, 0, 242.5);
	for (int i = 0; i < 18; i++) {
		glCallList(listID[5]);//실린더 나무
		glTranslatef(0, 0, -18);
	}
	glPopMatrix();
	glPopMatrix();
	glPushMatrix();				//오른쪽24
	glTranslatef(150, 0, 240);
	for (int i = 0; i < 18; i++) {
		glCallList(listID[5]);//실린더 나무
		glTranslatef(0, 0, -30);
	}
	glPopMatrix();
	glPopMatrix();
	glPushMatrix();				//오른쪽25
	glTranslatef(160, 0, 237.5);
	for (int i = 0; i < 18; i++) {
		glCallList(listID[5]);//실린더 나무
		glTranslatef(0, 0, -16);
	}
	glPopMatrix();
	glPopMatrix();
	glPushMatrix();				//오른쪽26
	glTranslatef(165, 0, 235);
	for (int i = 0; i < 18; i++) {
		glCallList(listID[5]);//실린더 나무
		glTranslatef(0, 0, -8);
	}
	glPopMatrix();
	glPopMatrix();
	glPushMatrix();				//오른쪽27
	glTranslatef(170, 0, 232.5);
	for (int i = 0; i < 20; i++) {
		glCallList(listID[5]);//실린더 나무
		glTranslatef(0, 0, -27);
	}
	glPopMatrix();
	glPopMatrix();
	glPushMatrix();				//오른쪽28
	glTranslatef(175, 0, 230);
	for (int i = 0; i < 20; i++) {
		glCallList(listID[5]);//실린더 나무
		glTranslatef(0, 0, -22);
	}
	glPopMatrix();
	glPopMatrix();
	glPushMatrix();				//오른쪽29
	glTranslatef(180, 0, 227.5);
	for (int i = 0; i < 18; i++) {
		glCallList(listID[5]);//실린더 나무
		glTranslatef(0, 0, -18);
	}
	glPopMatrix();
	glPopMatrix();
	glPushMatrix();				//오른쪽30
	glTranslatef(190, 0, 225);
	for (int i = 0; i < 18; i++) {
		glCallList(listID[5]);//실린더 나무
		glTranslatef(0, 0, -30);
	}
	glPopMatrix();
	glPushMatrix();				//오른쪽31
	glTranslatef(195, 0, 222.5);
	for (int i = 0; i < 18; i++) {
		glCallList(listID[5]);//실린더 나무
		glTranslatef(0, 0, -16);
	}
	glPopMatrix();
	glPushMatrix();				//오른쪽32
	glTranslatef(200, 0, 220);
	for (int i = 0; i < 18; i++) {
		glCallList(listID[5]);//실린더 나무
		glTranslatef(0, 0, -8);
	}
	glPopMatrix();
	glPushMatrix();				//오른쪽33
	glTranslatef(205, 0, 217.5);
	for (int i = 0; i < 18; i++) {
		glCallList(listID[5]);//실린더 나무
		glTranslatef(0, 0, -27);
	}
	glPopMatrix();
	glPushMatrix();				//오른쪽34
	glTranslatef(205, 0, 215.5);
	for (int i = 0; i < 18; i++) {
		glCallList(listID[5]);//실린더 나무
		glTranslatef(0, 0, -30);
	}
	glPopMatrix();

	glPushMatrix();				//왼쪽
	glTranslatef(-30, 0, 300);
	for (int i = 0; i < 20; i++) {
		glCallList(listID[5]);//실린더 나무
		glTranslatef(0, 0, -30);
	}
	glPopMatrix();
	glPushMatrix();				//왼쪽2
	glTranslatef(-35, 0, 297.5);
	for (int i = 0; i < 33; i++) {
		glCallList(listID[5]);//실린더 나무
		glTranslatef(0, 0, -35);
	}
	glPopMatrix();
	glPushMatrix();				//왼쪽3
	glTranslatef(-40, 0, 295);
	for (int i = 0; i < 33; i++) {
		glCallList(listID[5]);//실린더 나무
		glTranslatef(0, 0, -8);
	}
	glPopMatrix();
	glPushMatrix();				//왼쪽4
	glTranslatef(-45, 0, 292.5);
	for (int i = 0; i < 20; i++) {
		glCallList(listID[5]);//실린더 나무
		glTranslatef(0, 0, -27);
	}
	glPopMatrix();
	glPushMatrix();				//왼쪽5
	glTranslatef(-50, 0, 290);
	for (int i = 0; i < 20; i++) {
		glCallList(listID[5]);//실린더 나무
		glTranslatef(0, 0, -22);
	}
	glPopMatrix();
	glPushMatrix();				//왼쪽6
	glTranslatef(-55, 0, 287.5);
	for (int i = 0; i < 20; i++) {
		glCallList(listID[5]);//실린더 나무
		glTranslatef(0, 0, -18);
	}
	glPopMatrix();
	glPopMatrix();
	glPushMatrix();				//왼쪽7
	glTranslatef(-60, 0, 285);
	for (int i = 0; i < 20; i++) {
		glCallList(listID[5]);//실린더 나무
		glTranslatef(0, 0, -30);
	}
	glPopMatrix();
	glPopMatrix();

	glPushMatrix();				//왼쪽9
	glTranslatef(-70, 0, 280);
	for (int i = 0; i < 20; i++) {
		glCallList(listID[5]);//실린더 나무
		glTranslatef(0, 0, -8);
	}
	glPopMatrix();
	glPopMatrix();
	glPushMatrix();				//왼쪽10
	glTranslatef(-75, 0, 277.5);
	for (int i = 0; i < 20; i++) {
		glCallList(listID[5]);//실린더 나무
		glTranslatef(0, 0, -27);
	}
	glPopMatrix();
	glPopMatrix();

	glPushMatrix();				//왼쪽12
	glTranslatef(-85, 0, 272.5);
	for (int i = 0; i < 20; i++) {
		glCallList(listID[5]);//실린더 나무
		glTranslatef(0, 0, -18);
	}
	glPopMatrix();
	glPushMatrix();				//왼쪽13
	glTranslatef(-90, 0, 270);
	for (int i = 0; i < 20; i++) {
		glCallList(listID[5]);//실린더 나무
		glTranslatef(0, 0, -30);
	}
	glPopMatrix();
	glPushMatrix();				//왼쪽14
	glTranslatef(-95, 0, 267.5);
	for (int i = 0; i < 20; i++) {
		glCallList(listID[5]);//실린더 나무
		glTranslatef(0, 0, -16);
	}
	glPopMatrix();
	glPushMatrix();				//왼쪽15
	glTranslatef(-100, 0, 265);
	for (int i = 0; i < 20; i++) {
		glCallList(listID[5]);//실린더 나무
		glTranslatef(0, 0, -8);
	}
	glPopMatrix();

	glPushMatrix();				//왼쪽17
	glTranslatef(-110, 0, 260);
	for (int i = 0; i < 20; i++) {
		glCallList(listID[5]);//실린더 나무
		glTranslatef(0, 0, -22);
	}
	glPopMatrix();
	glPushMatrix();				//왼쪽18
	glTranslatef(-115, 0, 257.5);
	for (int i = 0; i < 18; i++) {
		glCallList(listID[5]);//실린더 나무
		glTranslatef(0, 0, -18);
	}
	glPopMatrix();
	glPushMatrix();				//왼쪽19
	glTranslatef(-120, 0, 255);
	for (int i = 0; i < 18; i++) {
		glCallList(listID[5]);//실린더 나무
		glTranslatef(0, 0, -30);
	}
	glPopMatrix();
	glPopMatrix();
	glPushMatrix();				//왼쪽20
	glTranslatef(-130, 0, 250);
	for (int i = 0; i < 18; i++) {
		glCallList(listID[5]);//실린더 나무
		glTranslatef(0, 0, -16);
	}
	glPopMatrix();
	glPopMatrix();
	glPushMatrix();				//왼쪽21
	glTranslatef(-135, 0, 247.5);
	for (int i = 0; i < 18; i++) {
		glCallList(listID[5]);//실린더 나무
		glTranslatef(0, 0, -8);
	}
	glPopMatrix();
	glPopMatrix();
	glPushMatrix();				//왼쪽22
	glTranslatef(-140, 0, 245);
	for (int i = 0; i < 20; i++) {
		glCallList(listID[5]);//실린더 나무
		glTranslatef(0, 0, -20);
	}
	glPopMatrix();
	glPopMatrix();
	glPushMatrix();				//왼쪽23
	glTranslatef(-145, 0, 242.5);
	for (int i = 0; i < 18; i++) {
		glCallList(listID[5]);//실린더 나무
		glTranslatef(0, 0, -10);
	}
	glPopMatrix();
	glPopMatrix();
	glPushMatrix();				//왼쪽24
	glTranslatef(-150, 0, 240);
	for (int i = 0; i < 18; i++) {
		glCallList(listID[5]);//실린더 나무
		glTranslatef(0, 0, -15);
	}
	glPopMatrix();
	glPopMatrix();
	glPushMatrix();				//왼쪽25
	glTranslatef(-160, 0, 237.5);
	for (int i = 0; i < 18; i++) {
		glCallList(listID[5]);//실린더 나무
		glTranslatef(0, 0, -20);
	}
	glPopMatrix();
	glPopMatrix();
	glPushMatrix();				//왼쪽26
	glTranslatef(-165, 0, 235);
	for (int i = 0; i < 18; i++) {
		glCallList(listID[5]);//실린더 나무
		glTranslatef(0, 0, -8);
	}
	glPopMatrix();
	glPopMatrix();
	glPushMatrix();				//왼쪽27
	glTranslatef(-170, 0, 232.5);
	for (int i = 0; i < 20; i++) {
		glCallList(listID[5]);//실린더 나무
		glTranslatef(0, 0, -27);
	}
	glPopMatrix();
	glPopMatrix();
	glPushMatrix();				//왼쪽28
	glTranslatef(-175, 0, 230);
	for (int i = 0; i < 20; i++) {
		glCallList(listID[5]);//실린더 나무
		glTranslatef(0, 0, -22);
	}
	glPopMatrix();
	glPopMatrix();
	glPushMatrix();				//왼쪽29
	glTranslatef(-180, 0, 227.5);
	for (int i = 0; i < 18; i++) {
		glCallList(listID[5]);//실린더 나무
		glTranslatef(0, 0, -18);
	}
	glPopMatrix();
	glPopMatrix();
	glPushMatrix();				//왼쪽30
	glTranslatef(-190, 0, 225);
	for (int i = 0; i < 18; i++) {
		glCallList(listID[5]);//실린더 나무
		glTranslatef(0, 0, -20);
	}
	glPopMatrix();
	glPushMatrix();				//왼쪽31
	glTranslatef(-195, 0, 222.5);
	for (int i = 0; i < 18; i++) {
		glCallList(listID[5]);//실린더 나무
		glTranslatef(0, 0, -16);
	}
	glPopMatrix();
	glPushMatrix();				//왼쪽32
	glTranslatef(-200, 0, 220);
	for (int i = 0; i < 18; i++) {
		glCallList(listID[5]);//실린더 나무
		glTranslatef(0, 0, -10);
	}
	glPopMatrix();
	glPushMatrix();				//왼쪽33
	glTranslatef(-205, 0, 217.5);
	for (int i = 0; i < 18; i++) {
		glCallList(listID[5]);//실린더 나무
		glTranslatef(0, 0, -27);
	}
	glPopMatrix();
	glPushMatrix();				//왼쪽34
	glTranslatef(-205, 0, 215.5);
	for (int i = 0; i < 18; i++) {
		glCallList(listID[5]);//실린더 나무
		glTranslatef(0, 0, -30);
	}
	glPopMatrix();

}

void DrawBoards() {
	glPushMatrix();

	glPopMatrix();
}

//키보드콜백
void MyKeyboard(unsigned char key, int x, int y)
{
	//	printf("key = %d \n",key);

	switch (key) {
	case 'q': case 'Q': case '\033':
		exit(0);
		break;
	case 't': case 'T':		//플랫쉐이딩, 구로쉐이딩 토글링
		if (FlatShaded) {
			FlatShaded = 0;
			glShadeModel(GL_SMOOTH);
		}
		else {
			FlatShaded = 1;
			glShadeModel(GL_FLAT);
		}
		break;
	case 'r': case 'R':	//와이어프레임 토글링
		if (Wireframed) {
			Wireframed = 0;
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
		else {
			Wireframed = 1;//			
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}
		break;
	case 52:		//pad ←
		objs.AtX -= .5;
		break;
	case 56:		//pad ↑
		objs.AtY += .5;
		break;
	case 54:		//pad →
		objs.AtX += .5;
		break;
	case 50:		//pad ↓
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
	case 'z': case 'Z':					//줌인
		objs.EyeX = objs.EyeX + 0.1 * (objs.AtX - objs.EyeX);
		objs.EyeY = objs.EyeY + 0.1 * (objs.AtY - objs.EyeY);
		objs.EyeZ = objs.EyeZ + 0.1 * (objs.AtZ - objs.EyeZ);
		break;
	case 'x': case 'X':					//줌아웃
		objs.EyeX = objs.EyeX - 0.1 * (objs.AtX - objs.EyeX);
		objs.EyeY = objs.EyeY - 0.1 * (objs.AtY - objs.EyeY);
		objs.EyeZ = objs.EyeZ - 0.1 * (objs.AtZ - objs.EyeZ);
		break;
	case 'f': case 'F':					//안개모드변경
		if (isFogDraw == false) {
			isFogDraw = true;
			glEnable(GL_FOG);		//안개 켬
		}
		else {
			isFogDraw = false;
			glDisable(GL_FOG);	//안개 끔
		}
		//fogFilter++;
		//if(fogFilter == 1){
		//	glDisable(GL_FOG);	//안개 끔
		//}else if(fogFilter > 1){
		//	fogFilter = 0;
		//	glFogi (GL_FOG_MODE, fogMode[fogFilter]);	//안개적용
		//	glEnable(GL_FOG);		//안개 켬
		//}

		break;
	case 'g': case 'G':					//트리 그리기 모드 토글
		treeDrawMode = ++treeDrawMode % 3;
		break;
	case 'v': case 'V':					//산 그리기 모드 0:단순   1:안그림
		mountainDrawMode = ++mountainDrawMode % 2;
		break;
	case 'c': case 'C':					//파티클 그리나 토글
		if (isParticleDraw == false) {
			isParticleDraw = true;
		}
		else {
			isParticleDraw = false;
		}
		break;
	case 'n': case 'N':					//시점이 원점에 가까워지게
		objs.EyeX = objs.EyeX + 0.1 * (-objs.EyeX);
		objs.EyeY = objs.EyeY + 0.1 * (-objs.EyeY);
		objs.EyeZ = objs.EyeZ + 0.1 * (-objs.EyeZ);
		break;
	case 'm': case 'M':					//시점이 원점에서 멀어지게
		objs.EyeX = objs.EyeX - 0.1 * (-objs.EyeX);
		objs.EyeY = objs.EyeY - 0.1 * (-objs.EyeY);
		objs.EyeZ = objs.EyeZ - 0.1 * (-objs.EyeZ);
		break;
	case 'p': case 'P':					//일시정지 변수 컨트롤
		if (isPaused == FALSE) {
			isPaused = TRUE;
			for (int i = 0; i < MAX_ANINUM; i++) {	//스레드 일시정지
				SuspendThread(Thread[i]);
			}
		}
		else {
			isPaused = FALSE;
			for (int i = 0; i < MAX_ANINUM; i++) {	//스레드 속행
				ResumeThread(Thread[i]);
			}
		}
		break;
	case 'e': case 'E':					//초점, 전역좌표계 원점 그리기 토글
		if (isAtDraw == FALSE) {
			isAtDraw = TRUE;
		}
		else {
			isAtDraw = FALSE;
		}
		break;
	case 'h': case 'H':					//도움말 ON/OFF
		if (isHelpDraw == false) {
			isHelpDraw = true;
			glDisable(GL_FOG);		//안개 켬
		}
		else {
			isHelpDraw = false;
			if (isFogDraw == true) {
				glEnable(GL_FOG);		//안개 켬
			}
		}
		break;
	case 93:						//] 키 누를시, 프레임 지연시간 감소(애니매이션 빠르게)
		sleepSpeed -= 17;
		if (sleepSpeed < 17) {
			sleepSpeed = 17;
		}
		break;
	case 91:						//[ 키 누를시, 프레임 지연시간 증가(애니매이션 느리게)
		sleepSpeed += 17;
		if (sleepSpeed > 200) {
			sleepSpeed = 200;
		}
		break;


	case 'a': case 'A':
		objs.people.at(0)->keyHold[1] = 1;
		if (objs.people.at(0)->keyDoubleTouchLevel[1] % 2 == 0) {
			objs.people.at(0)->keyDoubleTouch[1] = keyDoubleTouchTime;	//키 더블입력 감지용 배열 설정
			objs.people.at(0)->keyDoubleTouchLevel[1]++;
		}
		break;
	case 's': case 'S':
		objs.people.at(0)->keyHold[2] = 1;
		if (objs.people.at(0)->keyDoubleTouchLevel[2] % 2 == 0) {
			objs.people.at(0)->keyDoubleTouch[2] = keyDoubleTouchTime;	//키 더블입력 감지용 배열 설정
			objs.people.at(0)->keyDoubleTouchLevel[2]++;
		}
		break;
	case 'd': case 'D':
		objs.people.at(0)->keyHold[3] = 1;
		if (objs.people.at(0)->keyDoubleTouchLevel[3] % 2 == 0) {
			objs.people.at(0)->keyDoubleTouch[3] = keyDoubleTouchTime;	//키 더블입력 감지용 배열 설정
			objs.people.at(0)->keyDoubleTouchLevel[3]++;
		}
		break;
	case 'j': case 'J':			//펀치
		objs.people.at(0)->AttKeyHold[0] = 1;	//키눌림 설정
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
		objs.people.at(0)->AttKeyHold[1] = 1;	//키눌림 설정
		if (objs.people.at(0)->ctrl) {
			objs.people.at(0)->actNo = RUPPER;
			objs.people.at(0)->ctrl = 0;
		}

		break;
	case 'l': case 'L':
		objs.people.at(0)->AttKeyHold[2] = 1;	//키눌림 설정
		break;
	case 32:		//스페이스
		break;
	}

	//if ( GetAsyncKeyState(VK_) & 0x8000 )  {
	//	cout<<"w \n";
	//}

//	p1ActionChange();	//p1행동하게

	CalculateRadius();
	glutPostRedisplay();
}

//방향키등 콜백
void Specialkeys(int key, int x, int y)
{
	//	printf("Special Key = %d \n",key);

	switch (key) {
	case 1:		//F1		적 HP 1로
		if (objs.gameState == 1) {	//게임중이면
			for (unsigned int i = 1; i < objs.people.size(); i++) {	//적배열 돌며
				objs.people.at(i)->HP = 1;				//체력셋
			}
		}
		break;
	case 2:		//F2		주인공 HP 1로		
		if (objs.gameState == 1) {
			objs.people.at(0)->HP = 1;				//체력셋		
		}
		break;
	case 3:		//F3		주인공 HP 채움
		if (objs.gameState == 1) {
			objs.people.at(0)->HP = 1000;				//체력셋	
			if (isBgmPlay == true) {
				mciSendString(("play " + soundBGM1 + " repeat").c_str(), 0, 0, 0);//play bgm
			}
		}
		break;
		//case 4:		//F4		임시(엔딩호출)
		//	Thread[0] = (HANDLE)_beginthreadex(NULL, 0, AniEnding, (void*)NULL, 0, &ThreadState[0]);	//스레드로 함수호출
		//		objs.gameState = 3;
		//	break;
	case 100:		//←		//시점이동
		objs.EyeX -= .5;
		//		temp1 -= 0.03;
		break;
	case 101:		//↑
		objs.EyeY += .5;
		//		temp2 += 0.03;
		break;
	case 102:		//→
		objs.EyeX += .5;
		//		temp1 += 0.03;
		break;
	case 103:		//↓
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
	glutPostRedisplay();	//화면 새로고침
}

//키보드 뗌
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
		//case 'j': case 'J':			//펀치
		//	objs.people.at(0)->AttKeyHold[0] = 0;	//키뗌 설정
		//	break;
		//case 'k': case 'K':
		//	objs.people.at(0)->AttKeyHold[1] = 0;	//키눌림 설정
		//	break;
		//case 'l': case 'L':
		//	objs.people.at(0)->AttKeyHold[2] = 0;	//키눌림 설정
		//	break;
	}

	p1ActionChange();	//p1행동함수 호출
}

//주인공 방향조작에 따른 행동을 시키는 함수
void p1ActionChange() {
	up1 = objs.people.at(0)->keyHold[0];
	left1 = objs.people.at(0)->keyHold[1];
	down1 = objs.people.at(0)->keyHold[2];
	right1 = objs.people.at(0)->keyHold[3];

	up2 = (objs.people.at(0)->keyDoubleTouchLevel[0] == 3) ? 1 : 0;	//더블입력 되었나 가져옴
	left2 = (objs.people.at(0)->keyDoubleTouchLevel[1] == 3) ? 1 : 0;
	down2 = (objs.people.at(0)->keyDoubleTouchLevel[2] == 3) ? 1 : 0;
	right2 = (objs.people.at(0)->keyDoubleTouchLevel[3] == 3) ? 1 : 0;

	if (objs.people.at(0)->ctrl) {	//조작가능할때만 방향에 따른 이동설정

		if (up2 || left2 || right2 || down2) {		//달리기 검사
			objs.people.at(0)->aniNo = ANI_RUN;
			for (int i = 0; i < 4; i++) {
				objs.people.at(0)->keyDoubleTouchLevel[i] = -1;
			}
		}

		if (up1 && !left1 && !right1) {			//걷기 검사. 앞쪽
			objs.people.at(0)->actNo = WALK_FRONT;
		}
		else if (up1 && left1) {						//↖
			objs.people.at(0)->actNo = WALK_FRONT_LEFT;
		}
		else if (up1 && right1) {						//↗
			objs.people.at(0)->actNo = WALK_FRONT_RIGHT;
		}
		else if (left1 && !right1 && !up1 && !down1) {	//왼쪽
			objs.people.at(0)->actNo = WALK_LEFT;
		}
		else if (!left1 && right1 && !up1 && !down1) {	//오른쪽
			objs.people.at(0)->actNo = WALK_RIGHT;
		}
		else if (down1 && !left1 && !right1) {			//아래
			objs.people.at(0)->actNo = WALK_BACK;
		}
		else if (down1 && left1) {					//↙
			objs.people.at(0)->actNo = WALK_BACK_LEFT;
		}
		else if (down1 && right1) {					//↘
			objs.people.at(0)->actNo = WALK_BACK_RIGHT;
		}
		else {
			objs.people.at(0)->actNo = IDLE;
		}
	}

	//	printf("%d %d %d %d \n",up2,left2,right2,down2);
}

//우클릭 메인메뉴
void MyMainMenu(int entryID) {
	if (isCanMouse == false) {
		return;
	}

	switch (entryID) {
	case 50:			//인트로 애니메이션
		InitPosition();	//물체 위치초기화

		//for(int i=0; i < AniNum; i++){			
		//	GetExitCodeThread(Thread[i], NULL);	//
		//	TerminateThread(Thread[i], NULL);		//스레드 강제종료
		//	CloseHandle(Thread[i]);					//포인터 제거
		//}

		isLifeBarDraw = false;
		Thread[0] = (HANDLE)_beginthreadex(NULL, 0, AniIntro, (void*)NULL, 0, &ThreadState[0]);	//스레드로 함수호출
		break;
	case 100:			//핑퐁 애니매이션
		InitPosition();	//물체 위치초기화
		break;
	case 800:	//게임파트 테스트 (AI on)
		InitPosition();	//물체 위치초기화
		objs.gameState = 1;
		isLifeBarDraw = true;
		objs.isAniControl = false;//애니제어 off
		if (isBgmPlay == true) {
			mciSendString(("play " + soundBGM1 + " repeat").c_str(), 0, 0, 0);//play bgm
		}

		objs.people.at(0)->x = -40.6;//임시
		objs.people.at(0)->z = -38;//
		objs.EyeX = objs.people.at(0)->x + 10;		//시점셋
		objs.EyeZ = objs.people.at(0)->z; // + 5;
		objs.AtX = objs.people.at(0)->x;		//초점셋
		objs.AtZ = objs.people.at(0)->z;



		objs.people.at(0)->actNo = IDLE;	//주인공 움직일 수 있게
		objs.people.at(0)->ctrl = 1;
		objs.people.at(0)->isDraw = true;		//그린다 설정
		objs.people.at(0)->HP = 1000;

		break;
	case 850:	//게임파트 테스트 (AI off)
		InitPosition();	//물체 위치초기화
		objs.gameState = 1;
		isLifeBarDraw = true;
		objs.isAniControl = false;	//애니제어 off
		if (isBgmPlay == true) {
			mciSendString(("play " + soundBGM1 + " repeat").c_str(), 0, 0, 0);//play bgm
		}

		objs.people.at(0)->x = -40.6;//임시
		objs.people.at(0)->z = -38;//
		objs.EyeX = objs.people.at(0)->x + 10;		//시점셋
		objs.EyeZ = objs.people.at(0)->z; // + 5;
		objs.AtX = objs.people.at(0)->x;		//초점셋
		objs.AtZ = objs.people.at(0)->z;


		objs.people.at(0)->actNo = IDLE;	//주인공 움직일 수 있게
		objs.people.at(0)->ctrl = 1;
		objs.people.at(0)->isDraw = true;		//그린다 설정
		objs.people.at(0)->HP = 1000;

		break;
	case 900:		//취소(초기화)
		InitPosition();	//물체 위치초기화
		objs.gameState = 0;
		isLifeBarDraw = false;
		isCarDraw = false;		//사이렌효과
		isCyren = false;		//사이렌효과
		car->Init();
		objs.isAniControl = false;
		mciSendString(("stop " + soundBGM1).c_str(), 0, 0, 0);//stop bgm



		objs.gameState = 0;	//상황상태0
		break;
	}
}

//우클릭 서브메뉴
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

//마우스 클릭콜백(눌렀을시, 뗐을시)
void MyMouseMove(int button, int state, GLint X, GLint Y)
{
	if (isCanMouse == false) {
		return;
	}

	// 마우스 움직임을 X, Y를 변수에 할당
	if (state == 0 && button == 0) {	//좌push시
		oldX = X;
		oldY = Y;
		plusX = 0;
		plusY = 0;

		//	CalculateEyeXY();
//		printf("ViewX = %d, ViewY = %d \n", ViewX, ViewY);

		//	glutPostRedisplay();
	}

	if (state == 1) {		//버튼release 시
		CalculateRadius();		//시점-초점 거리구해 셋팅
	}
}
//마우스 강제클릭용
void MyMouseMove2(int button, int state, GLint X, GLint Y)
{
	if (state == 0 && button == 0) {	//좌push시
		oldX = X;
		oldY = Y;
		plusX = 0;
		plusY = 0;
	}
	if (state == 1) {		//버튼release 시
		CalculateRadius();		//시점-초점 거리구해 셋팅
	}
}

//마우스 클릭이동콜백
void MyMotion(GLint X, GLint Y)
{
	if (isCanMouse == false) {
		return;
	}

	int diffX = (X - oldX);		//좌표차이 감지용 변수 4개
	int diffY = -(Y - oldY);
	oldX = X;
	oldY = Y;

	plusX += diffX * (180.0 / width);	//더할각도 계산
	plusY += diffY * (180.0 / height);

	double angleX = objs.eyeAngleX + plusX;	//결과각도 계산
	double angleY = objs.eyeAngleY + plusY;

	EyePosChange(angleX, angleY);
	//printf("MViewX = %d, MViewY = %d \n", X, Y);
	//printf("eyeAngleX = %f \n",objs.eyeAngleX);
	//printf("eyeAngleY = %f \n",objs.eyeAngleY);
	//printf("AngleX = %f \n",angleX);
	//printf("AngleY = %f \n",angleY);
}
//마우스 강제클릭이동용
void MyMotion2(GLint X, GLint Y)
{
	int diffX = (X - oldX);		//좌표차이 감지용 변수 4개
	int diffY = -(Y - oldY);
	oldX = X;
	oldY = Y;

	plusX += diffX * (180.0 / width);	//더할각도 계산
	plusY += diffY * (180.0 / height);

	double angleX = objs.eyeAngleX + plusX;	//결과각도 계산
	double angleY = objs.eyeAngleY + plusY;

	EyePosChange(angleX, angleY);
}

//시점이동(MyMotion에서 분리. 애니에서 컨트롤가능케 하기위함)
void EyePosChange(GLint angleX, GLint angleY) {
	if (angleY >= 89) {		//시점 상하 제한
		angleY = 89;
	}
	else if (angleY <= 1) {
		angleY = 1;
	}

	double xPosX = radius * cos(angleX * pi / 180.0);		//라디안변환하여 초점으로부터의 좌우 x성분 구함
	double xPosY = radius * sin(angleX * pi / 180.0);		//라디안변환하여 초점으로부터의 좌우 Y성분 구함
	double yPosX = radius * cos(angleY * pi / 180.0);		//라디안변환하여 초점으로부터의 상하 x성분 구함
	double yPosY = radius * sin(angleY * pi / 180.0);		//라디안변환하여 초점으로부터의 상하 Y성분 구함

	double PosZ = yPosX * cos(angleX * pi / 180.0);		//좌우,상하 통합된 위치로부터 Z이동위치 계산
	double PosX = PosZ * tan(angleX * pi / 180.0);		//좌우,상하 통합된 위치로부터 X이동위치 계산

	objs.EyeZ = objs.AtZ + PosZ;			//시점 통합좌표설정
	objs.EyeX = objs.AtX + PosX;//xPosY;
	objs.EyeY = objs.AtY + yPosY;
}

//마우스 그냥이동콜백(현재 미사용)
void MyPassiveMotion(GLint X, GLint Y) {

	double angleX = objs.eyeAngleX + (X - width / 2) / (width / 2 / 180.0);		//좌우각도구함 -180 ~ 180
	double angleY = objs.eyeAngleY + -((Y - height / 2) / (height / 2 / 180.0));	//상하각도구함

	double xPosX = radius * cos(angleX * pi / 180.0);		//라디안변환하여 초점으로부터의 좌우 x성분 구함
	double xPosY = radius * sin(angleX * pi / 180.0);		//라디안변환하여 초점으로부터의 좌우 Y성분 구함
	double yPosX = radius * cos(angleY * pi / 180.0);		//라디안변환하여 초점으로부터의 상하 x성분 구함
	double yPosY = radius * sin(angleY * pi / 180.0);		//라디안변환하여 초점으로부터의 상하 Y성분 구함

	double PosZ = yPosX * cos(angleX * pi / 180.0);		//좌우,상하 통합된 위치로부터 Z이동위치 계산
	double PosX = PosZ * tan(angleX * pi / 180.0);		//좌우,상하 통합된 위치로부터 X이동위치 계산

	//EyeZ = AtZ + xPosX;	//좌우로만 이동시
	//EyeX = AtX + xPosY;

	//EyeZ = AtZ + yPosX;	//상하로만 이동시
	//EyeY = AtY + yPosY;

	objs.EyeZ = objs.AtZ + PosZ;			//시점 통합좌표설정
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

//EyeX, EyeY를 ViewX,Y로부터 계산
void CalculateEyeXY() {
	width = glutGet(GLUT_WINDOW_WIDTH);	//윈도우 위치받음
	height = glutGet(GLUT_WINDOW_HEIGHT);

	objs.EyeX = (ViewX - width / 2) / width * 20 + 2.5;			//시점계산
	objs.EyeY = (height / 2 - ViewY) / height * 20 + 2.5;
}

//초점-시점간 거리/각도 구함
void CalculateRadius() {
	radius = sqrt(pow((objs.EyeX - objs.AtX), 2) + pow((objs.EyeY - objs.AtY), 2) + pow((objs.EyeZ - objs.AtZ), 2));//초점-시점간 거리구함

	double XX = (objs.EyeZ - objs.AtZ);								//초점-시점간 각 선길이 구함
	double YX = sqrt(pow((objs.EyeX - objs.AtX), 2) + pow((objs.EyeZ - objs.AtZ), 2));

	objs.eyeAngleX = acos(XX / YX) * 180.0 / pi;			//선길이를 이용한 역삼각함수로 시점, 초점간 각도구함.
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

//화면조정 콜백
void MyReshape(int w, int h) {
	//	printf("width = %d, height = %d \n", w, h);

	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);
}

//도움말출력
void RenderHelp()
{
	glEnable(GL_LIGHT7);	//밝게 하기용 광원

	const short textsMax = 26;	//문자 수
	int len[textsMax];
	string texts[textsMax];
	char* text[textsMax];

	float posX = -0.8, posY = 0.7;

	//texts[0] = "- 마우스우클릭 : 메뉴";
	//texts[1] = "- E : WCS원점과 초점에 구 표시 & 공격판정 / 피격판정 구 표시";
	//texts[2] = "- R : 와이어프레임/솔리드렌더링 토글";
	//texts[3] = "- T : 플랫/구로 셰이딩 토글";
	//texts[4] = "- Z, X : 줌인/줌아웃";
	//texts[5] = "- F, G : 시점을 원점을 향해 이동/멀어지게";
	//texts[6] = "- ] : 애니매이션 속도 빠르게 (지연시간 감소)";
	//texts[7] = "- [ : 애니매이션 속도 느리게 (지연시간 증가)";
	//texts[8] = "- P : 애니매이션 일시정지/재생";
	//texts[9] = "- 화살표키, 키패드0, 키패드Delete : 시점이동";
	//texts[10] = "- 키패드의 방향키, 키패드+,- : 초점이동";
	//texts[11] = "- 마우스 클릭-드래그 : 시점회전 (삼각함수 이용)";
	//texts[12] = "- Q : 종료";
	//texts[13] = "";
	//texts[14] = "------------------------ 주인공 조작 ---------------------------------";
	//texts[15] = "- W,A,S,D (방향키): 이동";
	//texts[16] = "- 같은방향 2번입력 : 달리기";
	//texts[17] = "- J : 공격";
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

	for (int i = 0; i < textsMax; i++) {	//길이, char* 받음
		text[i] = (char*)texts[i].c_str();
		len[i] = texts[i].length();
	}

	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	//	glColor3f(1.0,1.0,0.1);//노랑
	glColor3f(0, 0, 0);	//검정

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

	glDisable(GL_LIGHT7);		//텍스트 밝게 하기용 광원 끔
}

//글자출력
void RenderText(char* text, float posX, float posY)
{
	glEnable(GL_LIGHT7);	//밝게 하기용 광원

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

	glDisable(GL_LIGHT7);		//텍스트 밝게 하기용 광원 끔
}

//라이프바 표시 (꼼수. 원래 3d좌표를 2d좌표로 변환후 그려야하나)q
void DrawLifeBar() {
	//	float gap1 = objs.people.at(0)->displayHP - objs.people.at(0)->HP;	//표시체력과 현체력 차이

	//	printf("%f %f \n",temp1, temp2);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glPushMatrix();

	glTranslatef(0, 0, -0.1);
	//		glRasterPos2f(0.0,-10);

	glEnable(GL_LIGHT7);	//밝게 하기용 광원

	glBegin(GL_LINE_LOOP);
	glColor4f(1.0, 1.0, 1.0, 1);
	glLineWidth(2);	//안먹힘
	glVertex3f(-0.90, 0.90, 0);
	glVertex3f(-0.90, 0.80, 0);
	glVertex3f(-0.08, 0.80, 0);
	glVertex3f(-0.08, 0.90, 0);
	glEnd();

	//glMaterialfv(GL_FRONT, GL_AMBIENT, mAmbi);		//물체특성 셋팅
	//glMaterialfv(GL_FRONT, GL_DIFFUSE, mDiff);
	//glMaterialfv(GL_FRONT, GL_SPECULAR, mSpec);
	glMateriali(GL_FRONT, GL_SHININESS, 0);		//반사정도 지정

	glBegin(GL_QUADS);
	glColor4f(119 / 255.0, 213 / 255.0, 253 / 255.0, 1);	//파랑
	glVertex3f(-0.899, 0.899, 0);
	glVertex3f(-0.899, 0.801, 0);
	glVertex3f(-0.899 + lifeDot * objs.people.at(0)->HP, 0.801, 0);
	glVertex3f(-0.899 + lifeDot * objs.people.at(0)->HP, 0.899, 0);

	glEnd();

	if (objs.people.at(0)->displayHP >= objs.people.at(0)->HP + 1) {
		glBegin(GL_QUADS);
		glColor3f(1.0, 0.0, 0.0);			//빨강
		glVertex3f(-0.899 + lifeDot * (objs.people.at(0)->HP + 1), 0.899, 0);
		glVertex3f(-0.899 + lifeDot * (objs.people.at(0)->HP + 1), 0.801, 0);
		glVertex3f(-0.899 + lifeDot * objs.people.at(0)->displayHP, 0.801, 0);
		glVertex3f(-0.899 + lifeDot * objs.people.at(0)->displayHP, 0.899, 0);
		glEnd();
	}

	glDisable(GL_LIGHT7);		//텍스트 밝게 하기용 광원 끔

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

//End출력
void RenderEnd() {

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glPushMatrix();

	glTranslatef(0, 0, -0.1);
	//		glEnable(GL_LIGHT7);	//밝게 하기용 광원
	//		glMateriali(GL_FRONT, GL_SHININESS, 255);		//반사정도 지정

	RenderText("  END", -0.1, 0.005);

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}

//타이머 (매타임 체크동작)
void MyTimer(int Value) {
	//printf("p1x = %f p1y = %f p1z = %f \n", p1x, p1y, p1z);//위치보기

	for (int i = 0; i < 4; i++) {
		if (objs.people.at(0)->keyDoubleTouch[i] > 0) {//키 더블입력 감지시간 -1
			objs.people.at(0)->keyDoubleTouch[i]--;
		}
		if (objs.people.at(0)->keyDoubleTouch[i] <= 0 && objs.people.at(0)->keyDoubleTouchLevel[i] != -1) {	//감지시간 다되면 감지레벨 -1로
			objs.people.at(0)->keyDoubleTouchLevel[i] = -1;
		}
		if (objs.people.at(0)->keyHold[i] == 0 && objs.people.at(0)->keyDoubleTouchLevel[i] == -1) {	//감지레벨 -1이면, 키뗀 상태에서 0으로.
			objs.people.at(0)->keyDoubleTouchLevel[i] = 0;
		}

	}

	if (isPaused == false) {	//일시정지 아닐시
		if (objs.people.at(0)->keyHold[0] || objs.people.at(0)->keyHold[1] || objs.people.at(0)->keyHold[2] || objs.people.at(0)->keyHold[3]) {	//방향키중 하나 눌렸으면
			p1ActionChange();		//행동시킴
		}

		//	printf("%d %d %d %d \n", objs.people.at(0)->AttKeyHold[0], objs.people.at(0)->AttKeyHold[1], objs.people.at(0)->AttKeyHold[2], objs.people.at(0)->AttKeyHold[3]);

		for (unsigned int i = 0; i < objs.people.size(); i++) {	//사람배열 돌며
			objs.people.at(i)->AlwaysDo();			//항상실행함수 호출			
		}

		for (int i = 0; i < 10; i++) {		//주인공 타격검사
			if (objs.people.at(0)->attacks[i][3] != 0) {		//주인공 타격판정들
				float ax = objs.people.at(0)->attacks[i][0];	//임시변수 셋팅
				float ay = objs.people.at(0)->attacks[i][1];
				float az = objs.people.at(0)->attacks[i][2];
				float ar = objs.people.at(0)->attacks[i][3];

				for (unsigned int j = 1; j < objs.people.size(); j++) {//적배열 돌며
					bool isHit = false;
					float hbx;	//임시변수 셋팅(히트한 몸체위치)
					float hby;
					float hbz;
					for (int k = 0; k < 10; k++) {		//몸체판정검사
						if (objs.people.at(j)->bodies[k][3] != 0) {	//몸체판정들
							float bx = objs.people.at(j)->bodies[k][0];	//임시변수 셋팅
							float by = objs.people.at(j)->bodies[k][1];
							float bz = objs.people.at(j)->bodies[k][2];
							float br = objs.people.at(j)->bodies[k][3];
							float dis = sqrt(pow(bx - ax, 2) + pow(by - ay, 2) + pow(bz - az, 2));//거리

							if (dis <= ar + br) {			//히트
								isHit = true;
								hbx = bx;		//히트위치 기억
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

								mciSendString(("stop " + currSound).c_str(), 0, 0, 0);//현재효과음 중지

								attAttr st;					//공격정보 구조체 설정								
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

								currSound = objs.people.at(0)->hitAttr.hitSound;	//현재 효과음설정
								mciSendString(("play " + currSound).c_str(), 0, 0, 0);//play sound

								objs.people.at(j)->actNo = GETHIT;	//맞는 동작으로
								objs.people.at(j)->ctrl = 0;
								objs.people.at(j)->hitAttr = st;
							}
						}
					}

					//타격효과 파티클생성
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

		bool isWin = true;	//이겼나
		for (unsigned int j = 1; j < objs.people.size(); j++) {//적 타격검사
			if (objs.people.at(j)->manType != 10 && (objs.people.at(j)->HP > 0 || objs.people.at(j)->aniNo != ANI_LIEDOWN)) {//적 한명이라도 살아있으면
				isWin = false;
			}

			for (int k = 0; k < 10; k++) {		//적 타격판정배열
				if (objs.people.at(j)->attacks[k][3] != 0) {
					float ax = objs.people.at(j)->attacks[k][0];	//임시변수 셋팅
					float ay = objs.people.at(j)->attacks[k][1];
					float az = objs.people.at(j)->attacks[k][2];
					float ar = objs.people.at(j)->attacks[k][3];

					bool isHit = false;
					float hbx;	//임시변수 셋팅(히트한 몸체위치)
					float hby;
					float hbz;
					for (int i = 0; i < 10; i++) {		//주인공 몸체판정배열
						if (objs.people.at(0)->bodies[i][3] != 0) {		//주인공 몸체판정들
							float bx = objs.people.at(0)->bodies[i][0];	//임시변수 셋팅
							float by = objs.people.at(0)->bodies[i][1];
							float bz = objs.people.at(0)->bodies[i][2];
							float br = objs.people.at(0)->bodies[i][3];
							float dis = sqrt(pow(bx - ax, 2) + pow(by - ay, 2) + pow(bz - az, 2));//거리

							if (dis <= ar + br) {			//적공격 히트
								isHit = true;
								hbx = bx;		//히트위치 기억
								hby = by;
								hbz = bz;

								float angleSub = objs.people.at(j)->BodyAngle[2] - objs.people.at(0)->BodyAngle[2];
								if (angleSub >= 360) {
									angleSub -= 360;
								}
								else if (angleSub < 0) {
									angleSub += 360;
								}

								mciSendString(("stop " + currSound).c_str(), 0, 0, 0);//현재효과음 중지

								attAttr st;					//공격정보 구조체 설정								
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

								currSound = objs.people.at(j)->hitAttr.hitSound;	//현재 효과음설정
								mciSendString(("play " + currSound).c_str(), 0, 0, 0);//play sound

								objs.people.at(0)->actNo = GETHIT;
								objs.people.at(0)->ctrl = 0;
								objs.people.at(0)->hitAttr = st;
							}
						}
					}

					//타격효과 파티클생성
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

		if (objs.gameState == 1 && isWin == true) {	//적이겼으면			
			objs.gameState = 2;			//이긴상태
		}

		if (objs.gameState == 2 && objs.people.at(0)->actNo == IDLE) {	//적이겼으면
			Thread[0] = (HANDLE)_beginthreadex(NULL, 0, AniEnding, (void*)NULL, 0, &ThreadState[0]);	//스레드로 함수호출
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

	//	glutTimerFunc(14, MyTimer, 1);//타이머 재등록. 초당 70프레임
	glutTimerFunc(sleepSpeed, MyTimer, 1);//타이머 재등록. 초당 60프레임
}

//인트로 애니매이션 =================================================================
UINT WINAPI AniIntro(void* arg) {
	objs.isAniControl = true;
	objs.gameState = 0;

	mciSendString(("stop " + soundBGM1).c_str(), 0, 0, 0);//stop bgm

	for (unsigned int i = 0; i < objs.people.size(); i++) {//사람배열 돌며
		objs.people.at(i)->actNo = MAIN_ANI;//움직일 사람은 이 상태로 설정.

		if (objs.people.at(i)->manType != 10) {			//여자 아니면
			objs.people.at(i)->aniNo = ANI_MANSTANDING;	//서있는 동작으로
			objs.people.at(i)->AniHandler();				//동작셋
		}
	}

	Person* m = objs.people.at(0);	//사람0 가리키는 포인터
	Person* m2 = objs.people.at(1);	//사람1 가리키는 포인터
	Person* m3 = objs.people.at(2);	//사람2 가리키는 포인터
	Person* w = objs.people.at(3);	//사람3 가리키는 포인터

	if (isBgmPlay == true) {
	}

	//걸어감------------------------------------------------------	
	timer2 = 0;
	m->timer = 0;
	m->aniNo = ANI_WALK;		//걷는 동작
	float tempX = 0;
	isCanMouse = false;		//회전중 마우스 제어
	MyMouseMove2(0, 0, 0, 0);	//강제 마우스클릭
	do {
		m->movePosFoward(m->walkSpeed);//앞으로 이동
		m->AniHandler();				//애니 시킴

		if (timer2 < 60) {

			tempX += 6;
			//			EyePosChange(tempX, 0);		//각도변화			
			//			CalculateRadius();	//마우스 뗌 효과
			MyMotion2(tempX, 0);		//강제 마우스이동(시점회전)
		}
		if (timer2 == 60) {
			MyMouseMove2(0, 1, 0, 0);	//강제 마우스뗌
			isCanMouse = true;		//마우스 제어
		}

		timer2++;			//시간++
		Sleep(sleepSpeed);			//초당 프레임을 맞추기 위한 지연
	} while (timer2 < 140);	//끝나는 시간설정(60 = 1초)

	//------멈춰섬
	timer2 = 0;
	m->timer = 0;
	m->aniNo = ANI_MANSTANDING;
	mciSendString(("play " + soundVoice01).c_str(), 0, 0, 0);	//voice01
	do {
		m->AniHandler();				//애니 시킴

		if (timer2 < 70) {
			m->HeadAngle[2] += 1;	//고개돌림
		}

		timer2++;			//시간++
		Sleep(sleepSpeed);			//초당 프레임을 맞추기 위한 지연
	} while (timer2 < 140);	//끝나는 시간설정

	//------다시 걷기 시작
	timer2 = 0;
	m->timer = 0;
	m->aniNo = ANI_WALK;		//걷는 동작
	do {

		m->movePosFoward(m->walkSpeed);//앞으로 이동
		m->AniHandler();				//애니 시킴

		if (timer2 < 35) {
			m->HeadAngle[2] -= 2;
			m->BodyAngle[2] += 2;
		}

		timer2++;			//시간++
		Sleep(sleepSpeed);			//초당 프레임을 맞추기 위한 지연
	} while (timer2 < 35);	//끝나는 시간설정

	//------달려감
	timer2 = 0;
	m->timer = 0;
	m->aniNo = ANI_RUN;		//달리는 동작
	do {
		m->movePosFoward(m->runSpeed);//앞으로 이동
		m->AniHandler();				//애니 시킴

		timer2++;			//시간++
		Sleep(sleepSpeed);			//초당 프레임을 맞추기 위한 지연
	} while (timer2 < 145);	//끝나는 시간설정(60 = 1초)

	//------멈춤
	objs.people.at(0)->x = -40.6;//임시
	objs.people.at(0)->z = -38;//
	objs.EyeX = objs.people.at(0)->x + 10;		//시점셋
	objs.EyeZ = objs.people.at(0)->z; // + 5;
	objs.AtX = objs.people.at(0)->x;		//초점셋
	objs.AtZ = objs.people.at(0)->z;
	m->BodyAngle[2] = 70;

	timer2 = 0;
	m->timer = 0;
	m->aniNo = IDLE;
	m->InitAngle();
	do {
		m->BodyAngle[2] += 1;

		timer2++;			//시간++
		Sleep(sleepSpeed);			//초당 프레임을 맞추기 위한 지연
	} while (timer2 < 20);	//끝나는 시간설정(60 = 1초)

	timer2 = 0;
	mciSendString(("play " + soundVoice02).c_str(), 0, 0, 0);	//voice - 너희들뭐
	tempX = 0;
	isCanMouse = false;		//회전중 마우스 제어
	MyMouseMove2(0, 0, 0, 0);	//강제 마우스클릭
	do {
		if (((objs.eyeAngleX < 170 && objs.eyeAngleX > 10) || (objs.eyeAngleX > -170 && objs.eyeAngleX < -10)) && timer2 < 59) {
			tempX += 6;
			MyMotion2(tempX, 0);		//강제 마우스이동(시점회전)
		}
		if (timer2 == 59) {
			MyMouseMove2(0, 1, 0, 0);	//강제 마우스뗌
			isCanMouse = true;		//마우스 제어
		}

		timer2++;			//시간++
		Sleep(sleepSpeed);			//초당 프레임을 맞추기 위한 지연		
	} while (timer2 < 70);	//끝나는 시간설정(60 = 1초)

	//------적 돌아섬
	timer2 = 0;
	do {

		//		m->movePosFoward(m->runSpeed);//앞으로 이동
		//		m->AniHandler();				//애니 시킴

		m2->BodyAngle[2] -= 5;
		m3->BodyAngle[2] += 7;

		timer2++;			//시간++
		Sleep(sleepSpeed);			//초당 프레임을 맞추기 위한 지연
	} while (timer2 < 20);	//끝나는 시간설정(60 = 1초)

	timer2 = 0;
	mciSendString(("play " + soundVoice03).c_str(), 0, 0, 0);	//voice - 신경꺼
	do {
		timer2++;			//시간++
		Sleep(sleepSpeed);			//초당 프레임을 맞추기 위한 지연
	} while (timer2 < 60);	//끝나는 시간설정(60 = 1초)

	timer2 = 0;
	mciSendString(("play " + soundVoice04).c_str(), 0, 0, 0);	//voice - 여러명이서
	do {
		timer2++;			//시간++
		Sleep(sleepSpeed);			//초당 프레임을 맞추기 위한 지연
	} while (timer2 < 60);	//끝나는 시간설정(60 = 1초)

	//------고개돌림
	timer2 = 0;
	do {
		if (timer2 >= 60 && timer2 < 80) {
			m->HeadAngle[2] += 1;
		}
		else if (timer2 >= 140 && timer2 < 160) {
			m->HeadAngle[2] -= 1;
		}

		timer2++;			//시간++
		Sleep(sleepSpeed);			//초당 프레임을 맞추기 위한 지연
	} while (timer2 < 160);	//끝나는 시간설정(60 = 1초)

	timer2 = 0;
	mciSendString(("play " + soundVoice05).c_str(), 0, 0, 0);	//voice - 인지 아닌지는
	do {
		if (timer2 == 120) {
			mciSendString(("play " + soundVoice06).c_str(), 0, 0, 0);	//voice
		}
		timer2++;			//시간++
		Sleep(sleepSpeed);			//초당 프레임을 맞추기 위한 지연
	} while (timer2 < 200);	//끝나는 시간설정(60 = 1초)

	timer2 = 0;
	mciSendString(("play " + soundVoice07).c_str(), 0, 0, 0);	//voice
	do {
		timer2++;			//시간++
		Sleep(sleepSpeed);			//초당 프레임을 맞추기 위한 지연
	} while (timer2 < 240);	//끝나는 시간설정(60 = 1초)

	timer2 = 0;
	mciSendString(("play " + soundVoice08).c_str(), 0, 0, 0);	//voice
	do {
		timer2++;			//시간++
		Sleep(sleepSpeed);			//초당 프레임을 맞추기 위한 지연
	} while (timer2 < 120);	//끝나는 시간설정(60 = 1초)


	//------시작
	objs.gametime = 0;
	isLifeBarDraw = true;
	if (isBgmPlay == true) {
		mciSendString(("play " + soundBGM1 + " repeat").c_str(), 0, 0, 0);//play bgm
	}

	for (unsigned int i = 1; i < objs.people.size(); i++) {//사람배열 돌며
		if (objs.people.at(i)->manType != 10) {
			objs.people.at(i)->actNo = IDLE;
			objs.people.at(i)->ctrl = 1;
			objs.people.at(i)->isAI = true;
			objs.people.at(i)->HP = 500;
		}
	}
	objs.people.at(0)->actNo = IDLE;	//주인공 움직일 수 있게
	objs.people.at(0)->ctrl = 1;
	objs.people.at(0)->HP = 1000;

	objs.gameState = 1;	//게임파트 중인가
	objs.isAniControl = false;	//애니제어 off
	return 0;
}//-------------------인트로 애니 끝

//엔딩 애니매이션 =================================================================
UINT WINAPI AniEnding(void* arg) {
	float destPosX;			//임시 목표x
	float destPosZ;			//임시 목표z
	float timerDest[5];		//임시 시각 배열
	float timerCut[5];		//임시 시각 배열
	float angleDis;			//임시 목표각
	float angleSub;			//임시 각도차이
	float posDis;			//임시 좌표차이
	float tempX = 0;		//시점회전용 임시변수
	float tempY = 0;

	isLifeBarDraw = false;
	objs.isAniControl = true;
	objs.gameState = 3;
	Person* m = objs.people.at(0);	//주인공 포인터 셋팅
	Person* w = NULL;			//여자 포인터

	mciSendString(("stop " + soundBGM1).c_str(), 0, 0, 0);//stop bgm

	for (unsigned int i = 0; i < objs.people.size(); i++) {//사람배열 돌며
		objs.people.at(i)->actNo = MAIN_ANI;//움직일 사람은 이 상태로 설정.
		objs.people.at(i)->ctrl = 0;
		if (objs.people.at(i)->manType == 10) {
			w = objs.people.at(i);	//여자 포인터 셋팅
		}
	}

	if (w == NULL) {	//여자 없으면 리턴
		return 0;
	}
	else {
		w->isAI = false;
		w->ctrl = 0;
	}

	//남자 걸으며 회전------------------------------------------------------	
	timer2 = 0;
	m->timer = 0;
	m->aniNo = ANI_WALK;		//걷는 동작
//	tempX = 0;
//	isCanMouse = false;		//회전중 마우스 제어
//	MyMouseMove2(0, 0, 0, 0);	//강제 마우스클릭
	destPosX = w->x + 3;		//목표위치
	destPosZ = w->z;
	do {
		angleDis = m->getP2AngleXY(destPosX, destPosZ);	//좌표받아, 나로부터 여자가 몇도각도에 있나 구함
		angleSub = m->BodyAngle[2] - angleDis;	//각도차
		posDis = sqrt(pow(destPosX - m->x, 2) + pow(destPosZ - m->z, 2));	//거리구함

		m->rotateCheck(angleDis, rotateSpeed);			//여자 향해 방향정해 회전

		if (posDis > 1) {						//거리 클때만 이동
			m->movePosFoward(m->walkSpeed);//앞으로 이동
		}
		m->AniHandler();				//애니 시킴

//		if(timer2 < 60){

//			tempX += 6;
//			MyMotion2(tempX, 0);		//강제 마우스이동(시점회전)
//		}
//		if(timer2 == 60){
//			MyMouseMove2(0, 1, 0, 0);	//강제 마우스뗌
//			isCanMouse = true;		//마우스 제어
//		}

		timer2++;			//시간++
		Sleep(sleepSpeed);			//초당 프레임을 맞추기 위한 지연
	} while (timer2 < 30);	//끝나는 시간설정(60 = 1초)

	//------남자 달림
	mciSendString(("stop " + soundBGM1).c_str(), 0, 0, 0);//stop bgm
	timer2 = 0;
	m->timer = 0;
	m->aniNo = ANI_RUN;
	do {
		angleDis = m->getP2AngleXY(destPosX, destPosZ);	//좌표받아, 나로부터 여자가 몇도각도에 있나 구함
		posDis = sqrt(pow(destPosX - m->x, 2) + pow(destPosZ - m->z, 2));	//거리구함

		m->rotateCheck(angleDis, rotateSpeed);			//방향정해 회전

		m->movePosFoward(m->runSpeed);//앞으로 이동
		m->AniHandler();				//애니 시킴

		timer2++;			//시간++
		Sleep(sleepSpeed);			//초당 프레임을 맞추기 위한 지연
	} while (posDis > 0.5);	//지속 조건설정

	//------여자앞에서 멈춰회전
	mciSendString(("stop " + soundBGM1).c_str(), 0, 0, 0);//stop bgm
	timer2 = 0;
	m->x = destPosX;		//좌표셋
	m->z = destPosZ;
	m->timer = 0;
	m->aniNo = ANI_MANSTANDING;
	do {
		angleDis = m->getP2AngleXY(w->x, w->z);	//좌표받아, 나로부터 여자이 몇도각도에 있나 구함
		angleSub = m->BodyAngle[2] - angleDis;

		m->rotateCheck(angleDis, rotateSpeed);			//방향정해 회전

		m->AniHandler();				//애니 시킴

		timer2++;			//시간++
		Sleep(sleepSpeed);			//초당 프레임을 맞추기 위한 지연
	} while (angleSub > rotateSpeed || angleSub < -rotateSpeed);	//지속 조건설정

	/////////////////임시///////////////
	//m->isHandCuffed = true;
	//w->x = -49.1;		//좌표셋
	//w->z = -36;	
	//float disX;
	//float disY;
	//destPosX = -47;		//목표위치
	//destPosZ = -36;
	//objs.EyeX += destPosX - m->x;
	//objs.EyeZ += destPosZ - m->z;
	//objs.AtX += destPosX - m->x;
	//objs.AtZ += destPosZ - m->z;	
	//m->x = destPosX;		//좌표셋
	//m->z = destPosZ;
	//m->BodyAngle[2] = 90;
	//m->aniNo = ANI_MANSTANDING;
	//m->AniHandler();				//애니 시킴	

	//------여자보고 얘기
	timerDest[0] = 30;		//목표 타임
	timerCut[0] = timerDest[0];//시간차
	timer2 = 0;
	m->BodyAngle[2] = 90;
	mciSendString(("play " + soundVoice09).c_str(), 0, 0, 0);	//voice
	tempX = 0;
	isCanMouse = false;		//회전중 마우스 제어
	MyMouseMove2(0, 0, 0, 0);	//강제 마우스클릭

	do {
		if (timer2 < timerDest[0]) {
			m->LshoulderAngle[0] += 45 / timerCut[0];	//왼팔
			m->LelbowAngle[0] += 10 / timerCut[0];	//왼팔		
			m->LelbowAngle[1] += 35 / timerCut[0];	//왼팔					
		}

		if ((objs.eyeAngleX < 170 && objs.eyeAngleX > 10) || (objs.eyeAngleX > -170 && objs.eyeAngleX < -10)) {
			tempX += 6;
			MyMotion2(tempX, 0);		//강제 마우스이동(시점회전)
		}
		if (timer2 == 59) {
			MyMouseMove2(0, 1, 0, 0);	//강제 마우스뗌
			isCanMouse = true;		//마우스 제어
		}


		timer2++;			//시간++
		Sleep(sleepSpeed);			//초당 프레임을 맞추기 위한 지연
	} while (timer2 < 60);	//지속 조건설정

	//------여자걸어옴
	timer2 = 0;
	w->aniNo = ANI_WALK;
	do {
		w->movePosFoward(0.03);//앞으로 이동
		w->AniHandler();				//애니 시킴

		timer2++;			//시간++
		Sleep(sleepSpeed);			//초당 프레임을 맞추기 위한 지연
	} while (timer2 < 30);	//지속 조건설정


	/////////////////임시///////////////
	//m->isHandCuffed = true;
	//w->x = -49.1;		//좌표셋
	//w->z = -36;	
	//float disX;
	//float disY;
	//destPosX = -47;		//목표위치
	//destPosZ = -36;
	//objs.EyeX += destPosX - m->x;
	//objs.EyeZ += destPosZ - m->z;
	//objs.AtX += destPosX - m->x;
	//objs.AtZ += destPosZ - m->z;	
	//m->x = destPosX;		//좌표셋
	//m->z = destPosZ;
	//m->BodyAngle[2] = 90;
	//m->aniNo = ANI_MANSTANDING;
	//m->AniHandler();				//애니 시킴


	//------여자숙임
	timerDest[0] = 30;		//목표 타임
	timerDest[1] = 60;		//목표 타임
	timerCut[0] = timerDest[0];//시간차
	timerCut[1] = timerDest[1] - timerDest[0];//시간차
	timer2 = 0;
	w->aniNo = ANI_IDLE;
	w->InitAngle();
	do {
		if (timer2 < timerDest[0]) {
			w->WaistAngle[0] -= 45 / timerCut[0];	//허리 굽힘
			w->LshoulderAngle[0] += 80 / timerCut[0];	//왼팔
			w->LelbowAngle[0] += 20 / timerCut[0];
			w->RelbowAngle[0] += 20 / timerCut[0];		//오른팔
			w->RelbowAngle[1] -= 35 / timerCut[0];		//손 주머니에
		}
		else if (timer2 < timerDest[1]) {
			w->WaistAngle[0] += 45 / timerCut[1];	//허리 폄
			w->LshoulderAngle[0] -= 60 / timerCut[1];
			w->RshoulderAngle[0] += 35 / timerCut[1];
			w->LelbowAngle[1] += 20 / timerCut[1];

			m->RshoulderAngle[0] += 45 / timerCut[0];
			m->RelbowAngle[0] += 10 / timerCut[0];
			m->RelbowAngle[1] -= 35 / timerCut[0];
		}

		timer2++;			//시간++
		Sleep(sleepSpeed);			//초당 프레임을 맞추기 위한 지연
	} while (timer2 < 60);	//지속 조건설정

	//------수갑 채워짐
	m->isHandCuffed = true;
	timer2 = 0;
	do {
		timer2++;			//시간++
		Sleep(sleepSpeed);			//초당 프레임을 맞추기 위한 지연
	} while (timer2 < 60);	//지속 조건설정

	//------대화	
	timer2 = 0;
	tempX = 3;	//차 속도
	mciSendString(("play " + soundVoice10).c_str(), 0, 0, 0);	//voice 뭐야
	do {
		//	cout<<w->x<<" "<<w->z<<" "<<m->x<<" "<<m->z<<"\n";
		if (timer2 == 60) {
			mciSendString(("play " + soundVoice11).c_str(), 0, 0, 0);	//voice  경찰
		}
		else if (timer2 == 120) {
			mciSendString(("play " + soundVoice12).c_str(), 0, 0, 0);	//voice  너를
		}
		else if (timer2 == 300) {
			mciSendString(("play " + soundVoice14).c_str(), 0, 0, 0);	//voice  묵비권
		}
		else if (timer2 == 330) {
			mciSendString(("play " + soundVoice13).c_str(), 0, 0, 0);	//voice  웃기지1
		}
		else if (timer2 == 420) {
			mciSendString(("play " + soundVoice15).c_str(), 0, 0, 0);	//voice  웃기지2
		}
		else if (timer2 == 540) {
			mciSendString(("play " + soundVoice16).c_str(), 0, 0, 0);	//voice  이런경우
		}
		else if (timer2 == 660) {
			mciSendString(("play " + soundVoice17).c_str(), 0, 0, 0);	//voice  나쁜
		}
		else if (timer2 == 770) {
			mciSendString(("play " + soundVoice18).c_str(), 0, 0, 0);	//voice  타
		}

		tempY = 500;	//임시변수
		//차 이동부
		if (timer2 == 0 + tempY) {
			isCarDraw = true;
			isCyren = true;		//사이렌효과
			car->pos.x = 0;
			car->pos.z = 300;
		}
		else if (timer2 < 80 + tempY) {
			car->movePosFoward(tempX);		//차 진행
		}
		else if (timer2 < 120 + tempY) {
			tempX -= 0.02;
			car->movePosFoward(tempX);		//차 속도감소하며 진행
		}
		else if (timer2 < 140 + tempY) {
			tempX -= 0.24;
			if (tempX > 0) {
				car->movePosFoward(tempX);		//차 속도감소하며 진행
			}
			else {
				car->movePosFoward(0.35);		//차 진행
			}
		}
		else if (timer2 < 150 + tempY) {
			tempX -= 0.04;
			if (tempX > 0) {
				car->movePosFoward(tempX);		//차 진행
			}
			else {
				//				car->movePosFoward(0.5);		//차 진행
			}
		}
		else if (timer2 > 150 + tempY && timer2 < 160 + tempY) {
			car->movePosFoward(-0.3);
		}

		//차 회전부
		if (timer2 >= 95 + tempY && timer2 < 140 + tempY) {
			car->angles.y += 2.5;				//차 회전
		}
		else if (timer2 >= 140 + tempY && timer2 < 150 + tempY) {
			if (car->angles.y + car->dAngle < 180 + tempY) {
				car->dAngle += 7;				//차 드리프트하며 회전
			}
		}
		else if (timer2 == 150 + tempY) {
			car->angles.y += car->dAngle;	//각 셋팅
			car->dAngle = 0;
		}

		if (timer2 == 65 + tempY) {
			mciSendString(("play " + soundCarStop).c_str(), 0, 0, 0);	//차멈추는음
		}

		timer2++;			//시간++
		Sleep(sleepSpeed);			//초당 프레임을 맞추기 위한 지연
	} while (timer2 < 830);	//지속 시간설정

	//------발참
	timer2 = 0;
	timerDest[0] = 10;		//목표 타임
	timerCut[0] = timerDest[0];//시간차
	do {
		w->LshoulderAngle[0] -= 20 / timerCut[0];	//왼팔		//팔 원래대로
		w->RshoulderAngle[0] -= 35 / timerCut[0];	//왼팔
		w->LelbowAngle[0] -= 20 / timerCut[0];
		w->LelbowAngle[1] -= 20 / timerCut[1];
		w->RelbowAngle[0] -= 20 / timerCut[0];		//오른팔
		w->RelbowAngle[1] += 35 / timerCut[0];		//손 주머니에

		w->BodyAngle[0] += 45 / timerCut[0];
		w->WaistAngle[0] -= 20 / timerCut[0];	//허리 굽힘

		w->LlegAngle[0] -= 45 / timerCut[0];	//왼다리
		w->RlegAngle[0] += 40 / timerCut[0];	//오른다리

		timer2++;			//시간++
		Sleep(sleepSpeed);			//초당 프레임을 맞추기 위한 지연
	} while (timer2 < 10);	//지속 시간설정
	mciSendString(("play " + soundHit1).c_str(), 0, 0, 0);	//히트음

	//------남자 날아감
	timer2 = 0;
	timerDest[0] = 30;		//목표 타임
	timerCut[0] = timerDest[0];//시간차
	float velY = 0.1;		//속도
	float addY = -0.0075;		//가속도
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

		timer2++;			//시간++
		Sleep(sleepSpeed);			//초당 프레임을 맞추기 위한 지연
	} while (timer2 < 30);	//지속 시간설정
	m->isDraw = false;	//남자 안그림
	mciSendString(("play " + soundVoice19).c_str(), 0, 0, 0);	//voice 말도

	//------발 원래대로
	timer2 = 0;
	timerDest[0] = 10;		//목표 타임
	timerCut[0] = timerDest[0];//시간차
	do {
		w->BodyAngle[0] -= 45 / timerCut[0];
		w->WaistAngle[0] += 20 / timerCut[0];	//허리 굽힘

		w->LlegAngle[0] += 45 / timerCut[0];	//왼다리
		w->RlegAngle[0] -= 40 / timerCut[0];	//오른다리

		timer2++;			//시간++
		Sleep(sleepSpeed);			//초당 프레임을 맞추기 위한 지연
	} while (timer2 < 10);	//지속 시간설정

	//------여자 걸어감
	timer2 = 0;
	w->aniNo = ANI_WALK;
	do {
		w->movePosFoward(w->walkSpeed / 2);//앞으로 이동
		w->AniHandler();				//애니 시킴
		objs.AtX += w->walkSpeed / 2;		//초점이동

		timer2++;			//시간++
		Sleep(sleepSpeed);			//초당 프레임을 맞추기 위한 지연
	} while (timer2 < 60);	//지속 시간설정

	//------여자 숙여 차에 탐
	timer2 = 0;
	w->aniNo = IDLE;
	timerDest[0] = 30;		//목표 타임
	timerDest[1] = 60;		//목표 타임
	timerCut[0] = timerDest[0];//시간차
	timerCut[1] = timerDest[1] - timerDest[0];//시간차
	do {
		if (timer2 < timerDest[0]) {
			w->WaistAngle[0] -= 70 / timerCut[0];	//허리 굽힘
		}
		else if (timer2 < timerDest[1]) {
			w->movePosFoward(w->walkSpeed / 2);//앞으로 이동
			objs.AtX += w->walkSpeed / 2;		//초점이동

			w->LlegAngle[0] += 30 / timerCut[1];	//왼다리
			w->RlegAngle[0] -= 30 / timerCut[1];	//오른다리
		}

		timer2++;			//시간++
		Sleep(sleepSpeed);			//초당 프레임을 맞추기 위한 지연
	} while (timer2 < 60);	//지속 시간설정

	w->isDraw = false;	//여자 안그림

	//------차 이동
	timer2 = 0;
	tempX = 0;
	do {
		if (timer2 < 40) {
			tempX -= 0.005;
			car->movePosFoward(tempX);//차 뒤로 

			car->angles.y += 1;

		}
		else if (timer2 < 60) {
			tempX += 0.01;
			car->movePosFoward(tempX);//차앞으로

			car->angles.y += 1;
		}
		else if (timer2 < 120) {
			tempX += 0.01;
			car->movePosFoward(tempX);//차 앞으로
		}
		else if (timer2 < 200) {
			if (car->angles.y > 180) {
				car->angles.y -= 1;
			}
			car->movePosFoward(tempX);//차 앞으로
		}
		else if (timer2 < 720) {
			car->movePosFoward(tempX);//차 앞으로
		}

		//음성 나오게
		if (timer2 == 30) {
			mciSendString(("play " + soundVoice20).c_str(), 0, 0, 0);	//voice 미란다
		}
		else if (timer2 == 90) {
			mciSendString(("play " + soundCarStart).c_str(), 0, 0, 0);	//차감
		}
		else if (timer2 == 270) {
			mciSendString(("play " + soundVoice21).c_str(), 0, 0, 0);	//voice 말도
		}
		else if (timer2 == 300) {
			mciSendString(("play " + soundVoice22).c_str(), 0, 0, 0);	//voice 신고
		}
		else if (timer2 == 360) {
			mciSendString(("play " + soundVoice23).c_str(), 0, 0, 0);	//voice 신고
		}
		else if (timer2 == 480) {
			mciSendString(("play " + soundHit1).c_str(), 0, 0, 0);	//voice 신고}
		}

		objs.AtX = car->pos.x;
		objs.AtZ = car->pos.z;

		if (timer2 > 120 && timer2 < 500) {
			objs.EyeY += 0.1;
		}

		timer2++;			//시간++
		Sleep(sleepSpeed);			//초당 프레임을 맞추기 위한 지연
	} while (timer2 < 720);	//지속 시간설정

	isCarDraw = false;	//차 안그림
	isCyren = false;	//사이렌효과 끔
	objs.gameState = 4;

	objs.isAniControl = false;	//애니제어 off
	return 0;
}//-------------------엔딩 애니 끝

//메인함수
int main(int argc, char** argv) {
	glutInit(&argc, argv);
	//	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);//GLUT_DOUBLE을 쓰므로 GLUT_SINGLE은 안씀
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_STENCIL | GLUT_MULTISAMPLE);
	glutInitWindowSize(800, 600);
	glutInitWindowPosition(200, 200);
	glutCreateWindow("OpenGL Project 3");
	glClearColor(0.6, 0.6, 0.6, 1.0);
	InitLight();

	srand((unsigned int)time(NULL));		//완전랜덤을 위해 이렇게 씀. 보통 시간별(time)이용.

	//devil 라이브러리 초기화
	ilInit();
	iluInit();
	ilutInit();
	ilutRenderer(ILUT_OPENGL);
	ilutEnable(ILUT_OPENGL_CONV);

	//공용구조체 변수초기화
	objs.manID = 0;	//사람 아이디 초기화
	objs.EyeX; //시점셋
	objs.EyeY;
	objs.EyeZ = 9;
	objs.AtX = TableX / 2;		//초점셋
	objs.AtY = TableHeight;
	objs.AtZ = -TableY / 2;
	objs.isAniControl = false;	//메인에서 애니 컨트롤하나
	objs.gameState = 0;	//상황상태0
	objs.gametime = 0; objs.gametime = 0;
	//	car->pos.z = 300;			//차 위치 설정	
	car->pos.z = 0;			//차 위치 설정

	//파티클생성기 초기화(c++는 초기화안해도)
	for (int i = 0; i < MAX_PGENERATOR; i++) {
		pGenerators[i].alive = false;
	}

	//사람 생성 예(다형성 이용)
	Person* m = new Man(&objs);
	m->manType = 0;
	m->ctrl = 0;
	m->actNo = MAIN_ANI;
	objs.people.push_back(m);	//배열에 넣음.  첫번째 사람이 주인공

	Person* m2 = new Man(&objs);
	m2->manType = 1;
	m2->actNo = MAIN_ANI;
	objs.people.push_back(m2);	//배열에 넣음

	Person* m3 = new Man(&objs);
	m3->manType = 2;
	m3->actNo = MAIN_ANI;
	objs.people.push_back(m3);	//배열에 넣음

	Person* m4 = new Man(&objs);
	m4->manType = 3;
	m4->actNo = MAIN_ANI;
	objs.people.push_back(m4);	//배열에 ㅂ넣음

	Person* w1 = new Woman(&objs);;		//여자생성
	w1->manType = 10;
	w1->actNo = MAIN_ANI;
	objs.people.push_back(w1);	//배열에 넣음.  첫번째 사람이 주인공

	objs.EyeX = objs.people.at(0)->x;		//시점셋
	objs.EyeY = objs.people.at(0)->y + 7.5;// + 5;
	objs.EyeZ = objs.people.at(0)->z + 10; // + 5;

	objs.AtX = objs.people.at(0)->x;		//초점셋
	objs.AtY = objs.people.at(0)->y + 2;
	objs.AtZ = objs.people.at(0)->z;

	InitPosition();
	//mciSendString("play pro2_runBgm.wav", 0, 0, 0);	//프로젝트속성 멀티바이트사용
	//	mciSendString("play sound\\bgm01.mp3 repeat", 0, 0, 0);//play bgm
//	mciSendString("pause sound\\bgm01.mp3", 0, 0, 0);//pause bgm
//	mciSendString("resume sound\\bgm01.mp3", 0, 0, 0);//resume bgm
//	mciSendString("stop sound\\bgm01.mp3", 0, 0, 0);		//stop bgm
//	mciSendString("play sound\\hit1.mp3 repeat ", 0, 0, 0);	

	//텍스처 로드
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

	CalculateEyeXY();		//시점계산
	CalculateRadius();		//초점-시점 거리계산

	glutDisplayFunc(MyDisplay);
	glutKeyboardFunc(MyKeyboard);
	glutSpecialFunc(Specialkeys);	//방향키
	glutKeyboardUpFunc(MyKeyboardUp);	//키보드 뗌 감지. 신버전 glut.dll 필요
	glutMouseFunc(MyMouseMove);
	glutMotionFunc(MyMotion);
	//	glutPassiveMotionFunc(MyPassiveMotion);	//마우스 그냥이동 감지 콜백
	glutReshapeFunc(MyReshape);

	glutTimerFunc(sleepSpeed, MyTimer, 1);		//초당70프레임. 타이머 등록


	GLint SubMenu2 = glutCreateMenu(MySubMenu);	//서브메뉴생성2
	glutAddMenuEntry("On", 4);
	glutAddMenuEntry("Off", 5);

	GLint MyMainMenuID = glutCreateMenu(MyMainMenu);	//메인메뉴등록
	glutAddMenuEntry("인트로 시작 ", 50);
	glutAddMenuEntry("게임파트 테스트 (AI on)", 800);
	glutAddMenuEntry("게임파트 테스트 (AI off)", 850);
	glutAddSubMenu("BGM on/off", SubMenu2);		//서브메뉴 붙임
	glutAddMenuEntry("초기화", 900);
	glutAttachMenu(GLUT_RIGHT_BUTTON);		//메인메뉴 붙임

	printf("--------------------------------------------------------------------- \n");
	printf("- E : WCS원점과 초점에 구 표시 & 공격판정 / 피격판정 구 표시 on/off \n");
	printf("- R : 와이어프레임 on/off \n");
	printf("- T : 플랫/구로셰이딩 토글 \n");
	printf("- Z, X : 줌인/줌아웃 \n");
	printf("- N, M : 시점을 원점을 향해 이동/멀어지게 \n");
	printf("- C : 타격 파티클 on/off \n");
	printf("- V : 산 그리기/안그리기 \n");
	printf("- F : 안개 on/off \n");
	printf("- G : 트리 그리기 모드 전환 - 실린더/프렉탈/Off \n");
	printf("- [,] : 애니매이션 속도 조절 \n");
	printf("- P : 애니매이션 일시정지/재생 \n");
	printf("- 화살표키, 키패드0, 키패드Delete : 시점이동 \n");
	printf("- 키패드의 방향키, 키패드+,- : 초점이동 \n");
	printf("- 마우스 클릭-드래그 : 시점회전 (삼각함수를 이용해 초점에 대한 위치 계산) \n");
	printf("- H : 도움말 \n");
	printf("- Q : 종료 \n");
	printf("------------------------ 주인공 조작 --------------------------------- \n");
	printf("- W,A,S,D (방향키): 이동 \n");
	printf("- 같은방향 2번입력 : 달리기 \n");
	printf("- J : 공격 \n");
	printf("- K : 어퍼컷 \n");
	printf("- F1 : 적체력1로      - F2 :주인공체력1로   - F3 : 주인공체력 채움 \n");
	printf("\n");

	glutMainLoop();
}