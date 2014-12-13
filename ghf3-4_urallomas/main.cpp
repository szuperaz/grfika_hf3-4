//=============================================================================================
// Szamitogepes grafika hazi feladat keret. Ervenyes 2014-tol.
// A //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// sorokon beluli reszben celszeru garazdalkodni, mert a tobbit ugyis toroljuk.
// A beadott program csak ebben a fajlban lehet, a fajl 1 byte-os ASCII karaktereket tartalmazhat.
// Tilos:
// - mast "beincludolni", illetve mas konyvtarat hasznalni
// - faljmuveleteket vegezni (printf is fajlmuvelet!)
// - new operatort hivni az onInitialization fÃ¼ggvÃ©nyt kivÃ©ve, a lefoglalt adat korrekt felszabadÃ­tÃ¡sa nÃ©lkÃ¼l
// - felesleges programsorokat a beadott programban hagyni
// - tovabbi kommenteket a beadott programba irni a forrasmegjelolest kommentjeit kiveve
// ---------------------------------------------------------------------------------------------
// A feladatot ANSI C++ nyelvu forditoprogrammal ellenorizzuk, a Visual Studio-hoz kepesti elteresekrol
// es a leggyakoribb hibakrol (pl. ideiglenes objektumot nem lehet referencia tipusnak ertekul adni)
// a hazibeado portal ad egy osszefoglalot.
// ---------------------------------------------------------------------------------------------
// A feladatmegoldasokban csak olyan gl/glu/glut fuggvenyek hasznalhatok, amelyek
// 1. Az oran a feladatkiadasig elhangzottak ES (logikai AND muvelet)
// 2. Az alabbi listaban szerepelnek:
// Rendering pass: glBegin, glVertex[2|3]f, glColor3f, glNormal3f, glTexCoord2f, glEnd, glDrawPixels
// Transzformaciok: glViewport, glMatrixMode, glLoadIdentity, glMultMatrixf, gluOrtho2D,
// glTranslatef, glRotatef, glScalef, gluLookAt, gluPerspective, glPushMatrix, glPopMatrix,
// Illuminacio: glMaterialfv, glMaterialfv, glMaterialf, glLightfv
// Texturazas: glGenTextures, glBindTexture, glTexParameteri, glTexImage2D, glTexEnvi,
// Pipeline vezerles: glShadeModel, glEnable/Disable a kovetkezokre:
// GL_LIGHTING, GL_NORMALIZE, GL_DEPTH_TEST, GL_CULL_FACE, GL_TEXTURE_2D, GL_BLEND, GL_LIGHT[0..7]
//
// NYILATKOZAT
// ---------------------------------------------------------------------------------------------
// Nev    : Szupera Zita
// Neptun : AWXUC6
// ---------------------------------------------------------------------------------------------
// ezennel kijelentem, hogy a feladatot magam keszitettem, es ha barmilyen segitseget igenybe vettem vagy
// mas szellemi termeket felhasznaltam, akkor a forrast es az atvett reszt kommentekben egyertelmuen jeloltem.
// A forrasmegjeloles kotelme vonatkozik az eloadas foliakat es a targy oktatoi, illetve a
// grafhazi doktor tanacsait kiveve barmilyen csatornan (szoban, irasban, Interneten, stb.) erkezo minden egyeb
// informaciora (keplet, program, algoritmus, stb.). Kijelentem, hogy a forrasmegjelolessel atvett reszeket is ertem,
// azok helyessegere matematikai bizonyitast tudok adni. Tisztaban vagyok azzal, hogy az atvett reszek nem szamitanak
// a sajat kontribucioba, igy a feladat elfogadasarol a tobbi resz mennyisege es minosege alapjan szuletik dontes.
// Tudomasul veszem, hogy a forrasmegjeloles kotelmenek megsertese eseten a hazifeladatra adhato pontokat
// negativ elojellel szamoljak el es ezzel parhuzamosan eljaras is indul velem szemben.
//=============================================================================================

#define _USE_MATH_DEFINES
#include <math.h>
#include <stdlib.h>

#if defined(__APPLE__)
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#else
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
#include <windows.h>
#endif
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#endif


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Innentol modosithatod...

const int screenWidth = 600;    // alkalmazÃ¡s ablak felbontÃ¡sa
const int screenHeight = 600;
const float pi = 3.14159265359;
unsigned int texid;
//--------------------------------------------------------
// 3D Vektor
//--------------------------------------------------------
struct Vector {
    float x, y, z;
    
    Vector( ) {
        x = y = z = 0;
    }
    Vector(float x0, float y0, float z0 = 0) {
        x = x0; y = y0; z = z0;
    }
    Vector operator*(float a) {
        return Vector(x * a, y * a, z * a);
    }
    Vector operator+(const Vector& v) {
        return Vector(x + v.x, y + v.y, z + v.z);
    }
    Vector operator-(const Vector& v) {
        return Vector(x - v.x, y - v.y, z - v.z);
    }
    float operator*(const Vector& v) {  // dot product
        return (x * v.x + y * v.y + z * v.z);
    }
    Vector operator%(const Vector& v) {     // cross product
        return Vector(y*v.z-z*v.y, z*v.x - x*v.z, x*v.y - y*v.x);
    }
    float Length() { return sqrt(x * x + y * y + z * z); }
};

//--------------------------------------------------------
// Spektrum illetve szin
//--------------------------------------------------------
struct Color {
    float r, g, b;
    
    Color( ) {
        r = g = b = 0;
    }
    Color(float r0, float g0, float b0) {
        r = r0; g = g0; b = b0;
    }
    Color operator*(float a) {
        return Color(r * a, g * a, b * a);
    }
    Color operator*(const Color& c) {
        return Color(r * c.r, g * c.g, b * c.b);
    }
    Color operator+(const Color& c) {
        return Color(r + c.r, g + c.g, b + c.b);
    }
};

struct Light {
public:
    float pos[4];
    float ambient[4];
    float diffuse[4];
    float specular[4];
    float shininess[1];
    Light () {}
    Light (float pos[4], float ambient[4], float diffuse[4], float specular[4], float shininess) {
        for (int i=0; i < 4; i++) {
            this->pos[i] = pos[i];
            this->ambient[i] = ambient[i];
            this->specular[i] = specular[i];
            this->diffuse[i] = diffuse[i];
        }
        this->shininess[0] = shininess;
    }
    void setOGL () {
        glLightfv(GL_LIGHT0, GL_POSITION, pos);
        glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
        glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
        glLightfv(GL_LIGHT0, GL_SHININESS, shininess);
        glEnable(GL_LIGHT0);
        
    }
};

struct Material {
public:
    float colour[4];
    int shininess;
    Material () {
    }
    Material (float r, float g, float b, float alpha, int shininess) {
        colour[0] = r;
        colour[1] = g;
        colour[2] = b;
        colour[3] = alpha;
        this->shininess = shininess;
    }
    Material (Material const &material) {
        colour[0] = material.colour[0];
        colour[1] = material.colour[1];
        colour[2] = material.colour[2];
        colour[3] = material.colour[3];
        shininess = material.shininess;
    }
    void setOGL () {
        glMaterialfv(GL_FRONT, GL_DIFFUSE, colour);
        glMaterialfv(GL_FRONT, GL_AMBIENT, colour);
        glMaterialfv(GL_FRONT, GL_SPECULAR, colour);
        glMaterialfv(GL_FRONT, GL_SPECULAR, colour);
        glMaterialf(GL_FRONT, GL_SHININESS, shininess);
    }
};

struct Object {
public:
    Vector position;
    Vector scale;
    Vector rotate;
    Material material;
    float angle;
    Object () {
    }
    Object (Material material, Vector position, float angle, Vector rotate, Vector scale) {
        this->position = position;
        this->rotate = rotate;
        this->scale = scale;
        this->material = Material(material);
        this->angle = angle;
    }
    Object (Material material, Vector scale) {
        this->position = Vector(0, 0, 0);
        this->rotate = Vector(0, 0, 0);
        this->scale = scale;
        this->material = Material(material);
        this->angle = 0;
    }
    void virtual draw()=0;
    void virtual control(float timeSlice)=0;
    void virtual animate()=0;
};

struct ParametricSurface : Object {
public:
    ParametricSurface () {
    }
    ParametricSurface (Material material, Vector position, float angle, Vector rotate, Vector scale) : Object (material, position, angle, rotate, scale){
    }
    ParametricSurface (Material material, Vector scale) : Object (material, scale){
    }
    void draw () {
        material.setOGL();
        glBegin(GL_QUADS);
        int ntess = 40;
        for (int i = 0; i < ntess; i++) {
            for (int j = 0; j < ntess; j++) {
                vertexOGL((float)i/ntess, (float)j/ntess);
                vertexOGL((float)(i+1)/ntess, (float)j/ntess);
                vertexOGL((float)(i+1)/ntess, (float)(j+1)/ntess);
                vertexOGL((float)i/ntess, (float)(j+1)/ntess);
            }
        }
        glEnd();
    }
    void virtual vertexOGL(float u, float v) =0;
};

struct Ellipsoid : ParametricSurface {
public:
    float a,b,c;
    Ellipsoid () : ParametricSurface(Material (0,0,0,0,0), Vector(0,0,0), 0, Vector(0,0,0), Vector(1,1,1)){
        a = b = c = 1;
    }
    Ellipsoid (Material m, float a, float b, float c, Vector position, float angle, Vector rotate, Vector scale) : ParametricSurface(m, position, angle, rotate, scale){
        this->a = a;
        this->b = b;
        this->c = c;
    }
    Ellipsoid (Material m, float a, float b, float c, Vector scale) : ParametricSurface(m, scale){
        this->a = a;
        this->b = b;
        this->c = c;
    }
    void vertexOGL (float u, float v) {
        glTexCoord2f(u, v);
        u = u*2*pi;
        v = v*pi;
        float normalX, normalY, normalZ, rX, rY, rZ;
        Vector du, dv, normal;
        rX = a * cosf(u) * sinf(v);
        rY = b * sinf(u) * sinf(v);
        rZ = c * cosf(v);
        normalX = -a * sinf(u)*sinf(v);
        normalY = b * cosf(u) * sinf(v);
        normalZ = 0;
        du = Vector(normalX, normalY, normalZ);
        normalX = a * cosf(u) * cosf(v);
        normalY = b * sinf(u) * cosf(v);
        normalZ = -c * sinf(v);
        dv = Vector(normalX, normalY, normalZ);
        normal = (du % dv);
        glNormal3f(normal.x, normal.y, normal.z);
        glVertex3f(rX, rY, rZ);
    }
    void control(float timeSlice) {}
    void animate() {}
};

struct Cylinder : ParametricSurface {
    float height, radius;
    Cylinder () {}
    Cylinder (Material m, float height, float radius, Vector position, float angle, Vector rotate, Vector scale) :ParametricSurface(m, position, angle, rotate, scale) {
        this->height = height;
        this->radius = radius;
    }
    Cylinder (Material m, float height, float radius, Vector scale) :ParametricSurface(m, position, angle, rotate, scale) {
        this->height = height;
        this->radius = radius;
    }
    
    void vertexOGL (float u, float v) {
        u = u*2*pi;
        v = v*height;
        float normalX, normalY, normalZ, rX, rY, rZ;
        Vector du, dv, normal;
        rX = radius * cosf(u);
        rY = radius * sinf(u);
        rZ = v;
        normalX = -radius * sinf(u);
        normalY = radius * cosf(u);
        normalZ = 0;
        du = Vector(normalX, normalY, normalZ);
        normalX = 0;
        normalY = 0;
        normalZ = height;
        dv = Vector(normalX, normalY, normalZ);
        normal = (du % dv);
        glNormal3f(normal.x, normal.y, normal.z);
        glVertex3f(rX, rY, rZ);
    }
    void control(float timeSlice) {}
    void animate() {}
};

struct Satellite : Object {
public:
    Ellipsoid body;
    Cylinder pipe1, pipe2, pipe3;
    float newAngle;
    Vector newPosition;
    Vector speed;
    Satellite () {
        Material mb = Material(0.4, 0.4, 0.4, 1, 10);
        Material mp1 = Material(0.4, 0.4, 1, 1, 10);
        Material mp2 = Material(1, 0.4, 0.4, 1, 10);
        Material mp3 = Material(0.4, 1, 0.4, 1, 10);
        body = Ellipsoid(mb, 7, 7, 7, Vector(1, 1, 1));
        position = Vector(45, -30, -10);
        angle = 0;
        rotate = Vector(0, 1, 0);
        speed = Vector(0, 0, 0.01);
        rotate = Vector(0, 1, 0);
        pipe1 = Cylinder(mp1, 1, 1, Vector(0, 0, -15), 0, Vector(0, 0, 0), Vector(1, 1, 30));
        pipe2 = Cylinder(mp2, 1, 1, Vector(0, 15, 0), 90, Vector(1, 0, 0), Vector(1, 1, 30));
        pipe3 = Cylinder(mp3, 1, 1, Vector(-15, 0, 0), 90, Vector(0, 1, 0), Vector(1, 1, 30));
    }
    void draw () {
        glPushMatrix();
        glTranslatef(position.x, position.y, position.z);
        glRotatef(angle, rotate.x, rotate.y, rotate.z);
        glScalef(body.scale.x, body.scale.y, body.scale.z);
        body.draw();
        glPushMatrix();
        glTranslatef(pipe1.position.x, pipe1.position.y, pipe1.position.z);
        glRotatef(pipe1.angle, pipe1.rotate.x, pipe1.rotate.y, pipe1.rotate.z);
        glScalef(pipe1.scale.x, pipe1.scale.y, pipe1.scale.z);
        pipe1.draw();
        glPopMatrix();
        glPushMatrix();
        glTranslatef(pipe2.position.x, pipe2.position.y, pipe2.position.z);
        glRotatef(pipe2.angle, pipe2.rotate.x, pipe2.rotate.y, pipe2.rotate.z);
        glScalef(pipe2.scale.x, pipe2.scale.y, pipe2.scale.z);
        pipe2.draw();
        glPopMatrix();
        glPushMatrix();
        glTranslatef(pipe3.position.x, pipe3.position.y, pipe3.position.z);
        glRotatef(pipe3.angle, pipe3.rotate.x, pipe3.rotate.y, pipe3.rotate.z);
        glScalef(pipe3.scale.x, pipe3.scale.y, pipe3.scale.z);
        pipe3.draw();
        glPopMatrix();
        glPopMatrix();
    }
    void control(float timeSlice) {
        float anglePerMs = 0.006;
        newAngle = anglePerMs * timeSlice + angle;
        newPosition = Vector(position.x+speed.x, position.y+speed.y, position.z-speed.z);
        
    }
    void animate() {
        angle = newAngle;
        position = newPosition;
    }
};

struct Planet : Object {
public:
    Ellipsoid planet;
    Ellipsoid atmosphere;
    Planet () {
        Material mp = Material(1, 0, 0, 1, 2);
        Material ma = Material(0.1, 0.1, 0.9, 0.2, 2);
        planet = Ellipsoid(mp, 18, 17, 18, Vector(2, 2, 2));
        atmosphere = Ellipsoid(ma, 20, 19, 18, Vector(1,1,1));
        position = Vector(0, 0, -30);
        angle = 0;
        rotate = Vector(0, 0, 0);
    }
    void draw () {
        glPushMatrix();
        glTranslatef(position.x, position.y, position.z);
        glRotatef(angle, rotate.x, rotate.y, rotate.z);
        glScalef(planet.scale.x, planet.scale.y, planet.scale.z);
        glEnable(GL_TEXTURE_2D);
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
        glBindTexture(GL_TEXTURE_2D, texid);
        planet.draw();
        glDisable(GL_TEXTURE_2D);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        atmosphere.draw();
        glDisable(GL_BLEND);
        glPopMatrix();
    }
    void control(float timeSlice) {}
    void animate() {}
};

struct CRForgastest : Object {
public:
    Vector controlPoints[5];
    float times[5];
    Vector speeds[5];
    Vector nextPoint;
    CRForgastest () {
    }
    
    CRForgastest (Vector position, float angle, Vector rotate, Vector scale): Object (Material(0.28, 0.28, 0.28, 1, 7), position, angle, rotate, scale) {
        controlPoints[0] = Vector(-15, 0, 0);
        times[0] = 3;
        controlPoints[1] = Vector(-5, 3, 0);
        times[1] = 6;
        controlPoints[2] = Vector(5, 3, 0);
        times[2] = 9;
        controlPoints[3] = Vector(10, 3, 0);
        times[3] = 12;
        controlPoints[4] = Vector(15, 6, 0);
        times[4] = 16;
        calculateSpeed();
    }
    
    void calculateSpeed () {
        float speedX, speedY, time, timePrev, timeNext;
        Vector cp, cpPrev, cpNext;
        speeds[0] = Vector(0,0,0);
        for (int i = 1; i <  4; i++) {
            cp = controlPoints[i];
            cpPrev = controlPoints[i-1];
            cpNext = controlPoints[i+1];
            time = times[i];
            timePrev = times[i-1];
            timeNext = times[i+1];
            speedX = 0.5 * (((cpNext.x - cp.x)/(timeNext - time)) + ((cp.x - cpPrev.x)/(time - timePrev)));
            speedY = 0.5 * (((cpNext.y - cp.y)/(timeNext - time)) + ((cp.y - cpPrev.y)/(time - timePrev)));
            speeds[i] = Vector(speedX, speedY);
        }
        speeds[5] = Vector(0,0);
        return;
    }
    
    Vector doHermiteInterpolation (Vector cp, Vector cpNext, Vector speed, Vector speedNext, float time, float cpTime, float cpNextTime) {
        Vector pointOnCurve, a0, a1, a2, a3;
        float x, y;
        a0 = cp;
        a1 = speed;
        x = ((3 * (cpNext.x - cp.x))/((float)pow((cpNextTime-cpTime), 2))) - ((speedNext.x + 2*speed.x)/(cpNextTime - cpTime));
        y = ((3 * (cpNext.y - cp.y))/((float)pow((cpNextTime-cpTime), 2))) - ((speedNext.y + 2*speed.y)/(cpNextTime - cpTime));
        a2 = Vector(x,y);
        x = ((2 * (cp.x - cpNext.x))/((float)pow((cpNextTime-cpTime), 3))) + ((speedNext.x + speed.x)/((float)pow((cpNextTime-cpTime), 2)));
        y = ((2 * (cp.y - cpNext.y))/((float)pow((cpNextTime-cpTime), 3))) + ((speedNext.y + speed.y)/((float)pow((cpNextTime-cpTime), 2)));
        a3 = Vector(x,y);
        
        pointOnCurve.x = a3.x*((float)pow((time-cpTime), 3)) + a2.x*((float)pow((time-cpTime), 2)) + a1.x*(time-cpTime) + a0.x;
        pointOnCurve.y = a3.y*((float)pow((time-cpTime), 3)) + a2.y*((float)pow((time-cpTime), 2)) + a1.y*(time-cpTime) + a0.y;
        
        return pointOnCurve;
    }
    void draw () {
        float cpTime, cpNextTime, step;
        Vector cp, cpNext, speed, speedNext, pointOnCurve;
        material.setOGL();
        glBegin(GL_QUADS);
        for (int i = 0; i < 4; i++) {
            cpTime = times[i];
            cpNextTime = times[i+1];
            cp = controlPoints[i];
            cpNext = controlPoints[i+1];
            speed = speeds[i];
            speedNext = speeds[i+1];
            step = (cpNextTime - cpTime) / 100;
            for (float time = cpTime; time < cpNextTime; time = time + step) {
                pointOnCurve = doHermiteInterpolation(cp, cpNext, speed, speedNext, time, cpTime, cpNextTime);
                nextPoint = doHermiteInterpolation(cp, cpNext, speed, speedNext, time+step, cpTime, cpNextTime);
                vertexOGL(pointOnCurve.x, pointOnCurve.y);
            }
        }
        glEnd();
        
    }
    
    void vertexOGL (float x, float y) {
        float step = 2*pi/100.0;
        for (float szog = 0; szog < 2*pi; szog += step) {
            Vector pointOnQuad1 = Vector(x, y*sinf(szog), y*cosf(szog));
            Vector pointOnQuad2 = Vector(nextPoint.x, nextPoint.y*sinf(szog), nextPoint.y*cosf(szog));
            Vector pointOnQuad3 = Vector(nextPoint.x, nextPoint.y*sinf(szog+step), nextPoint.y*cosf(szog+step));
            Vector pointOnQuad4 = Vector(x, y*sinf(szog+step), y*cosf(szog+step));
            Vector normal = (pointOnQuad2 - pointOnQuad1) % (pointOnQuad4 - pointOnQuad1);
            glNormal3f(normal.x, normal.y, normal.z);
            glVertex3f(pointOnQuad1.x, pointOnQuad1.y, pointOnQuad1.z);
            glVertex3f(pointOnQuad2.x, pointOnQuad2.y, pointOnQuad2.z);
            glVertex3f(pointOnQuad3.x, pointOnQuad3.y, pointOnQuad3.z);
            glVertex3f(pointOnQuad4.x, pointOnQuad4.y, pointOnQuad4.z);
        }
    }
    void control(float timeSlice) {}
    void animate() {}
};

struct Cube : Object {
public:
    Cube () : Object(){}
    Cube (Material m, Vector position, float angle, Vector rotate, Vector scale) : Object(m, position, angle, rotate, scale) {}
    void draw() {
        material.setOGL();
        glBegin(GL_QUADS);
        
        glNormal3f(0, 0, 1);
        glVertex3f(0, 0, 0);
        glVertex3f(0, -1, 0);
        glVertex3f(1, -1, 0);
        glVertex3f(1, 0, 0);
        
        glNormal3f(-1, 0, 0);
        glVertex3f(0, 0, 0);
        glVertex3f(0, 0, -1);
        glVertex3f(0, -1, -1);
        glVertex3f(0, -1, 0);
        
        glNormal3f(0, 1, 0);
        glVertex3f(0, 0, 0);
        glVertex3f(1, 0, 0);
        glVertex3f(1, 0, -1);
        glVertex3f(0, 0, -1);
        
        glNormal3f(0, 0, -1);
        glVertex3f(0, 0, -1);
        glVertex3f(0, -1, -1);
        glVertex3f(1, -1, -1);
        glVertex3f(1, 0, -1);
        
        glNormal3f(1, 0, 0);
        glVertex3f(1, 0, 0);
        glVertex3f(1, -1, 0);
        glVertex3f(1, -1, -1);
        glVertex3f(1, 0, -1);
        
        glNormal3f(0, -1, 0);
        glVertex3f(0, -1, 0);
        glVertex3f(1, -1, 0);
        glVertex3f(1, -1, -1);
        glVertex3f(0, -1, -1);
        
        glEnd();
    }
    void control(float timeSlice) {}
    void animate() {}
};

struct SpaceStation : Object {
public:
    Cube napelem;
    CRForgastest body;
    Ellipsoid hole;
    float newAngle;
    Vector newPosition;
    float fi;
    
    SpaceStation () {
        napelem = Cube(Material(0.1, 0.2, 0.9, 1, 1), Vector(5, 8, 1), 0, Vector(0, 0, 0), Vector(5, 16, 0.04));
        body = CRForgastest (Vector(0, 0, 0), 0, Vector(0, 1, 1), Vector(1.25, 1.25, 1.25));
        hole = Ellipsoid(Material(0.4, 0.2, 0.2, 0, 0), 4.9, 4.9, 4.9, Vector(0, 0, 1.7), 0, Vector(0, 0, 0), Vector(1/2.5, 1/2.5, 1/2.5));
        fi = 0;
        control(0);
        animate();
    }
    void draw () {
        glPushMatrix();
        glTranslatef(body.position.x, body.position.y, body.position.z);
        glRotatef(body.angle, body.rotate.x, body.rotate.y, body.rotate.z);
        glScalef(body.scale.x, body.scale.y, body.scale.z);
        glPushMatrix();
        glTranslatef(napelem.position.x, napelem.position.y, napelem.position.z);
        glScalef(napelem.scale.x, napelem.scale.y, napelem.scale.z);
        glRotatef(napelem.angle, napelem.rotate.x, napelem.rotate.y, napelem.rotate.z);
        napelem.draw();
        glPopMatrix();
        body.draw();
        glPushMatrix();
        glTranslatef(hole.position.x, hole.position.y, hole.position.z);
        glRotatef(angle, hole.rotate.x, hole.rotate.y, hole.rotate.z);
        glScalef(hole.scale.x, hole.scale.y, hole.scale.z);
        hole.draw();
        glPopMatrix();
        glPopMatrix();
    }
    
    void control(float timeSlice) {
        float anglePerMs = 0.01;
        newAngle = anglePerMs * timeSlice + body.angle;
        float deltaFi = timeSlice*2*pi/100000;
        newPosition.x = body.position.x;
        newPosition.y = 75*sinf(fi+deltaFi);
        newPosition.z = 100*cosf(fi + deltaFi);
        fi += deltaFi;
    }
    void animate() {
        body.angle = newAngle;
        body.position = newPosition;
    }
    
};

Planet planet;
Satellite satellite;
SpaceStation mir;
Object *objects[3];
int numberOfObjects;
float prevTime;
float k;
float a;
float m;
float v;
float x;
float tavolsag;
float kotelhossz;
bool dir;

struct Camera {
public:
    Vector eye;
    Vector lookAt;
    Vector up;
    int fieldOfView;
    int aspectRatio;
    int zNear;
    int zFar;
    Camera () {}
    Camera (Vector eye, Vector lookAt, Vector up, int fieldOfView, int aspectRatio, int zNear, int zFar) {
        this->eye = eye;
        this->lookAt = lookAt;
        this->up = up;
        this->fieldOfView = fieldOfView;
        this->aspectRatio = aspectRatio;
        this->zNear = zNear;
        this->zFar = zFar;
    }
    void setOGL () {
        glViewport(0, 0, screenWidth, screenHeight);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(fieldOfView, aspectRatio, zNear, zFar);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        gluLookAt(eye.x, eye.y, eye.z, lookAt.x, lookAt.y, lookAt.z, up.x, up.y, up.z);
    }
    void control (float timeslice) {
        if (dir) {
            tavolsag = tavolsag + v*timeslice;
            if (tavolsag >= kotelhossz) {
                tavolsag = kotelhossz;
                dir = false;
            }
        }
        else {
            tavolsag = tavolsag - v*timeslice;
            if (tavolsag <= 0) {
                dir = true;
            }
        }
    }
    void animate () {
        lookAt = mir.body.position;
        eye = Vector(lookAt.x, lookAt.y, lookAt.z +tavolsag+14);
    }
};

Camera camera;

struct Scene {
public:
    Light sun;
    Vector stars[200];
    Scene () {
        float pos[4] = {0, 0, 100, 0};
        float ambient[4] = {1, 1, 1, 1};
        float diffuse[4] = {0.2, 0.2, 0.2, 1};
        float specular[4] = {1, 1, 1, 1};
        float shininess = 128;
        sun = Light(pos, ambient, diffuse, specular, shininess);
        mir = SpaceStation();
        camera = Camera(Vector(0,0,100), Vector(0,0,0), Vector(0,1,0), 65, 1, 5, 400);
        objects[0] = &satellite;
        objects[1] = &planet;
        objects[2] = &mir;
        for (int i = 0; i < 200; i++) {
            Vector position =  Vector(rand()%100, rand()%100, -(rand()%40));
            if (i % 8 == 0) {
                
            }
            else if (i % 2 == 0) {
                position.x *= -1;
            }
            if (i % 4 == 0) {
                position.y *= -1;
            }
            if (position.z > -30) {
                position.z = -30;
                
            }
            stars[i] = position;
        }
    }
    void render () {
        camera.animate();
        camera.setOGL();
        sun.setOGL();
        float colour[3] = {1, 1, 1};
        float shininess = 128;
        glMaterialfv(GL_FRONT, GL_DIFFUSE, colour);
        glMaterialfv(GL_FRONT, GL_AMBIENT, colour);
        glMaterialfv(GL_FRONT, GL_SPECULAR, colour);
        glMaterialfv(GL_FRONT, GL_SPECULAR, colour);
        glMaterialf(GL_FRONT, GL_SHININESS, shininess);
        glBegin(GL_POINTS);
        for (int i = 0; i < 200; i++) {
            Vector pos = stars[i];
            glVertex3f(pos.x, pos.y, pos.z);
        }
        glEnd();
        for (int i = 0; i < numberOfObjects; i++) {
            objects[i]->draw();
        }
    }
    
};

Scene scene;

// Inicializacio, a program futasanak kezdeten, az OpenGL kontextus letrehozasa utan hivodik meg (ld. main() fv.)
void onInitialization( ) {
    glGenTextures(1, &texid);
    glBindTexture(GL_TEXTURE_2D, texid);
    int level = 0, border = 0, width = 128, height = 128;
    unsigned char image[width*height*3];
    for (int i = 0; i < width*height*3; i=i+3) {
        if(i % 8) {
            image[i] = 208;
            image[i+1] = 232;
            image[i+2] = 74;
        }
        else {
            image[i] = 0;
            image[i+1] = 128;
            image[i+2] = 0;
        }
    }
    glTexImage2D(GL_TEXTURE_2D,level, GL_RGB, width, height, border, GL_RGB, GL_UNSIGNED_BYTE, &image[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glEnable(GL_NORMALIZE);
    glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);
    numberOfObjects = 3;
    scene = Scene();
    k = 5;
    m = 1;
    v = 0.004;
    tavolsag = 0;
    kotelhossz = 10;
    dir = true;
    prevTime = 0;
}

// Rajzolas, ha az alkalmazas ablak ervenytelenne valik, akkor ez a fuggveny hivodik meg
void onDisplay( ) {
    glClearColor(0, 0, 0, 1.0f);        // torlesi szin beallitasa
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // kepernyo torles
    scene.render();
    glutSwapBuffers();                  // Buffercsere: rajzolas vege
    
}

// Billentyuzet esemenyeket lekezelo fuggveny (lenyomas)
void onKeyboard(unsigned char key, int x, int y) {
    if (key == 'w') {
        satellite.speed.z += sinf((satellite.angle+90)*pi/180)*0.1;
        satellite.speed.x += cosf((satellite.angle+90)*pi/180)*0.1;
    }
    if (key == 's') {
        satellite.speed.z -= sinf((satellite.angle+90)*pi/180)*0.1;
        satellite.speed.x -= cosf((satellite.angle+90)*pi/180)*0.1;
    }
    if (key == 'd') {
        satellite.speed.z += sinf((satellite.angle)*pi/180)*0.1;
        satellite.speed.x += cosf((satellite.angle)*pi/180)*0.1;
    }
    if (key == 'a') {
        satellite.speed.z -= sinf((satellite.angle)*pi/180)*0.1;
        satellite.speed.x -= cosf((satellite.angle)*pi/180)*0.1;
    }
    if (key == 'i') {
        satellite.speed.y += 0.1;
    }
    if (key == 'k') {
        satellite.speed.y -= 0.1;
    }
}

// Billentyuzet esemenyeket lekezelo fuggveny (felengedes)
void onKeyboardUp(unsigned char key, int x, int y) {
    
}

// Eger esemenyeket lekezelo fuggveny
void onMouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)   // A GLUT_LEFT_BUTTON / GLUT_RIGHT_BUTTON illetve GLUT_DOWN / GLUT_UP
        glutPostRedisplay( );                        // Ilyenkor rajzold ujra a kepet
}

// Eger mozgast lekezelo fuggveny
void onMouseMotion(int x, int y)
{
    
}

// `Idle' esemenykezelo, jelzi, hogy az ido telik, az Idle esemenyek frekvenciajara csak a 0 a garantalt minimalis ertek
void onIdle( ) {
    float now =  (float)glutGet(GLUT_ELAPSED_TIME);
    for (int i = 0; i < numberOfObjects; i++) {
        objects[i]->control(now - prevTime);
    }
    camera.control(now - prevTime);
    for (int i = 0; i < numberOfObjects; i++) {
        objects[i]->animate();
    }
    prevTime = now;
    onDisplay();
    
}

// ...Idaig modosithatod
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// A C++ program belepesi pontja, a main fuggvenyt mar nem szabad bantani
int main(int argc, char **argv) {
    glutInit(&argc, argv);              // GLUT inicializalasa
    glutInitWindowSize(600, 600);           // Alkalmazas ablak kezdeti merete 600x600 pixel
    glutInitWindowPosition(100, 100);           // Az elozo alkalmazas ablakhoz kepest hol tunik fel
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);  // 8 bites R,G,B,A + dupla buffer + melyseg buffer
    
    glutCreateWindow("Grafika hazi feladat");       // Alkalmazas ablak megszuletik es megjelenik a kepernyon
    
    glMatrixMode(GL_MODELVIEW);             // A MODELVIEW transzformaciot egysegmatrixra inicializaljuk
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);            // A PROJECTION transzformaciot egysegmatrixra inicializaljuk
    glLoadIdentity();
    
    onInitialization();                 // Az altalad irt inicializalast lefuttatjuk
    
    glutDisplayFunc(onDisplay);             // Esemenykezelok regisztralasa
    glutMouseFunc(onMouse);
    glutIdleFunc(onIdle);
    glutKeyboardFunc(onKeyboard);
    glutKeyboardUpFunc(onKeyboardUp);
    glutMotionFunc(onMouseMotion);
    
    glutMainLoop();                 // Esemenykezelo hurok
    
    return 0;
}

