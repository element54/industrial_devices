#include <iostream>
#include <property_server/server.hpp>
#include <wiringPi.h>
#include <softPwm.h>
#include <stdlib.h>

#define COIL_A 3
#define COIL_B 5

using namespace std;
using namespace PropertyServer;


const double one_step = 0.9;
bool run = false;
double ms_per_step = 0.0;

int main() {
    wiringPiSetupPhys();
    softPwmCreate (COIL_A, 0, 100) ;
    softPwmCreate (COIL_B, 0, 100) ;

    auto cb = make_shared<Device>( "conveyor_belt", "Conveyor Belt", "Conveyor Belt" );
    auto target_speed = make_shared<Property>( "target_speed", "Target speed", "percent", false, 0l, -100l, 100l );
    auto current_speed = make_shared<Property>( "current_speed", "Current speed", "percent", true, 0l, 100l, 100l );
    cb->add_property( target_speed );
    cb->add_property( current_speed );
    cb->register_listener( target_speed->get_id(),
    [ = ]( __attribute__ ( (unused) ) shared_ptr<Device> device,
           __attribute__ ( (unused) ) shared_ptr<Property> property ) {
        long speed = property->as_long();
        long pwm = labs(speed);
        if(speed == 0) {
            softPwmWrite (COIL_A, 0) ;
            softPwmWrite (COIL_B, 0) ;
        } else if(speed < 0) {
            softPwmWrite (COIL_A, pwm) ;
            softPwmWrite (COIL_B, 0) ;
        } else {
            softPwmWrite (COIL_A, 0) ;
            softPwmWrite (COIL_B, pwm) ;
        }
    } );
    Server propertyServer( "CB-PS", 8080 );
    propertyServer.add_device( cb );
    propertyServer.start();
    propertyServer.join();
    return 0;
}
