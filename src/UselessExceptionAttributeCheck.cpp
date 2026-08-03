#include "clang/tidy/fault_line/UselessExceptionAttributeCheck.hpp"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/tidy/fault_line/utilities.hpp"
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
  // clang-format off
  Finder->addMatcher(
    functionDecl(
      hasAttr(attr::Annotate),
      unless(
        hasDescendant(
          cxxThrowExpr(
            // TODO: Only match if the catch statement catches the throw type
            unless(hasAncestor(compoundStmt(hasParent(cxxTryStmt()))))
          )
        )
      )
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
      hasAttr(attr::Annotate),
      unless(anyOf(
        ExistingCallableMatcher,
        NewLambdaMatcher
      ))
    ).bind("decl"),
  this);

  Finder->addMatcher(
    fieldDecl(
      hasAttr(attr::Annotate),
      unless(anyOf(
        ExistingCallableMatcher,
        NewLambdaMatcher
      ))
    ).bind("decl"),
  this);
  // clang-format on
}

void UselessExceptionAttributeCheck::check(const MatchFinder::MatchResult &Result) {
  const auto *Decl = Result.Nodes.getNodeAs<DeclaratorDecl>("decl");
  if (Decl == nullptr) {
    return;
  }

  if (hasThrowsExceptionAnnotation(*Decl)) {
    // TODO: Add removal
    diag(Decl->getBeginLoc(), "Function '%0' cannot throw an exception but is annotated as such.") << Decl->getName();
  }
}

} // namespace clang::tidy::fault_line
