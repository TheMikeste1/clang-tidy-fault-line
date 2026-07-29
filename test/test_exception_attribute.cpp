// Test case for fault-line-missing-exception-attribute check

// CHECK-MESSAGES: :[[@LINE+2]]:6: warning: Function 'missingExceptionAttribute' can throw an exception but is not annotated as such. [fault-line-missing-exception-attribute]
// CHECK-FIXES: {{\[\[}}clang::annotate("throws_exception"){{\]\]}} void missingExceptionAttribute() { throw "Missing!"; }
void missingExceptionAttribute() { throw "Missing!"; }
