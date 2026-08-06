find_program(
    CLANG_TIDY_BIN
    NAMES clang-tidy
    HINTS ${LLVM_TOOLS_BINARY_DIR}
    REQUIRED
)
find_package(Python3 REQUIRED COMPONENTS Interpreter)

# Locate check_clang_tidy.py or download if not found
find_file(
    CHECK_CLANG_TIDY_PY
    NAMES check_clang_tidy.py
    HINTS
        ${LLVM_SOURCE_DIR}/tools/clang/tools/extra/test/clang-tidy
        ${LLVM_SOURCE_DIR}/../clang-tools-extra/test/clang-tidy
        /usr/share/clang
        /usr/local/share/clang
)

if(NOT CHECK_CLANG_TIDY_PY)
    message(
        STATUS
        "check_clang_tidy.py not found locally; downloading from LLVM repository..."
    )
    set(CHECK_CLANG_TIDY_PY "${CMAKE_CURRENT_BINARY_DIR}/check_clang_tidy.py")
    file(
        DOWNLOAD
            "https://raw.githubusercontent.com/llvm/llvm-project/release/${LLVM_VERSION_MAJOR}.x/clang-tools-extra/test/clang-tidy/check_clang_tidy.py"
        "${CHECK_CLANG_TIDY_PY}"
        STATUS DOWNLOAD_STATUS
    )
    list(GET DOWNLOAD_STATUS 0 STATUS_CODE)
    if(NOT STATUS_CODE EQUAL 0)
        message(
            STATUS
            "Download from release branch failed, trying main branch..."
        )
        file(
            DOWNLOAD
                "https://raw.githubusercontent.com/llvm/llvm-project/main/clang-tools-extra/test/clang-tidy/check_clang_tidy.py"
            "${CHECK_CLANG_TIDY_PY}"
            STATUS DOWNLOAD_STATUS
        )
        list(GET DOWNLOAD_STATUS 0 STATUS_CODE)
        if(NOT STATUS_CODE EQUAL 0)
            message(
                FATAL_ERROR
                "Failed to download check_clang_tidy.py: ${DOWNLOAD_STATUS}"
            )
        endif()
    endif()
endif()


message(STATUS "Using check_clang_tidy.py: ${CHECK_CLANG_TIDY_PY}")

# Add a clang-tidy plugin test using check_clang_tidy.py
function(add_clang_tidy_test TEST_NAME CHECKS)
    add_test(
        NAME ${TEST_NAME}
        COMMAND
            ${Python3_EXECUTABLE} ${CHECK_CLANG_TIDY_PY}
            ${CMAKE_CURRENT_SOURCE_DIR}/${TEST_NAME}.cpp ${CHECKS}
            ${CMAKE_CURRENT_BINARY_DIR}/${TEST_NAME}.cpp.tmp -std=c++11-or-later -fix-errors
            -load=$<TARGET_FILE:FaultLine> -- -std=c++11-or-later
    )
endfunction()

add_clang_tidy_test(test_caller_exception_attribute  "fault-line-caller-missing-exception-attribute")
add_clang_tidy_test(test_missing_exception_attribute "fault-line-missing-exception-attribute")
add_clang_tidy_test(test_must_not_throw_exception    "fault-line-must-not-throw")
add_clang_tidy_test(test_no_attribute_required       "fault-line-*")
add_clang_tidy_test(test_unhandleable_exception      "fault-line-unhandleable-exception")
add_clang_tidy_test(test_useless_exception_attribute "fault-line-useless-exception-attribute")
