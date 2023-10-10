#include <sys/socket.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <stdio.h>
#include <stdlib.h>
#include <cstdint>

int main(void) {

    struct ifaddrs* adresses;

    if(getifaddrs(&adresses) == -1) {
        printf("getifaddrs call failed\n");
        return -1;
    }

    struct ifaddrs* adress = adresses;
    while(adress) {
        const int family = adress->ifa_addr->sa_family;
	if(family == AF_INET || family == AF_INET6) {
		printf( "%s\t", adress->ifa_name );
		printf( "%s\t", family == AF_INET ? "IPv4" : "IPv6" );

		char ap[100] = {0};
		const size_t family_size = family == AF_INET ? sizeof(struct sockaddr_in) : sizeof( struct sockaddr_in6 );
		getnameinfo(adress->ifa_addr, 
			    family_size, 
			    ap, 
			    sizeof(ap),
			    0, 
			    0, 
			    NI_NUMERICHOST);
		printf("\t%s\n", ap);
	}
	adress = adress->ifa_next;
    }

    freeifaddrs(adresses);

    return 0;
}

