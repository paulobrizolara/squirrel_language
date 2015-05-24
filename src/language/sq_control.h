#ifndef SQ_CONTROL_H
#define SQ_CONTROL_H

        
/***************************** DO-WHILE MACROS ********************************/
#define DO_START(do_id) \
    do_start_##do_id :  \
        ;
        
#define DO_END(do_id, condition) \
    if(condition)                \
        goto do_start_##do_id ;

/****************************** WHILE MACROS ********************************/
#define WHILE_START(while_id, condition)\
    while_start_##while_id :            \
    if(!(condition))                    \
        goto while_exit_##while_id ;

#define WHILE_END(while_id)         \
    goto while_start_##while_id ;   \
    while_exit_##while_id :         \
        ;    
        
/****************************** FOR MACROS ********************************/

#define FOR_START(for_id, initialization, cond_test, incr_stmt)\
    initialization;             \
    goto for_test_##for_id ;    \
    for_incr_##for_id:          \
        incr_stmt;              \
    for_test_##for_id :         \
    if(!(cond_test))            \
       goto for_exit_##for_id ;
        
#define FOR_END(for_id)                 \
    goto for_incr_##for_id;             \
    for_exit_##for_id:                  \
    ; //empty statement

/****************************** IF-ELSE MACROS ********************************/

#define IF_START(if_id, condition) \
    if(!(condition))               \
        goto if_exit_##if_id ;

#define IF_END(if_id)   \
    if_exit_##if_id:     \
        ;

#define ELSE_START(if_id)       \
    /*Se o if for executado, próxima instrução fará execução pular para fim do else*/\
    goto else_exit_##if_id ;    \
    if_exit_##if_id:            \
        ;                       \

#define ELSE_END(if_id) \
    else_exit_##if_id : \
        ;             

/******************************************************************************/

#endif
