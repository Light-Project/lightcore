#ifndef _DRIVER_CLOCKSOURCE_H_
#define _DRIVER_CLOCKSOURCE_H_

#include "dt.h"

void timer_init(void);

#define TIMER_DT_DECLARE(name, compat, fn)  \
        DT_DECLARE_1_RET(timer, name, compat, fn)


#endif /* _DRIVER_CLOCKSOURCE_H_ */
