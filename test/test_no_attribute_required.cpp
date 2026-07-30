[[clang::annotate("throws_exception")]] bool hasExceptionAttribute();

void noExceptionInline() {}

void exceptionCaught() {
  try {
    throw "Exception!";
  } catch (...) {
  }
}

class TestClass {
  TestClass() {
    try {
      throw "Exception!";
    } catch (...) {
    }
  }

  ~TestClass() {
    try {
      throw "Exception!";
    } catch (...) {
    }
  }

  static void staticNoExceptionInline() {}

  static void staticExceptionCaught() {
    try {
      throw "Exception!";
    } catch (...) {
    }
  }

  void noExceptionInline() {}

  void exceptionCaught() {
    try {
      throw "Exception!";
    } catch (...) {
    }
  }
};
