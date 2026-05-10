/*
NES模拟器使用C++实现
*/
#include "public/test_runner.h"
#include "public/snake/snake.h"

int main()
{
    // todo: 增加op码和助记符的测试文件
    run_all_tests();
    test_snake();
}
