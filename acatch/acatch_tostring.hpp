/*
 *  Based on the work of Phil, Copyright 2010 Two Blue Cubes Ltd. All rights
 * reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

namespace ACatch {

// Why we're here.
template <typename T>
std::string toString( const T& value );

// Built in overloads

std::string toString( std::string const& value );
std::string toString( std::wstring const& value );
std::string toString( const char* const value );
std::string toString( char* const value );
std::string toString( const wchar_t* const value );
std::string toString( wchar_t* const value );
std::string toString( int value );
std::string toString( unsigned long value );
std::string toString( unsigned int value );
std::string toString( const double value );
std::string toString( const float value );
std::string toString( bool value );
std::string toString( char value );
std::string toString( signed char value );
std::string toString( unsigned char value );
std::string toString( std::nullptr_t );

namespace Detail {

extern std::string unprintableString;

struct BorgType {
  template <typename T>
  BorgType( const T& );
};

struct TrueType {
  char sizer[ 1 ];
};
struct FalseType {
  char sizer[ 2 ];
};

TrueType& testStreamable( std::ostream& );
FalseType testStreamable( FalseType );

FalseType operator<<( std::ostream const&, BorgType const& );

template <typename T>
struct IsStreamInsertable {
  static std::ostream& s;
  static T const& t;
  enum { value = sizeof( testStreamable( s << t ) ) == sizeof( TrueType ) };
};

template <typename T, bool IsEnum = std::is_enum<T>::value>
struct EnumStringMaker {
  static std::string convert( const T& ) {
    return unprintableString;
  }
};

template <typename T>
struct EnumStringMaker<T, true> {
  static std::string convert( const T& v ) {
    return ::ACatch::toString(
        static_cast<typename std::underlying_type<T>::type>( v ) );
  }
};

template <bool C>
struct StringMakerBase {
  template <typename T>
  static std::string convert( const T& v ) {
    return EnumStringMaker<T>::convert( v );
  }
};

template <>
struct StringMakerBase<true> {
  template <typename T>
  static std::string convert( const T& _value ) {
    std::ostringstream oss;
    oss << _value;
    return oss.str();
  }
};

std::string rawMemoryToString( const void* object, std::size_t size );

template <typename T>
inline std::string rawMemoryToString( const T& object ) {
  return rawMemoryToString( &object, sizeof( object ) );
}

} // namespace Detail

template <typename T>
struct StringMaker
    : Detail::StringMakerBase<Detail::IsStreamInsertable<T>::value> {};

template <typename T>
struct StringMaker<T*> {
  template <typename U>
  static std::string convert( U* p ) {
    if( !p )
      return "null"; // INTERNAL_CATCH_STRINGIFY( nullptr );
    else
      return Detail::rawMemoryToString( p );
  }
};

template <typename R, typename C>
struct StringMaker<R C::*> {
  static std::string convert( R C::*p ) {
    if( !p )
      return "null";
    else
      return Detail::rawMemoryToString( p );
  }
};

namespace Detail {
template <typename InputIterator>
std::string rangeToString( InputIterator first, InputIterator last );
} // namespace Detail

// template<typename T, typename Allocator>
// struct StringMaker<std::vector<T, Allocator> > {
//    static std::string convert( std::vector<T,Allocator> const& v ) {
//        return Detail::rangeToString( v.begin(), v.end() );
//    }
//};

template <typename T, typename Allocator>
std::string toString( std::vector<T, Allocator> const& v ) {
  return Detail::rangeToString( v.begin(), v.end() );
}

// toString for tuples
namespace TupleDetail {
template <typename Tuple, std::size_t N = 0,
          bool = ( N < std::tuple_size<Tuple>::value )>
struct ElementPrinter {
  static void print( const Tuple& tuple, std::ostream& os ) {
    os << ( N ? ", " : " " ) << ::ACatch::toString( std::get<N>( tuple ) );
    ElementPrinter<Tuple, N + 1>::print( tuple, os );
  }
};

template <typename Tuple, std::size_t N>
struct ElementPrinter<Tuple, N, false> {
  static void print( const Tuple&, std::ostream& ) {
  }
};
} // namespace TupleDetail

template <typename... Types>
struct StringMaker<std::tuple<Types...>> {

  static std::string convert( const std::tuple<Types...>& tuple ) {
    std::ostringstream os;
    os << '{';
    TupleDetail::ElementPrinter<std::tuple<Types...>>::print( tuple, os );
    os << " }";
    return os.str();
  }
};

namespace Detail {
template <typename T>
std::string makeString( const T& value ) {
  return StringMaker<T>::convert( value );
}
} // namespace Detail

/// \brief converts any type to a string
///
/// The default template forwards on to ostringstream - except when an
/// ostringstream overload does not exist - in which case it attempts to detect
/// that and writes {?}.
/// Overload (not specialize) this template for custom types that you don't want
/// to provide an ostream overload for.
template <typename T>
std::string toString( const T& value ) {
  return StringMaker<T>::convert( value );
}

namespace Detail {
template <typename InputIterator>
std::string rangeToString( InputIterator first, InputIterator last ) {
  std::ostringstream oss;
  oss << "{ ";
  if( first != last ) {
    oss << ::ACatch::toString( *first );
    for( ++first; first != last; ++first )
      oss << ", " << ::ACatch::toString( *first );
  }
  oss << " }";
  return oss.str();
}
} // namespace Detail

} // namespace ACatch
