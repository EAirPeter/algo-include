#include <algorithm>
#include <cstdint>
#include <functional>
#include <iostream>
#include <numeric>
#include <string>

using namespace std;

struct BigInt {
    static constexpr size_t Len = 6;
    static constexpr size_t Size = Len * sizeof(uint64_t);
    uint64_t Val[Len];
    BigInt() = default;
    BigInt(const BigInt&) = default;
    BigInt(uint32_t val) : Val{val} {}
    BigInt(const string& Dec) : Val{} {
        for (auto ch : Dec)
            *this = *this * 10 + (ch & 0x0f);
    }
    explicit operator string() const {
        string res;
        auto tmp = *this;
        while (tmp) {
            res += (char) ((tmp % 10).Val[0] | '0');
            tmp /= 10;
        }
        if (res.empty())
            res += '0';
        else
            reverse(res.begin(), res.end());
        return res;
    }
    explicit operator bool() const {
        return find_if(Val, Val + Len, [](uint64_t val) { return !!val; }) != Val + Len;
    }
    BigInt operator ~() const {
        BigInt res;
        transform(Val, Val + Len, res.Val, [](uint64_t val) { return ~val; });
        return res;
    }
    BigInt operator -() const {
        return ~*this + 1;
    }
    bool operator <(const BigInt& rhs) const {
        for (size_t i = Len; i--; )
            if (Val[i] != rhs.Val[i])
                return Val[i] < rhs.Val[i];
        return false;
    }
    bool operator >(const BigInt& rhs) const {
        return rhs < *this;
    }
    bool operator <=(const BigInt& rhs) const {
        return !(*this > rhs);
    }
    bool operator >=(const BigInt& rhs) const {
        return !(*this < rhs);
    }
    bool operator ==(const BigInt& rhs) const {
        return equal(Val, Val + Len, rhs.Val, rhs.Val + Len);
    }
    bool operator !=(const BigInt& rhs) const {
        return !(*this == rhs);
    }
    BigInt& operator <<=(size_t cnt) {
        if (auto mov = cnt >> 6) {
            copy_backward(Val, Val + Len - mov, Val + Len);
            fill(Val, Val + mov, 0);
        }
        if (auto vom = cnt & 0x2f) {
            Val[Len - 1] <<= vom;
            for (size_t i = Len - 1; i--; ) {
                Val[i + 1] |= Val[i] >> (64 - vom);
                Val[i] <<= vom;
            }
        }
        return *this;
    }
    BigInt& operator +=(const BigInt& rhs) {
        return *this = *this + rhs;
    }
    BigInt& operator -=(const BigInt& rhs) {
        return *this = *this - rhs;
    }
    BigInt& operator *=(const BigInt& rhs) {
        return *this = *this * rhs;
    }
    BigInt& operator /=(const BigInt& rhs) {
        return *this = *this / rhs;
    }
    BigInt& operator %=(const BigInt& rhs) {
        return *this = *this % rhs;
    }
    BigInt& operator >>=(size_t cnt) {
        if (auto mov = cnt >> 6) {
            copy(Val + mov, Val + Len, Val);
            fill(Val + Len - mov, Val + Len, 0);
        }
        if (auto vom = cnt & 0x2f) {
            Val[0] >>= vom;
            for (size_t i = 1; i < Len; ++i) {
                Val[i - 1] |= Val[i] << (64 - vom);
                Val[i] >>= vom;
            }
        }
        return *this;
    }
    BigInt operator +(const BigInt& rhs) const {
        BigInt res;
        bool c = false;
        for (size_t i = 0; i < Len; ++i) {
            auto val = Val[i] + rhs.Val[i] + c;
            c = val < Val[i] || val < rhs.Val[i];
            res.Val[i] = val;
        }
        return res;
    }
    BigInt operator -(const BigInt& rhs) const {
        return *this + -rhs;
    }
    BigInt operator *(const BigInt& rhs) const {
        uint64_t tmp[Size]{};
        for (size_t i = 0; i < Size; ++i)
            for (size_t j = 0; i + j < Size; ++j)
                tmp[i + j] += (uint32_t) ((const uint8_t*) Val)[i] * ((const uint8_t*) rhs.Val)[j];
        uint64_t c = 0;
        for (size_t i = 0; i < Size; ++i) {
            tmp[i] += c;
            c = tmp[i] >> 8;
        }
        BigInt res;
        for (size_t i = 0; i < Len; ++i) {
            uint64_t val = 0;
            for (size_t j = 0; j < sizeof(uint64_t); ++j)
                val |= (uint64_t) (uint8_t) tmp[i * sizeof(uint64_t) + j] << (j * 8);
            res.Val[i] = val;
        }
        return res;
    }
    BigInt operator /(const BigInt& rhs) const {
        BigInt Q, R;
        DivRem(Q, R, rhs);
        return Q;
    }
    BigInt operator %(const BigInt& rhs) const {
        BigInt Q, R;
        DivRem(Q, R, rhs);
        return R;
    }
    BigInt operator <<(size_t idx) const {
        BigInt tmp{*this};
        return tmp <<= idx;
    }
    BigInt operator >>(size_t idx) const {
        BigInt tmp{*this};
        return tmp >>= idx;
    }
    void DivRem(BigInt& Q, BigInt& R, const BigInt& D) const {
        Q = 0;
        R = 0;
        for (size_t i = Len; i--; ) {
            for (size_t j = 64; j--; ) {
                R <<= 1;
                R.Val[0] |= Val[i] >> j & 1;
                if (R >= D) {
                    R -= D;
                    Q.Val[i] |= uint64_t{1} << j;
                }
            }
        }
    }
};

template<class Int>
Int ISqrt(Int val) {
    Int res = 0;
    Int bit = (Int) 1 << ((sizeof(Int) << 3) - 2);
    while (bit > val)
        bit >>= 2;
    while (bit) {
        if (val >= res + bit) {
            val -= res + bit;
            res += bit << 1;
        }
        res >>= 1;
        bit >>= 2;
    }
    return res;
}

string integer_square_root(string n) {
    return (string) ISqrt(BigInt{n});
}

int main() {
    cout << integer_square_root("10000000000000000000000000000") << endl;
    cout << ISqrt(100u) << endl;
}
