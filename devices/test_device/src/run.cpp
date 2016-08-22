#include <iostream>
#include <property_server/server.hpp>

using namespace std;
using namespace PropertyServer;

int main() {
    auto rtt = make_shared<Device>( "rtt", "RTT", "Rotating Turntable" );
    auto target_speed = make_shared<Property>( "target_speed", "Target speed", "deg/s", false, 0.0, -720.0, 720.0 );
    auto current_speed = make_shared<Property>( "current_speed", "Current speed", "deg/s", true, 0.0, -720.0, 720.0 );
    rtt->add_property( target_speed );
    rtt->add_property( current_speed );
    Server propertyServer( "RTT-PS", 8080 );
    propertyServer.add_device( rtt );
    propertyServer.start();
    propertyServer.join();
    return 0;
}
