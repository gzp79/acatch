/*
 *  Based on the work of Phil, Copyright 2010 Two Blue Cubes Ltd. All rights
 * reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

namespace ACatch {

template <typename T>
class ExpressionBuilder;

struct STATIC_ASSERT_Expression_Too_Complex_Please_Rewrite_As_Binary_Comparison;

enum class Operator {
  NoOp,
  IsEqualTo,
  IsNotEqualTo,
  IsLessThan,
  IsGreaterThan,
  IsLessThanOrEqualTo,
  IsGreaterThanOrEqualTo
};

template <Operator Op>
struct OperatorTraits {
  static const char* getName() {
    return "*error*";
  }
};

template <>
struct OperatorTraits<Operator::IsEqualTo> {
  static const char* getName() {
    return "==";
  }
};

template <>
struct OperatorTraits<Operator::IsNotEqualTo> {
  static const char* getName() {
    return "!=";
  }
};

template <>
struct OperatorTraits<Operator::IsLessThan> {
  static const char* getName() {
    return "<";
  }
};

template <>
struct OperatorTraits<Operator::IsGreaterThan> {
  static const char* getName() {
    return ">";
  }
};

template <>
struct OperatorTraits<Operator::IsLessThanOrEqualTo> {
  static const char* getName() {
    return "<=";
  }
};

template <>
struct OperatorTraits<Operator::IsGreaterThanOrEqualTo> {
  static const char* getName() {
    return ">=";
  }
};

/// Capture and expand an expression
class ExpressionCapture {
public:
  ExpressionCapture( const char* aRaw )
      : mRaw( aRaw ) {
  }

  template <typename T>
  ExpressionBuilder<T> operator<=( const T& aOperand );

  void add( const std::string& aOperand ) {
    mExpr.push_back( aOperand );
  }

  std::string getRawString() const {
    return mRaw;
  }

  std::string getExpandedString() const {
    std::ostringstream ss;

    bool first = true;
    for( const std::string& ec : mExpr ) {
      if( !first )
        ss << " ";

      first = false;
      ss << ec;
    }

    return ss.str();
  }

private:
  std::string mRaw;
  std::vector<std::string> mExpr;
};

/// Capture and expand an expression
class MultiExpressionCapture {
public:
  enum EType {
    Any,
    All,
    One,
  };

  struct Expression {
    std::string raw;
    std::string expanded;

    Expression() {
    }

    Expression( const std::string& aRaw, const std::string& aExpanded )
        : raw( aRaw )
        , expanded( aExpanded ) {
    }
  };
  typedef std::vector<Expression> Expressions;

  MultiExpressionCapture( EType aType )
      : mType( aType ) {
  }

  void add( const ExpressionCapture& aExpr ) {
    mExpressions.emplace_back( aExpr.getRawString(),
                               aExpr.getExpandedString() );
  }

  EType getType() const {
    return mType;
  }

  const Expressions& getExpressions() const {
    return mExpressions;
  }

private:
  EType mType;
  Expressions mExpressions;
};

#define ACATCH_EXPRBUILD_OP( OP )                                              \
  template <typename RhsT>                                                     \
  ExpressionBuilder& operator OP( const RhsT& aRhs ) {                         \
    mExpr.add( "\"" #OP "\"" );                                                \
    mExpr.add( toString( aRhs ) );                                             \
    return *this;                                                              \
  }

#define ACATCH_EXPRBUILD_OP_DISABLE( OP )                                      \
  template <typename RhsT>                                                     \
  STATIC_ASSERT_Expression_Too_Complex_Please_Rewrite_As_Binary_Comparison     \
  operator OP( const RhsT& );

template <typename T>
class ExpressionBuilder {
public:
  ExpressionBuilder( ExpressionCapture& aEC, const T& aOperand )
      : mExpr( aEC ) {
    mExpr.add( toString( aOperand ) );
  }

  ExpressionBuilder( const ExpressionBuilder& ) = default;
  // ExpressionBuilder( ExpressionBuilder&& ) = default;

  ExpressionBuilder& operator=( const ExpressionBuilder& ) = delete;
  ExpressionBuilder& operator=( ExpressionBuilder&& ) = delete;

  ExpressionCapture& getCapture() {
    return mExpr;
  }

  ACATCH_EXPRBUILD_OP( == )
  ACATCH_EXPRBUILD_OP( != )
  ACATCH_EXPRBUILD_OP( < )
  ACATCH_EXPRBUILD_OP( > )
  ACATCH_EXPRBUILD_OP( <= )
  ACATCH_EXPRBUILD_OP( >= )
  ACATCH_EXPRBUILD_OP_DISABLE( &&)
  ACATCH_EXPRBUILD_OP_DISABLE( || )
  ACATCH_EXPRBUILD_OP_DISABLE( & )
  ACATCH_EXPRBUILD_OP_DISABLE ( ^)
  ACATCH_EXPRBUILD_OP_DISABLE( | )
  ACATCH_EXPRBUILD_OP_DISABLE( +)
  ACATCH_EXPRBUILD_OP_DISABLE( -)
  ACATCH_EXPRBUILD_OP_DISABLE( / )
  ACATCH_EXPRBUILD_OP_DISABLE( * )

private:
  ExpressionCapture& mExpr;
};

template <typename T>
inline ExpressionBuilder<T> ExpressionCapture::operator<=( const T& aOperand ) {
  mExpr.clear();
  return ExpressionBuilder<T>( *this, aOperand );
}

} // namespace ACatch
