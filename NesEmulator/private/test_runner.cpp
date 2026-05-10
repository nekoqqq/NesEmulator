#include <functional>
#include <iostream>
#include <vector>
#include "../public/test_runner.h"

// todo 这个宏展开看不懂
#define TEST_LIST \
    X(adc) X(asl) X(and) X(bcc) X(bcs) X(beq) X(bit) X(bmi) X(bne) X(bpl) \
    X(brk) X(bvc) X(bvs) X(clc) X(cld) X(cli) X(clv) X(cmp) X(cpx) X(cpy) \
    X(dec) X(dex) X(dey) X(eor) X(inc) X(inx) X(iny) X(jmp) X(jsr) X(lda) \
    X(ldx) X(ldy) X(lsr) X(nop) X(ora) X(pha) X(php) X(pla) X(plp) X(rol) \
    X(ror) X(rti) X(rts) X(sbc) X(sec) X(sed) X(sei) X(sta) X(stx) X(sty) \
    X(tax) X(tay)

#define X(func) void test_##func();
TEST_LIST
#undef X


bool test_op_codes()
{
    std::vector<std::pair<const char*, std::function<void()>>> tests = {
#define X(func) {#func, test_##func},
        TEST_LIST
#undef X
    };

    int passed = 0;
    for (const auto& test : tests)
    {
        const char* name = test.first;
        std::function < void() > func = test.second;
        std::cout << "Running " << name << "... \n";
        try
        {
            func();
            std::cout << "OK\n";
            ++passed;
        }
        catch (...)
        {
            std::cout << "FAILED\n";
            return false;
        }
    }
    std::cout << "\nAll " << passed << " test suites passed.\n";
    return true;
}
