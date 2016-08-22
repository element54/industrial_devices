#include <property_server/device.hpp>

namespace PropertyServer {
using namespace std;

Device::Device( const string id, const string name, const string description ) :
    id_( id ), name_( name ), description_( description ) {
};
const string Device::get_id( void ) {
    return id_;
};
const string Device::get_name( void ) {
    return name_;
};
const string Device::get_description( void ) {
    return description_;
};
shared_ptr<Property> Device::get_property( const string id ) {
    return properties_[ id ];
};
void Device::register_listener( const string id, std::function<void(shared_ptr<Device>, shared_ptr<Property>)> listener ) {
    listener_[ id ].push_back( listener );
};
void Device::add_property( shared_ptr<Property> property ) {
    properties_[ property->get_id() ] = property;
};
}
