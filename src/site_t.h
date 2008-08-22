#ifndef INCLUDE_site_t_h
#define INCLUDE_site_t_h


struct site_t {
    char* file;
    int line;
    char* fun;
    int cfg_node;
    char scheme_code;
    char* args[2];
    unsigned unit_index;
    unsigned site_offset; 
};


#endif // !INCLUDE_site_t_h
