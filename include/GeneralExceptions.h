#pragma once
#include<exception>

namespace smpl
{

    struct OutOfBoundsError : public std::exception
    {
        const char* what() const noexcept { return "Out of bounds access error."; }
    };

    struct InvalidSizeError : public std::exception
    {
        const char* what() const noexcept { return "Invalid Input Size"; }
    };

    struct InvalidDataError : public std::exception
    {
        const char* what() const noexcept { return "Invalid Input Data"; }
    };

} //NAMESPACE smpl END