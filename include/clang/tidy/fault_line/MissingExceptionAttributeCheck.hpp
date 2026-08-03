#pragma once

#include "clang-tidy/ClangTidyCheck.h"
#include <clang-tidy/ClangTidyDiagnosticConsumer.h>
#include <clang/AST/Decl.h>
#include <clang/AST/ExprCXX.h>
#include <clang/ASTMatchers/ASTMatchFinder.h>
#include <clang/Basic/LLVM.h>

namespace clang::tidy::fault_line {

/// \brief Checks for calls to functions marked with exception attributes that
/// are neither wrapped in a try-catch block nor inside a function marked with
/// an exception attribute.
class MissingExceptionAttributeCheck : public ClangTidyCheck {
public:
  MissingExceptionAttributeCheck(StringRef Name, ClangTidyContext *Context) : ClangTidyCheck(Name, Context) {}

  void registerMatchers(ast_matchers::MatchFinder *Finder) override;
  void check(const ast_matchers::MatchFinder::MatchResult &Result) override;

private:
  void checkFunction(const DeclaratorDecl &Declaration, const CXXThrowExpr &ThrowExpr);
  void checkLambdaAssignment(const DeclaratorDecl &Lhs, const VarDecl &Rhs);
};

} // namespace clang::tidy::fault_line
