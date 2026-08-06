[[clang::annotate("throws_exception")]] bool hasExceptionAttribute();
[[clang::annotate("throws_exception")]] auto lambda = []() -> bool { throw "Exception!"; };

// CHECK-MESSAGES: :[[@LINE+1]]:16: warning: Calling throwing function without being able to handle exceptions [fault-line-unhandleable-exception]
bool TestVal = hasExceptionAttribute();
// CHECK-MESSAGES: :[[@LINE+1]]:24: warning: Calling throwing function without being able to handle exceptions [fault-line-unhandleable-exception]
bool LambdaVal = lambda();
// CHECK-MESSAGES: :[[@LINE+1]]:31: warning: Calling throwing function without being able to handle exceptions [fault-line-unhandleable-exception]
bool IifeVal = []() -> bool { throw "Exception"; }();

class TestClass {
  // CHECK-MESSAGES: :[[@LINE+1]]:18: warning: Calling throwing function without being able to handle exceptions [fault-line-unhandleable-exception]
  bool TestVal = hasExceptionAttribute();
  // CHECK-MESSAGES: :[[@LINE+1]]:26: warning: Calling throwing function without being able to handle exceptions [fault-line-unhandleable-exception]
  bool LambdaVal = lambda();
  // CHECK-MESSAGES: :[[@LINE+1]]:33: warning: Calling throwing function without being able to handle exceptions [fault-line-unhandleable-exception]
  bool IifeVal = []() -> bool { throw "Exception"; }();
};
