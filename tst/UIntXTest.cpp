//
// Created by Aubrey on 2/29/2024.
//

#include <gtest/gtest.h>
#include "UIntX.h"

TEST(UIntX, Construct)
{
    EXPECT_DEATH(UIntX<22> test, "");
    EXPECT_DEATH(UIntX<65> test, "");
    EXPECT_DEATH(UIntX<129> test, "");
    EXPECT_DEATH(UIntX<1233> test, "");

    EXPECT_NO_THROW(UIntX<64> test);
    EXPECT_NO_THROW(UIntX<64> testA;
                    UIntX<64> testB = testA);
    EXPECT_NO_THROW(UIntX<128> test);
    EXPECT_NO_THROW(UIntX<256> test);
    EXPECT_NO_THROW(UIntX<512> test);
    EXPECT_NO_THROW(UIntX<1024> test);
    EXPECT_NO_THROW(UIntX<16384> test);

    EXPECT_NO_THROW(UIntX<64> test(55));
    EXPECT_NO_THROW(UIntX<256> test(123123));
    EXPECT_NO_THROW(UIntX<128> test(44444444));
}

TEST(UIntX, isGreater)
{
    UIntX<128> testA;
    UIntX<128> testB(0);
    testA = 1;
    testB = 1;
    EXPECT_EQ(testA.isGreater(testB), false);
    testB = 2;
    EXPECT_EQ(testA.isGreater(testB), false);
    testB = 0;
    EXPECT_EQ(testA.isGreater(testB), true);
}

TEST(UIntX, isEqual)
{
    UIntX<128> testA;
    UIntX<128> testB(0);
    testA = 1;
    testB = 1;
    EXPECT_EQ(testA.isEqual(testB), true);
    testB = 2;
    EXPECT_EQ(testA.isEqual(testB), false);
    testB = 0;
    EXPECT_EQ(testA.isEqual(testB), false);
}

TEST(UIntX, isLess)
{
    UIntX<128> testA;
    UIntX<128> testB(0);
    testA = 1;
    testB = 1;
    EXPECT_EQ(testA.isLess(testB), false);
    testB = 2;
    EXPECT_EQ(testA.isLess(testB), true);
    testB = 0;
    EXPECT_EQ(testA.isLess(testB), false);
}

TEST(UIntX, add)
{
    UIntX<128> testA = 25;
    UIntX<128> testB = 999;
    testA = testA.add(testB);
    EXPECT_EQ(testA.getElement(0), 1024);
    EXPECT_EQ(testA.getElement(1), 0);

    testA = UINT64_MAX;
    testB = 1;
    testA = testA.add(testB);
    EXPECT_EQ(testA.getElement(0), 0);
    EXPECT_EQ(testA.getElement(1), 1);

    testA = UINT64_MAX;
    UIntX<64> testC = 1;
    testA = testA.add(testC);
    EXPECT_EQ(testA.getElement(0), 0);
    EXPECT_EQ(testA.getElement(1), 1);

    testC = UINT64_MAX;
    testA = 1;
    testC = testC.add(testA);
    EXPECT_EQ(testC.getElement(0), 0);

    testA = 123;
    testB = 27;
    EXPECT_EQ(testA.add(testB).isEqual(testB.add(testA)), true);
}

TEST(UIntX, sub)
{
    UIntX<128> testA = 100;
    UIntX<128> testB = 12;
    testA = testA.sub(testB);
    EXPECT_EQ(testA.getElement(0), 88);
    EXPECT_EQ(testA.getElement(1), 0);

    testA = UINT64_MAX;
    testB = 1;
    testA = testA.add(testB);
    testA = testA.sub(testB);
    EXPECT_EQ(testA.getElement(0), UINT64_MAX);
    EXPECT_EQ(testA.getElement(1), 0);
}

TEST(UIntX, mult)
{
    UIntX<128> testA = 100;
    UIntX<128> testB = 12;
    testA = testA.mult(testB);
    EXPECT_EQ(testA.isEqual(UIntX<128>(1200)), 1);

    testA = 65'535;
    testB = 65'535;
    testA = testA.mult(testB);
    EXPECT_EQ(testA.isEqual(UIntX<128>(4'294'836'225)), 1);

    testA = 4'294'967'295;
    testB = 4'294'967'295;
    testA = testA.mult(testB);
    EXPECT_EQ(testA.isEqual(UIntX<128>(18'446'744'065'119'617'025)), 1);

    testA = UINT64_MAX;
    testB = UINT64_MAX;
    testA = testA.mult(testB); // 11111111111111111111111111111111111111111111111111111111111111100000000000000000000000000000000000000000000000000000000000000001
    EXPECT_EQ(testA.getElement(0), 1);
    EXPECT_EQ(testA.getElement(1), UINT64_MAX-1);

    testA = uint64_t(1) << 32;
    testB = uint64_t(1) << 32;
    testA = testA.mult(testB);
    EXPECT_EQ(testA.getElement(0), 0);
    EXPECT_EQ(testA.getElement(1), 1);

    UIntX<256>testC = UINT64_MAX;
    UIntX<256>testD = UINT64_MAX;
    testC = testC.mult(testD);
    testC = testC.mult(testD); // 1111111111111111111111111111111111111111111111111111111111111101 0000000000000000000000000000000000000000000000000000000000000010 1111111111111111111111111111111111111111111111111111111111111111
    EXPECT_EQ(testC.getElement(0), UINT64_MAX);
    EXPECT_EQ(testC.getElement(1), 2);
    EXPECT_EQ(testC.getElement(2), UINT64_MAX-2);
    EXPECT_EQ(testC.getElement(3), 0);
}