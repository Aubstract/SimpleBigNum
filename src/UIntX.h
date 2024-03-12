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

#define DEBUG

template<uint32_t N>
class UIntX
{
    public:
        UIntX();
        UIntX(uint64_t);
        template <uint32_t M>
        UIntX(const UIntX<M> &);

        std::string toString() const;
        
        template <uint32_t M>
        UIntX<N> add(const UIntX<M> &) const;
        template <uint32_t M>
        UIntX<N> sub(const UIntX<M> &) const;
        template <uint32_t M>
        UIntX<N*2> mult(const UIntX<M> &) const;
        template <uint32_t M>
        UIntX<N> div(const UIntX<M> &) const;
        template <uint32_t M>
        UIntX<N> mod(const UIntX<M> &) const;
        UIntX<N> sqrt() const;

        UIntX<N> rightShift(uint32_t) const;
        UIntX<N> leftShift(uint32_t) const;

        template <uint32_t M>
        bool isGreater(const UIntX<M> &) const;
        template <uint32_t M>
        bool isEqual(const UIntX<M> &) const;
        template <uint32_t M>
        bool isLess(const UIntX<M> &) const;
        template <uint32_t M>
        bool isGreaterOrEqual(const UIntX<M> &) const;
        template <uint32_t M>
        bool isLessOrEqual(const UIntX<M> &) const;
        template <uint32_t M>
        bool isNotEqual(const UIntX<M> &) const;
        bool isOdd() const;

        std::size_t getArrSize() const;
        uint64_t getElement(uint64_t) const;
        void setElement(uint32_t, uint64_t);

    private:
        static const uint8_t BITS_PER_ELEMENT = 64;
        std::array<uint64_t, N / BITS_PER_ELEMENT> data;

        int64_t getHighestBitIndex() const;
        uint8_t getByte(int64_t) const;
        uint8_t getNibble(uint32_t) const;
        void setNibble(uint32_t, uint8_t);

        bool isPowTwo(uint64_t) const;
};

template<uint32_t N>
UIntX<N>::UIntX()
{
    assert(N >= BITS_PER_ELEMENT*2 && isPowTwo(N));
    data.fill(0);
}

template<uint32_t N>
UIntX<N>::UIntX(const uint64_t num)
{
    assert(N >= BITS_PER_ELEMENT*2 && isPowTwo(N));
    data.fill(0);
    data[0] = num;
}

template<uint32_t N>
template <uint32_t M>
UIntX<N>::UIntX(const UIntX<M> &other)
{
    assert(N >= BITS_PER_ELEMENT*2 && isPowTwo(N));
    data.fill(0);
    for (std::size_t i=0; i < std::min(data.size(), other.getArrSize()); i++)
    {
        data[i] = other.getElement(i);
    }
}

/*
template <uint32_t N>
std::string UIntX<N>::toString() const
{
    // Implement double dabble
}
*/

template<uint32_t N>
template <uint32_t M>
UIntX<N> UIntX<N>::add(const UIntX<M> &other) const
{
    UIntX<N> sum;
    bool carry_in, carry_out = false;
    uint64_t diff_from_max;
    const uint64_t min_arr_size = std::min(data.size(), other.getArrSize());

    std::size_t i = 0;
    for (; i < min_arr_size; i++)
    {
        carry_in = carry_out;
        diff_from_max = UINT64_MAX - data[i];
        carry_out = diff_from_max == other.getElement(i) ? carry_in : diff_from_max < other.getElement(i);
        sum.data[i] = data[i] + carry_in + other.getElement(i);
    }

    if (data.size() > min_arr_size && carry_out)
    {
        sum.data[i] += carry_out;
    }

    return sum;
}


template<uint32_t N>
template <uint32_t M>
UIntX<N> UIntX<N>::sub(const UIntX<M> &other) const
{
    UIntX<N> diff;
    bool carry_in, carry_out = false;
    const uint64_t min_arr_size = std::min(data.size(), other.getArrSize());

    std::size_t i = 0;
    for (; i < min_arr_size; i++)
    {
        carry_in = carry_out;
        carry_out = data[i] == other.getElement(i) ? carry_in : data[i] < other.getElement(i);
        diff.data[i] = data[i] - carry_in - other.getElement(i);
    }

    if (data.size() > min_arr_size && carry_out)
    {
        diff.data[i] -= carry_out;
    }

    return diff;
}

template<uint32_t N>
template <uint32_t M>
UIntX<N*2> UIntX<N>::mult(const UIntX<M> &other) const
{
    const uint8_t SHIFT_AMT = BITS_PER_ELEMENT / 2;
    const uint64_t BASE = uint64_t(1) << SHIFT_AMT; // BASE == 2^SHIFT_AMT

    UIntX<N*2> a; // accumulator, init to 0
    uint64_t carry = 0,
             this_data_temp,
             other_data_temp,
             a_data_temp,
             cur;

    if (this->isNotEqual(UIntX<N>(0)) && other.isNotEqual(UIntX<N>(0)))
    {
        for (uint64_t i=0; i < data.size() * 2; i++)
        {
            for (uint64_t j=0; j < other.getArrSize() * 2; j++)
            {
                this_data_temp = i%2 ? data[i/2] >> SHIFT_AMT : data[i/2] & BASE-1;
                other_data_temp = j%2 ? other.getElement(j/2) >> SHIFT_AMT : other.getElement(j/2) & BASE-1;
                a_data_temp = (i+j)%2 ? a.getElement((i+j)/2) >> SHIFT_AMT : a.getElement((i+j)/2) & BASE-1;

                cur = this_data_temp * other_data_temp + carry;
                a.setElement((i+j)/2, a.getElement((i+j)/2) + ((i+j)%2 ? cur%BASE << SHIFT_AMT : cur%BASE));
                carry = (cur + a_data_temp) / BASE;
            }
        }
    }

    return a;
}

template<uint32_t N>
template <uint32_t M>
UIntX<N> UIntX<N>::div(const UIntX<M> &divisor) const
{
    assert(divisor.isNotEqual(UIntX<M>(0))); // divide by zero is undefined

    UIntX<N> q;

    if (this->isEqual(UIntX<N>(0)))
    {
        q = 0;
    }
    if (divisor.isGreater(*this))
    {
        q = 0;
    }
    else
    {
        const uint32_t MAX_ITER = 32;
        UIntX<N*2> reciprocal_guess = this->mult(UIntX<N>(2)).sub(divisor).leftShift(128);

        for (uint32_t i=0; i < MAX_ITER; i++)
        {
            reciprocal_guess = reciprocal_guess.sub(reciprocal_guess.mult(UIntX<N>(2).mult(*this).mult(reciprocal_guess)));

            if (UIntX<N*2>(1).sub(UIntX<N*2>(divisor).mult(reciprocal_guess)).isLess(UIntX<N*2>(32)))
            {
                return this->mult(reciprocal_guess);
            }
        }

        return this->mult(reciprocal_guess).rightShift(128);
    }


    return q;
}

template<uint32_t N>
template <uint32_t M>
UIntX<N> UIntX<N>::mod(const UIntX<M> &other) const
{
    if (this->isEqual(UIntX<N>(0)))
    {
        // 0 % n == 0
        return UIntX<N>(0);
    }
    else
    {
        // modulus == dividend - (quotient * divisor)
        return UIntX<N*2>(*this).sub(this->div(other).mult(other));
    }
}

/*
template <uint32_t N>
UIntX<N> UIntX<N>::sqrt() const
{

}
*/

template<uint32_t N>
UIntX<N> UIntX<N>::rightShift(uint32_t shift_amt) const
{
    UIntX<N> shifted = 0;

    if (shift_amt == 0)
    {
        shifted = *this;
    }
    else if (shift_amt < N)
    {
        const uint32_t element_shift = shift_amt / BITS_PER_ELEMENT,
                       bit_shift = shift_amt % BITS_PER_ELEMENT;
        const uint64_t bit_mask = ~(~uint64_t(0) << bit_shift);

        for (uint32_t i=0; i < data.size() - element_shift; i++)
        {
            shifted.data[i] = data[i + element_shift] >> bit_shift;
            if (i + 1 + element_shift < data.size())
            {
                shifted.data[i] |= (data[i + 1 + element_shift] & bit_mask) << (BITS_PER_ELEMENT - bit_shift);
            }
        }
    }
    return shifted;
}

template<uint32_t N>
UIntX<N> UIntX<N>::leftShift(uint32_t shift_amt) const
{
    UIntX<N> shifted = 0;

    if (shift_amt == 0)
    {
        shifted = *this;
    }
    else if (shift_amt < N)
    {
        const uint32_t element_shift = shift_amt / BITS_PER_ELEMENT,
                       bit_shift = shift_amt % BITS_PER_ELEMENT;
        const uint64_t bit_mask = ~(~uint64_t(0) >> bit_shift);

        for (int64_t i = data.size(); i > element_shift; i--)
        {
            shifted.data[i - 1] = data[i - 1 - element_shift] << bit_shift;
            if (i - 1 - element_shift > 0)
            {
                shifted.data[i - 1] |= (data[i - 2 - element_shift] & bit_mask) >> (BITS_PER_ELEMENT - bit_shift);
            }
        }
    }
    return shifted;
}

template<uint32_t N>
template <uint32_t M>
bool UIntX<N>::isGreater(const UIntX<M> &other) const
{
    if (data.size() > other.getArrSize())
    {
        for (std::size_t i = data.size() - 1; i >= other.getArrSize(); i--)
        {
            if (data[i] > 0)
            {
                return true;
            }
        }
    }
    else if (data.size() < other.getArrSize())
    {
        for (std::size_t i = other.getArrSize() - 1; i >= data.size(); i--)
        {
            if (other.getElement(i) > 0)
            {
                return false;
            }
        }
    }

    for (int64_t i = std::min(data.size(), other.getArrSize()) - 1; i >= 0; i--)
    {
        if (data[i] < other.getElement(i))
        {
            return false;
        }
        else if (data[i] > other.getElement(i))
        {
            return true;
        }
    }

    return false;
}

template<uint32_t N>
template <uint32_t M>
bool UIntX<N>::isEqual(const UIntX<M> &other) const
{
    return !(this->isGreater(other) || other.isGreater(*this));
}

template<uint32_t N>
template <uint32_t M>
bool UIntX<N>::isLess(const UIntX<M> &other) const
{
    return other.isGreater(*this);
}

template<uint32_t N>
template <uint32_t M>
bool UIntX<N>::isGreaterOrEqual(const UIntX<M> &other) const
{
    return (this->isGreater(other) || this->isEqual(other));
}

template<uint32_t N>
template <uint32_t M>
bool UIntX<N>::isLessOrEqual(const UIntX<M> &other) const
{
    return (this->isLess(other) || this->isEqual(other));
}

template<uint32_t N>
template <uint32_t M>
bool UIntX<N>::isNotEqual(const UIntX<M> &other) const
{
    return !(this->isEqual(other));
}

template <uint32_t N>
bool UIntX<N>::isOdd() const
{
    return data[0] & uint64_t(1);
}

template <uint32_t N>
std::size_t UIntX<N>::getArrSize() const
{
    return data.size();
}

template <uint32_t N>
uint64_t UIntX<N>::getElement(uint64_t index) const
{
#ifdef DEBUG
    assert(index < data.size());
#endif
    return data[index];
}

template <uint32_t N>
void UIntX<N>::setElement(uint32_t index, uint64_t value)
{
#ifdef DEBUG
    assert(index < data.size());
#endif
    data[index] = value;
}

template <uint32_t N>
int64_t UIntX<N>::getHighestBitIndex() const
{
    int64_t index = BITS_PER_ELEMENT * (data.size() - 1);
    uint64_t bitmask = uint64_t(1) << (BITS_PER_ELEMENT - 1);

    for (; index >= 0 && !(bitmask & data[index/BITS_PER_ELEMENT]); index--)
    {
        bitmask = bitmask == 1 ? uint64_t(1) << (BITS_PER_ELEMENT - 1) : bitmask >>= 1;
    }

    return index;
}

template <uint32_t N>
uint8_t UIntX<N>::getByte(int64_t index) const
{
#ifdef DEBUG
    assert(index >= 0);
#endif

    uint8_t eight_bits = 0;
    uint64_t bitmask = UINT8_MAX;

    if (index >= 0)
    {
        if (index % BITS_PER_ELEMENT < 8)
        {
            bitmask >>= index%BITS_PER_ELEMENT;
        }
        else
        {
            bitmask <<= index%BITS_PER_ELEMENT;
        }

        eight_bits = data[index/BITS_PER_ELEMENT] & bitmask;
    }

    return eight_bits;
}

template <uint32_t N>
uint8_t UIntX<N>::getNibble(uint32_t) const
{

}

template <uint32_t N>
void UIntX<N>::setNibble(uint32_t, uint8_t)
{
    
}

template <uint32_t N>
bool UIntX<N>::isPowTwo(uint64_t n) const
{
    // Based on:
    // https://www.educative.io/answers/how-to-check-if-a-number-is-a-power-of-2-in-cpp
    return !(n & (n-1));
}

#endif // UINTX_H