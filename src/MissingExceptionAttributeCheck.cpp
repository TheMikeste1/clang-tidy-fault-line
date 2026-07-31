#include "clang/tidy/fault_line/MissingExceptionAttributeCheck.hpp"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include <algorithm>
#include <clang/AST/Attr.h>
#include <clang/AST/Attrs.inc>
#include <clang/AST/Decl.h>
#include <clang/AST/Expr.h>
#include <clang/AST/ExprCXX.h>
#include <clang/Basic/AttrKinds.h>
#include <clang/Basic/Diagnostic.h>
#include <clang/Basic/LLVM.h>
#include <llvm/ADT/StringRef.h>

using namespace clang::ast_matchers;

namespace clang::tidy::fault_line {

void MissingExceptionAttributeCheck::registerMatchers(MatchFinder *Finder) {
  // clang-format off
  Finder->addMatcher(
    cxxThrowExpr(
      unless(hasAncestor(compoundStmt(hasParent(cxxTryStmt()))))
    ).bind("throwExpr"),
  this);
  // clang-format on
}

void MissingExceptionAttributeCheck::check(const MatchFinder::MatchResult &Result) {
  const auto *funcDecl = Result.Nodes.getNodeAs<FunctionDecl>("func");
  if (funcDecl == nullptr) {
    return;
  }

  const bool hasExceptionAnnotation = std::any_of(funcDecl->attrs().begin(), funcDecl->attrs().end(), [](const Attr *const attr) {
    const auto *const annotate = dyn_cast<AnnotateAttr>(attr);
    if (annotate == nullptr) {
      return false;
    }

    const llvm::StringRef annotationStr = annotate->getAnnotation();
    return annotationStr.contains("throws_exception");
  });

  if (!hasExceptionAnnotation) {
    diag(funcDecl->getLocation(), "Function %0 can throw an exception but is not annotated as such.") << funcDecl << FixItHint::CreateInsertion(funcDecl->getBeginLoc(), "[[clang::annotate(\"throws_exception\")]] ");
  }
}

} // namespace clang::tidy::fault_line
