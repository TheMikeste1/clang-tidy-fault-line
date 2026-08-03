#include "clang/tidy/fault_line/utilities.hpp"
#include <algorithm>
#include <clang/AST/Attr.h>
#include <clang/AST/DeclBase.h>
#include <clang/Basic/LLVM.h>
#include <llvm/Support/Casting.h>

namespace clang::tidy::fault_line {
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
} // namespace clang::tidy::fault_line
