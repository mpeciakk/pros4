#include <Debug/Tests.hpp>
#include <Lib/Log.hpp>

#include <Debug/Lib.hpp>

int (*tests[10])(){&memsetTest, &strlenTest, &memcpyTest, &strcmpTest, &strcpyTest, &countDigitTest, &itosTest, &bitmapTest1, &bitmapTest2, &bitmapTest3};

int expect(int value, int expectedValue, const char* function, char* file, int line) {
    if (value == expectedValue) {
        klog(3, "%s [%s:%d]: Success", function, file, line);
        return 0;
    } else {
        klog(2, "%s [%s:%d]: Got %d instead of %d", function, file, line, value, expectedValue);
        return -1;
    }
}

int expect(char* value, char* expectedValue, const char* function, char* file, int line) {
    if (strcmp(value, expectedValue) == 0) {
        klog(3, "%s [%s:%d]: Success", function, file, line);
        return 0;
    } else {
        klog(2, "%s [%s:%d]: Got %s instead of %s", function, file, line, value, expectedValue);
        return -1;
    }
}

void performTests() {
    klog(3, "-----------------------Performing Tests-----------------------");

    int passed = 0;
    int total = 0;

    for (auto& test : tests) {
        klog(3, "Test %d/%d", passed, 10);

        if (test() == 0) {
            passed++;
        }

        total++;
    }

    klog(3, "Passed %d/%d tests", passed, total);
}