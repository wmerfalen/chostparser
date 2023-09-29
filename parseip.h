#ifndef __C_HOST_PARSER_HEADER__
#define __C_HOST_PARSER_HEADER__
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

int extract_hostname(const char* name,const size_t nameSize,struct parsed_host* inParsed);


#endif
