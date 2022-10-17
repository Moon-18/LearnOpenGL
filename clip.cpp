//按鼠标左键确定一系列点,按回车进行填充,按delete擦除全部图形
#include <GLFW/glfw3.h>
#include <bits/stdc++.h>
#include <windows.h>

#define WIDTH 750.f
#define HEIGHT 750.f
#define length 0.02f
#define gap 20
#define pdd pair<double, double>
using namespace std;

// hide the console window
// #pragma comment(linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"")
double changex(double x) {
    return x / WIDTH * 2 - 1; //将像素坐标转换为标准化设备坐标
}
double changey(double y) {
    return 1 - y / HEIGHT * 2;
}
vector<pdd> rectangle, lines, cuts; //矩形,原本直线,裁剪后的直线
// mouse button callback
void releaseButton(GLFWwindow *window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE && rectangle.size() < 2) { //释放了鼠标右键
        double xposition, yposition;
        glfwGetCursorPos(window, &xposition, &yposition);
        cout << xposition << "," << yposition << endl;
        rectangle.push_back({xposition, yposition}); //添加矩形
    }
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) { //释放了鼠标左键
        double xposition, yposition;
        glfwGetCursorPos(window, &xposition, &yposition);
        cout << xposition << "," << yposition << endl;
        lines.push_back({xposition, yposition}); //直线
    }
}

void pressKeyBoard(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_BACKSPACE && action == GLFW_PRESS) { //删除全部
        cout << "删除全部线段" << endl;
        lines.clear();
        cuts.clear();
    }
    if (key == GLFW_KEY_DELETE && action == GLFW_PRESS) { //删除全部
        cout << "删除全部矩形" << endl;
        rectangle.clear();
    }
}
void L_Barskey(pdd sp, pdd ep) { // start point end point 给定一条线段起点和终点,得到一条新的线段起点和终点
    double p[4], q[4];
    double u1 = 0, u2 = 1;
    bool fail = false;
    p[0] = sp.first - ep.first, p[2] = sp.second - ep.second;
    p[1] = -p[0], p[3] = -p[2];
    q[0] = sp.first - rectangle[0].first, q[1] = rectangle[1].first - sp.first;
    q[2] = sp.second - rectangle[0].second, q[3] = rectangle[1].second - sp.second;
    for (int i = 0; i < 4; i++) {
        double r = q[i] / p[i];
        if (p[i] < 0) {
            u1 = max(u1, r);
        } else if (p[i] > 0) {
            u2 = min(u2, r);
        }
        if (u1 > u2 || (p[i] == 0 && q[i] < 0)) {
            fail = true;
        }
    }
    if (fail) {
        return;
    }
    pdd newLineStart = {sp.first - u1 * (sp.first - ep.first), sp.second - u1 * (sp.second - ep.second)},
        newLineEnd = {sp.first - u2 * (sp.first - ep.first), sp.second - u2 * (sp.second - ep.second)};
    cuts.push_back(newLineStart);
    cuts.push_back(newLineEnd);
}

void show() {
    //绘制矩形,红色
    glBegin(GL_LINES); // 直线
    for (int i = 0; i < (int)rectangle.size() - 1 && (int)rectangle.size() % 2 == 0; i += 2) {
        glColor3f(1.f, 0.f, 0.f);
        if (rectangle[i] > rectangle[i + 1]) {
            swap(rectangle[i], rectangle[i + 1]);
        }
        double x1 = changex(rectangle[i].first), x2 = changex(rectangle[i + 1].first);
        double y1 = changey(rectangle[i].second), y2 = changey(rectangle[i + 1].second);
        glVertex3f(x1, y1, 0.0f);
        glVertex3f(x1, y2, 0.0f);

        glVertex3f(x1, y2, 0.0f);
        glVertex3f(x2, y2, 0.0f);

        glVertex3f(x2, y2, 0.0f);
        glVertex3f(x2, y1, 0.0f);

        glVertex3f(x2, y1, 0.0f);
        glVertex3f(x1, y1, 0.0f);
    }
    glEnd(); // 矩形绘制结束
    //绘制线段.绿色
    glBegin(GL_LINES); // 直线
    for (int i = 0; i < (int)lines.size() - 1; i += 2) {
        if (i == (int)lines.size() - 2 && (int)lines.size() % 2 == 1) { //奇数个的时候就别画这一条了
            continue;
        }
        glColor3f(0.f, 1.f, 0.f);
        if (lines[i] > lines[i + 1]) {
            swap(lines[i], lines[i + 1]);
        }
        pdd p1 = lines[i], p2 = lines[i + 1];
        glVertex3f(changex(p1.first), changey(p1.second), 0.0f);
        glVertex3f(changex(p2.first), changey(p2.second), 0.0f);
        //对线段进行裁剪
        L_Barskey(p1, p2);
    }
    glEnd(); // 矩形绘制结束

    //绘制裁剪过的线段,蓝色
    glBegin(GL_LINES); // 直线
    for (int i = 0; i < (int)cuts.size() - 1; i += 2) {
        glColor3f(0.f, 0.f, 1.f);
        pdd p1 = cuts[i], p2 = cuts[i + 1];
        glVertex3f(changex(p1.first), changey(p1.second), 0.0f);
        glVertex3f(changex(p2.first), changey(p2.second), 0.0f);
        //对线段进行裁剪
    }
    glEnd(); // 矩形绘制结束
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
        glfwSetMouseButtonCallback(window, releaseButton);

        // keyboard
        glfwSetKeyCallback(window, pressKeyBoard);

        // draw here
        show();

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
