#include "clang/tidy/fault_line/UselessExceptionAttributeCheck.hpp"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/tidy/fault_line/utilities.hpp"
#include <clang/AST/Attr.h>
#include <clang/AST/Decl.h>
#include <clang/AST/Expr.h>
#include <clang/AST/ExprCXX.h>
#include <clang/Basic/AttrKinds.h>
#include <clang/Basic/Diagnostic.h>
#include <clang/Basic/LLVM.h>
#include <llvm/ADT/StringRef.h>
#include <llvm/Support/Casting.h>

using namespace clang::ast_matchers;

namespace clang::tidy::fault_line {

void UselessExceptionAttributeCheck::registerMatchers(MatchFinder *Finder) {
  // clang-format off
  const auto CanThrowStmt = stmt(
    anyOf(
      // TODO: Only match if the catch statement catches the throw type
      cxxThrowExpr(unless(hasAncestor(compoundStmt(hasParent(cxxTryStmt()))))),
      callExpr(
        anyOf(
          callee(functionDecl(hasAttr(attr::Annotate))),
          hasDescendant(declRefExpr(to(decl(hasAttr(attr::Annotate))))),
          hasDescendant(memberExpr(hasDeclaration(fieldDecl(hasAttr(attr::Annotate)))))
        ),
        unless(hasAncestor(compoundStmt(hasParent(cxxTryStmt()))))
      )
    )
  );

  Finder->addMatcher(
    functionDecl(
      hasAttr(attr::Annotate),
      unless(hasDescendant(CanThrowStmt))
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

  if (!hasThrowsExceptionAnnotation(*Decl)) {
    return;
  }

  auto Diagnostic = diag(Decl->getBeginLoc(), "Function '%0' cannot throw an exception but is annotated as such.") << Decl->getName();
  for (const Attr *Attr : Decl->attrs()) {
    const auto *Annotate = dyn_cast<AnnotateAttr>(Attr);
    if (Annotate == nullptr) {
      continue;
    }

    if (Annotate->getAnnotation() == "throws_exception" || Annotate->getAnnotation().contains("throws_exception")) {
      if (Annotate->getLocation().isValid()) {
        Diagnostic << FixItHint::CreateRemoval(Annotate->getRange());
      }
      break;
    }
  }
}
} // namespace clang::tidy::fault_line
