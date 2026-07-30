[[clang::annotate("throws_exception")]] bool hasExceptionAttribute();

// CHECK-MESSAGES: :[[@LINE+1]]:36: error: calling throwing function 'hasExceptionAttribute' without handling exceptions in a function that must not throw! [fault-line-caller-noexcept-exception]
void noexceptFunction() noexcept { hasExceptionAttribute(); }

class TestClass {
  // CHECK-MESSAGES: :[[@LINE+1]]:18: error: calling throwing function 'hasExceptionAttribute' without handling exceptions in a function that must not throw! [fault-line-caller-noexcept-exception]
  ~TestClass() { hasExceptionAttribute(); }

  // CHECK-MESSAGES: :[[@LINE+1]]:38: error: calling throwing function 'hasExceptionAttribute' without handling exceptions in a function that must not throw! [fault-line-caller-noexcept-exception]
  void noexceptFunction() noexcept { hasExceptionAttribute(); }

  // CHECK-MESSAGES: :[[@LINE+1]]:51: error: calling throwing function 'hasExceptionAttribute' without handling exceptions in a function that must not throw! [fault-line-caller-noexcept-exception]
  static void staticNoexceptFunction() noexcept { hasExceptionAttribute(); }
};
