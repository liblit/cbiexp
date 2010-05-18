#ifndef INCLUDE_site_t_h
#define INCLUDE_site_t_h


struct site_t {
    const char* file;
    int line;
    const char* fun;
    int cfg_node;
    char scheme_code;
    const char* args[2];
};


#endif // !INCLUDE_site_t_h
