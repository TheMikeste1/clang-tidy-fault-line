#include "clang-tidy/ClangTidyModule.h"
#include "clang/tidy/fault_line/CallerMissingExceptionAttributeCheck.hpp"
#include "clang/tidy/fault_line/MissingExceptionAttributeCheck.hpp"
#include "clang/tidy/fault_line/MustNotThrowCheck.hpp"
#include "clang/tidy/fault_line/UnhandleableExceptionCheck.hpp"
#include "clang/tidy/fault_line/UselessExceptionAttributeCheck.hpp"

namespace clang::tidy::fault_line {
class FaultLineModule : public ClangTidyModule { // NOLINT(misc-use-internal-linkage)
public:
  void addCheckFactories(ClangTidyCheckFactories &CheckFactories) override {
    CheckFactories.registerCheck<CallerMissingExceptionAttributeCheck>("fault-line-caller-missing-exception-attribute");
    CheckFactories.registerCheck<MissingExceptionAttributeCheck>("fault-line-missing-exception-attribute");
    CheckFactories.registerCheck<UnhandleableExceptionCheck>("fault-line-unhandleable-exception");
    CheckFactories.registerCheck<UselessExceptionAttributeCheck>("fault-line-useless-exception-attribute");
    CheckFactories.registerCheck<MustNotThrowCheck>("fault-line-must-not-throw");
  }
};

// Register the FaultLineModule using ClangTidyModuleRegistry
// NOLINTNEXTLINE(readability-identifier-length, bugprone-throwing-static-initialization, misc-use-anonymous-namespace, cppcoreguidelines-avoid-non-const-global-variables, cert-err58-cpp)
static ClangTidyModuleRegistry::Add<FaultLineModule> X("fault-line-module", "Adds FaultLine custom checks for C++ code.");

} // namespace clang::tidy::fault_line

// Force symbol registration in shared library plugin
// NOLINTNEXTLINE(misc-use-internal-linkage, cppcoreguidelines-avoid-non-const-global-variables)
volatile int FaultLineModuleAnchorSource = 0;
