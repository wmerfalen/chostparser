#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include <stdlib.h>
enum parse_state_t {
  ps_scheme,
  ps_domain,
  ps_port,
  ps_tld,
  ps_error,
  ps_okay,
};

struct parsed_host {
  size_t len;
  void* name;
  unsigned short port;
};

int extract_hostname(const char* name,const size_t nameSize,struct parsed_host* inParsed){
  const char* ptr = name;
  char tmpBuf[256];
  int parseState = ps_scheme;
  size_t tmpBufIndex = 0;
  char portBuf[6] = {0}; // max value of 65535
  int portIndex=0;
  memset(tmpBuf,0,sizeof(tmpBuf));
  memset((void*)inParsed,0,sizeof(struct parsed_host));
  
  for(size_t i=0; i < nameSize && ptr[i] != '\0';++i){
    if(ptr[i] == '.'){
      if(parseState == ps_scheme){
        parseState = ps_domain;
      }
      if(parseState == ps_port){
        return ps_error;
      }
    }
    if(!isdigit(ptr[i]) && parseState == ps_port){
      portBuf[portIndex] = '\0';
      inParsed->port = (unsigned short)atoi(portBuf);
      inParsed->name = (void*)strdup((const char*)tmpBuf);
      inParsed->len = tmpBufIndex;
      return ps_okay;
    }
    if(isdigit(ptr[i])){
      if(parseState == ps_scheme){
        parseState = ps_domain;
      }
      if(parseState == ps_port){
        if(portIndex >= 6){
          return ps_error;
        }
        portBuf[portIndex++] = ptr[i];
        continue;
      }
    }
    if(parseState == ps_domain && ptr[i] == '.'){
      if(tmpBufIndex - 1 >= 0 && tmpBuf[tmpBufIndex-1] == '.'){
        return ps_error;
      }
      tmpBuf[tmpBufIndex++] = ptr[i];
      continue;
    }
    if(isalpha(ptr[i]) || ptr[i] == '-' || isdigit(ptr[i])){
      tmpBuf[tmpBufIndex++] = ptr[i];
      continue;
    }
    if(parseState == ps_domain && ptr[i] == ':') {
      parseState = ps_port;
      continue;
    }
    if(parseState == ps_domain && ptr[i] == '/'){
      inParsed->name = (void*)strdup((const char*)tmpBuf);
      inParsed->len = tmpBufIndex;
      if(portIndex){
        inParsed->port = (unsigned short)atoi(&portBuf[0]);
      }
      return ps_okay;
    }
    if(parseState == ps_scheme && ptr[i] == ':' && i + 2 < nameSize && strncmp(&ptr[i],"://",3) == 0){
      memset(tmpBuf,0,i+1);
      tmpBufIndex = 0;
      i += 2;
      parseState = ps_domain;
      continue;
    }
  }
  if(portIndex){
    inParsed->name = (void*)strdup((const char*)tmpBuf);
    inParsed->len = tmpBufIndex;
    inParsed->port = (unsigned short)atoi(&portBuf[0]);
    return ps_okay;
  }
  return ps_error;
}

int main(int argc,char** argv){
  if(argc < 2){
    return 1;
  }
  struct parsed_host hostInfo;
  memset((void*)&hostInfo,0,sizeof(struct parsed_host));
  int n = extract_hostname(argv[1],strlen(argv[1]),&hostInfo);
  switch(n){
    case ps_error:
    fprintf(stderr,"Invalid hostname detected\n");
    return 1;
    case ps_okay:
    printf("host: '%s'\nport: %d\nlen: %zu\n",(char*)hostInfo.name,hostInfo.port,hostInfo.len);
    break;
    default:
    printf("idk\n");
    break;
  }
  return 0;
}
