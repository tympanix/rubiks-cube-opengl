#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <cstdio>
#include <unistd.h>
#include <vector>
#include <string>
#include <map>

bool rotate = true;

enum Move {U, D, L, R, F, B, M};

struct faceRotation {
    float angle = 0, x = 0, y = 0, z = 0;
};

struct faceSelect {
    int x = -1, y = -1, z = -1;
};

struct faceTransform {
    faceSelect s;
    faceRotation r;
};

class Cube {

    public:
    Cube(int _i) {
        i = _i;
        setPosition(i);
    }
    
    int i = 0;
    int x = 0, y = 0, z = 0;
    std::vector<faceRotation> rots;

    void setPosition(int i) {
        y = i / 9; 
        x = i % 3;
        z = (i % 9) / 3; 
    }

    void draw() {
        glPushMatrix();

        GLfloat vertices[] =
        {
            -1, -1, -1,   -1, -1,  1,   -1,  1,  1,   -1,  1, -1,
            1, -1, -1,    1, -1,  1,    1,  1,  1,    1,  1, -1,
            -1, -1, -1,   -1, -1,  1,    1, -1,  1,    1, -1, -1,
            -1,  1, -1,   -1,  1,  1,    1,  1,  1,    1,  1, -1,
            -1, -1, -1,   -1,  1, -1,    1,  1, -1,    1, -1, -1,
            -1, -1,  1,   -1,  1,  1,    1,  1,  1,    1, -1,  1
        };

        GLfloat colors[] =
        {
            0, 1, 1,   0, 1, 1,   0, 1, 1,   0, 1, 1,
            1, 0, 0,   1, 0, 0,   1, 0, 0,   1, 0, 0,
            0, 1, 0,   0, 1, 0,   0, 1, 0,   0, 1, 0,
            0, 0, 1,   0, 0, 1,   0, 0, 1,   0, 0, 1,
            1, 1, 0,   1, 1, 0,   1, 1, 0,   1, 1, 0,
            1, 0, 1,   1, 0, 1,   1, 0, 1,   1, 0, 1
        };

        glTranslatef(2*x-2,2*y-2,2*z-2);

        for (int i = rots.size()-1; i >= 0; i--) {
            glRotatef(rots[i].angle, rots[i].x, rots[i].y, rots[i].z);
        }

        /* We have a color array and a vertex array */
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_COLOR_ARRAY);
        glVertexPointer(3, GL_FLOAT, 0, vertices);
        glColorPointer(3, GL_FLOAT, 0, colors);

        /* Send data : 24 vertices */
        glDrawArrays(GL_QUADS, 0, 24);

        /* Cleanup states */
        glDisableClientState(GL_COLOR_ARRAY);
        glDisableClientState(GL_VERTEX_ARRAY);
        
        glPopMatrix();
    }
};

class Rubiks {

    public:
    Rubiks() {
        for (int i = 0; i < SIZE*SIZE*SIZE; i++) {
            positions.push_back(new Cube(i));
        }
    }

    bool isCubeOnFace(const faceSelect &s, const Cube *c) {
        if (s.x >= 0 && c->x == s.x) {
            return true;
        }
        if (s.y >= 0 && c->y == s.y) {
            return true;
        }
        if (s.z >= 0 && c->z == s.z) {
            return true;
        }
        return false;
    }

    faceTransform moveToSelector(Move m) {
        faceSelect s;
        faceRotation r;

        switch (m) {
        case U:
            s.y = 2;
            r.y = 1;
            break;
        case D:
            s.y = 0;
            r.y = 1;
            break;
        case L:
            s.x = 0;
            r.x = 1;
            break;
        case R:
            s.x = 2;
            r.x = 1;
            break;
        case F:
            s.z = 0;
            r.z = 1;
            break;
        case B:
            s.z = 2;
            r.z = 1;
            break;
        case M:
            s.x = 1;
            r.x = 1;
            break;
        default:
            throw std::invalid_argument("unknown move");
        }

        r.angle = 90.0;

        return {
            s, r
        };
    }
    
    std::vector<Cube*> selectFace(faceSelect &s) {
        std::vector<Cube*> face;
        for (int i = 0; i < positions.size(); i++) {
            if (isCubeOnFace(s, positions[i])) {
                face.push_back(positions[i]);
            }
        }
        return face;
    }

    std::vector<Cube*> positions;
    int SIZE = 3;

    void move(Move m) {
        // select face
        faceTransform t = moveToSelector(m);
        std::vector<Cube*> face = selectFace(t.s);

        // rotate each cube in face
        for (int i = 0; i < face.size(); i++) {
            face[i]->rots.push_back(t.r);
        }
        
        // permute face
        std::vector<Cube*> perm = face;
        for (int i = 0; i < perm.size(); i++) {
            int perm_idx = 2 * SIZE - i % 3 * SIZE + i / 3;
            perm[i] = face[perm_idx];
        }
        
        // copy back face
        int j = 0;
        for (int i = 0; i < positions.size(); i++) {
            if (isCubeOnFace(t.s, positions[i])) {
                positions[i] = perm[j];
                j++;
            }
        }

        // apply new positions
        for (int i = 0; i < positions.size(); i++) {
            positions[i]->setPosition(i);
        }

        if (isSolved()) {
            printf("Solved!\n");
        }
        return;
    }
    
    bool isSolved() {
        for (int i = 0; i < positions.size(); i++) {
            if (positions[i]->i != i) {
                return false;
            }
        }
        return true;
    }

    void draw() {
        for (int i = 0; i < positions.size(); i++) {
            positions[i]->draw();
        }
    }
};

Rubiks *r = new Rubiks();

void controls(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if(action == GLFW_PRESS) {
        if(key == GLFW_KEY_ESCAPE)
            glfwSetWindowShouldClose(window, GL_TRUE);
        if (key == GLFW_KEY_1)
            r->move(Move::L);
        if (key == GLFW_KEY_2)
            r->move(Move::R);
        if (key == GLFW_KEY_3)
            r->move(Move::U);
        if (key == GLFW_KEY_4)
            r->move(Move::D);
        if (key == GLFW_KEY_5)
            r->move(Move::F);
        if (key == GLFW_KEY_6)
            r->move(Move::B);
        if (key == GLFW_KEY_7)
            r->move(Move::M);
        if (key == GLFW_KEY_SPACE) {
            rotate = !rotate;
        }
    }
}

GLFWwindow* initWindow(const int resX, const int resY) {
    if(!glfwInit())
    {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return NULL;
    }
    glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing

    // Open a window and create its OpenGL context
    GLFWwindow* window = glfwCreateWindow(resX, resY, "TEST", NULL, NULL);

    if(window == NULL)
    {
        fprintf(stderr, "Failed to open GLFW window.\n");
        glfwTerminate();
        return NULL;
    }

    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, controls);

    // Get info of GPU and supported OpenGL version
    printf("Renderer: %s\n", glGetString(GL_RENDERER));
    printf("OpenGL version supported %s\n", glGetString(GL_VERSION));

    glEnable(GL_DEPTH_TEST); // Depth Testing
    glDepthFunc(GL_LEQUAL);
    glDisable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    return window;
}

void display( GLFWwindow* window ) {

    while(!glfwWindowShouldClose(window))
    {
        static float alpha = 0;

        // Scale to window size
        GLint windowWidth, windowHeight;
        glfwGetWindowSize(window, &windowWidth, &windowHeight);
        glViewport(0, 0, windowWidth*2, windowHeight*2);

        // Draw stuff
        glClearColor(0.2, 0.2, 0.2, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glMatrixMode(GL_PROJECTION_MATRIX);
        glLoadIdentity();
        gluPerspective( 60, (double)windowWidth / (double)windowHeight, 0.1, 100 );

        glMatrixMode(GL_MODELVIEW_MATRIX);
        glTranslatef(0,0,-12);

        glRotatef(alpha, 0, 1, 0);
        r->draw();

        if (rotate)
            alpha += 1;

        // Update Screen
        glfwSwapBuffers(window);

        // Check for any input, or window movement
        glfwPollEvents();

        usleep(16*1000);
    }
}

int main(int argc, char** argv) {
    GLFWwindow* window = initWindow(1024, 620);
    if( NULL != window )
    {
        display( window );
    }
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}