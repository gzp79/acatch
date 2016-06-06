/*
 *  Based on the work of Phil, Copyright 2010 Two Blue Cubes Ltd. All rights
 * reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

namespace ACatch {

struct CaseSensitive {
  enum Choice { Yes, No };
};

inline bool checkPrefix( const std::string& s1, const std::string& s2 ) {
  if( s1.size() > s2.size() )
    return s1.substr( 0, s2.size() ) == s2;
  else
    return s2.substr( 0, s1.size() ) == s1;
}

inline bool startsWith( const std::string& s, const std::string& prefix ) {
  return s.size() >= prefix.size() && s.substr( 0, prefix.size() ) == prefix;
}

inline bool endsWith( const std::string& s, const std::string& suffix ) {
  return s.size() >= suffix.size()
         && s.substr( s.size() - suffix.size(), suffix.size() ) == suffix;
}

inline bool contains( const std::string& s, const std::string& infix ) {
  return s.find( infix ) != std::string::npos;
}

inline void toLowerInPlace( std::string& s ) {
  std::transform( s.begin(), s.end(), s.begin(), ::tolower );
}

inline std::string toLower( const std::string& s ) {
  std::string lc = s;
  toLowerInPlace( lc );
  return lc;
}

inline std::string trim( const std::string& str ) {
  static char const* whitespaceChars = "\n\r\t ";
  std::string::size_type start = str.find_first_not_of( whitespaceChars );
  std::string::size_type end = str.find_last_not_of( whitespaceChars );

  return start != std::string::npos ? str.substr( start, 1 + end - start ) : "";
}

inline bool replaceInPlace( std::string& str, const std::string& replaceThis,
                            const std::string& withThis ) {
  bool replaced = false;
  std::size_t i = str.find( replaceThis );
  while( i != std::string::npos ) {
    replaced = true;
    str = str.substr( 0, i ) + withThis + str.substr( i + replaceThis.size() );
    if( i < str.size() - withThis.size() )
      i = str.find( replaceThis, i + withThis.size() );
    else
      i = std::string::npos;
  }
  return replaced;
}

} // namespace ACatch
