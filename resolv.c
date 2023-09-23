//#define MS_WINDOWS 1 
// compile mingw64 on windows.
// gcc -o ResolveIP ResolveIP.c -lws2_32

#ifdef MS_WINDOWS
#include <stdio.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <tchar.h>

// Initialize WinSock and handle errors (Windows only)
int InitializeWinSock() {
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        _ftprintf(stderr, _T("WSAStartup failed with error: %d\n"), result);
    }
    return result;
}

// Cleanup WinSock (Windows only)
void CleanupWinSock() {
    WSACleanup();
}

// Function to resolve an IP address from various input formats
int ResolveIP(const TCHAR *inString, TCHAR *ipBuffer, size_t ipBufferLen) {
    struct addrinfo hints;
    struct addrinfo *result, *rp;
    int status;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;    // Allow both IPv4 and IPv6
    hints.ai_socktype = SOCK_STREAM;

    // Initialize WinSock (Windows only)
    if (InitializeWinSock() != 0) {
        return 1;
    }

    // Check if the input contains "http://" and remove it
    TCHAR *hostname = _tcsstr(inString, _T("://"));
    if (hostname != NULL) {
        hostname += _tcslen(_T("://"));
    } else {
        // If "http://" is not present, use the input as it is
        hostname = (TCHAR *)inString;
    }

    // Check if the input doesn't contain ".com" and add it
    if (_tcsstr(hostname, _T(".com")) == NULL) {
        // Append ".com"
        _tcscat(hostname, _T(".com"));
    }

    // Use getaddrinfo to resolve the IP address
    status = getaddrinfo(hostname, NULL, &hints, &result);
    if (status != 0) {
        _ftprintf(stderr, _T("getaddrinfo: %s\n"), gai_strerror(status));
        CleanupWinSock();
        return 1;
    }

    for (rp = result; rp != NULL; rp = rp->ai_next) {
        void *addr;
        TCHAR ipstr[INET6_ADDRSTRLEN];

        if (rp->ai_family == AF_INET) {
            struct sockaddr_in *ipv4 = (struct sockaddr_in *)rp->ai_addr;
            addr = &(ipv4->sin_addr);
        } else {
            struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)rp->ai_addr;
            addr = &(ipv6->sin6_addr);
        }

        // Convert the IP address to a string
        if (InetNtop(rp->ai_family, addr, ipstr, sizeof(ipstr)) == NULL) {
            _ftprintf(stderr, _T("InetNtop failed with error: %d\n"), WSAGetLastError());
            freeaddrinfo(result);
            CleanupWinSock();
            return 1;
        }

        _tcsncpy(ipBuffer, ipstr, ipBufferLen);
        freeaddrinfo(result);
        CleanupWinSock();
        return 0;
    }

    _ftprintf(stderr, _T("Could not resolve IP address\n"));
    freeaddrinfo(result);
    CleanupWinSock();
    return 1;
}

int _tmain(int argc, TCHAR *argv[]) {
    if (argc != 2) {
        _ftprintf(stderr, _T("Usage: %s <URL or IP>\n"), argv[0]);
        return 1;
    }

    TCHAR resolvedIP[INET6_ADDRSTRLEN];
    if (ResolveIP(argv[1], resolvedIP, sizeof(resolvedIP)) == 0) {
        _tprintf(_T("Resolved IP address: %s\n"), resolvedIP);
    }

    return 0;
}

#else
// compile linux gcc -o ResolveIP ResolveIP.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <arpa/inet.h>

// Function to resolve an IP address from various input formats
int ResolveIP(const char *inString, char *ipBuffer, size_t ipBufferLen) {
    struct addrinfo hints;
    struct addrinfo *result, *rp;
    int status;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;    // Allow both IPv4 and IPv6
    hints.ai_socktype = SOCK_STREAM;

    // Check if the input contains "http://" and remove it
    const char *hostname = strstr(inString, "://");
    if (hostname != NULL) {
        hostname += strlen("://");
    } else {
        // If "http://" is not present, use the input as it is
        hostname = inString;
    }

    // Check if the input doesn't contain ".com" and add it
    if (strstr(hostname, ".com") == NULL) {
        // Append ".com" to a temporary buffer
        char tempBuffer[256];
        snprintf(tempBuffer, sizeof(tempBuffer), "%s.com", hostname);

        // Copy the modified hostname back to the original
        strcpy(ipBuffer, tempBuffer);
        hostname = ipBuffer;
    }

    // Use getaddrinfo to resolve the IP address
    status = getaddrinfo(hostname, NULL, &hints, &result);
    if (status != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
        return 1;
    }

    for (rp = result; rp != NULL; rp = rp->ai_next) {
        void *addr;
        char ipstr[INET6_ADDRSTRLEN];

        if (rp->ai_family == AF_INET) {
            struct sockaddr_in *ipv4 = (struct sockaddr_in *)rp->ai_addr;
            addr = &(ipv4->sin_addr);
        } else {
            struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)rp->ai_addr;
            addr = &(ipv6->sin6_addr);
        }

        // Convert the IP address to a string
        if (inet_ntop(rp->ai_family, addr, ipstr, sizeof(ipstr)) == NULL) {
            perror("inet_ntop");
            freeaddrinfo(result);
            return 1;
        }

        strncpy(ipBuffer, ipstr, ipBufferLen);
        freeaddrinfo(result);
        return 0;
    }

    fprintf(stderr, "Could not resolve IP address\n");
    freeaddrinfo(result);
    return 1;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <URL or IP>\n", argv[0]);
        return 1;
    }

    char resolvedIP[INET6_ADDRSTRLEN];
    if (ResolveIP(argv[1], resolvedIP, sizeof(resolvedIP)) == 0) {
        printf("Resolved IP address: %s\n", resolvedIP);
    }

    return 0;
}
#endif
