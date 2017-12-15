#include <stdio.h>
#include <runt.h>
#include "dsp.h"


#define PROC(name) rproc_##name
#define FUN(name) static int PROC(name)(runt_vm *vm, runt_ptr p)
#define KEYWORD(NAME,SIZE,FUN) runt_keyword_define(vm,NAME,SIZE,FUN,NULL)


FUN(foo)
{
    runt_print(vm, "we are foo.\n");
    return RUNT_OK;
}

static runt_int loader(runt_vm *vm)
{
    KEYWORD("foo", 3, PROC(foo));
    return runt_is_alive(vm);
}

int main(int argc, char *argv[])
{
    int rc;
    rc = irunt_begin(argc, argv, loader);
    return rc;
}
