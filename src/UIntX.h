//
// Created by Aubrey on 2/29/2024.
//
#ifndef UINTX_H
#define UINTX_H

#include <algorithm>
#include <array>
#include <cassert>
#include <cstdint>
#include <string>

template<uint32_t NumBits>
class UIntX
{
    public:
        UIntX();
        UIntX(uint64_t);
        template <uint32_t OtherNumBits>
        UIntX(const UIntX<OtherNumBits> &);

        std::string toString() const;
        
        template <uint32_t OtherNumBits>
        UIntX<NumBits> add(const UIntX<OtherNumBits> &) const;
        template <uint32_t OtherNumBits>
        UIntX<NumBits> sub(const UIntX<OtherNumBits> &) const;
        template <uint32_t OtherNumBits>
        UIntX<NumBits> mult(const UIntX<OtherNumBits> &) const;
        template <uint32_t OtherNumBits>
        UIntX<NumBits> div(const UIntX<OtherNumBits> &) const;
        template <uint32_t OtherNumBits>
        UIntX<NumBits> mod(const UIntX<OtherNumBits> &) const;
        UIntX<NumBits> sqrt() const;

        template <uint32_t OtherNumBits>
        bool isGreater(const UIntX<OtherNumBits> &) const;
        template <uint32_t OtherNumBits>
        bool isEqual(const UIntX<OtherNumBits> &) const;
        template <uint32_t OtherNumBits>
        bool isLess(const UIntX<OtherNumBits> &) const;

        bool isOdd() const;
        size_t getArrSize() const;
        uint64_t getElement(uint64_t) const;
        void setElement(uint32_t, uint64_t);

    private:
        static const uint8_t BITS_PER_ELMT = 64;
        std::array<uint64_t, NumBits / BITS_PER_ELMT> data;

        enum divmodChoice { divide, modulo };
        template <uint32_t OtherNumBits>
        UIntX<NumBits> divmod(const UIntX<OtherNumBits> &, divmodChoice) const;
        bool is_pow_two(uint64_t) const;
};

template<uint32_t NumBits>
UIntX<NumBits>::UIntX()
{
    assert(NumBits >= BITS_PER_ELMT && is_pow_two(NumBits));
    data.fill(0);
}

template<uint32_t NumBits>
UIntX<NumBits>::UIntX(const uint64_t num)
{
    assert(NumBits >= BITS_PER_ELMT && is_pow_two(NumBits));
    data.fill(0);
    data[0] = num;
}

template<uint32_t NumBits>
template <uint32_t OtherNumBits>
UIntX<NumBits>::UIntX(const UIntX<OtherNumBits> &other)
{
    assert(NumBits >= BITS_PER_ELMT && is_pow_two(NumBits));
    data.fill(0);
    for (size_t i=0; i < std::min(data.size(), other.getArrSize()); i++)
    {
        data[i] = other.getElement(i);
    }
}

/*
template <uint32_t NumBits>
std::string UIntX<NumBits>::toString() const
{
    // Implement double dabble
}
*/

template<uint32_t NumBits>
template <uint32_t OtherNumBits>
UIntX<NumBits> UIntX<NumBits>::add(const UIntX<OtherNumBits> &other) const
{
    UIntX<NumBits> sum;
    bool carryIn, carryOut = false;
    uint64_t diff_from_max;
    uint64_t minArrSize = std::min(data.size(), other.getArrSize());

    size_t i = 0;
    for (; i < minArrSize; i++)
    {
        carryIn = carryOut;
        diff_from_max = UINT64_MAX - data[i];
        carryOut = diff_from_max == other.getElement(i) ? carryIn : diff_from_max < other.getElement(i);
        sum.data[i] = data[i] + carryIn + other.getElement(i);
    }

    if (data.size() > minArrSize && carryOut)
    {
        sum.data[i] += carryOut;
    }

    return sum;
}


template<uint32_t NumBits>
template <uint32_t OtherNumBits>
UIntX<NumBits> UIntX<NumBits>::sub(const UIntX<OtherNumBits> &other) const
{
    UIntX<NumBits> diff;
    bool carryIn, carryOut = false;
    uint64_t minArrSize = std::min(data.size(), other.getArrSize());

    size_t i = 0;
    for (; i < minArrSize; i++)
    {
        carryIn = carryOut;
        carryOut = data[i] == other.getElement(i) ? carryIn : data[i] < other.getElement(i);
        diff.data[i] = data[i] - carryIn - other.getElement(i);
    }

    if (data.size() > minArrSize && carryOut)
    {
        diff.data[i] -= carryOut;
    }

    return diff;
}

template<uint32_t NumBits>
template <uint32_t OtherNumBits>
UIntX<NumBits> UIntX<NumBits>::mult(const UIntX<OtherNumBits> &other) const
{
    const uint8_t SHFT_AMT = BITS_PER_ELMT / 2;
    const uint64_t BASE = uint64_t(1) << SHFT_AMT; // BASE == 2^SHFT_AMT

    UIntX<NumBits> a; // accumulator
    uint64_t carry = 0,
             this_data_temp,
             other_data_temp,
             a_data_temp,
             cur;

    for (uint64_t i=0; i < data.size() * 2; i++)
    {
        for (uint64_t j=0; j < other.data.size() * 2; j++)
        {
            if ((i+j) < a.data.size()*2)
            {
                this_data_temp = i%2 ? data[i/2] >> SHFT_AMT : data[i/2] & BASE-1;
                other_data_temp = j%2 ? other.data[j/2] >> SHFT_AMT : other.data[j/2] & BASE-1;
                a_data_temp = (i+j)%2 ? a.data[(i+j)/2] >> SHFT_AMT : a.data[(i+j)/2] & BASE-1;

                cur = this_data_temp * other_data_temp + carry;
                a.data[(i+j)/2] += (i+j)%2 ? cur%BASE << SHFT_AMT : cur%BASE;
                carry = (cur + a_data_temp) / BASE;
            }
        }
    }

    return a;
}

template<uint32_t NumBits>
template <uint32_t OtherNumBits>
UIntX<NumBits> UIntX<NumBits>::div(const UIntX<OtherNumBits> &other) const
{
    return this->divmod(other, divide);
}

template<uint32_t NumBits>
template <uint32_t OtherNumBits>
UIntX<NumBits> UIntX<NumBits>::mod(const UIntX<OtherNumBits> &other) const
{
    return this->divmod(other, modulo);
}

template<uint32_t NumBits>
template <uint32_t OtherNumBits>
UIntX<NumBits> UIntX<NumBits>::divmod(const UIntX<OtherNumBits> &divisor, divmodChoice choice) const
{
    // *this / divisor = q, r
    static const uint64_t BASE = uint64_t(1) << 32; // BASE == 2^32
    UIntX<NumBits> q, r;

    if (divisor.isEqual(UIntX<NumBits>(0)) || this->isEqual(UIntX<NumBits>(0)))
    {
        q = 0;
        r = 0;
    }
    if (this->isLess(divisor))
    {
        q = 0;
        r = divisor;
    }
    else if (divisor.isLess(UIntX<NumBits>(BASE)))
    {
        q = *this;
        uint64_t cur, carry = 0;

        for (int64_t i = data.size()-1; i>=0; i--)
        {
            cur = q.data[i] + carry * BASE;
            q.data[i] = cur / divisor.getElement(0);
            carry = cur % divisor.getElement(0);
        }
        r = carry;
    }
    else
    {
        const uint8_t SHFT_AMT = BITS_PER_ELMT / 2;

        uint64_t msd = 0;
        for (int64_t i = divisor.getArrSize()-1; msd == 0 && i >= 0; i--)
        {
            msd = divisor.getElement(i);
        }
        msd = msd < BASE ? msd : msd >> SHFT_AMT;
        q = *this;

        while (r > divisor)
        {
            q = q.div(UIntX<NumBits>(msd), divide);
        }

    }

    switch (choice)
    {
        case divide:
            return q;
        case modulo:
            return r;
        default:
            return 0;
    }
}

/*
template <uint32_t NumBits>
UIntX<NumBits> UIntX<NumBits>::sqrt() const
{

}
*/

template<uint32_t NumBits>
template <uint32_t OtherNumBits>
bool UIntX<NumBits>::isGreater(const UIntX<OtherNumBits> &other) const
{
    if (data.size() > other.getArrSize())
    {
        for (size_t i = data.size() - 1; i > other.getArrSize() - 1; i--)
        {
            if (data[i] > 0)
            {
                return true;
            }
        }
    }
    else if (data.size() < other.getArrSize())
    {
        for (size_t i = other.getArrSize() - 1; i > data.size() - 1; i--)
        {
            if (other.getElement(i) > 0)
            {
                return false;
            }
        }
    }

    for (int64_t i = std::min(data.size(), other.getArrSize()) - 1; i >= 0; i--)
    {
        if (data[i] > other.getElement(i))
        {
            return true;
        }
    }

    return false;
}

template<uint32_t NumBits>
template <uint32_t OtherNumBits>
bool UIntX<NumBits>::isEqual(const UIntX<OtherNumBits> &other) const
{
    return !(this->isGreater(other) || other.isGreater(*this));
}

template<uint32_t NumBits>
template <uint32_t OtherNumBits>
bool UIntX<NumBits>::isLess(const UIntX<OtherNumBits> &other) const
{
    return other.isGreater(*this);
}

template <uint32_t NumBits>
bool UIntX<NumBits>::isOdd() const
{
    return data[0] & uint64_t(1);
}

template <uint32_t NumBits>
size_t UIntX<NumBits>::getArrSize() const
{
    return data.size();
}

template <uint32_t NumBits>
uint64_t UIntX<NumBits>::getElement(uint64_t index) const
{
    assert(index < data.size());
    return data[index];
}

template <uint32_t NumBits>
void UIntX<NumBits>::setElement(uint32_t index, uint64_t value)
{
    assert(index < data.size());
    data[index] = value;
}

template <uint32_t NumBits>
bool UIntX<NumBits>::is_pow_two(uint64_t n) const
{
    // Based on:
    // https://www.educative.io/answers/how-to-check-if-a-number-is-a-power-of-2-in-cpp
    return !(n & (n-1));
}

#endif // UINTX_H