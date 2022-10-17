/*INTRODUCTION
// Mouse:
//	- LEFT button:
//		1. draw a new line
//		2. choose the endpoint to modify a line
// Keyboard:
//  - DELETE:
//      clear the window
*/
#include <GLFW/glfw3.h>
#include <bits/stdc++.h>

#define WIDTH 1000.f
#define HEIGHT 1000.f
#define length 0.02f
#define gap 20
#define pdd pair<double, double>
using namespace std;

// hide the console window
// #pragma comment(linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"")

struct Color {
    float r, g, b;
    Color() {}
    Color(float x, float y, float z)
        : r(x), g(y), b(z) {}
} YELLOW(0.0f, 1.0f, 1.0f), RED(1.0f, 0.0f, 1.0f);

vector<pdd> points;       // 保存正在画的点
vector<vector<pdd>> Line; //保存画好的全部直线
bool Modifying = false;

//小的矩形块,用于模拟像素
float vctPnt[][2] = {
    {-length, length},
    {length, length},
    {length, -length},
    {-length, -length}};

// opengl绘制一个矩形块
void draw(double x, double y, Color color) {
    // transform the coordinates to the drawing: (0, 0) == (width/2, height/2)
    x = (x - WIDTH / 2) / WIDTH * 2;
    y = (HEIGHT / 2 - y) / HEIGHT * 2;
    glBegin(GL_POLYGON); // 矩形
    glColor3f(color.r, color.g, color.b);
    for (int i = 0; i < 4; i++) {
        glVertex3f(x + vctPnt[i][0], y + vctPnt[i][1], 0.0f);
    }
    glEnd(); // 矩形绘制结束
}
// use the window's coordinates: leftBottom(0,0), the drawing coordinates is different
void BresemhamLine(double x0, double y0, double x1, double y1, Color color) {
    double x, y, dx, dy;
    double k, e;
    dx = x1 - x0, dy = y1 - y0, k = dy / dx;
    e = -0.5, x = x0, y = y0;
    // |k| <= 1
    if (abs(k) <= 1) {
        for (int i = 0; i <= abs(dx) / gap; i++) {
            draw(x, HEIGHT - y, color); //一个一个的进行绘制
            // direction control
            if (dx >= 0)
                x += gap;
            else
                x -= gap;
            e += abs(k); // e = d - 0.5

            if (e >= 0) {
                // direction control
                if (dy >= 0)
                    y += gap;
                else
                    y -= gap;
                e -= 1;
            }
        }
    }
    // |k| > 1; change x and y
    else {
        for (int i = 0; i <= abs(dy) / gap; i++) {
            draw(x, HEIGHT - y, color);
            // direction control
            if (dy >= 0)
                y += gap;
            else
                y -= gap;
            e += abs(1 / k);

            if (e >= 0) {
                // direction control
                if (dx >= 0)
                    x += gap;
                else
                    x -= gap;
                e -= 1;
            }
        }
    }
}

// Display the lines
void showAllLine() {
    // display modifying line dynamically
    if (!points.empty()) {
        BresemhamLine(points.front().first, points.front().second, points.back().first, points.back().second, RED);
    }

    // display the certain lines
    for (auto &it : Line) {
        BresemhamLine(it.front().first, it.front().second, it.back().first, it.back().second, YELLOW);
    }
}

// mouse button callback
void releaseLeftButton(GLFWwindow *window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) { //释放了鼠标左键,说明增加了一条新的直线
        Modifying = false;                                            // stop modifying the end-point
        if (!points.empty()) {                                        // just finish drawing lines
            vector<pdd> tmp;
            tmp.push_back(points.front());
            tmp.push_back(points.back());
            Line.push_back(tmp);
            points.clear();
        }
    }
}

// CursorPos callback
void clickLeftButton(GLFWwindow *window, double x, double y) {
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == 1) {
        // press button to add or modify a line
        // transfer the coordinates
        y = HEIGHT - y;
        // preserve the drawing points
        points.push_back({x, y});
        Modifying = true; // drawing new line or moving a line
    }
}

void pressKeyBoard(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_BACKSPACE && action == GLFW_PRESS) {
        if (!Line.empty()) {
            Line.pop_back();
        }
    }
    if (key == GLFW_KEY_DELETE && action == GLFW_PRESS) {
        Line.clear();
    }
}
int main(void) {
    srand((int)time(NULL)); // 产生随机种子
    GLFWwindow *window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(WIDTH, HEIGHT, "计算机图形学实验_画线", NULL, NULL);
    glfwSetWindowPos(window, 40, 40);

    if (!window) {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window)) {
        /* Render here */
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // mouse
        glfwSetCursorPosCallback(window, clickLeftButton);
        glfwSetMouseButtonCallback(window, releaseLeftButton);

        // keyboard
        glfwSetKeyCallback(window, pressKeyBoard);

        // draw here
        showAllLine();

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
