#pragma once

#include <json/json.h>

#include <unordered_map>
#include <chrono>

#include <property_server/property.hpp>
#include <property_server/device.hpp>
#include <property_server/Simple-Web-Server/server_http.hpp>

#include <avahi_client/avahi_client.hpp>

namespace PropertyServer {
using namespace std;

typedef SimpleWeb::Server<SimpleWeb::HTTP> HttpServer;
typedef HttpServer::Request HttpRequest;
typedef HttpServer::Response HttpResponse;

class Server {
private:
    const string name_;
    HttpServer http_server_;
    unordered_map<string, shared_ptr<Device> > devices_;
    Json::StreamWriterBuilder stream_writer_builder_;
    Avahi::Client avahi_client_;
    Avahi::Service avahi_service_;
    thread *server_thread_;

    void callback_device( shared_ptr<HttpResponse> response, shared_ptr<HttpRequest> request );
    void callback_devices( shared_ptr<HttpResponse> response, shared_ptr<HttpRequest> request );
    void callback_properties( shared_ptr<HttpResponse> response, shared_ptr<HttpRequest> request );
    void callback_property( shared_ptr<HttpResponse> response, shared_ptr<HttpRequest> request );

    void write_response( shared_ptr<HttpResponse> response,
                         shared_ptr<HttpRequest> request,
                         Json::Value &node,
                         chrono::time_point<chrono::system_clock> *timestamp = NULL );
    void write_response( shared_ptr<HttpResponse> response,
                         shared_ptr<HttpRequest> request,
                         u_int statusCode,
                         const string &content,
                         chrono::time_point<chrono::system_clock> *timestamp = NULL );
    void write_response( shared_ptr<HttpResponse> response,
                         shared_ptr<HttpRequest> request,
                         u_int statusCode,
                         const string &contentType,
                         const string &content,
                         chrono::time_point<chrono::system_clock> *timestamp = NULL );

    const string create_uri( const string resource );

    Json::Value create_property_node( shared_ptr<Device> device, shared_ptr<Property> property );
    Json::Value create_property_list_node( shared_ptr<Device> device );
    Json::Value create_device_node( shared_ptr<Device> device );
    Json::Value create_device_list_node( unordered_map<string, shared_ptr<Device> > *devices );

    chrono::time_point<chrono::system_clock> get_timestamp( shared_ptr<Device> device );
    chrono::time_point<chrono::system_clock> get_timestamp_all_devices();


public:
    Server( const string name, u_int port );
    void start( void );
    void join( void );
    void add_device( shared_ptr<Device> device );
};
}
