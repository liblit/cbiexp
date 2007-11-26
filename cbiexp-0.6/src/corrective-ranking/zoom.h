#ifndef INCLUDE_corrective_ranking_zoom_h
#define INCLUDE_corrective_ranking_zoom_h

#include "Foci.h"

class Candidates;


void buildZooms(const Candidates &, const char projection[]);
void buildZooms(const Candidates &, const char projection[], const Foci &);


#endif // !INCLUDE_corrective_ranking_zoom_h
