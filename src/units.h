#ifndef INCLUDE_UNITS_H
#define INCLUDE_UNITS_H


struct Unit {
  const char *s;
  int c;
};

extern const struct Unit units[];

extern const int NumUnits;
extern const int NumBPreds;
extern const int NumRPreds;
extern const int NumSPreds;


#endif // !INCLUDE_UNITS_H
