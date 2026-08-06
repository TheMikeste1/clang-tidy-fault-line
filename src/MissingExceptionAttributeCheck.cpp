#include "clang/tidy/fault_line/MissingExceptionAttributeCheck.hpp"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/tidy/fault_line/utilities.hpp"
#include <clang/AST/Attr.h>
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
      ),
      unless(hasAncestor(lambdaExpr()))
    ).bind("decl"),
  this);

  const auto ExistingCallableMatcher = hasDescendant(
    declRefExpr(
      to(
        decl(
          hasAttr(attr::Annotate)
        ).bind("assignee")
      )
    )
  );
  const auto NewLambdaMatcher = hasDescendant(
    cxxThrowExpr(
      // TODO: Only match if the catch statement catches the throw type
      unless(hasAncestor(compoundStmt(hasParent(cxxTryStmt())))),
      // Don't match IIFE lambdas.
      // We want to match the containing function for IIFEs, but not the lambda itself.
      unless(hasAncestor(callExpr()))
    ).bind("throw_expr")
  );

  Finder->addMatcher(
    varDecl(
      anyOf(
        ExistingCallableMatcher,
        NewLambdaMatcher
      )
    ).bind("decl"),
  this);

  Finder->addMatcher(
    fieldDecl(
      anyOf(
        ExistingCallableMatcher,
        NewLambdaMatcher
      )
    ).bind("decl"),
  this);
  // clang-format on
}

void MissingExceptionAttributeCheck::check(const MatchFinder::MatchResult &Result) {
  const auto *Decl = Result.Nodes.getNodeAs<DeclaratorDecl>("decl");
  if (Decl == nullptr) {
    return;
  }

  const auto *ThrowExpr = Result.Nodes.getNodeAs<CXXThrowExpr>("throw_expr");
  if (ThrowExpr != nullptr) {
    checkFunction(*Decl, *ThrowExpr);
    return;
  }

  const auto *Rhs = Result.Nodes.getNodeAs<DeclaratorDecl>("assignee");
  if (Decl != nullptr && Rhs != nullptr) {
    checkLambdaAssignment(*Decl, *Rhs);
    return;
  }
}

void MissingExceptionAttributeCheck::checkFunction(const DeclaratorDecl &Declaration, const CXXThrowExpr &ThrowExpr) {
  // We want to check the function declaration for the attributes.
  const Decl *FuncDecl = Declaration.getCanonicalDecl();
  if (FuncDecl == nullptr) {
    FuncDecl = &Declaration;
  }

  if (hasThrowsExceptionAnnotation(*FuncDecl)) {
    return;
  }

  const char *ObjectTypeString = "Function";
  if (llvm::isa<CXXConstructorDecl>(Declaration)) {
    ObjectTypeString = "Constructor";
  } else if (llvm::isa<CXXDestructorDecl>(Declaration)) {
    ObjectTypeString = "Destructor";
  } else if (const auto *MethodDecl = llvm::dyn_cast<CXXMethodDecl>(FuncDecl)) {
    if (MethodDecl->getParent() != nullptr && MethodDecl->getParent()->isLambda()) {
      ObjectTypeString = "Lambda";
    } else {
      ObjectTypeString = "Method";
    }
  }

  auto Diagnostic = diag(ThrowExpr.getThrowLoc(), "%0 %1 can throw an exception but is not annotated as such.") << ObjectTypeString << Declaration.getDeclName();
  if (FuncDecl->getBeginLoc().isValid()) {
    Diagnostic << FixItHint::CreateInsertion(FuncDecl->getBeginLoc(), "[[clang::annotate(\"throws_exception\")]] ");
  }
}

void MissingExceptionAttributeCheck::checkLambdaAssignment(const DeclaratorDecl &Lhs, const DeclaratorDecl &Rhs) {
  if (hasThrowsExceptionAnnotation(Lhs) || !hasThrowsExceptionAnnotation(Rhs)) {
    return;
  }

  auto Diagnostic = diag(Lhs.getBeginLoc(), "'%0' assigned to a value that can throw an exception but is not annotated as such.") << Lhs.getName();
  if (Lhs.getBeginLoc().isValid()) {
    Diagnostic << FixItHint::CreateInsertion(Lhs.getBeginLoc(), "[[clang::annotate(\"throws_exception\")]] ");
  }
}
} // namespace clang::tidy::fault_line
