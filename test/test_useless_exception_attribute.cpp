// CHECK-MESSAGES: :[[@LINE+1]]:41: warning: Function 'noExceptionWithAttribute' cannot throw an exception but is annotated as such. [fault-line-useless-exception-attribute]
[[clang::annotate("throws_exception")]] void noExceptionWithAttribute() {}

// CHECK-MESSAGES: :[[@LINE+1]]:41: warning: Function 'lambda' cannot throw an exception but is annotated as such. [fault-line-useless-exception-attribute]
[[clang::annotate("throws_exception")]] auto lambda = []() {};

auto unmarkedLambda = []() {};

class TestClass {
  // CHECK-MESSAGES: :[[@LINE+1]]:3: warning: Function 'Lambda' cannot throw an exception but is annotated as such. [fault-line-useless-exception-attribute]
  [[clang::annotate("throws_exception")]] decltype(unmarkedLambda) Lambda = unmarkedLambda;
};
