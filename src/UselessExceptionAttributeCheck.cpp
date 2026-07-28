#include "clang/tidy/fault_line/UselessExceptionAttributeCheck.hpp"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include <clang/AST/Attr.h>
#include <clang/AST/Attrs.inc>
#include <clang/AST/Decl.h>
#include <clang/AST/Expr.h>
#include <clang/AST/ExprCXX.h>
#include <clang/Basic/AttrKinds.h>
#include <clang/Basic/Diagnostic.h>
#include <clang/Basic/LLVM.h>
#include <clang/Basic/SourceLocation.h>
#include <llvm/ADT/StringRef.h>

using namespace clang::ast_matchers;

namespace clang::tidy::fault_line {

void UselessExceptionAttributeCheck::registerMatchers(MatchFinder *Finder) {
  // TODO
  Finder->addMatcher(functionDecl(hasAttr(attr::Annotate), hasBody(stmt()), unless(hasDescendant(cxxThrowExpr())), unless(hasDescendant(cxxThrowExpr())),
                                  hasDescendant(callExpr(callee(functionDecl(hasAttr(attr::Annotate)).bind("callee_decl")))), unless(isExpansionInSystemHeader()))
                         .bind("func"),
                     this);
}

void UselessExceptionAttributeCheck::check(const MatchFinder::MatchResult &Result) {
  // TODO
  const auto *funcDecl = Result.Nodes.getNodeAs<FunctionDecl>("func");
  if (funcDecl == nullptr) {
    return;
  }

  const AnnotateAttr *annotation = nullptr;
  for (const auto *const attr : funcDecl->attrs()) {
    const auto *const maybeAnnotation = dyn_cast<AnnotateAttr>(attr);
    if (maybeAnnotation == nullptr) {
      continue;
    }

    const llvm::StringRef annotationStr = maybeAnnotation->getAnnotation();
    if (annotationStr.contains("throws_exception")) {
      annotation = maybeAnnotation;
      break;
    }
  }

  if (annotation != nullptr) {
    diag(annotation->getLocation(), "Function %0 cannot throw an exception but is annotated as such.") << funcDecl;
  }
}

} // namespace clang::tidy::fault_line
