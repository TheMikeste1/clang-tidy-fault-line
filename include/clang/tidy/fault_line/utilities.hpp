#pragma once

#include <clang/AST/DeclBase.h>

namespace clang::tidy::fault_line {
bool hasThrowsExceptionAnnotation(const Decl &Declaration);
} // namespace clang::tidy::fault_line
