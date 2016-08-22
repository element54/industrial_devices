#include <avahi_client/avahi_client.hpp>

namespace Avahi {
using namespace std;

Client::Client() {

}

Client::~Client() {
    if( avahi_client_ )
        avahi_client_free( avahi_client_ );

    if( avahi_simple_poll_ )
        avahi_simple_poll_free( avahi_simple_poll_ );
}

void Client::loop() {
    while( true ) {
        if( do_recover_ ) {
            if( avahi_group_ ) {
                avahi_entry_group_free( avahi_group_ );
                avahi_group_ = NULL;
            }
            if( avahi_client_ ) {
                avahi_client_free( avahi_client_ );
                avahi_client_ = NULL;
            }
            if( avahi_simple_poll_ ) {
                avahi_simple_poll_free( avahi_simple_poll_ );
                avahi_simple_poll_ = NULL;
            }
        }
        if( !avahi_simple_poll_ ) {
            int error;

            if( ( avahi_simple_poll_ = avahi_simple_poll_new() ) ) {
                avahi_client_ = avahi_client_new( avahi_simple_poll_get(
                                                      avahi_simple_poll_ ), AVAHI_CLIENT_NO_FAIL, Client::client_callback, this,
                                                  &error );
                if( !avahi_client_ ) {
                    avahi_simple_poll_free( avahi_simple_poll_ );
                }
            }
        }

        if( avahi_client_ ) {
            do_recover_ = false;
            avahi_simple_poll_loop( avahi_simple_poll_ );
        }
    }
}
bool Client::create_services() {
    if( !avahi_group_ )
        if( !( avahi_group_ = avahi_entry_group_new( avahi_client_, Client::group_callback, this ) ) ) {
            recover();
            avahi_simple_poll_quit( avahi_simple_poll_ );
            return false;
        }
    if( avahi_entry_group_is_empty( avahi_group_ ) ) {
        for( auto it = services_.begin(); it != services_.end(); ++it ) {
            auto service = (*it);
            if( !create_service( service ) ) {
                return false;
            }
        }
        int ret = avahi_entry_group_commit( avahi_group_ );
        if( ret < 0 ) {
            recover();
            avahi_simple_poll_quit( avahi_simple_poll_ );
            return false;
        }
    }

    return true;
}

bool Client::create_service( shared_ptr<Service> service ) {
    if( !avahi_client_ )
        return false;
    int ret = avahi_entry_group_add_service( avahi_group_, AVAHI_IF_UNSPEC, AVAHI_PROTO_UNSPEC,
                                             static_cast<AvahiPublishFlags>(0), service->modified_name_.c_str(),
                                             service->get_type().c_str(), NULL, NULL, service->get_port(), NULL );
    if( ret < 0 ) {
        if( ret == AVAHI_ERR_COLLISION ) {
            name_collision( service );
            return create_service( service );
        } else {
            recover();
            return false;
        }
    }
    return true;
}
void Client::client_callback( AvahiClient *c, AvahiClientState state, void *ud ) {
    Client *instance = static_cast<Client *>(ud);

    instance->avahi_client_ = c;
    switch( state ) {
    case AVAHI_CLIENT_S_RUNNING:
        instance->create_services();
        break;

    case AVAHI_CLIENT_FAILURE:
        instance->recover();
        avahi_simple_poll_quit( instance->avahi_simple_poll_ );
        break;

    case AVAHI_CLIENT_S_COLLISION:
    case AVAHI_CLIENT_S_REGISTERING:
        if( instance->avahi_group_ )
            avahi_entry_group_reset( instance->avahi_group_ );
        break;
    default:
        break;
    }
}

void Client::group_callback( AvahiEntryGroup *g, AvahiEntryGroupState state, void *ud ) {
    Client *instance = static_cast<Client *>(ud);

    instance->avahi_group_ = g;
    switch( state ) {
    case AVAHI_ENTRY_GROUP_ESTABLISHED:
        break;
    case AVAHI_ENTRY_GROUP_COLLISION:
        instance->name_collision();
        avahi_simple_poll_quit( instance->avahi_simple_poll_ );
        break;
    case AVAHI_ENTRY_GROUP_FAILURE:
        instance->recover();
        avahi_simple_poll_quit( instance->avahi_simple_poll_ );
        break;
    case AVAHI_ENTRY_GROUP_UNCOMMITED:
    case AVAHI_ENTRY_GROUP_REGISTERING:
        break;
    }
}

void Client::name_collision() {
    for( auto it = services_.begin(); it != services_.end(); ++it ) {
        auto service = (*it);
        name_collision( service );
    }
    recover();
}
void Client::name_collision( shared_ptr<Service> service ) {
    string name = service->modified_name_;
    char *new_name = avahi_alternative_service_name( name.c_str() );

    service->modified_name_ = new_name;
    avahi_free( new_name );
}

void Client::recover() {
    do_recover_ = true;
}

void Client::start() {
    thread_ = new thread( &Client::loop, this );
}

void Client::publish_service( shared_ptr<Service> service ) {
    services_.push_back( service );
}
}
