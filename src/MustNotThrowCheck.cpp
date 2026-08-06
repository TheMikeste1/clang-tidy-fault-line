#include "clang/tidy/fault_line/MustNotThrowCheck.hpp"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/tidy/fault_line/utilities.hpp"
#include <clang/AST/Attr.h>
#include <clang/AST/Decl.h>
#include <clang/AST/Expr.h>
#include <clang/AST/ExprCXX.h>
#include <clang/Basic/AttrKinds.h>
#include <clang/Basic/DiagnosticIDs.h>
#include <clang/Basic/LLVM.h>
#include <llvm/ADT/StringRef.h>

using namespace clang::ast_matchers;

namespace clang::tidy::fault_line {
void MustNotThrowCheck::registerMatchers(MatchFinder *Finder) {
  // clang-format off
  Finder->addMatcher(
    callExpr(
      anyOf(
        callee(functionDecl(hasAttr(attr::Annotate)).bind("callee_decl")),
        hasDescendant(declRefExpr(to(varDecl(hasAttr(attr::Annotate)).bind("callee_decl")))),
        hasDescendant(memberExpr(hasDeclaration(fieldDecl(hasAttr(attr::Annotate)).bind("callee_decl")))),
        hasDescendant(cxxThrowExpr(unless(hasAncestor(compoundStmt(hasParent(cxxTryStmt()))))).bind("throw_expr"))
      ),
      unless(hasAncestor(compoundStmt(hasParent(cxxTryStmt())))),
      forCallable(functionDecl(hasType(functionProtoType(isNoThrow()))))
    ).bind("unhandled_call"),
  this);
  // clang-format on
}

void MustNotThrowCheck::check(const MatchFinder::MatchResult &Result) {
  const auto *MatchedCall = Result.Nodes.getNodeAs<CallExpr>("unhandled_call");
  if (MatchedCall == nullptr) {
    return;
  }

  if (const auto *CalleeDecl = Result.Nodes.getNodeAs<DeclaratorDecl>("callee_decl"); CalleeDecl != nullptr) {
    if (hasThrowsExceptionAnnotation(*CalleeDecl)) {
      diag(MatchedCall->getBeginLoc(), "Calling throwing function %0 without handling exceptions in a function that must not throw!", DiagnosticIDs::Level::Error) << CalleeDecl;
    }
    return;
  }

  if (const auto *ThrowExpr = Result.Nodes.getNodeAs<CXXThrowExpr>("throw_expr"); ThrowExpr != nullptr) {
    diag(ThrowExpr->getBeginLoc(), "Calling IIFE without handling exceptions in a function that must not throw!", DiagnosticIDs::Level::Error);
    return;
  }
}
} // namespace clang::tidy::fault_line
