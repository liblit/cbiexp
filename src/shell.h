#ifndef INCLUDE_SHELL_H
#define INCLUDE_SHELL_H


extern bool verbose;

void shell(const char format[], ...) __attribute__((format (printf, 1, 2)));


#endif // !INCLUDE_SHELL_H
