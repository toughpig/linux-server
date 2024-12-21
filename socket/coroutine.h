#include <iostream>
#include <cassert>
#include <chrono>
#include <thread>
using namespace std;

inline void print(int a){printf("case %d\n",a);}

// 带上下文的coro
#define freen(p)    do {free(p); p = nullptr; } while(0);

#define CORO_REENTER(c)     int *foo = (int*)*c; \
                            if (!foo) {*c = malloc(sizeof(*foo));foo = (int*)*c;assert(foo); *foo = 0;} \
                            switch (*foo) case -11: if (false){break;} else case 0:

#define CORO_YIELD(n, ms)   for(*((int*)*c) = (n);;) { \
                                if (false) { \
                                    case (n): \
                                    print(n);break; \
                                } else { \
                                    printf("sleep\n"); \
                                    return;} \
                            }

#define yield(ms) CORO_YIELD(__COUNTER__ + 1, ms)
#define reenter CORO_REENTER(c)
#define end_coro  freen(*c); return; 