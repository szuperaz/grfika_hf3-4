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

const int screenWidth = 600;	// alkalmazÃ¡s ablak felbontÃ¡sa
const int screenHeight = 600;
const float pi = 3.14159265359;
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
    float operator*(const Vector& v) { 	// dot product
        return (x * v.x + y * v.y + z * v.z);
    }
    Vector operator%(const Vector& v) { 	// cross product
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
};

struct Object {
public:
    void virtual draw()=0;
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

struct ParametricSurface : Object {
public:
    Vector position;
    Vector scale;
    Vector rotate;
    Material material;
    float angle;
    ParametricSurface () {
        
    }
    ParametricSurface (Material material, Vector position, float angle, Vector rotate, Vector scale) {
        this->position = position;
        this->rotate = rotate;
        this->scale = scale;
        this->material = Material(material);
        this->angle = angle;
    }
    void draw () {
        material.setOGL();
        glBegin(GL_QUADS);
        int ntess = 150;
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
    Ellipsoid () {}
    Ellipsoid (Material m, float a, float b, float c, Vector position, float angle, Vector rotate, Vector scale) : ParametricSurface(m, position, angle, rotate, scale){
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
};

struct Cylinder : ParametricSurface {
    float height, radius;
    Cylinder () {}
    Cylinder (Material m, float height, float radius, Vector position, float angle, Vector rotate, Vector scale) :ParametricSurface(m, position, angle, rotate, scale) {
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
};

struct Satellite : Object {
public:
    Ellipsoid body;
    Cylinder pipe1, pipe2, pipe3;
    Satellite () {
        Material m = Material(0.7, 0.5, 0.5, 1, 10);
        Material m2 = Material(0.4, 0.4, 0.4, 1, 10);
        body = Ellipsoid(m, 10, 10, 10, Vector(45, 30, -40), 0, Vector(0, 0, 0), Vector(1, 1, 1));
        pipe1 = Cylinder(m2, 1, 1, Vector(0, 0, -15), 0, Vector(0, 0, 0), Vector(1, 1, 30));
        pipe2 = Cylinder(m2, 1, 1, Vector(0, 15, 0), 90, Vector(0, 0, 0), Vector(1, 1, 30));
        pipe3 = Cylinder(m2, 1, 1, Vector(-15, 0, 0), 90, Vector(0, 1, 0), Vector(1, 1, 30));
    }
    void draw () {
        glPushMatrix();
        glTranslatef(body.position.x, body.position.y, body.position.z);
        glRotatef(body.angle, body.rotate.x, body.rotate.y, body.rotate.z);
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
};

unsigned int texid;

struct Planet : Object {
public:
    Ellipsoid planet;
    Ellipsoid atmosphere;
    Planet () {
        Material mp = Material(1, 0, 0, 1, 2);
        Material ma = Material(0, 0, 0.9, 0.2, 2);
        planet = Ellipsoid(mp, 18, 17, 18, Vector(-30, 10, 0), 0, Vector(0,0,0), Vector(1,1,1));
        atmosphere = Ellipsoid(ma, 20, 19, 18, Vector(-30, 10, 0), 0, Vector(0,0,0), Vector(1,1,1));
    }
    void draw () {
        glPushMatrix();
        glTranslatef(planet.position.x, planet.position.y, planet.position.z);
        glRotatef(planet.angle, planet.rotate.x, planet.rotate.y, planet.rotate.z);
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
};

Planet planet;
Satellite satellite;

struct Scene {
public:
    Object *objects[2];
    Camera camera;
    Light sun;
    Scene () {
        camera = Camera(Vector(0,0,100), Vector(0,0,0), Vector(0,1,0), 65, 1, 5, 400);
        float pos[4] = {0, 0, 100, 0};
        float ambient[4] = {1, 1, 1, 1};
        float diffuse[4] = {0.2, 0.2, 0.2, 1};
        float specular[4] = {1, 1, 1, 1};
        float shininess = 128;
        sun = Light(pos, ambient, diffuse, specular, shininess);
        satellite = Satellite();
        planet = Planet();
        objects[0] = &satellite;
        objects[1] = &planet;
     }
    void render () {
        camera.setOGL();
        sun.setOGL();
        for (int i = 0; i < 50; i++) {
            Material m = Material(1, 1,  1, 1, 128);
            Ellipsoid star (m, 0.5, 0.5, 0.5, Vector(rand()%80, rand()%100, -rand()%300), 0, Vector(0,0,0), Vector(1,1,1));
            if (i % 2 == 0) {
                star.position.x *= -1;
            }
            if (i % 4 == 0) {
                star.position.y *= -1;
            }
            if (star.position.z > -100) {
                star.position.z = -100;
            
            }
            glPushMatrix();
            glTranslatef(star.position.x, star.position.y, star.position.z+120);
            glRotatef(0, star.rotate.x, star.rotate.y, star.rotate.z);
            glScalef(star.scale.x, star.scale.y, star.scale.z);
            star.draw();
            glPopMatrix();
        }
        for (int i = 0; i < 2; i++) {
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
    for (int i = 0; i < width*height*3; i++) {
        image[i] = rand();
    }
    glTexImage2D(GL_TEXTURE_2D,level, GL_RGB, width, height, border, GL_RGB, GL_UNSIGNED_BYTE, &image[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glEnable(GL_NORMALIZE);
    glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);
    scene = Scene();
}

// Rajzolas, ha az alkalmazas ablak ervenytelenne valik, akkor ez a fuggveny hivodik meg
void onDisplay( ) {
    glClearColor(0, 0, 0, 1.0f);		// torlesi szin beallitasa
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // kepernyo torles
    scene.render();
    glutSwapBuffers();     				// Buffercsere: rajzolas vege
    
}

// Billentyuzet esemenyeket lekezelo fuggveny (lenyomas)
void onKeyboard(unsigned char key, int x, int y) {
    if (key == 'd') glutPostRedisplay( ); 		// d beture rajzold ujra a kepet
    
}

// Billentyuzet esemenyeket lekezelo fuggveny (felengedes)
void onKeyboardUp(unsigned char key, int x, int y) {
    
}

// Eger esemenyeket lekezelo fuggveny
void onMouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)   // A GLUT_LEFT_BUTTON / GLUT_RIGHT_BUTTON illetve GLUT_DOWN / GLUT_UP
        glutPostRedisplay( ); 						 // Ilyenkor rajzold ujra a kepet
}

// Eger mozgast lekezelo fuggveny
void onMouseMotion(int x, int y)
{
    
}

// `Idle' esemenykezelo, jelzi, hogy az ido telik, az Idle esemenyek frekvenciajara csak a 0 a garantalt minimalis ertek
void onIdle( ) {
    long time = glutGet(GLUT_ELAPSED_TIME);		// program inditasa ota eltelt ido
    
}

// ...Idaig modosithatodjjjjj
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// A C++ program belepesi pontja, a main fuggvenyt mar nem szabad bantani
int main(int argc, char **argv) {
    glutInit(&argc, argv); 				// GLUT inicializalasa
    glutInitWindowSize(600, 600);			// Alkalmazas ablak kezdeti merete 600x600 pixel
    glutInitWindowPosition(100, 100);			// Az elozo alkalmazas ablakhoz kepest hol tunik fel
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);	// 8 bites R,G,B,A + dupla buffer + melyseg buffer
    
    glutCreateWindow("Grafika hazi feladat");		// Alkalmazas ablak megszuletik es megjelenik a kepernyon
    
    glMatrixMode(GL_MODELVIEW);				// A MODELVIEW transzformaciot egysegmatrixra inicializaljuk
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);			// A PROJECTION transzformaciot egysegmatrixra inicializaljuk
    glLoadIdentity();
    
    onInitialization();					// Az altalad irt inicializalast lefuttatjuk
    
    glutDisplayFunc(onDisplay);				// Esemenykezelok regisztralasa
    glutMouseFunc(onMouse);
    glutIdleFunc(onIdle);
    glutKeyboardFunc(onKeyboard);
    glutKeyboardUpFunc(onKeyboardUp);
    glutMotionFunc(onMouseMotion);
    
    glutMainLoop();					// Esemenykezelo hurok
    
    return 0;
}

