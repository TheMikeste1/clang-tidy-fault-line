// Test case for fault-line-missing-exception-attribute check

[[clang::annotate("throws_exception")]] void hasExceptionAttribute();

// CHECK-MESSAGES: :[[@LINE+2]]:6: warning: Function 'missingExceptionAttribute' can throw an exception but is not annotated as such. [fault-line-missing-exception-attribute]
// CHECK-FIXES: {{\[\[}}clang::annotate("throws_exception"){{\]\]}} void missingExceptionAttribute() { throw "Missing!"; }
void missingExceptionAttribute() { throw "Missing!"; }

void parentMissingExceptionAttribute() {
  // CHECK-MESSAGES: :[[@LINE+1]]:3: warning: calling function 'hasExceptionAttribute' marked with exception attribute without handling exceptions or marking parent function [fault-line-caller-missing-exception-attribute]
  hasExceptionAttribute();
}

[[clang::annotate("throws_exception")]] void callerWithAttribute() { hasExceptionAttribute(); }

void callerWithTryCatch() {
  try {
    hasExceptionAttribute();
  } catch (...) {
  }
}

void noException() {}

// CHECK-MESSAGES: :[[@LINE+2]]:6: warning: Function 'noExceptionWithAttribute' cannot throw an exception but is annotated as such. [fault-line-useless-exception-attribute]
[[clang::annotate("throws_exception")]] void noExceptionWithAttribute() {}
