#pragma once

#include "clang-tidy/ClangTidyCheck.h"

namespace clang::tidy::fault_line {

/// \brief Checks that function names adhere to lower_snake_case naming convention.
/// Ignores main, operators, constructors, destructors, and C linkage functions.
class SnakeCaseFunctionCheck : public ClangTidyCheck {
public:
  SnakeCaseFunctionCheck(StringRef Name, ClangTidyContext *Context)
      : ClangTidyCheck(Name, Context) {}

  void registerMatchers(ast_matchers::MatchFinder *Finder) override;
  void check(const ast_matchers::MatchFinder::MatchResult &Result) override;
};

} // namespace clang::tidy::fault_line

