#include <GLFW/glfw3.h>
#include <bits/stdc++.h>
#include <windows.h>

#define WIDTH 750.f
#define HEIGHT 750.f
#define length 0.02f
#define gap 20
#define pdd pair<double, double>
const int delay = 1; //填充延迟
using namespace std;
struct edge {
    double x;     // 下端点的x坐标
    double delta; //斜率倒数
    double ymax;  //上端点y坐标
    double ymin;  //下端点y坐标
    edge *next;
};

vector<pdd> picture;         //初始图形的顶点
vector<pdd> pictureAfterCut; //经过切割后图形的顶点
vector<pdd> rectangle;       //矩形边框的两个顶点

bool finished = false;
double changex(double x) {
    return x / WIDTH * 2 - 1; //将像素坐标转换为标准化设备坐标
}
double changey(double y) {
    return 1 - y / HEIGHT * 2;
}

void showPicture() {       //绘制多边形
    glBegin(GL_LINE_LOOP); // 多边形
    glColor3f(1.f, 0.f, 0.f);
    for (auto it : picture) {
        double x = it.first, y = it.second;
        glVertex3f(changex(x), changey(y), 0.0f);
    }
    glEnd(); // 多边形绘制结束
}
void showCutPicture() {    //绘制切割好的多边形
    glBegin(GL_LINE_LOOP); // 多边形
    glColor3f(0.f, 1.f, 0.f);
    for (auto it : pictureAfterCut) {
        double x = it.first, y = it.second;
        glVertex3f(changex(x), changey(y), 0.0f);
    }
    glEnd(); // 多边形绘制结束
}
void showRectangle() { //绘制矩形,红色
    glBegin(GL_LINES); // 直线
    for (int i = 0; i < (int)rectangle.size() - 1 && (int)rectangle.size() % 2 == 0; i += 2) {
        glColor3f(0.f, 0.f, 1.f);
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
}
bool isInRectangle(double x, double y, int index) {          //某点是否在矩形index号边的一侧
    double x1 = rectangle[0].first, x2 = rectangle[1].first; //矩形信息
    double y1 = rectangle[0].second, y2 = rectangle[1].second;
    if ((index == 0 && y >= y1) || (index == 1 && x <= x2) || (index == 2 && y <= y2) || (index == 3 && x >= x1)) {
        return true;
    }
    return false;
}
pdd getPublicPoint(pdd startPoint, pdd endPoint, int index) { //求Point和矩形第index条边的交点
    double x1 = rectangle[0].first, x2 = rectangle[1].first;  //矩形信息
    double y1 = rectangle[0].second, y2 = rectangle[1].second;
    pdd res;
    if (index == 0) { //上
        res.second = y1;
        res.first = startPoint.first + (y1 - startPoint.second) * (endPoint.first - startPoint.first) / (endPoint.second - startPoint.second);
    } else if (index == 1) { //右
        res.first = x2;
        res.second = startPoint.second + (x2 - startPoint.first) * (endPoint.second - startPoint.second) / (endPoint.first - startPoint.first);
    } else if (index == 2) { //下
        res.second = y2;
        res.first = startPoint.first + (y2 - startPoint.second) * (endPoint.first - startPoint.first) / (endPoint.second - startPoint.second);
    } else if (index == 3) { //左
        res.first = x1;
        res.second = startPoint.second + (x1 - startPoint.first) * (endPoint.second - startPoint.second) / (endPoint.first - startPoint.first);
    }
    return res;
}
void cutPicture() {//切割多边形
    double x1 = rectangle[0].first, x2 = rectangle[1].first; //矩形信息
    double y1 = rectangle[0].second, y2 = rectangle[1].second;

    vector<pdd> outputR, inputR = picture; //裁剪四次,每一次输出的结果重新输入,进行裁剪
    /*矩形编号
        0
    3        1
        2
    */
    for (int i = 0; i <= 3; i++) {         //矩形边框,一共需要裁剪4条边,顺时针从上编号
        pdd lastPoint = inputR.back();//最后一个点和第一个点组成最开始的向量
        for (auto cur : inputR) {                                           //尝试处理多边形的每一个顶点,把这个点视作向量终点
            if (isInRectangle(cur.first, cur.second, i)) {                  //如果终点在矩形里边
                if (!isInRectangle(lastPoint.first, lastPoint.second, i)) { //如果起点不在里面,需要计算交点,并放入
                    outputR.push_back(getPublicPoint(cur, lastPoint, i));
                }
                outputR.push_back(cur);                                    //放入当前点
            } else {                                                       //终点不在矩形之内
                if (isInRectangle(lastPoint.first, lastPoint.second, i)) { //如果起点在里面,需要计算交点,并放入
                    outputR.push_back(getPublicPoint(cur, lastPoint, i));
                }
            }
            lastPoint = cur;
        }
        inputR = outputR;//更新操作
        outputR.clear();
    }
    pictureAfterCut = inputR;
}
// mouse button callback
void releaseLeftButton(GLFWwindow *window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) { //释放了鼠标左键,添加多边形
        double x, y;
        glfwGetCursorPos(window, &x, &y);
        cout << "当前位置 " << x << "," << y << std::endl;
        picture.push_back({x, y});
    }
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE && rectangle.size() < 2) { //释放了鼠标右键，添加矩形
        double xposition, yposition;
        glfwGetCursorPos(window, &xposition, &yposition);
        cout << xposition << "," << yposition << endl;
        rectangle.push_back({xposition, yposition}); //添加矩形
    }
}

void pressKeyBoard(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ENTER && action == GLFW_PRESS) { //回车确定当前图形
        cout << "开始填充" << endl;
        cutPicture();
        finished = true;
    }
    if (key == GLFW_KEY_DELETE && action == GLFW_PRESS) { //删除全部
        cout << "删除全部顶点" << endl;
        picture.clear();
        rectangle.clear();
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
        showPicture();   // 展示多边形
        showRectangle(); //展示矩形
        if (finished) {  //展示切割结果
            showCutPicture();
        }

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
