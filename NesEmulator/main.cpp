/*
NES模拟器使用C++实现
*/

void test_asl();
void test_adc();
void test_and();
void test_bcc();
void test_bcs();
void test_beq();
void test_bit();
void test_bmi();
void test_bne();
void test_bpl();
void test_brk();
void test_bvc();
void test_bvs();
void test_clc();
void test_cld();
void test_cli();
void test_clv();
void test_cmp();
void test_cpx();
void test_cpy();
void test_dec();
void test_dex();
void test_dey();

int main()
{
    test_adc();
    test_asl();
    test_and();
    test_bcc();
    test_bcs();
    test_beq();
    test_bit();
    test_bmi();
    test_bne();
    test_bpl();
    test_brk();
    test_bvc();
    test_bvs();
    test_clc();
    // todo: 增加op码和助记符的测试文件
    test_cld();
    test_cli();
    test_clv();
    test_cmp();
    test_cpx();
    test_cpy();
    test_dec();
    test_dex();
    test_dey();
}
