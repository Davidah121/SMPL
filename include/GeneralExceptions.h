#pragma once
#include<exception>

struct OutOfBoundsError : public std::exception
{
    const char* what() noexcept { return "Out of bounds access error."; }
};

struct InvalidSizeError : public std::exception
{
    const char* what() noexcept { return "Invalid Input Size"; }
};

struct InvalidDataError : public std::exception
{
    const char* what() noexcept { return "Invalid Input Data"; }
};