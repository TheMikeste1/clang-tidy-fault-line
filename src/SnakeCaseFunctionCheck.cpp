#include "clang/tidy/fault_line/SnakeCaseFunctionCheck.hpp"
#include "clang/AST/ASTContext.h"
#include "clang/AST/Decl.h"
#include "clang/AST/DeclCXX.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include <cctype>

using namespace clang::ast_matchers;

namespace clang::tidy::fault_line {

static std::string toSnakeCase(StringRef Name) {
  std::string Result;
  for (size_t i = 0; i < Name.size(); ++i) {
    char C = Name[i];
    if (std::isupper(C)) {
      if (i > 0 && Name[i - 1] != '_' && !std::isupper(Name[i - 1])) {
        Result += '_';
      }
      Result += static_cast<char>(std::tolower(C));
    } else {
      Result += C;
    }
  }
  return Result;
}

static bool isLowerSnakeCase(StringRef Name) {
  for (char C : Name) {
    if (std::isupper(C))
      return false;
  }
  return true;
}

void SnakeCaseFunctionCheck::registerMatchers(MatchFinder *Finder) {
  Finder->addMatcher(functionDecl(unless(isImplicit())).bind("func"), this);
}

void SnakeCaseFunctionCheck::check(const MatchFinder::MatchResult &Result) {
  const auto *FD = Result.Nodes.getNodeAs<FunctionDecl>("func");
  if (!FD || FD->isMain() || FD->isOverloadedOperator())
    return;

  // Ignore constructors and destructors
  if (isa<CXXConstructorDecl>(FD) || isa<CXXDestructorDecl>(FD))
    return;

  // Ignore extern "C" functions
  if (FD->isExternC())
    return;

  StringRef Name = FD->getName();
  if (Name.empty() || isLowerSnakeCase(Name))
    return;

  std::string SuggestedName = toSnakeCase(Name);

  auto Diag = diag(FD->getLocation(),
                   "function name '%0' should use lower_snake_case; consider '%1'")
              << Name << SuggestedName;

  // Add Fix-It hint to rename the identifier token
  Diag << FixItHint::CreateReplacement(
      CharSourceRange::getTokenRange(FD->getLocation()), SuggestedName);
}

} // namespace clang::tidy::fault_line

