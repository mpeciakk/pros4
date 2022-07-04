#ifndef __LIB_HPP
#define __LIB_HPP

#include <Lib/String.hpp>
#include <Lib/Bitmap.hpp>
#include <Debug/Tests.hpp>
#include <Debug/Lib.hpp>
#include <MM/kmalloc.hpp>

int strlenTest() {
    EXPECT(strlen("some string"), 11);
}

int memsetTest() {
    char str[10] = "123456789";

    memset(str + 2, '.', 3*sizeof(char));

    EXPECT(str, "123...789");
}

int memcpyTest() {
    char str[10] = "917284618";
    char str2[10];
    memcpy(str2, str, 10);

    EXPECT(str2, str);
}

int strcmpTest() {
    char str[10] = "jskw9s07f";
    char str2[10] = "jskw9s07f";

    EXPECT(strcmp(str, str2), 0);
}

int strcpyTest() {
    char str[10] = "917284618";
    char str2[10];
    strcpy(str2, str);

    EXPECT(str2, str);
}

int countDigitTest() {
    EXPECT(countDigit(12849172), 8);
}

int itosTest() {
    int n = 916243;
    char str[countDigit(n)];
    itos(n, str, 10);

    EXPECT(str, "916243");
}

int bitmapTest1() {
    u32* a = (u32*) kmalloc(4);

    Bitmap bitmap(a, 4, false);

    bitmap.set(35);

    EXPECT(bitmap.test(35), 1);
}

int bitmapTest2() {
    BasicMemoryAllocator::instance->heap.dump();

    u32* a = (u32*) kmalloc(4);

    Bitmap bitmap(a, 4, false);

    bitmap.set(35);
    bitmap.unset(35);

    EXPECT(bitmap.test(35), 0);
}

int bitmapTest3() {
    u32* a = (u32*) kmalloc(4);

    Bitmap bitmap(a, 4, false);

    bitmap.set(35);
    bitmap.set(435);
    bitmap.set(30);
    bitmap.set(65);
    bitmap.unset(35);
    bitmap.unset(27);
    bitmap.unset(65);

    EXPECT(bitmap.test(35), 0);
}

#endif