// Test case for awesome-no-printf check

extern "C" int printf(const char *, ...);

void bad_function() {
    // CHECK-MESSAGES: :[[@LINE+1]]:5: warning: do not use 'printf'; consider using modern C++ streams (std::cout) or std::print [awesome-no-printf]
    printf("Hello from printf!\n");
}

void good_function() {
}

int main() {
    bad_function();
    good_function();
    return 0;
}

