/*
 *  Based on the work of Phil, Copyright 2010 Two Blue Cubes Ltd. All rights
 * reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include "acatch/acatch_core.hpp"

#include <iomanip>

namespace ACatch {

namespace Detail {

std::string unprintableString = "{?}";

namespace {
struct Endianness {
  enum Arch { Big, Little };

  static Arch which() {
    union _ {
      int asInt;
      char asChar[ sizeof( int ) ];
    } u;

    u.asInt = 1;
    return ( u.asChar[ sizeof( int ) - 1 ] == 1 ) ? Big : Little;
  }
};
}

std::string rawMemoryToString( const void* object, std::size_t size ) {
  // Reverse order for little endian architectures
  int i = 0, end = static_cast<int>( size ), inc = 1;
  if( Endianness::which() == Endianness::Little ) {
    i = end - 1;
    end = inc = -1;
  }

  unsigned char const* bytes = static_cast<unsigned char const*>( object );
  std::ostringstream os;
  os << "0x" << std::setfill( '0' ) << std::hex;
  for( ; i != end; i += inc )
    os << std::setw( 2 ) << static_cast<unsigned>( bytes[ i ] );
  return os.str();
}
} // namespace Detail

std::string toString( std::string const& value ) {
  std::string s = value;
  // if( getCurrentContext().getConfig()->showInvisibles() )
  {
    for( size_t i = 0; i < s.size(); ++i ) {
      std::string subs;
      switch( s[ i ] ) {
      case '\n':
        subs = "\\n";
        break;
      case '\t':
        subs = "\\t";
        break;
      default:
        break;
      }
      if( !subs.empty() ) {
        s = s.substr( 0, i ) + subs + s.substr( i + 1 );
        ++i;
      }
    }
  }
  return "\"" + s + "\"";
}
std::string toString( std::wstring const& value ) {

  std::string s;
  s.reserve( value.size() );
  for( size_t i = 0; i < value.size(); ++i )
    s += value[ i ] <= 0xff ? static_cast<char>( value[ i ] ) : '?';
  return ::ACatch::toString( s );
}

std::string toString( const char* const value ) {
  return value ? ::ACatch::toString( std::string( value ) )
               : std::string( "{null string}" );
}

std::string toString( char* const value ) {
  return ::ACatch::toString( static_cast<const char*>( value ) );
}

std::string toString( const wchar_t* const value ) {
  return value ? ::ACatch::toString( std::wstring( value ) )
               : std::string( "{null string}" );
}

std::string toString( wchar_t* const value ) {
  return ::ACatch::toString( static_cast<const wchar_t*>( value ) );
}

std::string toString( int value ) {
  std::ostringstream oss;
  oss << value;
  if( value >= 255 )
    oss << " (0x" << std::hex << value << ")";
  return oss.str();
}

std::string toString( unsigned long value ) {
  std::ostringstream oss;
  oss << value;
  if( value >= 255 )
    oss << " (0x" << std::hex << value << ")";
  return oss.str();
}

std::string toString( unsigned int value ) {
  return ::ACatch::toString( static_cast<unsigned long>( value ) );
}

template <typename T>
std::string fpToString( T value, int precision ) {
  std::ostringstream oss;
  oss << std::setprecision( precision ) << std::fixed << value;
  std::string d = oss.str();
  std::size_t i = d.find_last_not_of( '0' );
  if( i != std::string::npos && i != d.size() - 1 ) {
    if( d[ i ] == '.' )
      i++;
    d = d.substr( 0, i + 1 );
  }
  return d;
}

std::string toString( const double value ) {
  return fpToString( value, 10 );
}
std::string toString( const float value ) {
  return fpToString( value, 5 ) + "f";
}

std::string toString( bool value ) {
  return value ? "true" : "false";
}

std::string toString( char value ) {
  return value < ' ' ? toString( static_cast<unsigned int>( value ) )
                     : Detail::makeString( value );
}

std::string toString( signed char value ) {
  return toString( static_cast<char>( value ) );
}

std::string toString( unsigned char value ) {
  return toString( static_cast<char>( value ) );
}

std::string toString( std::nullptr_t ) {
  return "nullptr";
}

} // namespace ACatch
