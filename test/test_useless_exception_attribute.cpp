// CHECK-MESSAGES: :[[@LINE+2]]:3: warning: Function 'noExceptionWithAttribute' cannot throw an exception but is annotated as such. [fault-line-useless-exception-attribute]
[[clang::annotate("throws_exception")]] void noExceptionWithAttribute() {}

// CHECK-MESSAGES: :[[@LINE+2]]:3: warning: Lambda cannot throw an exception but is annotated as such. [fault-line-useless-exception-attribute]
[[clang::annotate("throws_exception")]] auto lambda = []() {};
