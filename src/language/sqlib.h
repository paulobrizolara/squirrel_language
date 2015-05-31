#ifndef SQLIB_H
#define SQLIB_H

#include "sq_types.h"
#include "sq_memory.h"
#include "sq_control.h"
#include "sq_input.h"
#include "sq_metatype.h"
#include "sq_cast.h"
#include <stdio.h> //printf

void squirrel_setup(){
    sq_memory_setup();
}

#endif
