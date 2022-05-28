#pragma once

namespace god
{
	template <class T>
	inline void HashCombine ( std::size_t& seed , T const& v )
	{
		seed ^= std::hash<T> ()( v ) + 0x9e3779b9 + ( seed << 6 ) + ( seed >> 2 );
	}
}