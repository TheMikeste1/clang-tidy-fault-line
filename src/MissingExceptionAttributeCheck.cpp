#include "clang/tidy/fault_line/MissingExceptionAttributeCheck.hpp"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include <algorithm>
#include <clang/AST/Attr.h>
#include <clang/AST/Attrs.inc>
#include <clang/AST/Decl.h>
#include <clang/AST/DeclCXX.h>
#include <clang/AST/Expr.h>
#include <clang/AST/ExprCXX.h>
#include <clang/Basic/AttrKinds.h>
#include <clang/Basic/Diagnostic.h>
#include <clang/Basic/LLVM.h>
#include <llvm/ADT/StringRef.h>
#include <llvm/Support/Casting.h>

using namespace clang::ast_matchers;

namespace clang::tidy::fault_line {

void MissingExceptionAttributeCheck::registerMatchers(MatchFinder *Finder) {
  // clang-format off
  Finder->addMatcher(
    functionDecl(
      hasDescendant(
        cxxThrowExpr(
          // TODO: Only match if the catch statement catches the throw type
          unless(hasAncestor(compoundStmt(hasParent(cxxTryStmt()))))
        ).bind("throw_expr")
      )
    ).bind("func"),
  this);
  // clang-format on
}

void MissingExceptionAttributeCheck::check(const MatchFinder::MatchResult &Result) {
  const auto *FuncDefinition = Result.Nodes.getNodeAs<FunctionDecl>("func");
  const auto *ThrowExpr = Result.Nodes.getNodeAs<CXXThrowExpr>("throw_expr");

  if (FuncDefinition == nullptr || ThrowExpr == nullptr) {
    return;
  }

  // We want to check the function declaration for the attributes.
  const FunctionDecl *FuncDecl = FuncDefinition->getCanonicalDecl();
  if (FuncDecl == nullptr) {
    return;
  }

  const bool hasExceptionAnnotation = std::any_of(FuncDecl->attrs().begin(), FuncDecl->attrs().end(), [](const Attr *const Attr) {
    const auto *const Annotate = dyn_cast<AnnotateAttr>(Attr);
    if (Annotate == nullptr) {
      return false;
    }

    const llvm::StringRef annotationStr = Annotate->getAnnotation();
    return annotationStr.contains("throws_exception");
  });

  if (hasExceptionAnnotation) {
    return;
  }

  const char *pFunctionType = "Function";
  if (llvm::isa<CXXConstructorDecl>(FuncDefinition)) {
    pFunctionType = "Constructor";
  } else if (llvm::isa<CXXDestructorDecl>(FuncDefinition)) {
    pFunctionType = "Destructor";
  } else if (const auto *MethodDecl = llvm::dyn_cast<CXXMethodDecl>(FuncDecl)) {
    if (MethodDecl->getParent()->isLambda()) {
      // TODO: Figure out how to fix lambdas
      // TODO: If this is an IIFE, we don't need to annotate. The caller should though.
      diag(ThrowExpr->getThrowLoc(), "Lambda can throw an exception but is not annotated as such.") << FuncDecl;
      return;
    }

    pFunctionType = "Method";
  }

  diag(ThrowExpr->getThrowLoc(), "%0 %1 can throw an exception but is not annotated as such.") << pFunctionType << FuncDecl << FixItHint::CreateInsertion(FuncDecl->getBeginLoc(), "[[clang::annotate(\"throws_exception\")]] ");
}

} // namespace clang::tidy::fault_line
