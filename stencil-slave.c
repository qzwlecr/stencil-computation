#include <string.h>
#include "slave.h"
#include "cal.h"
#include "common.h"

//#define PROFILING

#ifdef PROFILING
#define CPE
#define LWPF_KERNELS K(A) K(B) K(C)
#define LWPF_UNIT U(TEST)
#include "lwpf2.h"
#endif

#define DUMP(varname) cal_locked_printf("[%d][%d]%s = %d\n", pid, id, #varname, varname);
#define DUMPF(varname) cal_locked_printf("[%d][%d]%s = %lf\n", pid, id, #varname, varname);

#define THREAD_NUM 64

void stencil_7_com(grid_param *p) {
#ifdef PROFILING
    lwpf_enter(TEST);
#endif
    ptr_t src, dest;

    int id = athread_get_id(-1);
    int pid = p->grid_info->p_id;
    volatile int get_reply = 0, put_reply = 0;

    int x_size = p->grid_info->local_size_x;
    int y_size = p->grid_info->local_size_y / THREAD_NUM;

    int x_begin = p->grid_info->halo_size_x;
    int x_end = p->grid_info->local_size_x + p->grid_info->halo_size_x;

    int y_begin = y_size * id + p->grid_info->halo_size_y;
    int y_end = y_size * (id + 1) + p->grid_info->halo_size_y;

    int z_begin = p->grid_info->halo_size_z;
    int z_end = p->grid_info->local_size_z + p->grid_info->halo_size_z;

    int ldx = p->grid_info->local_size_x + 2 * p->grid_info->halo_size_x;
    int ldy = p->grid_info->local_size_y + 2 * p->grid_info->halo_size_y;

    int nt = p->nt;

    int ori_size, ans_size;
    //hardcode to satisfy ldm memory size
    if (x_size == 256) {
        ori_size = 4;
        ans_size = 2;
    } else {
        if (x_size == 192) {
            ori_size = 5;
            ans_size = 3;
        } else {
            cal_locked_printf("x size is not as expected...\n");
            return;
        }
    }
    data_t origin[ldx * ori_size * ori_size];
    data_t answer[ldx * ans_size * ans_size];
    for (int t = 0; t < nt; t++) {
        while (*p->sync == 0);
        src = *p->src, dest = *p->dest;
        for (int zz = z_begin; zz < z_end; zz += ans_size) {
            for (int yy = y_begin; yy < y_end; yy += ans_size) {

                get_reply = 0;
                athread_get(PE_MODE, &src[INDEX(0, yy - 1, zz - 1, ldx, ldy)], origin,
                            ldx * ori_size * ori_size * sizeof(data_t),
                            (void *) &get_reply, 0, ldx * (ldy - ori_size) * sizeof(data_t),
                            ldx * ori_size * sizeof(data_t));
                while (get_reply != 1);

                for (int z = 1; z <= ans_size; ++z) {
                    for (int y = 1; y <= ans_size; ++y) {
                        for (int x = x_begin; x < x_end; ++x) {
                            answer[INDEX(x, y - 1, z - 1, ldx, ans_size)] =
                                    ALPHA_ZZZ * origin[INDEX(x, y, z, ldx, ori_size)]
                                    + ALPHA_NZZ * origin[INDEX(x - 1, y, z, ldx, ori_size)]
                                    + ALPHA_PZZ * origin[INDEX(x + 1, y, z, ldx, ori_size)]
                                    + ALPHA_ZNZ * origin[INDEX(x, y - 1, z, ldx, ori_size)]
                                    + ALPHA_ZPZ * origin[INDEX(x, y + 1, z, ldx, ori_size)]
                                    + ALPHA_ZZN * origin[INDEX(x, y, z - 1, ldx, ori_size)]
                                    + ALPHA_ZZP * origin[INDEX(x, y, z + 1, ldx, ori_size)];

                        }

                    }
                }

                put_reply = 0;
                athread_put(PE_MODE, answer, &dest[INDEX(0, yy, zz, ldx, ldy)],
                            ldx * ans_size * ans_size * sizeof(data_t),
                            (void *) &put_reply, ldx * (ldy - ans_size) * sizeof(data_t),
                            ldx * ans_size * sizeof(data_t));
                while (put_reply != 1);
            }
        }
        athread_syn(ARRAY_SCOPE, 0xffff);
        if (id == 0) {
            *p->sync = 0;
        }
        athread_syn(ARRAY_SCOPE, 0xffff);
    }
#ifdef PROFILING
    lwpf_exit(TEST);
#endif
    return;
}

void stencil_27_com(grid_param *p) {
#ifdef PROFILING
    lwpf_enter(TEST);
#endif
    int ori_size, ans_size;
    ptr_t src, dest;

    int id = athread_get_id(-1);
    int pid = p->grid_info->p_id;
    volatile int get_reply = 0, put_reply = 0;

    int x_size = p->grid_info->local_size_x;
    int y_size = p->grid_info->local_size_y / THREAD_NUM;

    int x_begin = p->grid_info->halo_size_x;
    int x_end = p->grid_info->local_size_x + p->grid_info->halo_size_x;

    int y_begin = y_size * id + p->grid_info->halo_size_y;
    int y_end = y_size * (id + 1) + p->grid_info->halo_size_y;

    int z_begin = p->grid_info->halo_size_z;
    int z_end = p->grid_info->local_size_z + p->grid_info->halo_size_z;

    int ldx = p->grid_info->local_size_x + 2 * p->grid_info->halo_size_x;
    int ldy = p->grid_info->local_size_y + 2 * p->grid_info->halo_size_y;

    int nt = p->nt;

    //hardcode to satisfy ldm memory size
    if (x_size == 256) {
        ori_size = 4;
        ans_size = 2;
    } else {
        if (x_size == 192) {
            ori_size = 5;
            ans_size = 3;
        } else {
            cal_locked_printf("x size is not as expected...\n");
            return;
        }
    }
    data_t origin[ldx * ori_size * ori_size];
    data_t answer[ldx * ans_size * ans_size];
    for (int t = 0; t < nt; t++) {
        while (*p->sync == 0);
        src = *p->src, dest = *p->dest;
        for (int zz = z_begin; zz < z_end; zz += ans_size) {
            for (int yy = y_begin; yy < y_end; yy += ans_size) {

                get_reply = 0;
                athread_get(PE_MODE, &src[INDEX(0, yy - 1, zz - 1, ldx, ldy)], origin,
                            ldx * ori_size * ori_size * sizeof(data_t),
                            (void *) &get_reply, 0, ldx * (ldy - ori_size) * sizeof(data_t),
                            ldx * ori_size * sizeof(data_t));
                while (get_reply != 1);

                for (int z = 1; z <= ans_size; ++z) {
                    for (int y = 1; y <= ans_size; ++y) {
                        for (int x = x_begin; x < x_end; ++x) {
                            answer[INDEX(x, y - 1, z - 1, ldx, ans_size)] =
                                    ALPHA_ZZZ * origin[INDEX(x, y, z, ldx, ori_size)]
                                    + ALPHA_NZZ * origin[INDEX(x - 1, y, z, ldx, ori_size)]
                                    + ALPHA_PZZ * origin[INDEX(x + 1, y, z, ldx, ori_size)]
                                    + ALPHA_ZNZ * origin[INDEX(x, y - 1, z, ldx, ori_size)]
                                    + ALPHA_ZPZ * origin[INDEX(x, y + 1, z, ldx, ori_size)]
                                    + ALPHA_ZZN * origin[INDEX(x, y, z - 1, ldx, ori_size)]
                                    + ALPHA_ZZP * origin[INDEX(x, y, z + 1, ldx, ori_size)]
                                    + ALPHA_NNZ * origin[INDEX(x - 1, y - 1, z, ldx, ori_size)]
                                    + ALPHA_PNZ * origin[INDEX(x + 1, y - 1, z, ldx, ori_size)]
                                    + ALPHA_NPZ * origin[INDEX(x - 1, y + 1, z, ldx, ori_size)]
                                    + ALPHA_PPZ * origin[INDEX(x + 1, y + 1, z, ldx, ori_size)]
                                    + ALPHA_NZN * origin[INDEX(x - 1, y, z - 1, ldx, ori_size)]
                                    + ALPHA_PZN * origin[INDEX(x + 1, y, z - 1, ldx, ori_size)]
                                    + ALPHA_NZP * origin[INDEX(x - 1, y, z + 1, ldx, ori_size)]
                                    + ALPHA_PZP * origin[INDEX(x + 1, y, z + 1, ldx, ori_size)]
                                    + ALPHA_ZNN * origin[INDEX(x, y - 1, z - 1, ldx, ori_size)]
                                    + ALPHA_ZPN * origin[INDEX(x, y + 1, z - 1, ldx, ori_size)]
                                    + ALPHA_ZNP * origin[INDEX(x, y - 1, z + 1, ldx, ori_size)]
                                    + ALPHA_ZPP * origin[INDEX(x, y + 1, z + 1, ldx, ori_size)]
                                    + ALPHA_NNN * origin[INDEX(x - 1, y - 1, z - 1, ldx, ori_size)]
                                    + ALPHA_PNN * origin[INDEX(x + 1, y - 1, z - 1, ldx, ori_size)]
                                    + ALPHA_NPN * origin[INDEX(x - 1, y + 1, z - 1, ldx, ori_size)]
                                    + ALPHA_PPN * origin[INDEX(x + 1, y + 1, z - 1, ldx, ori_size)]
                                    + ALPHA_NNP * origin[INDEX(x - 1, y - 1, z + 1, ldx, ori_size)]
                                    + ALPHA_PNP * origin[INDEX(x + 1, y - 1, z + 1, ldx, ori_size)]
                                    + ALPHA_NPP * origin[INDEX(x - 1, y + 1, z + 1, ldx, ori_size)]
                                    + ALPHA_PPP * origin[INDEX(x + 1, y + 1, z + 1, ldx, ori_size)];


                        }

                    }
                }

                put_reply = 0;
                athread_put(PE_MODE, answer, &dest[INDEX(0, yy, zz, ldx, ldy)],
                            ldx * ans_size * ans_size * sizeof(data_t),
                            (void *) &put_reply, ldx * (ldy - ans_size) * sizeof(data_t),
                            ldx * ans_size * sizeof(data_t));
                while (put_reply != 1);
            }
        }
        athread_syn(ARRAY_SCOPE, 0xffff);
        if (id == 0) {
            *p->sync = 0;
        }
        athread_syn(ARRAY_SCOPE, 0xffff);
    }
#ifdef PROFILING
    lwpf_exit(TEST);
#endif
    return;
}
