// Copyright 2022 The Tint Authors.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef SRC_TINT_LANG_CORE_CONSTANT_EVAL_H_
#define SRC_TINT_LANG_CORE_CONSTANT_EVAL_H_

#include <stddef.h>
#include <string>

#include "src/tint/lang/core/number.h"
#include "src/tint/lang/core/type/type.h"
#include "src/tint/utils/containers/vector.h"
#include "src/tint/utils/result/result.h"

// Forward declarations
namespace tint {
class Source;
}  // namespace tint
namespace tint::core::constant {
class Manager;
class Value;
}  // namespace tint::core::constant
namespace tint::diag {
class List;
}  // namespace tint::diag

namespace tint::core::constant {

/// Eval performs shader creation-time (const-expression) expression evaluation.
class Eval {
  public:
    /// The result type of a method that may raise a diagnostic error, upon which the caller should
    /// handle the error. Can be one of three distinct values:
    /// * A non-null Value pointer. Returned when a expression resolves to a creation
    ///   time value.
    /// * A null Value pointer. Returned when a expression cannot resolve to a creation time value,
    ///   but is otherwise legal.
    /// * `tint::Failure`. Returned when there was an error. In this situation the method will have
    ///   already reported a diagnostic error message, and the caller should abort resolving.
    using Result = tint::Result<const Value*>;

    /// Typedef for a constant evaluation function
    using Function = Result (Eval::*)(const type::Type* result_ty,
                                      VectorRef<const Value*>,
                                      const Source&);

    /// Constructor
    /// @param manager the constant manager
    /// @param diagnostics the diagnostic list, used to report errors and warnings
    /// @param use_runtime_semantics if `true`, use the behavior defined for runtime evaluation, and
    ///                              emit overflow and range errors as warnings instead of errors
    Eval(Manager& manager, diag::List& diagnostics, bool use_runtime_semantics = false);

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Constant value evaluation methods
    ////////////////////////////////////////////////////////////////////////////////////////////////

    /// @param ty the target type - must be an array or struct
    /// @param args the input arguments
    /// @return the constructed value, or null if the value cannot be calculated
    Result ArrayOrStructCtor(const type::Type* ty, VectorRef<const Value*> args);

    /// @param ty the target type
    /// @param value the value being converted
    /// @param source the source location
    /// @return the bit-cast of the given expression to the given type, or null if the value cannot
    ///         be calculated
    Result Bitcast(const type::Type* ty, const Value* value, const Source& source);

    /// @param obj the object being indexed. May be null, in which case Index() will still validate
    /// the index is in bounds for the type.
    /// @param obj_ty the type of the object being indexed.
    /// @param idx the index value.
    /// @param idx_source the source of the index expression
    /// @return the result of the index, or null if the value cannot be calculated
    Result Index(const Value* obj,
                 const type::Type* obj_ty,
                 const Value* idx,
                 const Source& idx_source);

    /// @param ty the result type
    /// @param vector the vector being swizzled
    /// @param indices the swizzle indices
    /// @return the result of the swizzle, or null if the value cannot be calculated
    Result Swizzle(const type::Type* ty, const Value* vector, VectorRef<uint32_t> indices);

    /// Convert the `value` to `target_type`
    /// @param ty the result type
    /// @param value the value being converted
    /// @param source the source location
    /// @return the converted value, or null if the value cannot be calculated
    Result Convert(const type::Type* ty, const Value* value, const Source& source);

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Constant value evaluation methods, to be indirectly called via the intrinsic table
    ////////////////////////////////////////////////////////////////////////////////////////////////

    /// Value conversion
    /// @param ty the result type
    /// @param args the input arguments
    /// @param source the source location
    /// @return the converted value, or null if the value cannot be calculated
    Result Conv(const type::Type* ty, VectorRef<const Value*> args, const Source& source);

    /// Zero value constructor
    /// @param ty the result type
    /// @param args the input arguments (no arguments provided)
    /// @param source the source location
    /// @return the constructed value, or null if the value cannot be calculated
    Result Zero(const type::Type* ty, VectorRef<const Value*> args, const Source& source);

    /// Identity value constructor
    /// @param ty the result type
    /// @param args the input arguments
    /// @param source the source location
    /// @return the constructed value, or null if the value cannot be calculated
    Result Identity(const type::Type* ty, VectorRef<const Value*> args, const Source& source);

    /// Vector splat constructor
    /// @param ty the vector type
    /// @param args the input arguments
    /// @param source the source location
    /// @return the constructed value, or null if the value cannot be calculated
    Result VecSplat(const type::Type* ty, VectorRef<const Value*> args, const Source& source);

    /// Vector constructor using scalars
    /// @param ty the vector type
    /// @param args the input arguments
    /// @param source the source location
    /// @return the constructed value, or null if the value cannot be calculated
    Result VecInitS(const type::Type* ty, VectorRef<const Value*> args, const Source& source);

    /// Vector constructor using a mix of scalars and smaller vectors
    /// @param ty the vector type
    /// @param args the input arguments
    /// @param source the source location
    /// @return the constructed value, or null if the value cannot be calculated
    Result VecInitM(const type::Type* ty, VectorRef<const Value*> args, const Source& source);

    /// Matrix constructor using scalar values
    /// @param ty the matrix type
    /// @param args the input arguments
    /// @param source the source location
    /// @return the constructed value, or null if the value cannot be calculated
    Result MatInitS(const type::Type* ty, VectorRef<const Value*> args, const Source& source);

    /// Matrix constructor using column vectors
    /// @param ty the matrix type
    /// @param args the input arguments
    /// @param source the source location
    /// @return the constructed value, or null if the value cannot be calculated
    Result MatInitV(const type::Type* ty, VectorRef<const Value*> args, const Source& source);

    ////////////////////////////////////////////////////////////////////////////
    // Unary Operators
    ////////////////////////////////////////////////////////////////////////////

    /// Complement operator '~'
    /// @param ty the integer type
    /// @param args the input arguments
    /// @param source the source location
    /// @return the result value, or null if the value cannot be calculated
    Result Complement(const type::Type* ty, VectorRef<const Value*> args, const Source& source);

    /// Unary minus operator '-'
    /// @param ty the expression type
    /// @param args the input arguments
    /// @param source the source location
    /// @return the result value, or null if the value cannot be calculated
    Result UnaryMinus(const type::Type* ty, VectorRef<const Value*> args, const Source& source);

    /// Unary not operator '!'
    /// @param ty the expression type
    /// @param args the input arguments
    /// @param source the source location
    /// @return the result value, or null if the value cannot be calculated
    Result Not(const type::Type* ty, VectorRef<const Value*> args, const Source& source);

    ////////////////////////////////////////////////////////////////////////////
    // Binary Operators
    ////////////////////////////////////////////////////////////////////////////

    /// Plus operator '+'
    /// @param ty the expression type
    /// @param args the input arguments
    /// @param source the source location
    /// @return the result value, or null if the value cannot be calculated
    Result Plus(const type::Type* ty, VectorRef<const Value*> args, const Source& source);

    /// Minus operator '-'
    /// @param ty the expression type
    /// @param args the input arguments
    /// @param source the source location
    /// @return the result value, or null if the value cannot be calculated
    Result Minus(const type::Type* ty, VectorRef<const Value*> args, const Source& source);

    /// Multiply operator '*' for the same type on the LHS and RHS
    /// @param ty the expression type
    /// @param args the input arguments
    /// @param source the source location
    /// @return the result value, or null if the value cannot be calculated
    Result Multiply(const type::Type* ty, VectorRef<const Value*> args, const Source& source);

    /// Multiply operator '*' for matCxR<T> * vecC<T>
    /// @param ty the expression type
    /// @param args the input arguments
    /// @param source the source location
    /// @return the result value, or null if the value cannot be calculated
    Result MultiplyMatVec(const type::Type* ty, VectorRef<const Value*> args, const Source& source);

    /// Multiply operator '*' for vecR<T> * matCxR<T>
    /// @param ty the expression type
    /// @param args the input arguments
    /// @param source the source location
    /// @return the result value, or null if the value cannot be calculated
    Result MultiplyVecMat(const type::Type* ty, VectorRef<const Value*> args, const Source& source);

    /// Multiply operator '*' for matKxR<T> * matCxK<T>
    /// @param ty the expression type
    /// @param args the input arguments
    /// @param source the source location
    /// @return the result value, or null if the value cannot be calculated
    Result MultiplyMatMat(const type::Type* ty, VectorRef<const Value*> args, const Source& source);

    /// Divide operator '/'
    /// @param ty the expression type
    /// @param args the input arguments
    /// @param source the source location
    /// @return the result value, or null if the value cannot be calculated
    Result Divide(const type::Type* ty, VectorRef<const Value*> args, const Source& source);

    /// Modulo operator '%'
    /// @param ty the expression type
    /// @param args the input arguments
    /// @param source the source location
    /// @return the result value, or null if the value cannot be calculated
    Result Modulo(const type::Type* ty, VectorRef<const Value*> args, const Source& source);

    /// Equality operator '=='
    /// @param ty the expression type
    /// @param args the input arguments
    /// @param source the source location
    /// @return the result value, or null if the value cannot be calculated
    Result Equal(const type::Type* ty, VectorRef<const Value*> args, const Source& source);

    /// Inequality operator '!='
    /// @param ty the expression type
    /// @param args the input arguments
    /// @param source the source location
    /// @return the result value, or null if the value cannot be calculated
    Result NotEqual(const type::Type* ty, VectorRef<const Value*> args, const Source& source);

    /// Less than operator '<'
    /// @param ty the expression type
    /// @param args the input arguments
    /// @param source the source location
    /// @return the result value, or null if the value cannot be calculated
    Result LessThan(const type::Type* ty, VectorRef<const Value*> args, const Source& source);

    /// Greater than operator '>'
    /// @param ty the expression type
    /// @param args the input arguments
    /// @param source the source location
    /// @return the result value, or null if the value cannot be calculated
    Result GreaterThan(const type::Type* ty, VectorRef<const Value*> args, const Source& source);

    /// Less than or equal operator '<='
    /// @param ty the expression type
    /// @param args the input arguments
    /// @param source the source location
    /// @return the result value, or null if the value cannot be calculated
    Result LessThanEqual(const type::Type* ty, VectorRef<const Value*> args, const Source& source);

    /// Greater than or equal operator '>='
    /// @param ty the expression type
    /// @param args the input arguments
    /// @param source the source location
    /// @return the result value, or null if the value cannot be calculated
    Result GreaterThanEqual(const type::Type* ty,
                            VectorRef<const Value*> args,
                            const Source& source);

    /// Logical and operator '&&'
    /// @param ty the expression type
    /// @param args the input arguments
    /// @param source the source location
    /// @return the result value, or null if the value cannot be calculated
    Result LogicalAnd(const type::Type* ty, VectorRef<const Value*> args, const Source& source);

    /// Logical or operator '||'
    /// @param ty the expression type
    /// @param args the input arguments
    /// @param source the source location
    /// @return the result value, or null if the value cannot be calculated
    Result LogicalOr(const type::Type* ty, VectorRef<const Value*> args, const Source& source);

    /// Bitwise and operator '&'
    /// @param ty the expression type
    /// @param args the input arguments
    /// @param source the source location
    /// @return the result value, or null if the value cannot be calculated
    Result And(const type::Type* ty, VectorRef<const Value*> args, const Source& source);

    /// Bitwise or operator '|'
    /// @param ty the expression type
    /// @param args the input arguments
    /// @param source the source location
    /// @return the result value, or null if the value cannot be calculated
    Result Or(const type::Type* ty, VectorRef<const Value*> args, const Source& source);

    /// Bitwise xor operator '^'
    /// @param ty the expression type
    /// @param args the input arguments
    /// @param source the source location
    /// @return the result value, or null if the value cannot be calculated
    Result Xor(const type::Type* ty, VectorRef<const Value*> args, const Source& source);

    /// Bitwise shift left operator '<<'
    /// @param ty the expression type
    /// @param args the input arguments
    /// @param source the source location
    /// @return the result value, or null if the value cannot be calculated
    Result ShiftLeft(const type::Type* ty, VectorRef<const Value*> args, const Source& source);

    /// Bitwise shift right operator '<<'
    /// @param ty the expression type
    /// @param args the input arguments
    /// @param source the source location
    /// @return the result value, or null if the value cannot be calculated
    Result ShiftRight(const type::Type* ty, VectorRef<const Value*> args, const Source& source);

    ////////////////////////////////////////////////////////////////////////////
    // Builtins
    ////////////////////////////////////////////////////////////////////////////

    /// abs builtin
    /// @param ty the expression type
    /// @param args the input arguments
    /// @param source the source location
    /// @return the result value, or null if the value cannot be calculated
    Result abs(const type::Type* ty, VectorRef<const Value*> args, const Source& source);

    /// acos builtin
    /// @param ty the expression type
    /// @param args the input arguments
    /// @param source the source location
    /// @return the result value, or null if the value cannot be calculated
    Result acos(const type::Type* ty, VectorRef<const Value*> args, const Source& source);

    /// acosh builtin
    /// @param ty the expression type
    /// @param args the input arguments
    /// @param source the source location
    /// @return the result value, or null if the value cannot be calculated
    Result acosh(const type::Type* ty, VectorRef<const Value*> args, const Source& source);

    /// all builtin
    /// @param ty the expression type
    /// @param args the input arguments
    /// @param source the source location
    /// @return the result value, or null if the value cannot be calculated
    Result all(const type::Type* ty, VectorRef<const Value*> args, const Source& source);

    /// any builtin
    /// @param ty the expression type
    /// @param args the input arguments
    /// @param source the source location
    /// @return the result value, or null if the value cannot be calculated
    Result any(const type::Type* ty, VectorRef<const Value*> args, const Source& source);

    /// asin builtin
    /// @param ty the expression type
    /// @param args the input arguments
    /// @param source the source location
    /// @return the result value, or null if the value cannot be calculated
    Result asin(const type::Type* ty, VectorRef<const Value*> args, const Source& source);

    /// asinh builtin
    /// @param ty the expression type
    /// @param args the input arguments
    /// @param source the source location
    /// @return the result value, or null if the value cannot be calculated
    Result asinh(const type::Type* ty, VectorRef<const Value*> args, const Source& source);

    /// atan builtin
    /// @param ty the expression type
    /// @param args the input arguments
    /// @param source the source location
    /// @return the result value, or null if the value cannot be calculated
    Result atan(const type::Type* ty, VectorRef<const Value*> args, const Source& source);

    /// atanh builtin
    /// @param ty the expression type
    /// @param args the input arguments
    /// @param source the source location
    /// @return the result value, or null if the value cannot be calculated
    Result atanh(const type::Type* ty, VectorRef<const Value*> args, const Source& source);

    /// atan2 builtin
    /// @param ty the expression type
    /// @param args the input arguments
    /// @param source the source location
    /// @return the result value, or null if the value cannot be calculated
    Result atan2(const type::Type* ty, VectorRef<const Value*> args, const Source& source);

    /// ceil builtin
    /// @param ty the expression type
    /// @param args the input arguments
    /// @param source the source location
    /// @return the result value, or null if the value cannot be calculated
    Result ceil(const type::Type* ty, VectorRef<const Value*> args, const Source& source);

    /// clamp builtin
    /// @param ty the expression type
    /// @param args the input arguments
    /// @param source the source location
    /// @return the result value, or null if the value cannot be calculated
    Result clamp(const type::Type* ty, VectorRef<const Value*> args, const Source& source);

    /// cos builtin
    /// @param ty the expression type
    /// @param args the input arguments
    /// @param source the source location
    /// @return the result value, or null if the value cannot be calculated
    Result cos(const type::Type* ty, VectorRef<const Value*> args, const Source& source);

    /// cosh builtin
    /// @param ty the expression type
    /// @param args the input arguments
    /// @param source the source location
    /// @return the result value, or null if the value cannot be calculated
    Result cosh(const type::Type* ty, VectorRef<const Value*> args, const Source& source);

    /// countLeadingZeros builtin
    /// @param ty the expression type
    /// @param args the input arguments
    /// @param source the source location
    /// @return the result value, or null if the value cannot be calculated
    Result countLeadingZeros(const type::Type* ty,
                             VectorRef<const Value*> args,
                             const Source& source);

    /// countOneBits builtin
    /// @param ty the expression type
    /// @param args the input arguments
    /// @param source the source location
    /// @return the result value, or null if the value cannot be calculated
    Result countOneBits(const type::Type* ty, VectorRef<const Value*> args, const Source& source);

    /// countTrailingZeros builtin
    /// @param ty the expression type
    /// @param args the input arguments
    /// @param source the source location
    /// @return the result value, or null if the value cannot be calculated
    Result countTrailingZeros(const type::Type* ty,
                              VectorRef<const Value*> args,
                              const Source& source);

    /// cross builtin
    /// @param ty the expression type
    /// @param args the input arguments
    /// @param source the source location
    /// @return the result value, or null if the value cannot be calculated
    Result cross(const type::Type* ty, VectorRef<const Value*> args, const Source& source);

    /// degrees builtin
    /// @param ty the expression type
    /// @param args the input arguments
    /// @param source the source location of the conversion
    /// @return the result value, or null if the value cannot be calculated
    Result degrees(const type::Type* ty, VectorRef<const Value*> args, const Source& source);

    /// determinant builtin
    /// @param ty the expression type
    /// @param args the input arguments
    /// @param source the source location of the conversion
    /// @return the result value, or null if the value cannot be calculated
    Result determinant(const type::Type* ty, VectorRef<const Value*> args, const Source& source);

    /// distance builtin
    /// @param ty the expression type
    /// @param args the input arguments
    /// @param source the source location of the conversion
    /// @return the result value, or null if the value cannot be calculated
    Result distance(const type::Type* ty, VectorRef<const Value*> args, const Source& source);

    /// dot builtin
    /// @param ty the expression type
    /// @param args the input arguments
    /// @param source the source location
    /// @return the result value, or null if the value cannot be calculated
    Result dot(const type::Type* ty, VectorRef<const Value*> args, const Source& source);

    /// exp builtin
    /// @param ty the expression type
    /// @param args the input arguments
    /// @param source the source location
    /// @return the result value, or null if the value cannot be calculated
    Result exp(const type::Type* ty, VectorRef<const Value*> args, const Source& source);

    /// exp2 builtin
    /// @param ty the expression type
    /// @param args the input arguments
    /// @param source the source location
    /// @return the result value, or null if the value cannot be calculated
    Result exp2(const type::Type* ty, VectorRef<const Value*> args, const Source& source);

    /// extractBits builtin
    /// @param ty the expression type
    /// @param args the input arguments
    /// @param source the source location
    /// @return the result value, or null if the value cannot be calculated
    Result extractBits(const type::Type* ty, VectorRef<const Value*> args, const Source& source);

    /// faceForward builtin
    /// @param ty the expression type
    /// @param args the input arguments
    /// @param source the source location
    /// @return the result value, or null if the value cannot be calculated
    Result faceForward(const type::Type* ty, VectorRef<const Value*> args, const Source& source);

    /// firstLeadingBit builtin
    /// @param ty the expression type
    /// @param args the input arguments
    /// @param source the source location
    /// @return the result value, or null if the value cannot be calculated
    Result firstLeadingBit(const type::Type* ty,
                           VectorRef<const Value*> args,
                           const Source& source);

    /// firstTrailingBit builtin
    /// @param ty the expression type
    /// @param args the input arguments
    /// @param source the source location
    /// @return the result value, or null if the value cannot be calculated
    Result firstTrailingBit(const type::Type* ty,
                            VectorRef<const Value*> args,
                            const Source& source);

    /// floor builtin
    /// @param ty the expression type
    /// @param args the input arguments
    /// @param source the source location
    /// @return the result value, or null if the value cannot be calculated
    Result floor(const type::Type* ty, VectorRef<const Value*> args, const Source& source);

    /// fma builtin
    /// @param ty the expression type
    /// @param args the input arguments
    /// @param source the source location
    /// @return the result value, or null if the value cannot be calculated
    Result fma(const type::Type* ty, VectorRef<const Value*> args, const Source& source);

    /// fract builtin
    /// @param ty the expression type
    /// @param args the input arguments
    /// @param source the source location
    /// @return the result value, or null if the value cannot be calculated
    Result fract(const type::Type* ty, VectorRef<const Value*> args, const Source& source);

    /// frexp builtin
    /// @param ty the expression type
    /// @param args the input arguments
    /// @param source the source location
    /// @return the result value, or null if the value cannot be calculated
    Result frexp(const type::Type* ty, VectorRef<const Value*> args, const Source& source);

    /// insertBits builtin
    /// @param ty the expression type
    /// @param args the input arguments
    /// @param source the source location
    /// @return the result value, or null if the value cannot be calculated
    Result insertBits(const type::Type* ty, VectorRef<const Value*> args, const Source& source);

    /// inverseSqrt builtin
    /// @param ty the expression type
    /// @param args the input arguments
    /// @param source the source location
    /// @return the result value, or null if the value cannot be calculated
    Result inverseSqrt(const type::Type* ty, VectorRef<const Value*> args, const Source& source);

    /// ldexp builtin
    /// @param ty the expression type
    /// @param args the input arguments
    /// @param source the source location
    /// @return the result value, or null if the value cannot be calculated
    Result ldexp(const type::Type* ty, VectorRef<const Value*> args, const Source& source);

    /// length builtin
    /// @param ty the expression type
    /// @param args the input arguments
    /// @param source the source location
    /// @return the result value, or null if the value cannot be calculated
    Result length(const type::Type* ty, VectorRef<const Value*> args, const Source& source);

    /// log builtin
    /// @param ty the expression type
    /// @param args the input arguments
    /// @param source the source location
    /// @return the result value, or null if the value cannot be calculated
    Result log(const type::Type* ty, VectorRef<const Value*> args, const Source& source);

    /// log2 builtin
    /// @param ty the expression type
    /// @param args the input arguments
    /// @param source the source location
    /// @return the result value, or null if the value cannot be calculated
    Result log2(const type::Type* ty, VectorRef<const Value*> args, const Source& source);

    /// max builtin
    /// @param ty the expression type
    /// @param args the input arguments
    /// @param source the source location
    /// @return the result value, or null if the value cannot be calculated
    Result max(const type::Type* ty, VectorRef<const Value*> args, const Source& source);

    /// min builtin
    /// @param ty the expression type
    /// @param args the input arguments
    /// @param source the source location
    /// @return the result value, or null if the value cannot be calculated
    Result min(const type::Type* ty,  // NOLINT(build/include_what_you_use)  -- confused by min
               VectorRef<const Value*> args,
               const Source& source);

    /// mix builtin
    /// @param ty the expression type
    /// @param args the input arguments
    /// @param source the source location
    /// @return the result value, or null if the value cannot be calculated
    Result mix(const type::Type* ty, VectorRef<const Value*> args, const Source& source);

    /// modf builtin
    /// @param ty the expression type
    /// @param args the input arguments
    /// @param source the source location
    /// @return the result value, or null if the value cannot be calculated
    Result modf(const type::Type* ty, VectorRef<const Value*> args, const Source& source);

    /// normalize builtin
    /// @param ty the expression type
    /// @param args the input arguments
    /// @param source the source location
    /// @return the result value, or null if the value cannot be calculated
    Result normalize(const type::Type* ty, VectorRef<const Value*> args, const Source& source);

    /// pack2x16float builtin
    /// @param ty the expression type
    /// @param args the input arguments
    /// @param source the source location
    /// @return the result value, or null if the value cannot be calculated
    Result pack2x16float(const type::Type* ty, VectorRef<const Value*> args, const Source& source);

    /// pack2x16snorm builtin
    /// @param ty the expression type
    /// @param args the input arguments
    /// @param source the source location
    /// @return the result value, or null if the value cannot be calculated
    Result pack2x16snorm(const type::Type* ty, VectorRef<const Value*> args, const Source& source);

    /// pack2x16unorm builtin
    /// @param ty the expression type
    /// @param args the input arguments
    /// @param source the source location
    /// @return the result value, or null if the value cannot be calculated
    Result pack2x16unorm(const type::Type* ty, VectorRef<const Value*> args, const Source& source);

    /// pack4x8snorm builtin
    /// @param ty the expression type
    /// @param args the input arguments
    /// @param source the source location
    /// @return the result value, or null if the value cannot be calculated
    Result pack4x8snorm(const type::Type* ty, VectorRef<const Value*> args, const Source& source);

    /// pack4x8unorm builtin
    /// @param ty the expression type
    /// @param args the input arguments
    /// @param source the source location
    /// @return the result value, or null if the value cannot be calculated
    Result pack4x8unorm(const type::Type* ty, VectorRef<const Value*> args, const Source& source);

    /// pow builtin
    /// @param ty the expression type
    /// @param args the input arguments
    /// @param source the source location
    /// @return the result value, or null if the value cannot be calculated
    Result pow(const type::Type* ty, VectorRef<const Value*> args, const Source& source);

    /// radians builtin
    /// @param ty the expression type
    /// @param args the input arguments
    /// @param source the source location of the conversion
    /// @return the result value, or null if the value cannot be calculated
    Result radians(const type::Type* ty, VectorRef<const Value*> args, const Source& source);

    /// reflect builtin
    /// @param ty the expression type
    /// @param args the input arguments
    /// @param source the source location of the conversion
    /// @return the result value, or null if the value cannot be calculated
    Result reflect(const type::Type* ty, VectorRef<const Value*> args, const Source& source);

    /// refract builtin
    /// @param ty the expression type
    /// @param args the input arguments
    /// @param source the source location of the conversion
    /// @return the result value, or null if the value cannot be calculated
    Result refract(const type::Type* ty, VectorRef<const Value*> args, const Source& source);

    /// reverseBits builtin
    /// @param ty the expression type
    /// @param args the input arguments
    /// @param source the source location
    /// @return the result value, or null if the value cannot be calculated
    Result reverseBits(const type::Type* ty, VectorRef<const Value*> args, const Source& source);

    /// round builtin
    /// @param ty the expression type
    /// @param args the input arguments
    /// @param source the source location
    /// @return the result value, or null if the value cannot be calculated
    Result round(const type::Type* ty, VectorRef<const Value*> args, const Source& source);

    /// saturate builtin
    /// @param ty the expression type
    /// @param args the input arguments
    /// @param source the source location
    /// @return the result value, or null if the value cannot be calculated
    Result saturate(const type::Type* ty, VectorRef<const Value*> args, const Source& source);

    /// select builtin with single bool third arg
    /// @param ty the expression type
    /// @param args the input arguments
    /// @param source the source location
    /// @return the result value, or null if the value cannot be calculated
    Result select_bool(const type::Type* ty, VectorRef<const Value*> args, const Source& source);

    /// select builtin with vector of bool third arg
    /// @param ty the expression type
    /// @param args the input arguments
    /// @param source the source location
    /// @return the result value, or null if the value cannot be calculated
    Result select_boolvec(const type::Type* ty, VectorRef<const Value*> args, const Source& source);

    /// sign builtin
    /// @param ty the expression type
    /// @param args the input arguments
    /// @param source the source location
    /// @return the result value, or null if the value cannot be calculated
    Result sign(const type::Type* ty, VectorRef<const Value*> args, const Source& source);

    /// sin builtin
    /// @param ty the expression type
    /// @param args the input arguments
    /// @param source the source location
    /// @return the result value, or null if the value cannot be calculated
    Result sin(const type::Type* ty, VectorRef<const Value*> args, const Source& source);

    /// sinh builtin
    /// @param ty the expression type
    /// @param args the input arguments
    /// @param source the source location
    /// @return the result value, or null if the value cannot be calculated
    Result sinh(const type::Type* ty, VectorRef<const Value*> args, const Source& source);

    /// smoothstep builtin
    /// @param ty the expression type
    /// @param args the input arguments
    /// @param source the source location
    /// @return the result value, or null if the value cannot be calculated
    Result smoothstep(const type::Type* ty, VectorRef<const Value*> args, const Source& source);

    /// step builtin
    /// @param ty the expression type
    /// @param args the input arguments
    /// @param source the source location
    /// @return the result value, or null if the value cannot be calculated
    Result step(const type::Type* ty, VectorRef<const Value*> args, const Source& source);

    /// sqrt builtin
    /// @param ty the expression type
    /// @param args the input arguments
    /// @param source the source location
    /// @return the result value, or null if the value cannot be calculated
    Result sqrt(const type::Type* ty, VectorRef<const Value*> args, const Source& source);

    /// tan builtin
    /// @param ty the expression type
    /// @param args the input arguments
    /// @param source the source location
    /// @return the result value, or null if the value cannot be calculated
    Result tan(const type::Type* ty, VectorRef<const Value*> args, const Source& source);

    /// tanh builtin
    /// @param ty the expression type
    /// @param args the input arguments
    /// @param source the source location
    /// @return the result value, or null if the value cannot be calculated
    Result tanh(const type::Type* ty, VectorRef<const Value*> args, const Source& source);

    /// transpose builtin
    /// @param ty the expression type
    /// @param args the input arguments
    /// @param source the source location
    /// @return the result value, or null if the value cannot be calculated
    Result transpose(const type::Type* ty, VectorRef<const Value*> args, const Source& source);

    /// trunc builtin
    /// @param ty the expression type
    /// @param args the input arguments
    /// @param source the source location
    /// @return the result value, or null if the value cannot be calculated
    Result trunc(const type::Type* ty, VectorRef<const Value*> args, const Source& source);

    /// unpack2x16float builtin
    /// @param ty the expression type
    /// @param args the input arguments
    /// @param source the source location
    /// @return the result value, or null if the value cannot be calculated
    Result unpack2x16float(const type::Type* ty,
                           VectorRef<const Value*> args,
                           const Source& source);

    /// unpack2x16snorm builtin
    /// @param ty the expression type
    /// @param args the input arguments
    /// @param source the source location
    /// @return the result value, or null if the value cannot be calculated
    Result unpack2x16snorm(const type::Type* ty,
                           VectorRef<const Value*> args,
                           const Source& source);

    /// unpack2x16unorm builtin
    /// @param ty the expression type
    /// @param args the input arguments
    /// @param source the source location
    /// @return the result value, or null if the value cannot be calculated
    Result unpack2x16unorm(const type::Type* ty,
                           VectorRef<const Value*> args,
                           const Source& source);

    /// unpack4x8snorm builtin
    /// @param ty the expression type
    /// @param args the input arguments
    /// @param source the source location
    /// @return the result value, or null if the value cannot be calculated
    Result unpack4x8snorm(const type::Type* ty, VectorRef<const Value*> args, const Source& source);

    /// unpack4x8unorm builtin
    /// @param ty the expression type
    /// @param args the input arguments
    /// @param source the source location
    /// @return the result value, or null if the value cannot be calculated
    Result unpack4x8unorm(const type::Type* ty, VectorRef<const Value*> args, const Source& source);

    /// quantizeToF16 builtin
    /// @param ty the expression type
    /// @param args the input arguments
    /// @param source the source location
    /// @return the result value, or null if the value cannot be calculated
    Result quantizeToF16(const type::Type* ty, VectorRef<const Value*> args, const Source& source);

  private:
    /// Adds the given error message to the diagnostics
    void AddError(const std::string& msg, const Source& source) const;

    /// Adds the given warning message to the diagnostics
    void AddWarning(const std::string& msg, const Source& source) const;

    /// Adds the given note message to the diagnostics
    void AddNote(const std::string& msg, const Source& source) const;

    /// CreateScalar constructs and returns a constant::Scalar<T>.
    /// @param source the source location
    /// @param t the result type
    /// @param v the scalar value
    /// @return the constant value with the same type and value
    template <typename T>
    Eval::Result CreateScalar(const Source& source, const type::Type* t, T v);

    /// ZeroValue returns a Constant for the zero-value of the type `type`.
    const Value* ZeroValue(const type::Type* type);

    /// Adds two Number<T>s
    /// @param source the source location
    /// @param a the lhs number
    /// @param b the rhs number
    /// @returns the result number on success, or logs an error and returns Failure
    template <typename NumberT>
    tint::Result<NumberT> Add(const Source& source, NumberT a, NumberT b);

    /// Subtracts two Number<T>s
    /// @param source the source location
    /// @param a the lhs number
    /// @param b the rhs number
    /// @returns the result number on success, or logs an error and returns Failure
    template <typename NumberT>
    tint::Result<NumberT> Sub(const Source& source, NumberT a, NumberT b);

    /// Multiplies two Number<T>s
    /// @param source the source location
    /// @param a the lhs number
    /// @param b the rhs number
    /// @returns the result number on success, or logs an error and returns Failure
    template <typename NumberT>
    tint::Result<NumberT> Mul(const Source& source, NumberT a, NumberT b);

    /// Divides two Number<T>s
    /// @param source the source location
    /// @param a the lhs number
    /// @param b the rhs number
    /// @returns the result number on success, or logs an error and returns Failure
    template <typename NumberT>
    tint::Result<NumberT> Div(const Source& source, NumberT a, NumberT b);

    /// Returns the (signed) remainder of the division of two Number<T>s
    /// @param source the source location
    /// @param a the lhs number
    /// @param b the rhs number
    /// @returns the result number on success, or logs an error and returns Failure
    template <typename NumberT>
    tint::Result<NumberT> Mod(const Source& source, NumberT a, NumberT b);

    /// Returns the dot product of (a1,a2) with (b1,b2)
    /// @param source the source location
    /// @param a1 component 1 of lhs vector
    /// @param a2 component 2 of lhs vector
    /// @param b1 component 1 of rhs vector
    /// @param b2 component 2 of rhs vector
    /// @returns the result number on success, or logs an error and returns Failure
    template <typename NumberT>
    tint::Result<NumberT> Dot2(const Source& source,
                               NumberT a1,
                               NumberT a2,
                               NumberT b1,
                               NumberT b2);

    /// Returns the dot product of (a1,a2,a3) with (b1,b2,b3)
    /// @param source the source location
    /// @param a1 component 1 of lhs vector
    /// @param a2 component 2 of lhs vector
    /// @param a3 component 3 of lhs vector
    /// @param b1 component 1 of rhs vector
    /// @param b2 component 2 of rhs vector
    /// @param b3 component 3 of rhs vector
    /// @returns the result number on success, or logs an error and returns Failure
    template <typename NumberT>
    tint::Result<NumberT> Dot3(const Source& source,
                               NumberT a1,
                               NumberT a2,
                               NumberT a3,
                               NumberT b1,
                               NumberT b2,
                               NumberT b3);

    /// Returns the dot product of (a1,b1,c1,d1) with (a2,b2,c2,d2)
    /// @param source the source location
    /// @param a1 component 1 of lhs vector
    /// @param a2 component 2 of lhs vector
    /// @param a3 component 3 of lhs vector
    /// @param a4 component 4 of lhs vector
    /// @param b1 component 1 of rhs vector
    /// @param b2 component 2 of rhs vector
    /// @param b3 component 3 of rhs vector
    /// @param b4 component 4 of rhs vector
    /// @returns the result number on success, or logs an error and returns Failure
    template <typename NumberT>
    tint::Result<NumberT> Dot4(const Source& source,
                               NumberT a1,
                               NumberT a2,
                               NumberT a3,
                               NumberT a4,
                               NumberT b1,
                               NumberT b2,
                               NumberT b3,
                               NumberT b4);

    /// Returns the determinant of the 2x2 matrix:
    /// | a c |
    /// | b d |
    /// @param source the source location
    /// @param a component 1 of the first column vector
    /// @param b component 2 of the first column vector
    /// @param c component 1 of the second column vector
    /// @param d component 2 of the second column vector
    template <typename NumberT>
    tint::Result<NumberT> Det2(const Source& source,  //
                               NumberT a,
                               NumberT b,
                               NumberT c,
                               NumberT d);

    /// Returns the determinant of the 3x3 matrix:
    /// | a d g |
    /// | b e h |
    /// | c f i |
    /// @param source the source location
    /// @param a component 1 of the first column vector
    /// @param b component 2 of the first column vector
    /// @param c component 3 of the first column vector
    /// @param d component 1 of the second column vector
    /// @param e component 2 of the second column vector
    /// @param f component 3 of the second column vector
    /// @param g component 1 of the third column vector
    /// @param h component 2 of the third column vector
    /// @param i component 3 of the third column vector
    template <typename NumberT>
    tint::Result<NumberT> Det3(const Source& source,
                               NumberT a,
                               NumberT b,
                               NumberT c,
                               NumberT d,
                               NumberT e,
                               NumberT f,
                               NumberT g,
                               NumberT h,
                               NumberT i);

    /// Returns the determinant of the 4x4 matrix:
    /// | a e i m |
    /// | b f j n |
    /// | c g k o |
    /// | d h l p |
    /// @param source the source location
    /// @param a component 1 of the first column vector
    /// @param b component 2 of the first column vector
    /// @param c component 3 of the first column vector
    /// @param d component 4 of the first column vector
    /// @param e component 1 of the second column vector
    /// @param f component 2 of the second column vector
    /// @param g component 3 of the second column vector
    /// @param h component 4 of the second column vector
    /// @param i component 1 of the third column vector
    /// @param j component 2 of the third column vector
    /// @param k component 3 of the third column vector
    /// @param l component 4 of the third column vector
    /// @param m component 1 of the fourth column vector
    /// @param n component 2 of the fourth column vector
    /// @param o component 3 of the fourth column vector
    /// @param p component 4 of the fourth column vector
    template <typename NumberT>
    tint::Result<NumberT> Det4(const Source& source,
                               NumberT a,
                               NumberT b,
                               NumberT c,
                               NumberT d,
                               NumberT e,
                               NumberT f,
                               NumberT g,
                               NumberT h,
                               NumberT i,
                               NumberT j,
                               NumberT k,
                               NumberT l,
                               NumberT m,
                               NumberT n,
                               NumberT o,
                               NumberT p);

    template <typename NumberT>
    tint::Result<NumberT> Sqrt(const Source& source, NumberT v);

    /// Clamps e between low and high
    /// @param source the source location
    /// @param e the number to clamp
    /// @param low the lower bound
    /// @param high the upper bound
    /// @returns the result number on success, or logs an error and returns Failure
    template <typename NumberT>
    tint::Result<NumberT> Clamp(const Source& source, NumberT e, NumberT low, NumberT high);

    /// Returns a callable that calls Add, and creates a Constant with its result of type `elem_ty`
    /// if successful, or returns Failure otherwise.
    /// @param source the source location
    /// @param elem_ty the element type of the Constant to create on success
    /// @returns the callable function
    auto AddFunc(const Source& source, const type::Type* elem_ty);

    /// Returns a callable that calls Sub, and creates a Constant with its result of type `elem_ty`
    /// if successful, or returns Failure otherwise.
    /// @param source the source location
    /// @param elem_ty the element type of the Constant to create on success
    /// @returns the callable function
    auto SubFunc(const Source& source, const type::Type* elem_ty);

    /// Returns a callable that calls Mul, and creates a Constant with its result of type `elem_ty`
    /// if successful, or returns Failure otherwise.
    /// @param source the source location
    /// @param elem_ty the element type of the Constant to create on success
    /// @returns the callable function
    auto MulFunc(const Source& source, const type::Type* elem_ty);

    /// Returns a callable that calls Div, and creates a Constant with its result of type `elem_ty`
    /// if successful, or returns Failure otherwise.
    /// @param source the source location
    /// @param elem_ty the element type of the Constant to create on success
    /// @returns the callable function
    auto DivFunc(const Source& source, const type::Type* elem_ty);

    /// Returns a callable that calls Mod, and creates a Constant with its result of type `elem_ty`
    /// if successful, or returns Failure otherwise.
    /// @param source the source location
    /// @param elem_ty the element type of the Constant to create on success
    /// @returns the callable function
    auto ModFunc(const Source& source, const type::Type* elem_ty);

    /// Returns a callable that calls Dot2, and creates a Constant with its result of type `elem_ty`
    /// if successful, or returns Failure otherwise.
    /// @param source the source location
    /// @param elem_ty the element type of the Constant to create on success
    /// @returns the callable function
    auto Dot2Func(const Source& source, const type::Type* elem_ty);

    /// Returns a callable that calls Dot3, and creates a Constant with its result of type `elem_ty`
    /// if successful, or returns Failure otherwise.
    /// @param source the source location
    /// @param elem_ty the element type of the Constant to create on success
    /// @returns the callable function
    auto Dot3Func(const Source& source, const type::Type* elem_ty);

    /// Returns a callable that calls Dot4, and creates a Constant with its result of type `elem_ty`
    /// if successful, or returns Failure otherwise.
    /// @param source the source location
    /// @param elem_ty the element type of the Constant to create on success
    /// @returns the callable function
    auto Dot4Func(const Source& source, const type::Type* elem_ty);

    /// Returns a callable that calls Det2, and creates a Constant with its result of type `elem_ty`
    /// if successful, or returns Failure otherwise.
    /// @param source the source location
    /// @param elem_ty the element type of the Constant to create on success
    /// @returns the callable function
    auto Det2Func(const Source& source, const type::Type* elem_ty);

    /// Returns a callable that calls Det3, and creates a Constant with its result of type `elem_ty`
    /// if successful, or returns Failure otherwise.
    /// @param source the source location
    /// @param elem_ty the element type of the Constant to create on success
    /// @returns the callable function
    auto Det3Func(const Source& source, const type::Type* elem_ty);

    /// Returns a callable that calls Det4, and creates a Constant with its result of type `elem_ty`
    /// if successful, or returns Failure otherwise.
    /// @param source the source location
    /// @param elem_ty the element type of the Constant to create on success
    /// @returns the callable function
    auto Det4Func(const Source& source, const type::Type* elem_ty);

    /// Returns a callable that calls Clamp, and creates a Constant with its result of type
    /// `elem_ty` if successful, or returns Failure otherwise.
    /// @param source the source location
    /// @param elem_ty the element type of the Constant to create on success
    /// @returns the callable function
    auto ClampFunc(const Source& source, const type::Type* elem_ty);

    /// Returns a callable that calls SqrtFunc, and creates a Constant with its
    /// result of type `elem_ty` if successful, or returns Failure otherwise.
    /// @param source the source location
    /// @param elem_ty the element type of the Constant to create on success
    /// @returns the callable function
    auto SqrtFunc(const Source& source, const type::Type* elem_ty);

    /// Returns the dot product of v1 and v2.
    /// @param source the source location
    /// @param v1 the first vector
    /// @param v2 the second vector
    /// @returns the dot product
    Result Dot(const Source& source, const Value* v1, const Value* v2);

    /// Returns the length of c0
    /// @param source the source location
    /// @param ty the return type
    /// @param c0 the constant to calculate the length of
    /// @returns the length of c0
    Result Length(const Source& source, const type::Type* ty, const Value* c0);

    /// Returns the product of v1 and v2
    /// @param source the source location
    /// @param ty the return type
    /// @param v1 lhs value
    /// @param v2 rhs value
    /// @returns the product of v1 and v2
    Result Mul(const Source& source, const type::Type* ty, const Value* v1, const Value* v2);

    /// Returns the difference between v2 and v1
    /// @param source the source location
    /// @param ty the return type
    /// @param v1 lhs value
    /// @param v2 rhs value
    /// @returns the difference between v2 and v1
    Result Sub(const Source& source, const type::Type* ty, const Value* v1, const Value* v2);

  private:
    Manager& mgr;
    diag::List& diags;
    bool use_runtime_semantics_ = false;
};

}  // namespace tint::core::constant

#endif  // SRC_TINT_LANG_CORE_CONSTANT_EVAL_H_
