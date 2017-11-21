#include <stdio.h>
#include <runt.h>
#include "dsp.h"

#define P0(X) printf("    "X)
#define P1(X,A) printf("    "X,A)
#define P2(X,A0,A1) printf("    "X,A0,A1)

#define PARAM(S) \
    runt_int rc;\
    runt_float val;\
    rc = pop_float(vm, &val);\
    RUNT_ERROR_CHECK(rc);\
    P1(S"(surg, %ff);\n", val);

#define PARAMI(S) \
    runt_int rc;\
    runt_float val;\
    rc = pop_float(vm, &val);\
    RUNT_ERROR_CHECK(rc);\
    P1(S"(surg, %g);\n", val);

#define OP_PARAM(S) \
    runt_int rc;\
    runt_float val;\
    rc = pop_float(vm, &val);\
    RUNT_ERROR_CHECK(rc);\
    P2(S"(surg, %d, %ff);\n", op, val);

#define OP_PARAMI(S) \
    runt_int rc;\
    runt_float val;\
    rc = pop_float(vm, &val);\
    RUNT_ERROR_CHECK(rc);\
    P2(S"(surg, %d, %g);\n", op, val);

static int op = 0;

static runt_int pop_float(runt_vm *vm, runt_float *f)
{
    runt_int rc;
    runt_stacklet *s;
    
    rc = runt_ppop(vm, &s);
    RUNT_ERROR_CHECK(rc);
    *f = s->f;

    return RUNT_OK;
}

static runt_int rproc_algo(runt_vm *vm, runt_ptr p)
{
    PARAMI("surgeon_algo");
    return RUNT_OK;
}

static runt_int rproc_transpose(runt_vm *vm, runt_ptr p)
{
    PARAMI("surgeon_transpose");
    return RUNT_OK;
}

static runt_int rproc_operator(runt_vm *vm, runt_ptr p)
{
    runt_int rc;
    runt_float val;
    rc = pop_float(vm, &val);
    RUNT_ERROR_CHECK(rc);
    op = val;
    return RUNT_OK;
}

static runt_int rproc_scale(runt_vm *vm, runt_ptr p)
{
    OP_PARAM("surgeon_op_scale");
    return RUNT_OK;
}

static runt_int rproc_freq(runt_vm *vm, runt_ptr p)
{
    OP_PARAM("surgeon_op_freq");
    return RUNT_OK;
}

static runt_int rproc_index(runt_vm *vm, runt_ptr p)
{
    OP_PARAM("surgeon_op_index");
    return RUNT_OK;
}

static runt_int rproc_amp(runt_vm *vm, runt_ptr p)
{
    OP_PARAM("surgeon_op_amp");
    return RUNT_OK;
}

static runt_int rproc_atk(runt_vm *vm, runt_ptr p)
{
    OP_PARAM("surgeon_op_atk");
    return RUNT_OK;
}

static runt_int rproc_dec(runt_vm *vm, runt_ptr p)
{
    OP_PARAM("surgeon_op_dec");
    return RUNT_OK;
}

static runt_int rproc_sus(runt_vm *vm, runt_ptr p)
{
    OP_PARAM("surgeon_op_sus");
    return RUNT_OK;
}

static runt_int rproc_rel(runt_vm *vm, runt_ptr p)
{
    OP_PARAM("surgeon_op_rel");
    return RUNT_OK;
}

static runt_int rproc_mode(runt_vm *vm, runt_ptr p)
{
    OP_PARAMI("surgeon_op_mode");
    return RUNT_OK;
}

static runt_int loader(runt_vm *vm)
{
    runt_keyword_define(vm, "algo", 4, rproc_algo, NULL);
    runt_keyword_define(vm, "transpose", 9, rproc_transpose, NULL);
    runt_keyword_define(vm, "operator", 8, rproc_operator, NULL);
    runt_keyword_define(vm, "scale", 5, rproc_scale, NULL);
    runt_keyword_define(vm, "freq", 4, rproc_freq, NULL);
    runt_keyword_define(vm, "index", 5, rproc_index, NULL);
    runt_keyword_define(vm, "amp", 3, rproc_amp, NULL);
    runt_keyword_define(vm, "atk", 3, rproc_atk, NULL);
    runt_keyword_define(vm, "dec", 3, rproc_dec, NULL);
    runt_keyword_define(vm, "sus", 3, rproc_sus, NULL);
    runt_keyword_define(vm, "rel", 3, rproc_rel, NULL);
    runt_keyword_define(vm, "mode", 4, rproc_mode, NULL);
    return runt_is_alive(vm);
}

int main(int argc, char *argv[])
{
    int rc;
    printf("static void surgeon_PRESET(surgeon_instr *surg)\n{\n");
    rc = irunt_begin(argc, argv, loader);
    printf("}\n");
    return rc;
}
