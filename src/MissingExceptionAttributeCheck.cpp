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
namespace {
bool hasThrowsExceptionAnnotation(const Decl &Declaration) {
  if (!Declaration.hasAttrs()) {
    return false;
  }

  return std::any_of(Declaration.attrs().begin(), Declaration.attrs().end(), [](const Attr *Attr) {
    if (const auto *Annotate = dyn_cast<AnnotateAttr>(Attr)) {
      return Annotate->getAnnotation().contains("throws_exception");
    }
    return false;
  });
}
} // namespace

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
    ).bind("lhs"),
  this);
  // clang-format on

  // clang-format off
  Finder->addMatcher(
    varDecl(
      anyOf(
        // Assign to existing lambda
        hasDescendant(
          declRefExpr(
            to(
              varDecl(
                hasAttr(attr::Annotate)
              ).bind("assignee")
            )
          )
        ),

        // Assignment to new lambda
        hasDescendant(
          cxxThrowExpr(
            // TODO: Only match if the catch statement catches the throw type
            unless(hasAncestor(compoundStmt(hasParent(cxxTryStmt())))),
            // Don't match IIFE lambdas.
            // We want to match the containing function for IIFEs, but not the lambda itself.
            unless(hasAncestor(callExpr()))
          ).bind("throw_expr")
        )
      )
    ).bind("lhs"),
  this);
  // clang-format on
}

void MissingExceptionAttributeCheck::check(const MatchFinder::MatchResult &Result) {
  const auto *Lhs = Result.Nodes.getNodeAs<DeclaratorDecl>("lhs");
  if (Lhs == nullptr) {
    return;
  }

  const auto *ThrowExpr = Result.Nodes.getNodeAs<CXXThrowExpr>("throw_expr");
  if (ThrowExpr != nullptr) {
    checkFunction(*Lhs, *ThrowExpr);
    return;
  }

  const auto *Rhs = Result.Nodes.getNodeAs<VarDecl>("assignee");
  if (Lhs != nullptr && Rhs != nullptr) {
    checkLambdaAssignment(*Lhs, *Rhs);
    return;
  }
}

void MissingExceptionAttributeCheck::checkFunction(const DeclaratorDecl &Declaration, const CXXThrowExpr &ThrowExpr) {
  // We want to check the function declaration for the attributes.
  const Decl *FuncDecl = Declaration.getCanonicalDecl();
  if (FuncDecl == nullptr) {
    return;
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
    if (MethodDecl->getParent()->isLambda()) {
      ObjectTypeString = "Lambda";
    } else {
      ObjectTypeString = "Method";
    }
  }

  diag(ThrowExpr.getThrowLoc(), "%0 '%1' can throw an exception but is not annotated as such.")
      << ObjectTypeString << Declaration.getNameAsString() << FixItHint::CreateInsertion(FuncDecl->getBeginLoc(), "[[clang::annotate(\"throws_exception\")]] ");
}

void MissingExceptionAttributeCheck::checkLambdaAssignment(const DeclaratorDecl &Lhs, const VarDecl &Rhs) {
  if (hasThrowsExceptionAnnotation(Lhs) || !hasThrowsExceptionAnnotation(Rhs)) {
    return;
  }

  diag(Lhs.getBeginLoc(), "'%0' assigned to a value that can throw an exception but is not annotated as such.") << Lhs.getName() << FixItHint::CreateInsertion(Lhs.getBeginLoc(), "[[clang::annotate(\"throws_exception\")]] ");
}
} // namespace clang::tidy::fault_line
