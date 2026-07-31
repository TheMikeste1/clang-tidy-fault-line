// Test case for fault-line-missing-exception-attribute check

// CHECK-MESSAGES: :[[@LINE+2]]:36: warning: Function 'missingExceptionAttribute' can throw an exception but is not annotated as such. [fault-line-missing-exception-attribute]
// CHECK-FIXES: {{\[\[}}clang::annotate("throws_exception"){{\]\]}} void missingExceptionAttribute() { throw "Missing!"; }
void missingExceptionAttribute() { throw "Missing!"; }

// CHECK-MESSAGES: :[[@LINE+3]]:46: warning: Function 'missingExceptionAttributeDefinition' can throw an exception but is not annotated as such. [fault-line-missing-exception-attribute]
// CHECK-FIXES: {{\[\[}}clang::annotate("throws_exception"){{\]\]}} void missingExceptionAttributeDefinition();
void missingExceptionAttributeDefinition();
void missingExceptionAttributeDefinition() { throw "Missing!"; }

// CHECK-FIXES: {{\[\[}}clang::annotate("throws_exception"){{\]\]}} void missingExceptionAttributeInCatch() {
void missingExceptionAttributeInCatch() {
  try {
    // . . .
  } catch (...) {
    // CHECK-MESSAGES: :[[@LINE+1]]:5: warning: Function 'missingExceptionAttributeInCatch' can throw an exception but is not annotated as such. [fault-line-missing-exception-attribute]
    throw "Missing!";
  }
}

// CHECK-MESSAGES: :[[@LINE+1]]:22: warning: Lambda can throw an exception but is not annotated as such. [fault-line-missing-exception-attribute]
auto lambda = []() { throw "Missing!"; };

class TestClass {
  // CHECK-MESSAGES: :[[@LINE+2]]:17: warning: Constructor 'TestClass' can throw an exception but is not annotated as such. [fault-line-missing-exception-attribute]
  // CHECK-FIXES: {{\[\[}}clang::annotate("throws_exception"){{\]\]}} TestClass() { throw "Missing!"; }
  TestClass() { throw "Missing!"; }

  // CHECK-MESSAGES: :[[@LINE+2]]:18: warning: Destructor '~TestClass' can throw an exception but is not annotated as such. [fault-line-missing-exception-attribute]
  // CHECK-FIXES: {{\[\[}}clang::annotate("throws_exception"){{\]\]}} ~TestClass() { throw "Missing!"; }
  ~TestClass() { throw "Missing!"; }

  // CHECK-MESSAGES: :[[@LINE+2]]:38: warning: Method 'missingExceptionAttribute' can throw an exception but is not annotated as such. [fault-line-missing-exception-attribute]
  // CHECK-FIXES: {{\[\[}}clang::annotate("throws_exception"){{\]\]}} void missingExceptionAttribute() { throw "Missing!"; }
  void missingExceptionAttribute() { throw "Missing!"; }

  // CHECK-MESSAGES: :[[@LINE+2]]:51: warning: Method 'staticMissingExceptionAttribute' can throw an exception but is not annotated as such. [fault-line-missing-exception-attribute]
  // CHECK-FIXES: {{\[\[}}clang::annotate("throws_exception"){{\]\]}} static void staticMissingExceptionAttribute() { throw "Missing!"; }
  static void staticMissingExceptionAttribute() { throw "Missing!"; }
};
