[[clang::annotate("throws_exception")]] bool hasExceptionAttribute();

class TestClass {
  // CHECK-MESSAGES: :[[@LINE+1]]:18: warning: calling throwing function 'hasExceptionAttribute' without being able to handle exceptions [fault-line-caller-unhandleable-exception]
  bool TestVal = hasExceptionAttribute();
};
