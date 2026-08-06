#include "clang/tidy/fault_line/utilities.hpp"
#include <algorithm>
#include <clang/AST/Attr.h>
#include <clang/AST/Decl.h>
#include <clang/AST/DeclBase.h>
#include <clang/AST/DeclCXX.h>
#include <clang/Basic/LLVM.h>
#include <llvm/Support/Casting.h>

namespace clang::tidy::fault_line {

namespace {
bool checkSingleDeclAttrs(const Decl &Declaration) {
  if (!Declaration.hasAttrs()) {
    return false;
  }

  return std::any_of(Declaration.attrs().begin(), Declaration.attrs().end(), [](const Attr *Attr) {
    if (const auto *Annotate = dyn_cast<AnnotateAttr>(Attr)) {
      return Annotate->getAnnotation() == "throws_exception" || Annotate->getAnnotation().contains("throws_exception");
    }
    return false;
  });
}
} // namespace

bool hasThrowsExceptionAnnotation(const Decl &Declaration) {
  for (const Decl *Redecl : Declaration.redecls()) {
    if (Redecl != nullptr && checkSingleDeclAttrs(*Redecl)) {
      return true;
    }
  }

  if (const auto *Method = dyn_cast<CXXMethodDecl>(&Declaration)) {
    if (Method->getParent() != nullptr && Method->getParent()->isLambda()) {
      if (const Decl *ContextDecl = Method->getParent()->getLambdaContextDecl()) {
        for (const Decl *Redecl : ContextDecl->redecls()) {
          if (Redecl != nullptr && checkSingleDeclAttrs(*Redecl)) {
            return true;
          }
        }
      }
    }
  }

  return false;
}
} // namespace clang::tidy::fault_line
