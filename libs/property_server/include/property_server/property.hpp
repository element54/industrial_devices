#pragma once

#include <string>
#include <string.h>
#include <memory>
#include <chrono>

namespace PropertyServer {
using namespace std;

class Property {
    friend class Server;
public:
    Property( const string id, const string name, const string description, const bool readonly, long value, long min, long max );
    Property( const string id,
              const string name,
              const string description,
              const bool readonly,
              unsigned long value,
              unsigned long min,
              unsigned long max );
    Property( const string id, const string name, const string description, const bool readonly, double value, double min, double max );
    Property( const string id, const string name, const string description, const bool readonly, bool value );
    Property( const string id, const string name, const string description, const bool readonly, const string value );
    const string get_id( void );
    const string get_name( void );
    const string get_description( void );
    chrono::time_point<chrono::system_clock>& get_timestamp( void );
    bool is_readonly( void );

    bool is_long( void );
    bool is_ulong( void );
    bool is_double( void );
    bool is_bool( void );
    bool is_string( void );

    long as_long( void );
    unsigned long as_ulong( void );
    bool as_bool( void );
    double as_double( void );
    const string as_string( void );

    operator double( void ) {
        return as_double();
    };
    operator long( void ) {
        return as_long();
    };
    operator unsigned long( void ) {
        return as_ulong();
    };
    operator bool( void ) {
        return as_bool();
    };
    operator const string( void ) {
        return as_string();
    };

    long min_long( void );
    long max_long( void );
    unsigned long min_ulong( void );
    unsigned long max_ulong( void );
    double min_double( void );
    double max_double( void );

    void set_long( long value );
    void set_ulong( unsigned long value );
    void set_bool( bool value );
    void set_double( double value );
    void set_string( const string &value );

    void operator=( long value ) {
        set_long( value );
    };
    void operator=( unsigned long value ) {
        set_ulong( value );
    };
    void operator=( bool value ) {
        set_bool( value );
    };
    void operator=( double value ) {
        set_double( value );
    };
    void operator=( const string &value ) {
        set_string( value );
    };
private:
    enum ValueType {
        bool_,
        string_,
        long_,
        ulong_,
        double_,
        unknown_
    };
    union ValueHolder {
        long long_;
        double double_;
        bool bool_;
        unsigned long ulong_;
        const char *string_;
    };
    const string id_;
    const string name_;
    const string description_;
    const bool readonly_;
    const ValueType type_;
    chrono::time_point<chrono::system_clock> timestamp_;
    union ValueHolder value_;
    union ValueHolder min_value_;
    union ValueHolder max_value_;

    Property( const string id, const string name, const string description, const bool readonly, const ValueType type );

    void update_timestamp( void );
};

}
