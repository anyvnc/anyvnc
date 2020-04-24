#pragma once

#include <algorithm>

namespace AnyVnc
{

namespace Core
{

class Utils
{
public:
	template<class T>
	static inline bool ends_with( const T& a, const T& b )
	{
		if( b.size() > a.size() ) return false;
		return std::equal( a.begin() + a.size() - b.size(), a.end(), b.begin() );
	}

};

}

}

