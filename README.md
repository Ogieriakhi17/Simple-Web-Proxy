[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-22041afd0340ce965d47ae6ef1cefeee28c7c493a6346c4f15d667ab976d596c.svg)](https://classroom.github.com/a/vezoAnG3)

#### CPTS 360 Lab 4

Base code and documentation for WSU CPTS360 Lab 4 (Web Proxy)

`proxy.c`
`csapp.h`
`csapp.c`
    These are starter files.  `csapp.c` and `csapp.h` are described in
    your textbook. You may make any changes you like to these files.  
    You may create and submit any additional files you like. 
    You may use `port-for-user.pl` or `free-port.sh` to generate
    unique ports for your proxy or tiny server. 

`Makefile`
    This is the makefile that builds the proxy program.  Type `make`
    to build your solution, or `make clean` followed by `make` for a
    fresh build. 

`port-for-user.pl`
    Generates a random port for a particular user
    usage: `./port-for-user.pl <userID>`

`free-port.sh`
    Handy script that identifies an unused TCP port that you can use
    for your proxy or tiny. 
    usage: `./free-port.sh`

`tiny`
    Tiny Web server from the CSAPP textbook

