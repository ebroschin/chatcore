add_compile_options(
    /W4
    /WX
    /permissive-
    /Zc:__cplusplus

    # Extra warnings roughly aligned with -Wall/-Wextra/-Wpedantic on Clang/GCC
    /w14242 # conversion: possible loss of data
    /w14254 # operator: conversion from T to U, possible loss of data
    /w14263 # member function does not override any base class virtual member function
    /w14265 # class has virtual functions, but destructor is not virtual
    /w14287 # unsigned/negative constant mismatch
    /we4289 # nonstandard extension: loop control variable declared in for-loop used outside
    /w14296 # expression is always false
    /w14311 # pointer truncation from T to U
    /w14545 # expression before comma evaluates to a function missing an argument list
    /w14546 # function call before comma missing argument list
    /w14547 # operator before comma has no effect
    /w14549 # operator before comma has no effect; did you intend 'operator'?
    /w14555 # expression has no effect; expected expression with side-effect
    /w14619 # pragma warning: there is no warning number N
    /w14640 # thread construction with static storage duration may throw
    /w14826 # conversion from T to U is sign-extended
    /w14905 # wide string literal cast to LPSTR
    /w14906 # string literal cast to LPWSTR
    /w14928 # illegal copy-initialization; more than one user-defined conversion
)

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    add_compile_options(/Zc:preprocessor)
endif()
