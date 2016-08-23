#include <property_server/server.hpp>
//#include <iomanip>
#include <time.h>

namespace PropertyServer {
using namespace std;

typedef SimpleWeb::Server<SimpleWeb::HTTP> HttpServer;
typedef HttpServer::Request HttpRequest;
typedef HttpServer::Response HttpResponse;

void Server::callback_device( shared_ptr<HttpResponse> response, shared_ptr<HttpRequest> request ) {
    string id = request->path_match[ 1 ];
    auto find = devices_.find( id );

    if( find == devices_.end() ) {
        write_response( response, request, 404, "Not Found" );
    } else {
        shared_ptr<Device> device = find->second;
        Json::Value node_data = create_device_node( device );
        chrono::time_point<chrono::system_clock> timestamp = get_timestamp( device );
        write_response( response, request, node_data, &timestamp );
    }
};
void Server::callback_devices( shared_ptr<HttpResponse> response, shared_ptr<HttpRequest> request ) {
    Json::Value node_devices = create_device_list_node( &devices_ );

    chrono::time_point<chrono::system_clock> timestamp = get_timestamp_all_devices();
    write_response( response, request, node_devices, &timestamp );
};

void Server::callback_properties( shared_ptr<HttpResponse> response, shared_ptr<HttpRequest> request ) {
    string id = request->path_match[ 1 ];
    auto find = devices_.find( id );

    if( find == devices_.end() ) {
        write_response( response, request, 404, "Not Found" );
    } else {
        auto device = find->second;
        Json::Value node_list = create_property_list_node( device );
        chrono::time_point<chrono::system_clock> timestamp = get_timestamp( device );
        write_response( response, request, node_list, &timestamp );
    }
};

void Server::callback_property( shared_ptr<HttpResponse> response, shared_ptr<HttpRequest> request ) {
    string device_id = request->path_match[ 1 ];
    auto device_find = devices_.find( device_id );

    if( device_find == devices_.end() ) {
        write_response( response, request, 404, "Not Found" );
        return;
    }
    auto device = device_find->second;

    string property_id = request->path_match[ 2 ];
    auto property_find = device->properties_.find( property_id );
    if( property_find == device->properties_.end() ) {
        write_response( response, request, 404, "Not Found" );
        return;
    }
    auto property = property_find->second;

    if( request->method == "PUT" ) {
        if( property->is_readonly() ) {
            //TODO GET übermitteln?
            write_response( response, request, 405, "Method Not Allowed" );
            return;
        }
        //cout << request->content.string() << endl;
        Json::Value root;
        Json::CharReaderBuilder rbuilder;
        rbuilder[ "collectComments" ] = false;
        std::string errs;
        bool ok = Json::parseFromStream( rbuilder, request->content, &root, &errs );
        if( !ok ) {
            write_response( response, request, 400, "Bad Request" );
            return;
        }
        Json::Value value = root[ "value" ];
        if( property->is_bool() && value.isBool() ) {
            property->set_bool( value.asBool() );
        } else if( property->is_long() && value.isInt64() ) {
            property->set_long( value.asInt64() );
        } else if( property->is_ulong() && value.isUInt64() ) {
            property->set_ulong( value.asUInt64() );
        } else if( property->is_double() && value.isDouble() ) {
            property->set_double( value.asDouble() );
        } else if( property->is_string() && value.isString() ) {
            property->set_string( value.asString() );
        } else {
            write_response( response, request, 400, "Bad Request" );
            return;
        }
        auto listener = device->listener_[ property_id ];
        for( auto it = listener.begin(); it != listener.end(); ++it ) {
            (*it)(device, property);
        }
    }

    Json::Value node = create_property_node( device, property );
    write_response( response, request, node, &property->get_timestamp() );
};

void Server::write_response( shared_ptr<HttpResponse> response,
                             shared_ptr<HttpRequest> request,
                             Json::Value &node,
                             chrono::time_point<chrono::system_clock> *timestamp )
{
    const string document = Json::writeString( stream_writer_builder_, node );

    write_response( response, request, 200, "application/json", document, timestamp );
};

void Server::write_response( shared_ptr<HttpResponse> response,
                             shared_ptr<HttpRequest> request,
                             u_int statusCode,
                             const string &content,
                             chrono::time_point<chrono::system_clock> *timestamp )
{
    write_response( response, request, statusCode, "text/plain", content, timestamp );
};

void Server::write_response( shared_ptr<HttpResponse> response,
                             shared_ptr<HttpRequest> request,
                             u_int statusCode,
                             const string &contentType,
                             const string &content,
                             chrono::time_point<chrono::system_clock> *timestamp )
{
    *response << "HTTP/1.1 " << statusCode << " OK\r\nContent-Type: " << contentType << "\r\n";
    if( timestamp != NULL ) {
        std::time_t std_time = chrono::system_clock::to_time_t( *timestamp );
        std::tm *gmtime = std::gmtime( &std_time );
        char *time_string = asctime(gmtime);
        //Remove trailing \n
        time_string[strlen(time_string) - 1] = '\0';
        *response << "Last-Modified: " << time_string << "\r\n";

    }
    if( request->method == "HEAD" ) {
        *response << "\r\n";
    } else {
        *response << "Content-Length: " << content.length() << "\r\n\r\n" << content;
    }
};

const string Server::create_uri( const string resource ) {
    return resource;
};

Json::Value Server::create_property_node( shared_ptr<Device> device, shared_ptr<Property> property ) {
    Json::Value node_property;

    node_property[ "id" ] = property->get_id();
    node_property[ "uri" ] = create_uri( "/properties/" + device->get_id() + "/" + property->get_id() );;
    node_property[ "name" ] = property->get_name();
    node_property[ "description" ] = property->get_description();
    node_property[ "readonly" ] = property->is_readonly();
    node_property[ "timestamp" ] = (Json::Value::UInt64) std::chrono::duration_cast<std::chrono::milliseconds>(
        property->get_timestamp().time_since_epoch() ).count();
    if( property->is_bool() ) {
        node_property[ "type" ] = "bool";
        node_property[ "value" ] = property->as_bool();
    } else if( property->is_long() ) {
        node_property[ "type" ] = "long";
        node_property[ "value" ] = (Json::Value::Int64) property->as_long();
        node_property[ "min" ] = (Json::Value::Int64) property->min_long();
        node_property[ "max" ] = (Json::Value::Int64) property->max_long();
    } else if( property->is_ulong() ) {
        node_property[ "type" ] = "ulong";
        node_property[ "value" ] = (Json::Value::UInt64) property->as_ulong();
        node_property[ "min" ] = (Json::Value::UInt64) property->min_ulong();
        node_property[ "max" ] = (Json::Value::UInt64) property->max_ulong();
    } else if( property->is_double() ) {
        node_property[ "type" ] = "double";
        node_property[ "value" ] = property->as_double();
        node_property[ "min" ] = property->min_double();
        node_property[ "max" ] = property->max_double();
    } else {
        node_property[ "type" ] = "string";
        node_property[ "value" ] = property->as_string();
    }
    return node_property;
};

Json::Value Server::create_property_list_node( shared_ptr<Device> device ) {
    unordered_map<string, shared_ptr<Property> > *properties = &device->properties_;
    Json::Value node_list = Json::arrayValue;

    for( auto it = properties->begin(); it != properties->end(); ++it ) {
        auto property = it->second;
        node_list.append( create_property_node( device, property ) );
    }
    return node_list;
};

Json::Value Server::create_device_node( shared_ptr<Device> device ) {
    Json::Value node_data;

    node_data[ "id" ] = device->get_id();
    node_data[ "uri" ] = create_uri( "/devices/" + device->get_id() );
    node_data[ "name" ] = device->get_name();
    node_data[ "description" ] = device->get_description();
    node_data[ "properties" ] = create_property_list_node( device );
    return node_data;
};

Json::Value Server::create_device_list_node( unordered_map<string, shared_ptr<Device> > *devices ) {
    Json::Value node_devices = Json::arrayValue;

    for( auto it = devices->begin(); it != devices->end(); ++it ) {
        auto device = it->second;
        Json::Value node_device = create_device_node( device );
        node_devices.append( node_device );
    }
    return node_devices;
};

Server::Server( const string name, u_int port ) : name_( name ),
    http_server_( port, 2 ), avahi_client_(), avahi_service_( name, "_property_server._tcp", port ), server_thread_( NULL ) {
    stream_writer_builder_[ "indentation" ] = "  ";
};

void Server::start( void ) {
    #define CALLBACK( name ) ( bind( &Server::callback_ ## name, this, placeholders::_1, placeholders::_2 ) )
    http_server_.resource[ "^/devices$" ][ "GET" ] = CALLBACK( devices );
    http_server_.resource[ "^/devices$" ][ "HEAD" ] = CALLBACK( devices );
    http_server_.resource[ "^/devices\\/([a-z0-9\\-_]+)$" ][ "GET" ] = CALLBACK( device );
    http_server_.resource[ "^/devices\\/([a-z0-9\\-_]+)$" ][ "HEAD" ] = CALLBACK( device );
    http_server_.resource[ "^/properties\\/([a-z0-9\\-_]+)$" ][ "GET" ] = CALLBACK( properties );
    http_server_.resource[ "^/properties\\/([a-z0-9\\-_]+)$" ][ "HEAD" ] = CALLBACK( properties );
    http_server_.resource[ "^/properties\\/([a-z0-9\\-_]+)\\/([a-z0-9\\-_]+)$" ][ "GET" ] = CALLBACK( property );
    http_server_.resource[ "^/properties\\/([a-z0-9\\-_]+)\\/([a-z0-9\\-_]+)$" ][ "HEAD" ] = CALLBACK( property );
    http_server_.resource[ "^/properties\\/([a-z0-9\\-_]+)\\/([a-z0-9\\-_]+)$" ][ "PUT" ] = CALLBACK( property );
    http_server_.default_resource[ "GET" ] =
    [ this ]( shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request ) {
        write_response( response, request, 404, request->path + ": not found", NULL );
    };
    #undef CALLBACK

    avahi_client_.publish_service( make_shared<Avahi::Service>( avahi_service_ ) );

    avahi_client_.start();
    server_thread_ = new thread([ this ]() {
                                    http_server_.start();
                                } );
};

void Server::join( void ) {
    if( server_thread_ != NULL )
        server_thread_->join();
};
void Server::add_device( shared_ptr<Device> device ) {
    devices_[ device->get_id() ] = device;
};
chrono::time_point<chrono::system_clock> Server::get_timestamp( shared_ptr<Device> device ) {
    unordered_map<string, shared_ptr<Property> > *properties = &device->properties_;
    if( properties->size() == 0 ) {
        return std::chrono::system_clock::now();
    } else {
        auto it = properties->begin();
        chrono::time_point<std::chrono::system_clock> latest = it++->second->get_timestamp();
        for(; it != properties->end(); ++it ) {
            auto property = it->second;
            if( latest < property->get_timestamp() )
                latest = property->get_timestamp();
        }
        return latest;
    }
};

chrono::time_point<chrono::system_clock> Server::get_timestamp_all_devices() {
    if( devices_.size() == 0 ) {
        return std::chrono::system_clock::now();
    } else {
        auto it = devices_.begin();
        chrono::time_point<std::chrono::system_clock> latest = get_timestamp( it++->second );
        for(; it != devices_.end(); ++it ) {
            auto device = it->second;
            chrono::time_point<std::chrono::system_clock> tmp = get_timestamp( device );
            if( latest < tmp )
                latest = tmp;
        }
        return latest;
    }
}
}
