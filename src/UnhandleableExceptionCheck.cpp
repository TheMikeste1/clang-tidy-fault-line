#include "clang/tidy/fault_line/UnhandleableExceptionCheck.hpp"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/tidy/fault_line/utilities.hpp"
#include <clang/AST/Attr.h>
#include <clang/AST/Decl.h>
#include <clang/AST/Expr.h>
#include <clang/AST/ExprCXX.h>
#include <clang/Basic/AttrKinds.h>
#include <clang/Basic/LLVM.h>
#include <llvm/ADT/StringRef.h>

using namespace clang::ast_matchers;

namespace clang::tidy::fault_line {
void UnhandleableExceptionCheck::registerMatchers(MatchFinder *Finder) {
  // clang-format off
  Finder->addMatcher(
    callExpr(
      anyOf(
        callee(functionDecl(hasAttr(attr::Annotate)).bind("callee_decl")),
        hasDescendant(declRefExpr(to(varDecl(hasAttr(attr::Annotate)).bind("callee_decl")))),
        hasDescendant(memberExpr(hasDeclaration(fieldDecl(hasAttr(attr::Annotate)).bind("callee_decl")))),
        hasDescendant(cxxThrowExpr(unless(hasAncestor(compoundStmt(hasParent(cxxTryStmt()))))).bind("throw_expr"))
        // TODO: Handle IIFEs with attributed functions/lambdas
        // TODO: Handle nested IIFEs
      ),
      unless(hasAncestor(functionDecl()))
    ).bind("unhandled_call"),
  this);
  // clang-format on
}

void UnhandleableExceptionCheck::check(const MatchFinder::MatchResult &Result) {
  const auto *MatchedCall = Result.Nodes.getNodeAs<CallExpr>("unhandled_call");
  if (MatchedCall == nullptr) {
    return;
  }

  if (const auto *CalleeDecl = Result.Nodes.getNodeAs<DeclaratorDecl>("callee_decl"); CalleeDecl != nullptr) {
    if (hasThrowsExceptionAnnotation(*CalleeDecl)) {
      diag(MatchedCall->getBeginLoc(), "Calling throwing function without being able to handle exceptions");
    }
    return;
  }

  if (const auto *ThrowExpr = Result.Nodes.getNodeAs<CXXThrowExpr>("throw_expr"); ThrowExpr != nullptr) {
    diag(ThrowExpr->getBeginLoc(), "Calling throwing function without being able to handle exceptions");
    return;
  }
}
} // namespace clang::tidy::fault_line
