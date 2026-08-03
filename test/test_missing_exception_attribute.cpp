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

// CHECK-MESSAGES: :[[@LINE+2]]:22: warning: Function 'lambda' can throw an exception but is not annotated as such. [fault-line-missing-exception-attribute]
// CHECK-FIXES: {{\[\[}}clang::annotate("throws_exception"){{\]\]}} auto lambda = []() { throw "Missing!"; };
auto lambda = []() { throw "Missing!"; };

[[clang::annotate("throws_exception")]] auto markedLambda = []() { throw "Missing!"; };
// CHECK-MESSAGES: :[[@LINE+2]]:1: warning: 'unmarkedLambda' assigned to a value that can throw an exception but is not annotated as such. [fault-line-missing-exception-attribute]
// CHECK-FIXES: {{\[\[}}clang::annotate("throws_exception"){{\]\]}} auto unmarkedLambda = markedLambda;
auto unmarkedLambda = markedLambda;

// CHECK-FIXES: {{\[\[}}clang::annotate("throws_exception"){{\]\]}} void iife() {
void iife() {
  // CHECK-MESSAGES: :[[@LINE+1]]:10: warning: Function 'iife' can throw an exception but is not annotated as such. [fault-line-missing-exception-attribute]
  []() { throw "Missing!"; }();
}

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

void dontWarnOnCaughtIife() {
  try {
    []() { throw "Unused"; }();
  } catch (...) {
  }
}
