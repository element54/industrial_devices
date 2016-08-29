# industrial_devices
## Installation
1. Install [Raspbian Jessie](https://www.raspberrypi.org/downloads/raspbian/)
    1. Expand root filesystem (`sudo raspi-config`)
    2. Change hostname
    3. `sudo apt-get update`
    4. `sudo apt-get dist-upgrade`
2. Install dependencies  
    1. `sudo apt-get install libboost-all-dev build-essential avahi-daemon cmake libavahi-client-dev git cmake`
    2. Install jsoncpp
        1. `git clone https://github.com/open-source-parsers/jsoncpp.git -b 1.7.3`
        2. `cd jsoncpp`
        3. `mkdir build`
        4. `cd build`
        5. `cmake -DCMAKE_BUILD_TYPE=release -DBUILD_STATIC_LIBS=OFF -DBUILD_SHARED_LIBS=ON ..`
        6. `make`
        7. `sudo make install`
    3. Install wiringPi
        1. `git clone git://git.drogon.net/wiringPi -b 2.32`
        2. `cd wiringPi`
        3. `./build`
3. Install industrial_devices
    1. `git clone https://github.com/element54/industrial_devices.git`
    2. `cd industrial_devices`
    3. `mkdir build`
    4. `cd build`
    5. `cmake ..`
    6. `make install`
4. Install systemd service  
    Open file `/etc/systemd/system/rotating_turntable.service` and insert:  
    
        [Unit]
        Description=rotating_turntable
        After=network.target
        
        [Service]
        ExecStart=/home/aw/industrial_devices/bin/rotating_turntable
        KillMode=process
        Restart=always
        
        [Install]
        WantedBy=multi-user.target
        
    Enable service: `sudo systemctl enable rotating_turntable.service`
