#ifndef TIMINGS_H
#define TIMINGS_H

#include <sys/time.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct timeval CLOCK_T;
#define CLOCK(c) gettimeofday(&c,(struct timezone *)NULL)
#define CLOCK_DIFF(c1,c2)  \
((double)(c1.tv_sec-c2.tv_sec)+(double)(c1.tv_usec-c2.tv_usec)/1e+6)
#define CLOCK_DISPLAY(c) fprintf(stderr,"%d.%d",(int)c.tv_sec,(int)c.tv_usec)

#endif /*TIMINGS_H*/

