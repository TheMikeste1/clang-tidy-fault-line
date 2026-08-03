#include "clang/tidy/fault_line/CallerMissingExceptionAttributeCheck.hpp"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/tidy/fault_line/utilities.hpp"
#include <clang/AST/Attr.h>
#include <clang/AST/Decl.h>
#include <clang/AST/Expr.h>
#include <clang/Basic/AttrKinds.h>
#include <clang/Basic/LLVM.h>
#include <llvm/ADT/StringRef.h>

using namespace clang::ast_matchers;

namespace clang::tidy::fault_line {
void CallerMissingExceptionAttributeCheck::registerMatchers(MatchFinder *Finder) {
  // clang-format off
  Finder->addMatcher(
    callExpr(
      anyOf(
        callee(functionDecl(hasAttr(attr::Annotate)).bind("callee_decl")),
        hasDescendant(declRefExpr(to(varDecl(hasAttr(attr::Annotate)).bind("callee_decl")))),
        hasDescendant(memberExpr(hasDeclaration(fieldDecl(hasAttr(attr::Annotate)).bind("callee_decl"))))
      ),
      unless(hasAncestor(compoundStmt(hasParent(cxxTryStmt())))),
      forCallable(functionDecl().bind("caller_decl"))
    ).bind("unhandled_call"),
  this);
  // clang-format on
}

void CallerMissingExceptionAttributeCheck::check(const MatchFinder::MatchResult &Result) {
  const auto *MatchedCall = Result.Nodes.getNodeAs<CallExpr>("unhandled_call");
  const auto *CalleeDecl = Result.Nodes.getNodeAs<DeclaratorDecl>("callee_decl");
  if (MatchedCall == nullptr || CalleeDecl == nullptr) {
    return;
  }

  if (!hasThrowsExceptionAnnotation(*CalleeDecl)) {
    return;
  }

  const auto *CallerDecl = Result.Nodes.getNodeAs<FunctionDecl>("caller_decl");
  if (CallerDecl != nullptr && hasThrowsExceptionAnnotation(*CallerDecl)) {
    return;
  }

  diag(MatchedCall->getBeginLoc(), "calling function %0 marked with exception attribute without handling "
                                   "exceptions or marking parent function")
      << CalleeDecl;
}
} // namespace clang::tidy::fault_line
