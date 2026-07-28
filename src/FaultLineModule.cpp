#include "clang-tidy/ClangTidyModule.h"
#include "clang/tidy/fault_line/NoPrintfCheck.hpp"
#include "clang/tidy/fault_line/SnakeCaseFunctionCheck.hpp"

namespace clang::tidy::fault_line {
class FaultLineModule : public ClangTidyModule {
public:
  void addCheckFactories(ClangTidyCheckFactories &CheckFactories) override {
    CheckFactories.registerCheck<NoPrintfCheck>("awesome-no-printf");
    CheckFactories.registerCheck<SnakeCaseFunctionCheck>(
        "awesome-snake-case-function");
  }
};

// Register the AwesomeModule using ClangTidyModuleRegistry
static ClangTidyModuleRegistry::Add<FaultLineModule>
    X("fault-line-module", "Adds FaultLine custom checks for C++ code.");

} // namespace clang::tidy::fault_line

// Force symbol registration in shared library plugin
// NOLINTNEXTLINE(misc-use-internal-linkage)
volatile int FaultLineModuleAnchorSource = 0;
