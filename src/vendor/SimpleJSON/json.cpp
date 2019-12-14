#include "json.hpp"

namespace json
{
	JSON JSON::Load( const string &str ) {
		size_t offset = 0;
		return std::move( parse_next( str, offset ) );
	}
}
