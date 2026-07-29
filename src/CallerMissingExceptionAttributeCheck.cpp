#include "clang/tidy/fault_line/CallerMissingExceptionAttributeCheck.hpp"
#include <clang/AST/Attr.h>
#include <clang/AST/Decl.h>
#include <clang/AST/Expr.h>
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include <clang/Basic/AttrKinds.h>
#include <clang/Basic/LLVM.h>
#include <llvm/ADT/StringRef.h>
#include <algorithm>

using namespace clang::ast_matchers;

namespace clang::tidy::fault_line {
namespace {
bool hasThrowsExceptionAnnotation(const FunctionDecl *FunctionDeclaration) {
  if (FunctionDeclaration == nullptr || !FunctionDeclaration->hasAttrs()) {
    return false;
  }

  return std::any_of(FunctionDeclaration->attrs().begin(), FunctionDeclaration->attrs().end(), [](const Attr *attr) {
    if (const auto *Annotate = dyn_cast<AnnotateAttr>(attr)) {
      return Annotate->getAnnotation().contains("throws_exception");
    }
    return false;
  });
}
} // namespace

void CallerMissingExceptionAttributeCheck::registerMatchers(MatchFinder *Finder) {
  Finder->addMatcher(callExpr(callee(functionDecl(hasAttr(attr::Annotate)).bind("callee_decl")), unless(hasAncestor(compoundStmt(hasParent(cxxTryStmt())))), forCallable(functionDecl().bind("caller_decl"))).bind("unhandled_call"), this);
}

void CallerMissingExceptionAttributeCheck::check(const MatchFinder::MatchResult &Result) {
  const auto *MatchedCall = Result.Nodes.getNodeAs<CallExpr>("unhandled_call");
  const auto *CalleeDecl = Result.Nodes.getNodeAs<FunctionDecl>("callee_decl");
  if (MatchedCall == nullptr || CalleeDecl == nullptr) {
    return;
  }

  if (!hasThrowsExceptionAnnotation(CalleeDecl)) {
    return;
  }

  const auto *CallerDecl = Result.Nodes.getNodeAs<FunctionDecl>("caller_decl");
  if (CallerDecl != nullptr && hasThrowsExceptionAnnotation(CallerDecl)) {
    return;
  }

  diag(MatchedCall->getBeginLoc(), "calling function %0 marked with exception attribute without handling "
                                   "exceptions or marking parent function")
      << CalleeDecl;
}
} // namespace clang::tidy::fault_line
