//按鼠标左键确定一系列点,回车进行绘制
#include <GLFW/glfw3.h>
#include <bits/stdc++.h>
#include <windows.h>

#define WIDTH 750.f
#define HEIGHT 750.f
#define length 0.02f
#define gap 20
#define pdd pair<double, double>
using namespace std;

vector<pdd> points; //初始图形的顶点
vector<pdd> line;   //生成曲线上的每一个点

bool finished = false;
double changex(double x) {
    return x / WIDTH * 2 - 1; //将像素坐标转换为标准化设备坐标
}
double changey(double y) {
    return 1 - y / HEIGHT * 2;
}

void showPicture() {        //绘制折线段
    glBegin(GL_LINE_STRIP); // 折线段
    glColor3f(0.f, 1.f, 0.f);
    for (auto it : points) {
        double x = it.first, y = it.second;
        glVertex3f(changex(x), changey(y), 0.0f);
    }
    glEnd(); // 多边形绘制结束
}
void showBezier() {
    glBegin(GL_LINE_STRIP); // 折线段
    glColor3f(0.f, 0.f, 1.f);
    for (auto it : line) {
        double x = it.first, y = it.second;
        glVertex3f(changex(x), changey(y), 0.0f);
    }
    glEnd(); // 多边形绘制结束
}
void createBezier() { //绘制曲线
    int len = points.size();
    vector<pdd> v(len + 1, {0, 0});
    for (double t = 0; t <= 1; t += 0.05) {//精细程度
        for (int i = 1; i < len; i++) {
            for (int j = 0; j < len - i; j++) {
                if (i == 1) { //第一次迭代,由已知控制点计算
                    v[j].first = points[j].first * (1 - t) + points[j + 1].first * t;
                    v[j].second = points[j].second * (1 - t) + points[j + 1].second * t;
                    continue;
                }
                v[j].first = v[j].first * (1 - t) + v[j + 1].first * t; //通过上一次迭代结果计算
                v[j].second = v[j].second * (1 - t) + v[j + 1].second * t;
            }
        }
        line.push_back(v.front());
    }
}

// mouse button callback
void releaseLeftButton(GLFWwindow *window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) { //释放了鼠标左键,添加多边形
        double x, y;
        glfwGetCursorPos(window, &x, &y);
        cout << "当前位置 " << x << "," << y << std::endl;
        points.push_back({x, y});
    }
}

void pressKeyBoard(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ENTER && action == GLFW_PRESS) { //回车确定当前图形
        line.clear();
        cout << "开始绘制" << endl;
        createBezier();
        finished = true;
    }
    if (key == GLFW_KEY_DELETE && action == GLFW_PRESS) { //删除全部
        cout << "删除全部顶点" << endl;
        points.clear();
        line.clear();
        finished = false;
    }
}
int main(void) {
    srand((int)time(NULL)); // 产生随机种子
    GLFWwindow *window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(WIDTH, HEIGHT, "计算机图形学实验_填充", NULL, NULL);
    glfwSetWindowPos(window, 150, 40);

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
        glfwSetMouseButtonCallback(window, releaseLeftButton);

        // keyboard
        glfwSetKeyCallback(window, pressKeyBoard);

        // drawYLine(100, 200, 300);
        showPicture();  // 展示线
        if (finished) { //展示绘制结果
            showBezier();
        }

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
