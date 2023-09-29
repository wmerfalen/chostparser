# Overview

A simple host parser written in C

# How to use

```c
#include "parseip.h"

//
// Usage: ./a.out <URL>
// 
int main(int argc,char** argv){
  // Expecting 1 argument which is the URL or Host
  if(argc < 2){
    return 1;
  }
  // Step 1: Create a parsed_host structure
  struct parsed_host hostInfo;

  // Step 2: zero out the struct
  memset((void*)&hostInfo,0,sizeof(struct parsed_host));

  // Step 3:
  // - Pass the char buffer of the url/host to parse
  // - Pass the size of said buffer. Using strlen
  // - Pass in a pointer to the parsed_host struct
  int n = extract_hostname(argv[1],strlen(argv[1]),&hostInfo);

  // The function will return ps_okay if a URL/host was parsed
  // successfully. Otherwise, it'll return ps_error.
  //
  switch(n){
    case ps_error:
    fprintf(stderr,"Invalid hostname detected\n");
    return 1;
    case ps_okay:
    // hostInfo.name - will contain the parsed host
    // hostInfo.port - will contain a port if one is detected
    //  i.e.: http://foobar.com:8181/ - hostInfo.port will be unsigned short 8181
    // hostInfo.len - will contain the length of the buffer
    printf("host: '%s'\nport: %d\nlen: %zu\n",(char*)hostInfo.name,hostInfo.port,hostInfo.len);
    break;
    default:
    printf("idk\n");
    break;
  }
  // The hostInfo struct requires that you free the name member
  // if a successfull parse was completed
  if(n == ps_okay){
    free(hostInfo.name);
  }
  return 0;

}

```
