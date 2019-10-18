#include "ModArith.hpp"

int main() {
    typedef ModArith<998244353, U32, U64> MA;
    return (int) MA::Div(100000000, 10000);
}
