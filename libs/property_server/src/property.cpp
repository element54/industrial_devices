#include <property_server/property.hpp>

namespace PropertyServer {
using namespace std;

Property::Property( const string id, const string name, const string description, const bool readonly, const ValueType type )
    : id_( id ), name_( name ), description_( description ), readonly_( readonly ), type_( type ) {
    update_timestamp();
};
Property::Property( const string id, const string name, const string description, const bool readonly, long value, long min, long max )
    : Property( id, name, description, readonly, ValueType::long_ ) {
    value_.long_ = value;
    min_value_.long_ = min;
    max_value_.long_ = max;
};
Property::Property( const string id,
                    const string name,
                    const string description,
                    const bool readonly,
                    unsigned long value,
                    unsigned long min,
                    unsigned long max )
    : Property( id, name, description, readonly, ValueType::ulong_ ) {
    value_.ulong_ = value;
    min_value_.ulong_ = min;
    max_value_.ulong_ = max;
};
Property::Property( const string id, const string name, const string description, const bool readonly, double value, double min, double max )
    : Property( id, name, description, readonly, ValueType::double_ ) {
    value_.double_ = value;
    min_value_.double_ = min;
    max_value_.double_ = max;
};
Property::Property( const string id, const string name, const string description, const bool readonly, bool value )
    : Property( id, name, description, readonly, ValueType::bool_ ) {
    value_.bool_ = value;
};

Property::Property( const string id, const string name, const string description, const bool readonly, const string value )
    : Property( id, name, description, readonly, ValueType::string_ ) {
    value_.string_ = value.c_str();
};
const string Property::get_id( void ) {
    return id_;
}
const string Property::get_name( void ) {
    return name_;
}
const string Property::get_description( void ) {
    return description_;
}
chrono::time_point<chrono::system_clock>& Property::get_timestamp( void ) {
    return timestamp_;
}
bool Property::is_readonly( void ) {
    return readonly_;
}
bool Property::is_long( void ) {
    return type_ == ValueType::long_;
}
bool Property::is_ulong( void ) {
    return type_ == ValueType::ulong_;
}
bool Property::is_double( void ) {
    return type_ == ValueType::double_;
}
bool Property::is_bool( void ) {
    return type_ == ValueType::bool_;
}
bool Property::is_string( void ) {
    return type_ == ValueType::string_;
}
long Property::as_long( void ) {
    if( !is_long() )
        return 0;
    return value_.long_;
}
unsigned long Property::as_ulong( void ) {
    if( !is_ulong() )
        return 0;
    return value_.ulong_;
}
bool Property::as_bool( void ) {
    if( !is_bool() )
        return false;
    return value_.bool_;
}
double Property::as_double( void ) {
    if( !is_double() )
        return 0.0;
    return value_.double_;
}
const string Property::as_string( void ) {
    if( !is_string() )
        return "";
    return value_.string_;
}
long Property::min_long( void ) {
    if( !is_long() )
        return 0;
    return min_value_.long_;
}
long Property::max_long( void ) {
    if( !is_long() )
        return 0;
    return max_value_.long_;
}
unsigned long Property::min_ulong( void ) {
    if( !is_ulong() )
        return 0;
    return min_value_.ulong_;
}
unsigned long Property::max_ulong( void ) {
    if( !is_ulong() )
        return 0;
    return max_value_.ulong_;
}
double Property::min_double( void ) {
    if( !is_double() )
        return 0.0;
    return min_value_.double_;
}
double Property::max_double( void ) {
    if( !is_double() )
        return 0.0;
    return max_value_.double_;
}
void Property::update_timestamp( void ) {
    timestamp_ = chrono::system_clock::now();
    /*auto timestamp = chrono::system_clock::now().time_since_epoch();

       timestamp_ = std::chrono::duration_cast<std::chrono::milliseconds>( timestamp ).count();*/
}
void Property::set_long( long value ) {
    if( !is_long() )
        return;
    if( value > max_value_.long_ )
        return;
    if( value < min_value_.long_ )
        return;
    value_.long_ = value;
    update_timestamp();
}
void Property::set_ulong( unsigned long value ) {
    if( !is_ulong() )
        return;
    if( value > max_value_.ulong_ )
        return;
    if( value < min_value_.ulong_ )
        return;
    value_.ulong_ = value;
    update_timestamp();
}
void Property::set_bool( bool value ) {
    if( !is_bool() )
        return;
    value_.bool_ = value;
    update_timestamp();
}
void Property::set_double( double value ) {
    if( !is_double() )
        return;
    if( value > max_value_.double_ )
        return;
    if( value < min_value_.double_ )
        return;
    value_.double_ = value;
    update_timestamp();
}
void Property::set_string( const string &value ) {
    if( !is_string() )
        return;
    value_.bool_ = value.c_str();
    update_timestamp();
}
}
