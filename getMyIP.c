#include "profinet.h"

// Returns hostname for the local computer 
void checkHostName(int hostname) 
{ 
    if (hostname == -1) 
    { 
        perror("gethostname"); 
        exit(1); 
    } 
} 
  
// Returns host information corresponding to host name 
void checkHostEntry(struct hostent * hostentry) 
{ 
    if (hostentry == NULL) 
    { 
        perror("gethostbyname"); 
        exit(1); 
    } 
} 
  
// Converts space-delimited IPv4 addresses 
// to dotted-decimal format 
void checkIPbuffer(char *IPbuffer) 
{ 
    if (NULL == IPbuffer) 
    { 
        perror("inet_ntoa"); 
        exit(1); 
    } 
} 
  
void getMyIP(char *ip_address)
{
    int fd;
    struct ifreq ifr;
    fd = socket(AF_INET, SOCK_DGRAM, 0);
     
    ifr.ifr_addr.sa_family = AF_INET;
     
    memcpy(ifr.ifr_name, "enp2s0", IFNAMSIZ-1);
     
    ioctl(fd, SIOCGIFADDR, &ifr);
    close(fd);
     
    strcpy(ip_address,inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));
     
}
int check_ip_address()
{
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in sin;
    sin.sin_family = AF_INET;
    sin.sin_port   = htons(65432);  // Could be anything
    inet_pton(AF_INET, "172.16.0.70", &sin.sin_addr);

    if (connect(sockfd, (struct sockaddr *) &sin, sizeof(sin)) == -1)
    {
        printf("Error connecting 172.16.0.70: %d (%s)\n", errno, strerror(errno));
    }

    sin.sin_family = AF_INET;
    sin.sin_port   = htons(65432);  // Could be anything
    inet_pton(AF_INET, "172.16.0.7", &sin.sin_addr);

    if (connect(sockfd, (struct sockaddr *) &sin, sizeof(sin)) == -1)
    {
        printf("Error connecting 172.16.0.7: %d (%s)\n", errno, strerror(errno));
    }
}
