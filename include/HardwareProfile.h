#ifndef HARDWARE_PROFILE_H
#define HARDWARE_PROFILE_H
/** HARDVER DEFIN�CI�K BECSATOL�SA ***********************************/
  #if defined(__18F14K50)
    #include "piccolo-14k50.h"
  #elif defined(__18F4550)
    #include "piccolo-4550.h"
  #else
     #error "A v�lasztott mikrovez�rl� t�pust nem t�mogatja a PICCOLO project!"  
  #endif

#endif  //HARDWARE_PROFILE_H

