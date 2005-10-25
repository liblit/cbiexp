#ifndef INCLUDE_unit_t_h
#define INCLUDE_unit_t_h

struct site_t;


struct unit_t {
  char scheme_code;
  const char *signature;
  unsigned num_sites;
  const site_t *sites;
};


#endif // !INCLUDE_unit_t_h
