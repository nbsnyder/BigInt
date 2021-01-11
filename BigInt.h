#ifndef BIGINT_H
#define BIGINT_H

#include <iostream>

class BigInt {
    //typedef std::variant<const BigInt, int> _bi_types;
public:
    BigInt();
    BigInt(int a);
    BigInt(const BigInt& otherBigInt);
    ~BigInt();
    BigInt& operator+=(const BigInt& otherBigInt);
    BigInt& operator+=(int otherInt);
    BigInt& operator*=(int otherInt);
    int size() const;
    void print() const;
private:
    int maxSize;
    bool *bits;
    void resize();
};

BigInt::BigInt() : maxSize(8) {
    bits = new bool[maxSize];
    for (int i = 0; i < maxSize; i++)
        bits[i] = 0;
}

BigInt::BigInt(int a) : maxSize(8) {
    bits = new bool[maxSize];
    bool neg = 0;
    int i = 0;

    if (a < 0) {
        neg = 1;
        a *= -1;
    }

    bits[i++] = neg;

    while ((i < maxSize) && (a > 0)) {
        bits[i] = (a & 1) ^ neg;
        a >>= 1;
        i++;
    }

    while (i < maxSize) {
        bits[i] = neg;
        i++;
    }

    if (neg) *this = this->operator+=(1);
}

BigInt::BigInt(const BigInt& otherBigInt) {
    this->maxSize = otherBigInt.maxSize;
    this->bits = new bool[maxSize];
    for (int i = 0; i < maxSize; i++) {
        this->bits[i] = otherBigInt.bits[i];
    }
}

BigInt::~BigInt() {
    delete[] bits;
}

// double the size of the bigint
void BigInt::resize() {
    bool *newBits = new bool[maxSize * 2];
    int i = 0;

    while (i < maxSize) {
        newBits[i] = bits[i];
        i++;
    }

    while (i < (maxSize * 2)) {
        newBits[i] = bits[0];
        i++;
    }

    maxSize *= 2;
    delete[] bits;
    bits = newBits;
}

// returns size of the BigInt in bits
int BigInt::size() const {
    return maxSize;
}

BigInt& BigInt::operator+=(const BigInt& otherBigInt) {
    bool bit1, bit2, carry = 0;
    int i = 1;

    while (otherBigInt.size() > maxSize) {
        std::cout << "Size was " << maxSize;
        resize();
        std::cout << ". Other one was bigger, so now it's " << maxSize << "\n";
    }

    for (int i = maxSize - 2; i < maxSize; i++) {
        if (bits[i]) {
            resize();
            break;
        }
    }

    while (i <= maxSize) {
        if ((i == maxSize) || (i >= otherBigInt.size())) i = 0;
        bit1 = bits[i];
        bit2 = otherBigInt.bits[i];
        bits[i] = (bit1 ^ bit2 ^ carry);
        carry = (bit1 & bit2) | (bit1 & carry) | (bit2 & carry);
        if (i == 0) break;
        i++;
    }

    return *this;
}

BigInt& BigInt::operator+=(int otherInt) {
    return operator+=(BigInt(otherInt));
}

BigInt& BigInt::operator*=(int otherInt) {
    BigInt temp(*this);
    for (int i = 1; i < otherInt; i++) {
        *this = this->operator+=(temp);
    }
    return *this;
}

void BigInt::print() const {
    std::cout << bits[0];
    for (int i = (maxSize - 1); i > 0; i--) {
        std::cout << bits[i];
    }
    std::cout << "\n";
}

#endif