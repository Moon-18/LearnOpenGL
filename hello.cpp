#include <cstdlib>
#include <ctime>
#include <iostream>
#define randomN (float)(rand() % 256)
using namespace std;

int main() {
    srand((int)time(NULL)); // 产生随机种子  把0换成NULL也行
    for (int i = 0; i < 10; i++) {
        cout << randomN << " ";
    }
    return 0;
}