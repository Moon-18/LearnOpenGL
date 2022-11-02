//按鼠标左键确定一系列点,按回车进行填充,按delete擦除全部图形
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

bool isInRectangle(double x, double y, int index) {          //某点是否在index 矩形一侧,更强的限制条件!
    double x1 = rectangle[0].first, x2 = rectangle[1].first; //矩形信息
    double y1 = rectangle[0].second, y2 = rectangle[1].second;
    if ((index == 0 && y >= y1) || (index == 1 && x <= x2) || (index == 2 && y <= y2) || (index == 3 && x >= y1)) {
        return true;
    }
    return false;
}
pdd getPublicPoint(pdd startPoint, pdd endPoint, int index) { //求和矩形第index条边的交点
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
void cutPicture() {
    double x1 = rectangle[0].first, x2 = rectangle[1].first; //矩形信息
    double y1 = rectangle[0].second, y2 = rectangle[1].second;
    vector<pdd> outputR, inputR = picture; //裁剪四次
    for (int i = 0; i <= 3; i++) {         //矩形边框,一共需要裁剪4条边,顺时针编号
        pdd lastPoint = inputR.back();
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
            inputR = outputR;
            outputR.clear();
        }
    }
    pictureAfterCut = inputR;
}
int main(void) {
    rectangle.push_back({1, 1});
    rectangle.push_back({3, 3});
    picture.push_back({2, 0});
    picture.push_back({4, 2});
    picture.push_back({2, 4});
    cutPicture();
    return 0;
}
