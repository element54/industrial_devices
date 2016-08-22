#pragma once

#include <string>
#include <memory>
#include <vector>
#include <unordered_map>

#include <property_server/property.hpp>

namespace PropertyServer {
using namespace std;

class Device {
    friend class Server;
public:
    Device( const string id, const string name, const string description );
    const string get_id( void );
    const string get_name( void );
    const string get_description( void );
    shared_ptr<Property> get_property( const string id );
    void register_listener( const string id, std::function<void(shared_ptr<Device>, shared_ptr<Property>)> listener );
    void add_property( shared_ptr<Property> property );
private:
    const string id_;
    const string name_;
    const string description_;
    unordered_map<string, shared_ptr<Property> > properties_;
    unordered_map<string, vector<std::function<void(shared_ptr<Device>, shared_ptr<Property>)> > > listener_;
};
}
