#include <signal.h>

int Signal(int signo, void *func_ptr)
{  
    struct sigaction act, oact;
   
    act.sa_handler = (void(*)(int))func_ptr;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
   
    if(signo == SIGALRM)  {
        #ifdef SA_INTERRUPT
        /* Sun OS */
        act.sa_flags |= SA_INTERRUPT;
        #endif
    } else  {
        #ifdef SA_RESTART
        /* SVR4, 4.3+BSD */
        act.sa_flags |= SA_RESTART;
        #endif
    }

    return(sigaction(signo, &act, &oact));
}
