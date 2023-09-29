# Overview

A simple host parser written in C

# The API
```c
int extract_hostname(
  const char* name,
  const size_t nameSize,
  struct parsed_host* inParsed
  );
```
## `name`
A character buffer pointing to the url/host to parse

## `nameSize`
An unsigned integer representing the size of the `name` buffer

## `inParsed`
A pointer to a `parsed_host` struct. 
The results of `extract_hostname` will be placed in this pointer.

## return value
`extract_hostname` will return one of two possible values:
- `ps_okay`
- `ps_error`

### `ps_okay`
If `ps_okay` is returned, the `parsed_host.name` char buffer points to an allocated string buffer. This buffer must be freed by the calling function using `free(ptr->name)`.

### `ps_error`
If `ps_error` is returned, the `parsed_host.name` char buffer will *NOT* contain an allocated string buffer. Attempting to call `free` on this buffer is undefined behavior.


# `struct parsed_host`
## `name`
A `char*` buffer pointing to the host name. This must be freed by the calling function.

## `port`
An `unsigned short` representing the port number (if any).
If a port was specified in a URL such as:
`http://foobar.com:8181`, then upon successful parse, `.port` will be `8181`

## `len`
An unsigned int representing the size of the `name` buffer.



# Example

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


