#include "parseip.h"

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
