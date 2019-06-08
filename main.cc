#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <cstdio>
#include <unistd.h>
#include <vector>
#include <map>


enum Move {U, D, L, R, F, B, M};

class Cube {

    public:
    Cube(int _i) {
        i = _i;
        y = i / 9; 
        x = i % 3;
        z = (i % 9) / 3; 
    }
    
    int i = 0;
    int x = 0, y = 0, z = 0;
    float rx = 0, ry = 0, rz = 0;

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
        glRotatef(rx, 1, 0, 0);
        glRotatef(ry, 0, 1, 0);
        glRotatef(rz, 0, 0, 1);

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
        for (int i = 0; i < 27; i++) {
            positions.push_back(new Cube(i));
        }
        
        faces[Move::U] = {
            0, 1, 2, 3, 4, 5, 6, 7, 8
        };
        
        faces[Move::D] = {
            18, 19, 20, 21, 22, 23, 24, 25, 26
        };

        faces[Move::L] = {
            0, 3, 6, 9, 12, 15, 18, 21, 24
        };

        faces[Move::R] = {
            2, 5, 8, 11, 14, 17, 20, 23, 26
        };

        faces[Move::F] = {
            6, 7, 8, 15, 16, 17, 24, 25, 26
        };

        faces[Move::B] = {
            0, 1, 2, 9, 10, 11, 18, 19, 20
        };

        faces[Move::M] = {
            1, 4, 7, 10, 13, 16, 19, 22, 25
        };
    }

    std::vector<Cube*> positions;
    std::map< Move, std::vector<int> > faces;
    int SIZE = 3;

    void move(Move m) {
        for (int i = 0; i < faces[m].size(); i++) {
            positions[faces[m][i]]->rx += 90;
        }
        printf("Rotate..!\n");
        return;
    }
    
    std::vector<int> permute(std::vector<int> face, bool rev) {
        std::vector<int> copy = face;
        for (int i = 0; i < SIZE; i++) {
            int step = (SIZE-1)*SIZE;
            int offset = step+1;
            for (int j = 0; j < SIZE; j++) {
                copy[i*SIZE+j] = face[(offset+step*j)%(SIZE*SIZE)]; 
            }
        }
        return copy;
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
            r->move(Move::B);
        if (key == GLFW_KEY_5)
            r->move(Move::F);
        if (key == GLFW_KEY_6)
            r->move(Move::B);
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