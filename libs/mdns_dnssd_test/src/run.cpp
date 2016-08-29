#include <iostream>
#include <avahi_client/avahi_client.hpp>
#include <unistd.h>

using namespace std;

int main() {
    int count = 0;
    pid_t pid;
    while(count < 200) {
        pid = fork();
        if(pid == 0) {
            Avahi::Client *avahi_client = new Avahi::Client();
            avahi_client->publish_service( make_shared<Avahi::Service>( "RTT-PS", "_property_server._tcp", count ) );
            avahi_client->start();
            break;
        } else {
            cout << "started #" << count << endl;
            usleep(50000);
        }
        count++;
    }
    if(pid != 0) {
        cout << "Ready" << endl;
    }
    while(true){}
    return 0;
}
