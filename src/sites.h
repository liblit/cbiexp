#ifndef INCLUDE_SITES_H
#define INCLUDE_SITES_H


struct site_t {
    char* file;
    int line;
    char* fun;
    char* args[2];
};

extern const struct site_t sites[];


#endif // !INCLUDE_SITES_H
