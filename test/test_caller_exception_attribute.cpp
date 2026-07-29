// Test case for fault-line-missing-exception-attribute check

[[clang::annotate("throws_exception")]] void hasExceptionAttribute();

void callerMissingAttribute() {
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

void callerWithTryCatchAndFreeException() {
  try {
    hasExceptionAttribute();
  } catch (...) {
  }

  // CHECK-MESSAGES: :[[@LINE+1]]:3: warning: calling function 'hasExceptionAttribute' marked with exception attribute without handling exceptions or marking parent function [fault-line-caller-missing-exception-attribute]
  hasExceptionAttribute();
}

void callerWithTryCatchAndExceptionInCatch() {
  try {
    hasExceptionAttribute();
  } catch (...) {
    // CHECK-MESSAGES: :[[@LINE+1]]:5: warning: calling function 'hasExceptionAttribute' marked with exception attribute without handling exceptions or marking parent function [fault-line-caller-missing-exception-attribute]
    hasExceptionAttribute();
  }
}
