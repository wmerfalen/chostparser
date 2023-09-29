#include "parseip.h"

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
  if(n == ps_okay){
    free(hostInfo.name);
  }
  return 0;
}
