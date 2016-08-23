#include <iostream>
#include <property_server/server.hpp>
#include <wiringPi.h>

#define COIL_1A 3
#define COIL_1B 5
#define COIL_2A 7
#define COIL_2B 8

using namespace std;
using namespace PropertyServer;


const double one_step = 0.9;
bool run = false;
double ms_per_step = 0.0;

int main() {
    auto rtt = make_shared<Device>( "rtt", "RTT", "Rotating Turntable" );
    auto target_speed = make_shared<Property>( "target_speed", "Target speed", "deg/s", false, 0.0, -720.0, 720.0 );
    auto current_speed = make_shared<Property>( "current_speed", "Current speed", "deg/s", true, 0.0, -720.0, 720.0 );
    rtt->add_property( target_speed );
    rtt->add_property( current_speed );
    rtt->register_listener( target_speed->get_id(),
                            [ = ]( __attribute__ ( (unused) ) shared_ptr<Device> device,
                                   __attribute__ ( (unused) ) shared_ptr<Property> property ) {
                                double speed = property->as_double();
                                current_speed->set_double( speed );
                                if( speed == 0.0 ) {
                                    run = false;
                                } else {
                                    run = true;
                                    double steps_per_second = speed / one_step;
                                    double second_per_step = 1.0 / (steps_per_second);
                                    ms_per_step = (second_per_step * 1000.0);
                                }
                            } );
    Server propertyServer( "RTT-PS", 8080 );
    propertyServer.add_device( rtt );
    propertyServer.start();
    wiringPiSetupPhys () ;
       pinMode (COIL_1A, OUTPUT);
       pinMode (COIL_1B, OUTPUT);
       pinMode (COIL_2A, OUTPUT);
       pinMode (COIL_2B, OUTPUT);
       int states[][4] = {{1,0,1,0},{1,0,0,0},{1,0,0,1},{0,0,0,1},{0,1,0,1},{0,1,0,0},{0,1,1,0},{0,0,1,0}};
       int count = 0;
       for (;;) {
        if(run) {
            digitalWrite(COIL_1A, states[count][0]);
            digitalWrite(COIL_1B, states[count][1]);
            digitalWrite(COIL_2A, states[count][2]);
            digitalWrite(COIL_2B, states[count][3]);
            count = (count + 1) % 8;
            delay (ms_per_step);
        } else {
            delay (100);
        }
       }
       return 0;
}
