#ifndef INCLUDE_UNITS_H
#define INCLUDE_UNITS_H


struct unit_t {
  char scheme_code;
  const char *signature;
  unsigned num_sites;
};

extern const struct unit_t units[];

extern const unsigned num_units;

extern const int NumBPreds;
extern const int NumRPreds;
extern const int NumSPreds;
extern const int NumGPreds;


#endif // !INCLUDE_UNITS_H
