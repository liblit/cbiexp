#ifndef INCLUDE_moss_reporting_h
#define INCLUDE_moss_reporting_h


void reportError(             unsigned errorId, const char format[], ...) __attribute__((format (printf, 2, 3)));
void reportErrorIf(int guard, unsigned errorId, const char format[], ...) __attribute__((format (printf, 3, 4)));

void checkNewlineInCommentError();
void checkDatabaseCountError();


#endif /* INCLUDE_moss_reporting_h */
