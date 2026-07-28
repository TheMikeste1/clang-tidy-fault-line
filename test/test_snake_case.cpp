// Test case for awesome-snake-case-function check

// CHECK-MESSAGES: :[[@LINE+1]]:5: warning: function name 'CalculateTotal' should use lower_snake_case; consider 'calculate_total' [awesome-snake-case-function]
int CalculateTotal(int a, int b) {
    return a + b;
}

// CHECK-MESSAGES: :[[@LINE+1]]:6: warning: function name 'doSomethingAwesome' should use lower_snake_case; consider 'do_something_awesome' [awesome-snake-case-function]
void doSomethingAwesome() {
}

// Valid snake_case function - no warning expected
void valid_snake_case_func() {
}

// main function should be ignored
int main() {
    CalculateTotal(1, 2);
    doSomethingAwesome();
    valid_snake_case_func();
    return 0;
}

