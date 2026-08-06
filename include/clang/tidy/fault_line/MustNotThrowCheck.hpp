#pragma once

#include "clang-tidy/ClangTidyCheck.h"
#include <clang-tidy/ClangTidyDiagnosticConsumer.h>
#include <clang/ASTMatchers/ASTMatchFinder.h>
#include <clang/Basic/LLVM.h>

namespace clang::tidy::fault_line {

/**
 * @brief Checks for calls to throwing functions or throw expressions inside
 * functions or destructors marked as noexcept without handling exceptions.
 */
class MustNotThrowCheck : public ClangTidyCheck {
public:
  MustNotThrowCheck(StringRef Name, ClangTidyContext *Context) : ClangTidyCheck(Name, Context) {}

  void registerMatchers(ast_matchers::MatchFinder *Finder) override;
  void check(const ast_matchers::MatchFinder::MatchResult &Result) override;
};

} // namespace clang::tidy::fault_line
