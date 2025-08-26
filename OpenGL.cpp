#include <glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <fstream>

#define PI 3.1415927

// 전역 변수
float angleX = 0.0f;
float angleY = 0.0f;
float zoom = 1.0f;
int lastMouseX, lastMouseY;
bool isDragging = false;

GLuint texture;

unsigned char* LoadBMP(const char* filepath, int& width, int& height) {
    std::ifstream file(filepath, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Failed to open BMP file!" << std::endl;
        return nullptr;
    }

    // BMP 파일 헤더 읽기
    char header[54];
    file.read(header, 54);

    width = *(int*)&header[18];
    height = *(int*)&header[22];
    int imageSize = *(int*)&header[34];

    if (imageSize == 0) {
        imageSize = width * height * 3;
    }

    unsigned char* data = new unsigned char[imageSize];
    file.read(reinterpret_cast<char*>(data), imageSize);
    file.close();

    // BGR to RGB 변환
    for (int i = 0; i < imageSize; i += 3) {
        std::swap(data[i], data[i + 2]);
    }

    return data;
}

GLuint LoadTexture(const char* filepath) {
    int width, height;
    unsigned char* image = LoadBMP(filepath, width, height);
    if (!image) {
        return 0;
    }

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    delete[] image;
    return texture;
}

void DrawSphereWithTexture(GLuint texture) {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture);

    GLUquadric* quad = gluNewQuadric();
    gluQuadricTexture(quad, GL_TRUE);

    glPushMatrix();
    glColor3ub(240, 224, 189); // 스킨색 설정
    gluSphere(quad, 0.5, 20, 20);
    glPopMatrix();

    gluDeleteQuadric(quad);
    glBindTexture(GL_TEXTURE_2D, 0); // 텍스처 바인딩 해제
    glDisable(GL_TEXTURE_2D);
}

void init() {
    glEnable(GL_DEPTH_TEST);
    texture = LoadTexture("face.bmp");
}

void draw_cylinder(GLfloat radius, GLfloat height, GLubyte R, GLubyte G, GLubyte B) {
    GLfloat x = 0.0;
    GLfloat y = 0.0;
    GLfloat angle = 0.0;
    GLfloat angle_stepsize = 0.1;

    glColor3ub(R, G, B); // 색상 설정
    glBegin(GL_QUAD_STRIP);
    angle = 0.0;
    while (angle < 2 * PI) {
        x = radius * cos(angle);
        y = radius * sin(angle);
        glVertex3f(x, y, height);
        glVertex3f(x, y, 0.0);
        angle += angle_stepsize;
    }
    glVertex3f(radius, 0.0, height);
    glVertex3f(radius, 0.0, 0.0);
    glEnd();

    glBegin(GL_POLYGON);
    angle = 0.0;
    while (angle < 2 * PI) {
        x = radius * cos(angle);
        y = radius * sin(angle);
        glVertex3f(x, y, height);
        angle += angle_stepsize;
    }
    glVertex3f(radius, 0.0, height);
    glEnd();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    // 카메라 설정
    glTranslatef(0.0f, 0.0f, -10.0f);
    glScalef(zoom, zoom, zoom);
    glRotatef(angleX, 1.0f, 0.0f, 0.0f);
    glRotatef(angleY, 0.0f, 1.0f, 0.0f);

    // 모델링

    // 머리 (구 형태)
    glPushMatrix();
    glTranslatef(0.0f, 1.5f, 0.0f);
    glRotatef(275, 1.0, 0.0, 0.0);
    glRotatef(5, 0.0, 0.0, 1.0);
    DrawSphereWithTexture(texture);
    glPopMatrix();

    // 몸통 (직선 형태)
    glPushMatrix();
    glColor3ub(0, 255, 0); // 초록색 설정
    glTranslatef(0.0, -0.3f, 0.0f);
    glRotatef(-90, 1.0, 0.0, 0.0);
    draw_cylinder(0.5, 1.3, 0, 255, 0);
    glPopMatrix();

    // 오른쪽 다리
    glPushMatrix();
    glColor3ub(0, 0, 255); // 파란색 설정
    glTranslatef(0.3f, -1.6f, 0.0f);
    glRotatef(-90, 1.0, 0.0, 0.0);
    draw_cylinder(0.2, 1.3, 0, 0, 255);
    glPopMatrix();

    // 왼쪽 다리
    glPushMatrix();
    glColor3ub(0, 0, 255); // 파란색 설정
    glTranslatef(-0.3f, -1.6f, 0.0f);
    glRotatef(-90, 1.0, 0.0, 0.0);
    draw_cylinder(0.2, 1.3, 0, 0, 255);
    glPopMatrix();

    // 오른쪽 팔
    glPushMatrix();
    glColor3ub(0, 255, 0); // 초록색 설정
    glTranslatef(0.8f, -0.3f, 0.0f);
    glRotatef(-90, 1.0, 0.0, 0.0);
    glRotatef(-5, 0.0, 1.0, 0.0);
    draw_cylinder(0.2, 1.3, 0, 255, 0);
    glPopMatrix();

    // 왼쪽 팔
    glPushMatrix();
    glColor3ub(0, 255, 0); // 초록색 설정
    glTranslatef(-0.8f, -0.3f, 0.0f);
    glRotatef(-90, 1.0, 0.0, 0.0);
    glRotatef(5, 0.0, 1.0, 0.0);
    draw_cylinder(0.2, 1.3, 0, 255, 0);
    glPopMatrix();

    glutSwapBuffers();
}

void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (float)w / (float)h, 1.0, 100.0);
    glMatrixMode(GL_MODELVIEW);
}

void keyboard(unsigned char key, int x, int y) {
    if (key == 'a') {
        zoom *= 1.1f;
    }
    else if (key == 'z') {
        zoom /= 1.1f;
    }
    glutPostRedisplay();
}

void mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON) {
        if (state == GLUT_DOWN) {
            isDragging = true;
            lastMouseX = x;
            lastMouseY = y;
        }
        else {
            isDragging = false;
        }
    }
}

void motion(int x, int y) {
    if (isDragging) {
        angleY += (x - lastMouseX);
        angleX += (y - lastMouseY);
        lastMouseX = x;
        lastMouseY = y;
        glutPostRedisplay();
    }
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("OpenGL");

    init();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);

    glutMainLoop();
    return 0;
}
