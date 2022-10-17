//按鼠标左键确定一系列点,按回车进行填充,按delete擦除全部图形
#include <GLFW/glfw3.h>
#include <bits/stdc++.h>

#define WIDTH 750.f
#define HEIGHT 750.f
#define length 0.02f
#define gap 20
#define pdd pair<double, double>
using namespace std;

// hide the console window
// #pragma comment(linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"")

vector<pdd> mid, res; //中间图形,最终结果
// mouse button callback
void releaseLeftButton(GLFWwindow *window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) { //释放了鼠标左键
        double xposition, yposition;
        glfwGetCursorPos(window, &xposition, &yposition);
        std::cout << xposition << "," << yposition << std::endl;
        mid.push_back({xposition,yposition});
    }
}

// CursorPos callback
void clickLeftButton(GLFWwindow *window, double x, double y) {
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == 1) {
    }
}

void pressKeyBoard(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ENTER && action == GLFW_PRESS) { //回车确定当前图形
        cout << "回车确定当前图形" << endl;
        res = mid;
        mid.clear();
    }
    if (key == GLFW_KEY_DELETE && action == GLFW_PRESS) { //删除全部
        cout << "删除全部" << endl;
        res.clear();
        mid.clear();
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
        glfwSetCursorPosCallback(window, clickLeftButton);
        glfwSetMouseButtonCallback(window, releaseLeftButton);

        // keyboard
        glfwSetKeyCallback(window, pressKeyBoard);

        // draw here
        // showAllLine();

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
