//use:按鼠标左键确定一系列点,按回车进行填充,按delete擦除全部图形
#include <GLFW/glfw3.h>
#include <bits/stdc++.h>
#include <windows.h>

#define WIDTH 750.f
#define HEIGHT 750.f
#define length 0.02f
#define gap 20
#define pdd pair<double, double>
using namespace std;
struct edge {
    double x;     // 下端点的x坐标
    double delta; //斜率倒数
    double ymax;  //上端点y坐标
    double ymin;  //下端点y坐标
    edge *next;
};
bool cmpEdge(edge e1, edge e2) { //先比较最低点高度,再比较最低点的横坐标,再比较斜率
    // return e1.ymin == e2.ymin ? e1.x < e2.x : e1.ymin < e2.ymin;
    if (e1.ymin < e2.ymin) {
        return true;
    } else if (e1.ymin > e2.ymin) {
        return false;
    }
    if (e1.x < e2.x) {
        return true;
    } else if (e1.x > e2.x) {
        return false;
    }
    if (e1.delta * e2.delta > 0 && e1.delta < 0) { //同为负
        return e1.delta < e2.delta;
    } else if (e1.delta * e2.delta > 0 && e1.delta > 0) { //同为正
        return e1.delta > e2.delta;
    } else { //异号,负的放前
        return e1.delta < 0 ? true : false;
    }
}
vector<pdd> picture; //图形的顶点
bool finished = false;
void showLine() {
    glBegin(GL_LINE_LOOP); // 矩形
    glColor3f(1.f, 1.f, 1.f);
    for (auto it : picture) {
        double x = it.first, y = it.second;
        x = x / WIDTH * 2 - 1; //将像素坐标转换为标准化设备坐标
        y = 1 - y / HEIGHT * 2;
        glVertex3f(x, y, 0.0f);
    }
    glEnd(); // 矩形绘制结束
}
void drawYLine(double y, double x1, double x2) { //在特定高度y上绘制从x1到x2的一条直线
    glBegin(GL_LINES);                           // 直线
    glColor3f(1.f, 1.f, 1.f);
    // glLineWidth(5.0f);
    x1 = x1 / WIDTH * 2 - 1; //将像素坐标转换为标准化设备坐标
    x2 = x2 / WIDTH * 2 - 1;
    y = 1 - y / HEIGHT * 2;
    glVertex3f(x1, y, 0.0f);
    glVertex3f(x2, y, 0.0f);
    glEnd(); // 矩形绘制结束
}
void fillPicture() {
    deque<edge> net, aet;
    // 1.计算出所有的直线
    int n = picture.size();
    for (int i = 0; i < n; i++) {
        pdd p1 = picture[i % n], p2 = picture[(i + 1) % n]; //保证最后一个点和第一个点也参与
        if (p2.second < p1.second) {
            swap(p1, p2); //让p1是y较小的点
        }
        edge e;
        e.x = p1.first, e.ymin = p1.second, e.ymax = p2.second;    //加1保证交点处断开1个像素,出现两个点
        e.delta = (p2.first - p1.first) / (p2.second - p1.second); //斜率倒数
        net.push_back(e);
    }
    sort(net.begin(), net.end(), cmpEdge);
    //开始逐高度填充
    double curY = net.front().ymin;
    while (!net.empty() or !aet.empty()) {

        //删去AET中满足y=ymax的边
        for (auto it = aet.begin(); it != aet.end(); it++) {
            if (it->ymax == curY) {
                it = aet.erase(it);
                it--;
            }
        }

        //构造当前的活性边表
        bool isInsert = false;
        for (auto it = net.begin(); it != net.end(); it++) {
            if (it->ymin == curY) {
                edge tmp = *it;
                aet.push_back(tmp);
                it = net.erase(it);
                it--;
                isInsert = true;
            }
        }
        if (isInsert) { //只有加入新的边时才更新顺序
            sort(aet.begin(), aet.end(), cmpEdge);
        }
        //对于当前的活性边表,两两配对,画线
        for (int i = 0; i < aet.size(); i += 2) {
            drawYLine(curY, aet[i].x, aet[i + 1].x);
        }
        // y = y + 1，扫描线上移一像素。
        curY++;
        //对于AET中所有边，更新x = x + ∆x ymin
        for (auto &it : aet) {
            it.x += it.delta;
            it.ymin = curY;
        }
    }
}
// mouse button callback
void releaseLeftButton(GLFWwindow *window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) { //释放了鼠标左键
        double x, y;
        glfwGetCursorPos(window, &x, &y);
        cout << "当前位置 " << x << "," << y << std::endl;
        picture.push_back({x, y});
    }
}

void pressKeyBoard(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ENTER && action == GLFW_PRESS) { //回车确定当前图形
        cout << "开始填充" << endl;
        finished = true;
    }
    if (key == GLFW_KEY_DELETE && action == GLFW_PRESS) { //删除全部
        cout << "删除全部顶点并取消填充" << endl;
        picture.clear();
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
        showLine(); // 展示全部线
        if (finished) {
            fillPicture(); //进行填充
        }

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
