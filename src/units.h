#ifndef INCLUDE_UNITS_H
#define INCLUDE_UNITS_H


struct unit_t {
  char scheme_code;
  const char *signature;
  int num_sites;
};

extern const struct unit_t units[];

extern const int num_units;

extern const int NumBPreds;
extern const int NumRPreds;
extern const int NumSPreds;


#endif // !INCLUDE_UNITS_H
