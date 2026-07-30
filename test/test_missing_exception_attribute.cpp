// Test case for fault-line-missing-exception-attribute check

// CHECK-MESSAGES: :[[@LINE+2]]:6: warning: Function 'missingExceptionAttribute' can throw an exception but is not annotated as such. [fault-line-missing-exception-attribute]
// CHECK-FIXES: {{\[\[}}clang::annotate("throws_exception"){{\]\]}} void missingExceptionAttribute() { throw "Missing!"; }
void missingExceptionAttribute() { throw "Missing!"; }

// CHECK-MESSAGES: :[[@LINE+2]]:6: warning: Function 'missingExceptionAttributeInCatch' can throw an exception but is not annotated as such. [fault-line-missing-exception-attribute]
// CHECK-FIXES: {{\[\[}}clang::annotate("throws_exception"){{\]\]}} void missingExceptionAttributeInCatch() {
void missingExceptionAttributeInCatch() {
  try {
    // . . .
  } catch (...) {
    throw "Missing!";
  }
}

class TestClass {
  // CHECK-MESSAGES: :[[@LINE+2]]:3: warning: Function 'TestClass' can throw an exception but is not annotated as such. [fault-line-missing-exception-attribute]
  // CHECK-FIXES: {{\[\[}}clang::annotate("throws_exception"){{\]\]}} TestClass() { throw "Missing!"; }
  TestClass() { throw "Missing!"; }

  // CHECK-MESSAGES: :[[@LINE+2]]:3: warning: Function '~TestClass' can throw an exception but is not annotated as such. [fault-line-missing-exception-attribute]
  // CHECK-FIXES: {{\[\[}}clang::annotate("throws_exception"){{\]\]}} ~TestClass() { throw "Missing!"; }
  ~TestClass() { throw "Missing!"; }

  // CHECK-MESSAGES: :[[@LINE+2]]:8: warning: Function 'missingExceptionAttribute' can throw an exception but is not annotated as such. [fault-line-missing-exception-attribute]
  // CHECK-FIXES: {{\[\[}}clang::annotate("throws_exception"){{\]\]}} void missingExceptionAttribute() { throw "Missing!"; }
  void missingExceptionAttribute() { throw "Missing!"; }

  // CHECK-MESSAGES: :[[@LINE+2]]:15: warning: Function 'staticMissingExceptionAttribute' can throw an exception but is not annotated as such. [fault-line-missing-exception-attribute]
  // CHECK-FIXES: {{\[\[}}clang::annotate("throws_exception"){{\]\]}} static void staticMissingExceptionAttribute() { throw "Missing!"; }
  static void staticMissingExceptionAttribute() { throw "Missing!"; }
};
