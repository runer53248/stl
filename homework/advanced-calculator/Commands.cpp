#include "Commands.hpp"
#include <algorithm>
#include <cmath>
#include "Command.hpp"

const CommandsMap commands{
    {'+', Command{std::plus<double>{}}},
    {'-', Command{std::minus<double>{}}},
    {'*', Command{std::multiplies<double>{}}},
    {'/', Command{[](auto lhs, auto rhs) -> CallbackVariants {
         if (rhs == 0.0) {
             return ErrorCode::DivideBy0;
         }
         return lhs / rhs;
     }}},
    {'%', Command{[](auto lhs, auto rhs) -> CallbackVariants {
         Value int_part;
         if (std::modf(rhs, &int_part) != 0.0 or rhs == 0.0) {
             return ErrorCode::ModuleOfNonIntegerValue;
         }
         return std::fmod(lhs, rhs);
     }}},
    {'!', Command{[](auto lhs) {
         if (lhs >= 0.0) {
             return std::tgamma(lhs + 1);
         }
         return -std::tgamma(-lhs + 1);
     }}},
    {'^', Command{[](auto lhs, auto rhs) { return std::pow(lhs, rhs); }}},
    {'$', Command{[](auto lhs, auto rhs) -> CallbackVariants {
         if (rhs == 0.0) {
             return ErrorCode::DivideBy0;
         }
         if (lhs < 0.0) {
             return ErrorCode::SqrtOfNegativeNumber;
         }
         return std::pow(lhs, 1.0 / rhs);
     }}}};
