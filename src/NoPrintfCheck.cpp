#include "clang/tidy/fault_line/NoPrintfCheck.hpp"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/ASTMatchers/ASTMatchers.h"

using namespace clang::ast_matchers;

namespace clang::tidy::fault_line {

void NoPrintfCheck::registerMatchers(MatchFinder *Finder) {
  // Match any call expression where the callee is a function named "printf"
  Finder->addMatcher(
      callExpr(callee(functionDecl(hasName("printf")))).bind("printf_call"),
      this);
}

void NoPrintfCheck::check(const MatchFinder::MatchResult &Result) {
  const auto *MatchedCall = Result.Nodes.getNodeAs<CallExpr>("printf_call");
  if (!MatchedCall)
    return;

  diag(MatchedCall->getBeginLoc(),
       "do not use 'printf'; consider using modern C++ streams (std::cout) or std::print")
      << MatchedCall->getSourceRange();
}

} // namespace clang::tidy::fault_line

