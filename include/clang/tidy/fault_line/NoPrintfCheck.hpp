#pragma once

#include "clang-tidy/ClangTidyCheck.h"

namespace clang::tidy::fault_line {

/// \brief Checks for usages of C-style `printf` function calls and recommends
/// modern C++ alternatives like `std::cout` or `std::print`.
class NoPrintfCheck : public ClangTidyCheck {
public:
  NoPrintfCheck(StringRef Name, ClangTidyContext *Context)
      : ClangTidyCheck(Name, Context) {}

  void registerMatchers(ast_matchers::MatchFinder *Finder) override;
  void check(const ast_matchers::MatchFinder::MatchResult &Result) override;
};

} // namespace clang::tidy::fault_line

