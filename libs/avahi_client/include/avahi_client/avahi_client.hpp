#pragma once

#include <vector>
#include <thread>
#include <unordered_map>
#include <avahi-client/client.h>
#include <avahi-client/publish.h>

#include <avahi-common/alternative.h>
#include <avahi-common/simple-watch.h>
#include <avahi-common/malloc.h>
#include <avahi-common/error.h>
#include <avahi-common/timeval.h>

namespace Avahi {
using namespace std;
class Client;

class Service {
friend class Client;
private:
    const string name_;
    string modified_name_;
    const string type_;
    const u_int port_;
public:
    Service( string name, string type, u_int port ) : name_( name ), modified_name_( name ), type_( type ),
        port_( port ) {
    };
    const string get_name()  {
        return name_;
    };
    const string get_type() {
        return type_;
    };
    u_int get_port() {
        return port_;
    };
};

class Client {
private:
    AvahiSimplePoll  *avahi_simple_poll_ = NULL;
    AvahiClient *avahi_client_ = NULL;
    AvahiEntryGroup *avahi_group_ = NULL;
    thread *thread_;
    bool do_recover_;

    vector<shared_ptr<Service> > services_;

    void loop();

    static void client_callback( AvahiClient *c, AvahiClientState state, void *instance );
    static void group_callback( AvahiEntryGroup *g, AvahiEntryGroupState state, void *instance );

    bool create_services();
    bool create_service( shared_ptr<Service> service );
    void recover();
    void name_collision();
    void name_collision( shared_ptr<Service> service );
public:
    Client();
    ~Client();
    void start();

    void publish_service( shared_ptr<Service> service );

};

}
