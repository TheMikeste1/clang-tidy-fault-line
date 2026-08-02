[[clang::annotate("throws_exception")]] bool hasExceptionAttribute();

// CHECK-MESSAGES: :[[@LINE+1]]:16: warning: calling throwing function 'hasExceptionAttribute' without being able to handle exceptions [fault-line-caller-unhandleable-exception]
bool TestVal = hasExceptionAttribute();
// CHECK-MESSAGES: :[[@LINE+1]]:31: warning: IIFE without being able to handle exceptions [fault-line-caller-unhandleable-exception]
bool IifeVal = []() -> bool { throw "Exception"; }();

class TestClass {
  // CHECK-MESSAGES: :[[@LINE+1]]:18: warning: calling throwing function 'hasExceptionAttribute' without being able to handle exceptions [fault-line-caller-unhandleable-exception]
  bool TestVal = hasExceptionAttribute();
  // CHECK-MESSAGES: :[[@LINE+1]]:33: warning: IIFE without being able to handle exceptions [fault-line-caller-unhandleable-exception]
  bool IifeVal = []() -> bool { throw "Exception"; }();
};
