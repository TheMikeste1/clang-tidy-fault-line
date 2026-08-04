[[clang::annotate("throws_exception")]] bool hasExceptionAttribute();
[[clang::annotate("throws_exception")]] auto lambda = []() { throw "Exception!"; };

// CHECK-MESSAGES: :[[@LINE+1]]:36: error: calling throwing function 'hasExceptionAttribute' without handling exceptions in a function that must not throw! [fault-line-must-not-throw]
void noexceptFunction() noexcept { hasExceptionAttribute(); }

class TestClass {
  // CHECK-MESSAGES: :[[@LINE+1]]:18: error: calling throwing function 'hasExceptionAttribute' without handling exceptions in a function that must not throw! [fault-line-must-not-throw]
  ~TestClass() { hasExceptionAttribute(); }

  // CHECK-MESSAGES: :[[@LINE+1]]:38: error: calling throwing function 'hasExceptionAttribute' without handling exceptions in a function that must not throw! [fault-line-must-not-throw]
  void noexceptFunction() noexcept { hasExceptionAttribute(); }

  // CHECK-MESSAGES: :[[@LINE+1]]:51: error: calling throwing function 'hasExceptionAttribute' without handling exceptions in a function that must not throw! [fault-line-must-not-throw]
  static void staticNoexceptFunction() noexcept { hasExceptionAttribute(); }

  // CHECK-MESSAGES: :[[@LINE+1]]:46: error: calling throwing function 'lambda' without handling exceptions in a function that must not throw! [fault-line-must-not-throw]
  void noexceptCallLambda() noexcept { lambda(); }
};
