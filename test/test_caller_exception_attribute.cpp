[[clang::annotate("throws_exception")]] bool hasExceptionAttribute();
[[clang::annotate("throws_exception")]] auto lambda = []() { throw "Exception"; };

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

void callerLambda() {
  // CHECK-MESSAGES: :[[@LINE+1]]:9: warning: calling function 'lambda' marked with exception attribute without handling exceptions or marking parent function [fault-line-caller-missing-exception-attribute]
  lambda();
}

void callerLambdaWithTryCatch() {
  try {
    lambda();
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

class TestClass {
  bool TestVal;

  // CHECK-MESSAGES: :[[@LINE+1]]:25: warning: calling function 'hasExceptionAttribute' marked with exception attribute without handling exceptions or marking parent function [fault-line-caller-missing-exception-attribute]
  TestClass() : TestVal(hasExceptionAttribute()) {
    // CHECK-MESSAGES: :[[@LINE+1]]:5: warning: calling function 'hasExceptionAttribute' marked with exception attribute without handling exceptions or marking parent function [fault-line-caller-missing-exception-attribute]
    hasExceptionAttribute();
  }

  ~TestClass() {
    // CHECK-MESSAGES: :[[@LINE+1]]:5: warning: calling function 'hasExceptionAttribute' marked with exception attribute without handling exceptions or marking parent function [fault-line-caller-missing-exception-attribute]
    hasExceptionAttribute();
  }

  void callerMissingAttribute() {
    // CHECK-MESSAGES: :[[@LINE+1]]:5: warning: calling function 'hasExceptionAttribute' marked with exception attribute without handling exceptions or marking parent function [fault-line-caller-missing-exception-attribute]
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

    // CHECK-MESSAGES: :[[@LINE+1]]:5: warning: calling function 'hasExceptionAttribute' marked with exception attribute without handling exceptions or marking parent function [fault-line-caller-missing-exception-attribute]
    hasExceptionAttribute();
  }

  void callerWithTryCatchAndExceptionInCatch() {
    try {
      hasExceptionAttribute();
    } catch (...) {
      // CHECK-MESSAGES: :[[@LINE+1]]:7: warning: calling function 'hasExceptionAttribute' marked with exception attribute without handling exceptions or marking parent function [fault-line-caller-missing-exception-attribute]
      hasExceptionAttribute();
    }
  }

  static void staticCallerMissingAttribute() {
    // CHECK-MESSAGES: :[[@LINE+1]]:5: warning: calling function 'hasExceptionAttribute' marked with exception attribute without handling exceptions or marking parent function [fault-line-caller-missing-exception-attribute]
    hasExceptionAttribute();
  }

  [[clang::annotate("throws_exception")]] static void staticCallerWithAttribute() { hasExceptionAttribute(); }

  static void staticCallerWithTryCatch() {
    try {
      hasExceptionAttribute();
    } catch (...) {
    }
  }

  static void staticCallerWithTryCatchAndFreeException() {
    try {
      hasExceptionAttribute();
    } catch (...) {
    }

    // CHECK-MESSAGES: :[[@LINE+1]]:5: warning: calling function 'hasExceptionAttribute' marked with exception attribute without handling exceptions or marking parent function [fault-line-caller-missing-exception-attribute]
    hasExceptionAttribute();
  }

  static void staticCallerWithTryCatchAndExceptionInCatch() {
    try {
      hasExceptionAttribute();
    } catch (...) {
      // CHECK-MESSAGES: :[[@LINE+1]]:7: warning: calling function 'hasExceptionAttribute' marked with exception attribute without handling exceptions or marking parent function [fault-line-caller-missing-exception-attribute]
      hasExceptionAttribute();
    }
  }

  [[clang::annotate("throws_exception")]] decltype(lambda) Lambda = lambda;
  void callerLambda() {
    // CHECK-MESSAGES: :[[@LINE+1]]:17: warning: calling function 'Lambda' marked with exception attribute without handling exceptions or marking parent function [fault-line-caller-missing-exception-attribute]
    this->Lambda();
  }
};
