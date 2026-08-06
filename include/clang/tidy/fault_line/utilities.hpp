#pragma once

#include <clang/AST/DeclBase.h>

namespace clang::tidy::fault_line {

/**
 * @brief Checks whether a given declaration (or any of its redeclarations or lambda context)
 * is annotated with the [[clang::annotate("throws_exception")]] attribute.
 *
 * @param Declaration The AST declaration to check.
 * @return true if the declaration is annotated with "throws_exception", false otherwise.
 */
bool hasThrowsExceptionAnnotation(const Decl &Declaration);

} // namespace clang::tidy::fault_line
