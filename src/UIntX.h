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
        UIntX<N> mult(const UIntX<M> &) const;
        template <uint32_t M>
        UIntX<N> div(const UIntX<M> &) const;
        template <uint32_t M>
        UIntX<N> mod(const UIntX<M> &) const;
        UIntX<N> sqrt() const;

        template <uint32_t M>
        bool isGreater(const UIntX<M> &) const;
        template <uint32_t M>
        bool isEqual(const UIntX<M> &) const;
        template <uint32_t M>
        bool isLess(const UIntX<M> &) const;

        bool isOdd() const;
        std::size_t getArrSize() const;
        uint64_t getElement(uint64_t) const;
        void setElement(uint32_t, uint64_t);

    private:
        static const uint8_t BITS_PER_ELEMENT = 64;
        std::array<uint64_t, N / BITS_PER_ELEMENT> data;

        enum DivModChoice { divide, modulo };
        template <uint32_t M>
        UIntX<N> divmod(const UIntX<M> &, DivModChoice) const;
        bool isPowTwo(uint64_t) const;
};

template<uint32_t N>
UIntX<N>::UIntX()
{
    assert(N >= BITS_PER_ELEMENT && isPowTwo(N));
    data.fill(0);
}

template<uint32_t N>
UIntX<N>::UIntX(const uint64_t num)
{
    assert(N >= BITS_PER_ELEMENT && isPowTwo(N));
    data.fill(0);
    data[0] = num;
}

template<uint32_t N>
template <uint32_t M>
UIntX<N>::UIntX(const UIntX<M> &other)
{
    assert(N >= BITS_PER_ELEMENT && isPowTwo(N));
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
    uint64_t min_arr_size = std::min(data.size(), other.getArrSize());

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
    uint64_t min_arr_size = std::min(data.size(), other.getArrSize());

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
UIntX<N> UIntX<N>::mult(const UIntX<M> &other) const
{
    const uint8_t SHIFT_AMT = BITS_PER_ELEMENT / 2;
    const uint64_t BASE = uint64_t(1) << SHIFT_AMT; // BASE == 2^SHIFT_AMT

    UIntX<N> a; // accumulator
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
                this_data_temp = i%2 ? data[i/2] >> SHIFT_AMT : data[i/2] & BASE-1;
                other_data_temp = j%2 ? other.data[j/2] >> SHIFT_AMT : other.data[j/2] & BASE-1;
                a_data_temp = (i+j)%2 ? a.data[(i+j)/2] >> SHIFT_AMT : a.data[(i+j)/2] & BASE-1;

                cur = this_data_temp * other_data_temp + carry;
                a.data[(i+j)/2] += (i+j)%2 ? cur%BASE << SHIFT_AMT : cur%BASE;
                carry = (cur + a_data_temp) / BASE;
            }
        }
    }

    return a;
}

template<uint32_t N>
template <uint32_t M>
UIntX<N> UIntX<N>::div(const UIntX<M> &other) const
{
    return this->divmod(other, divide);
}

template<uint32_t N>
template <uint32_t M>
UIntX<N> UIntX<N>::mod(const UIntX<M> &other) const
{
    return this->divmod(other, modulo);
}

template<uint32_t N>
template <uint32_t M>
UIntX<N> UIntX<N>::divmod(const UIntX<M> &divisor, DivModChoice choice) const
{
    // *this / divisor = q, r

    static const uint8_t SHIFT_AMT = BITS_PER_ELEMENT / 2;
    static const uint64_t BASE = uint64_t(1) << 32; // BASE == 2^32
    UIntX<N> q, r;

    if (divisor.isEqual(UIntX<N>(0)) || this->isEqual(UIntX<N>(0)))
    {
        q = 0;
        r = 0;
    }
    if (divisor.isGreater(*this))
    {
        q = 0;
        r = divisor;
    }
    else if (divisor.isLess(UIntX<N>(BASE)))
    {
        uint64_t cur,
                 carry = 0,
                 q_data_temp;

        for (int64_t i = (data.size()*2)-1; i>=0; i--)
        {
            q_data_temp = i%2 ? data[i/2] >> SHIFT_AMT : data[i/2] & BASE-1;
            cur = q_data_temp + carry * BASE;
            q.data[i/2] |= i%2 ? (cur/divisor.getElement(0)) << SHIFT_AMT : (cur/divisor.getElement(0)) & BASE-1;
            carry = cur % divisor.getElement(0);
        }
        r = carry;
    }
    else
    {
        uint64_t most_sig_digit = 0,
                 curr = 0;
        int64_t i = divisor.getArrSize()-1;
        for (; curr == 0 && i >= 0; i--)
        {
            curr = divisor.getElement(i);
        }
        most_sig_digit = curr < BASE ? curr : curr >> SHIFT_AMT;
        q = *this;

        while (r.isGreater(divisor))
        {
            q = q.divmod(UIntX<N>(most_sig_digit), divide);
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
template <uint32_t N>
UIntX<N> UIntX<N>::sqrt() const
{

}
*/

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
    assert(index < data.size());
    return data[index];
}

template <uint32_t N>
void UIntX<N>::setElement(uint32_t index, uint64_t value)
{
    assert(index < data.size());
    data[index] = value;
}

template <uint32_t N>
bool UIntX<N>::isPowTwo(uint64_t n) const
{
    // Based on:
    // https://www.educative.io/answers/how-to-check-if-a-number-is-a-power-of-2-in-cpp
    return !(n & (n-1));
}

#endif // UINTX_H