#ifndef WII_INPUT_H
#define WII_INPUT_H

#include <SDL3/SDL.h>

#ifdef __cplusplus
extern "C" {
#endif

void WII_InputInit(void);
void WII_InputShutdown(void);
void WII_InputPoll(void);

#ifdef __cplusplus
}
#endif

#endif // WII_INPUT_H
