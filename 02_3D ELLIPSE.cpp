#include <gl/glut.h>  
#include <math.h>

#define		PI	3.1415926
#define		N	40
#define		M	20 

static int	sphere_drawing_type = 3;

float	sphere_radius = 1.0;
float	camera_radius = 25.0;
float	camera_theta = 0.0;
float	camera_phi = 0.0;

float px = 0.0;
float mx = 0.0;
float py = 0.0;
float my = 0.0;
float pz = 0.0;
float mz = 0.0;


float	theta, phi;
float	delta_theta, delta_phi;
float	start_theta, start_phi;

float	Radius = 1.0;
float 	ver[N][M + 1][3];
float	normal[N][M + 1][3];

float light_position[] = { -7.0, 5.0, 5.0, 1.0 };
float light_ambient[] = { 0.8, 0.0, 0.0, 1.0 };
float light_diffuse[] = { 0.8, 0.8, 0.0, 1.0 };
float light_specular[] = { 0.0, 0.0, 1.0, 1.0 };

struct  vector3d {
	float		x;
	float		y;
	float		z;
};

void Vertex_Generation(void) {
	start_theta = 0.0;
	delta_theta = 2.0 * PI / N;

	start_phi = -1.0 * PI / 2.0;
	delta_phi = PI / M;

	for (int j = 0; j <= M; j++) {
		for (int i = 0; i < N; i++) {
			theta = start_theta + i * delta_theta;
			phi = start_phi + j * delta_phi;
			ver[i][j][0] = sphere_radius * cos(phi*-1.0) * cos(theta);
			ver[i][j][1] = sphere_radius * cos(phi*-1.0) * sin(theta);
			ver[i][j][2] = sphere_radius/3 * sin(phi);
		}
	}
}

vector3d	Normalization(vector3d U) {
	float		distance;
	vector3d	Unit_vector;

	distance = sqrt(U.x * U.x + U.y * U.y + U.z * U.z);

	Unit_vector.x = 1 / distance * U.x;
	Unit_vector.y = 1 / distance * U.y;
	Unit_vector.z = 1 / distance * U.z;

	return Unit_vector;
}

vector3d Cross_Product(vector3d U, vector3d V) {
	vector3d cross_vector;

	cross_vector.x = U.y * V.z - U.z * V.y;
	cross_vector.y = -1.0 * (U.x * V.z - U.z * V.x);
	cross_vector.z = U.x * V.y - U.y * V.x;

	return cross_vector;
}


vector3d Face_Normal_Vector(int a, int b, int c, int d, int e, int f) {
	vector3d U, V;

	U.x = ver[c][d][0] - ver[a][b][0];
	U.y = ver[c][d][1] - ver[a][b][1];
	U.z = ver[c][d][2] - ver[a][b][2];

	V.x = ver[e][f][0] - ver[a][b][0];
	V.y = ver[e][f][1] - ver[a][b][1];
	V.z = ver[e][f][2] - ver[a][b][2];

	return (Normalization(Cross_Product(U, V)));
}


void Vertex_Normal_Computation() {
	vector3d S, normal1, normal2, normal3, normal4, temp_vector;

	S.x = 0.0;
	S.y = 0.0;
	S.z = 0.0;
	for (int i = 0; i < N; i++) {
		normal1 = Face_Normal_Vector(i, 0, (i + 1) % N, 1, i, 1);
		S.x += normal1.x;
		S.y += normal1.y;
		S.z += normal1.z;
	}
	S = Normalization(S);

	for (int i = 0; i < N; i++) {
		normal[i][0][0] = S.x;
		normal[i][0][1] = S.y;
		normal[i][0][2] = S.z;
	}

	for (int j = 1; j < M; j++) {
		for (int i = 0; i < N; i++) {
			normal1 = Face_Normal_Vector(i, j, (i + 1) % N, j, i, j + 1);
			normal2 = Face_Normal_Vector(i, j, i, j + 1, (i - 1 + N) % N, j);
			normal3 = Face_Normal_Vector(i, j, (i - 1 + N) % N, j, i, j - 1);
			normal4 = Face_Normal_Vector(i, j, i, j - 1, (i + 1) % N, j);
			temp_vector.x = (normal1.x + normal2.x + normal3.x + normal4.x) / 4.0;
			temp_vector.y = (normal1.y + normal2.y + normal3.y + normal4.y) / 4.0;
			temp_vector.z = (normal1.z + normal2.z + normal3.z + normal4.z) / 4.0;

			temp_vector = Normalization(temp_vector);
			normal[i][j][0] = temp_vector.x;
			normal[i][j][1] = temp_vector.y;
			normal[i][j][2] = temp_vector.z;
		}
	}

	S.x = 0.0;
	S.y = 0.0;
	S.z = 0.0;
	for (int i = 0; i < N; i++) {
		normal1 = Face_Normal_Vector(i, M, i, M - 1, (i + 1) % N, M - 1);
		S.x += normal1.x;
		S.y += normal1.y;
		S.z += normal1.z;
	}
	S = Normalization(S);
	for (int i = 0; i < N; i++) {
		normal[i][M][0] = S.x;
		normal[i][M][1] = S.y;
		normal[i][M][2] = S.z;
	}
}

void Sphere3(void) {
	for (int j = 0; j < M; j++) {
		for (int i = 0; i < N; i++) {
			glColor3f(cos(exp(i + j) * sin(i + j)), sin(j * sin(i * j)), sin(i * j));
			glBegin(GL_POLYGON);
			glVertex3fv(ver[i][j]);
			glVertex3fv(ver[(i + 1) % N][j]);
			glVertex3fv(ver[(i + 1) % N][j + 1]);
			glVertex3fv(ver[i][j + 1]);
			glEnd();
		}
	}
}

void DrawSphere(void) {
	for (int j = 0; j < M; j++) {
		for (int i = 0; i < N; i++) {
			glBegin(GL_POLYGON);

			Vertex_Normal_Computation();

			glNormal3fv(normal[i][j]);
			//Vertex_Normal_Computation();
			glVertex3fv(ver[i][j]);
			
			glNormal3fv(normal[(i + 1) % N][j]);
			//Vertex_Normal_Computation();
			glVertex3fv(ver[(i + 1) % N][j]);

			glNormal3fv(normal[(i + 1) % N][j + 1]);
			//Vertex_Normal_Computation();
			glVertex3fv(ver[(i + 1) % N][j + 1]);

			glNormal3fv(normal[i][j + 1]);
			//Vertex_Normal_Computation();
			glVertex3fv(ver[i][j + 1]);
			glEnd();
		}
	}
	Sphere3();
}


void init(void)
{	
	glShadeModel(GL_SMOOTH);      //GL_FLAT 

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);

	glMaterialfv(GL_FRONT, GL_SPECULAR, light_specular);
	glMateriali(GL_FRONT, GL_SHININESS, 50);

	
	glEnable(GL_DEPTH_TEST);
	Vertex_Generation();
}



void axis(void) {
	glBegin(GL_LINES);
	glColor3f(1.0, 0.0, 0.0); // x�� 
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(10.0, 0.0, 0.0);

	glColor3f(0.0, 1.0, 0.0); // y�� 
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(0.0, 10.0, 0.0);

	glColor3f(0.0, 0.0, 1.0); // z�� 
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(0.0, 0.0, 10.0);
	glEnd();
}

void reshape(int w, int h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, 1.0, 1.0, 1000);
}


void camera_setting(void)
{
	float	x, y, z;

	x = camera_radius * cos(camera_theta) * cos(camera_phi);
	y = camera_radius * sin(camera_theta) * cos(camera_phi);
	z = camera_radius * sin(camera_phi);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(x, y, z, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0);
}


void display(void)
{
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	camera_setting();
	axis();

	glTranslatef(px+mx, py+my, pz+mz);
	DrawSphere();

	glFlush();

}

void special_key(int key, int x, int y)
{
	switch (key) {
	case GLUT_KEY_LEFT:		camera_theta -= 0.01; 	break;
	case GLUT_KEY_RIGHT:	camera_theta += 0.01;	break;
	case GLUT_KEY_UP:		camera_phi += 0.01;		break;
	case GLUT_KEY_DOWN:		camera_phi -= 0.01;		break;
	default: break;
	}
	glutPostRedisplay();
}

void mykey(unsigned char key, int x, int y)
{
	switch (key) {
	case '[': px += 0.1; break; //x축
	case ']': mx -= 0.1; break; //x축

	case 'l': py += 0.1; break; //y축
	case ';': my -= 0.1; break; //y축

	case ',': pz += 0.1; break; //z축
	case '.': mz -= 0.1; break; //z축
		
	case 'a':	camera_radius += 5.1; break;
	case 's':	camera_radius -= 5.1; break;

	default: break;
	}
	glutPostRedisplay();
}

void main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowPosition(100, 0);
	glutInitWindowSize(1600, 1200);
	glutCreateWindow("3D_Ellipse");
	init();
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(mykey);
	glutSpecialFunc(special_key);
	glutMainLoop();
}
