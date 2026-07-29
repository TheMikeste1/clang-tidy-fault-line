// CHECK-MESSAGES: :[[@LINE+2]]:6: warning: Function 'noExceptionWithAttribute' cannot throw an exception but is annotated as such. [fault-line-useless-exception-attribute]
[[clang::annotate("throws_exception")]] void noExceptionWithAttribute() {}
