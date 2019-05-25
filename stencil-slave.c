#include <slave.h>
#include <string.h>
#include "cal.h"
#include "common.h"

#define DUMP(varname) cal_locked_printf("[%d][%d]%s = %d\n", pid, id, #varname, varname);

#define LENGTH (384+2)
#define THREAD_NUM 64

void stencil_7_com(param *p) {
    data_t origin[4 * 4 * LENGTH];
    data_t answer[2 * 2 * LENGTH];
    volatile int id = athread_get_id(-1);
    volatile int pid = p->grid_info->p_id;
    volatile int get_reply = 0, put_reply = 0;

    int x_begin = p->grid_info->halo_size_x;
    int x_end = p->grid_info->local_size_x + p->grid_info->halo_size_x;

    int y_size = p->grid_info->local_size_y / THREAD_NUM;

    int y_begin = y_size * id + p->grid_info->halo_size_y;
    int y_end = y_size * (id + 1) + p->grid_info->halo_size_y;

    int z_begin = p->grid_info->halo_size_z;
    int z_end = p->grid_info->local_size_z + p->grid_info->halo_size_z;

    int ldx = p->grid_info->local_size_x + 2 * p->grid_info->halo_size_x;
    int ldy = p->grid_info->local_size_y + 2 * p->grid_info->halo_size_y;

//    if(id == 0){
//        DUMP(x_begin);
//        DUMP(x_end);
//        DUMP(y_size);
//        DUMP(y_begin);
//        DUMP(y_end);
//        DUMP(z_begin);
//        DUMP(z_end);
//        DUMP(ldx);
//        DUMP(ldy);
//    }

    ptr_t src, dest;
    int nt = p->nt;
    if(pid == 0 && id == 0){
        DUMP(nt);
    }

    // cache strategy is full x, splited y, zero z
    // maybe too slow when block size is small

    for (int t = 0; t < nt; t++) {
        while (*p->sync == 0);
        //DUMP(*p->sync);
        src = *p->src, dest = *p->dest;
        for (int zz = z_begin; zz < z_end; zz += 2) {
            for (int yy = y_begin; yy < y_end; yy += 2) {

                get_reply = 0;
                athread_get(PE_MODE, &src[INDEX(0, yy - 1, zz - 1, ldx, ldy)], &origin, ldx * 16 * sizeof(data_t),
                            &get_reply, 0, ldx * (ldy - 4) * sizeof(data_t), ldx * 2 * sizeof(data_t));
                while (get_reply != 1);

                for (int z = 1; z <= 2; ++z) {
                    for (int y = 1; y <= 2; ++y) {
                        int ldy = 4;
                        for (int x = x_begin; x < x_end; ++x) {
                            answer[INDEX(x, y, z, ldx, ldy)] =
                                    ALPHA_ZZZ * origin[INDEX(x, y, z, ldx, ldy)]
                                    + ALPHA_NZZ * origin[INDEX(x - 1, y, z, ldx, ldy)]
                                    + ALPHA_PZZ * origin[INDEX(x + 1, y, z, ldx, ldy)]
                                    + ALPHA_ZNZ * origin[INDEX(x, y - 1, z, ldx, ldy)]
                                    + ALPHA_ZPZ * origin[INDEX(x, y + 1, z, ldx, ldy)]
                                    + ALPHA_ZZN * origin[INDEX(x, y, z - 1, ldx, ldy)]
                                    + ALPHA_ZZP * origin[INDEX(x, y, z + 1, ldx, ldy)];

                        }

                    }
                }

                put_reply = 0;
                athread_put(PE_MODE, &answer, &dest[INDEX(0, yy, zz, ldx, ldy)], ldx * 4 * sizeof(data_t),
                            &put_reply, ldx * (ldy - 2) * sizeof(data_t), ldx * 2 * sizeof(data_t));
                while (put_reply != 1);
            }
        }
        athread_syn(ARRAY_SCOPE, 0xffff);
        if(id == 0){
            *p->sync = 0;
        }
    }
    return;
}

void stencil_27_com(param *p) {
    data_t origin[4 * 4 * LENGTH];
    data_t answer[2 * 2 * LENGTH];
    volatile int id = athread_get_id(-1);
    volatile int pid = p->grid_info->p_id;
    volatile int get_reply = 0, put_reply = 0;

    int x_begin = p->grid_info->halo_size_x;
    int x_end = p->grid_info->local_size_x + p->grid_info->halo_size_x;

    int y_size = p->grid_info->local_size_y / THREAD_NUM;

    int y_begin = y_size * id + p->grid_info->halo_size_y;
    int y_end = y_size * (id + 1) + p->grid_info->halo_size_y;

    int z_begin = p->grid_info->halo_size_z;
    int z_end = p->grid_info->local_size_z + p->grid_info->halo_size_z;

    int ldx = p->grid_info->local_size_x + 2 * p->grid_info->halo_size_x;
    int ldy = p->grid_info->local_size_y + 2 * p->grid_info->halo_size_y;

//    if(id == 0){
//        DUMP(x_begin);
//        DUMP(x_end);
//        DUMP(y_size);
//        DUMP(y_begin);
//        DUMP(y_end);
//        DUMP(z_begin);
//        DUMP(z_end);
//        DUMP(ldx);
//        DUMP(ldy);
//    }

    ptr_t src, dest;
    int nt = p->nt;
    if(pid == 0 && id == 0){
        DUMP(nt);
    }

    // cache strategy is full x, splited y, zero z
    // maybe too slow when block size is small

    for (int t = 0; t < nt; t++) {
        while (*p->sync == 0);
        //DUMP(*p->sync);
        src = *p->src, dest = *p->dest;
        for (int zz = z_begin; zz < z_end; zz += 2) {
            for (int yy = y_begin; yy < y_end; yy += 2) {

                get_reply = 0;
                athread_get(PE_MODE, &src[INDEX(0, yy - 1, zz - 1, ldx, ldy)], &origin, ldx * 16 * sizeof(data_t),
                            &get_reply, 0, ldx * (ldy - 4) * sizeof(data_t), ldx * 2 * sizeof(data_t));
                while (get_reply != 1);

                for (int z = 1; z <= 2; ++z) {
                    for (int y = 1; y <= 2; ++y) {
                        int ldy = 4;
                        for (int x = x_begin; x < x_end; ++x) {
                            answer[INDEX(x, y, z, ldx, ldy)] =
                                    ALPHA_ZZZ * origin[INDEX(x, y, z, ldx, ldy)]
                                    + ALPHA_NZZ * origin[INDEX(x - 1, y, z, ldx, ldy)]
                                    + ALPHA_PZZ * origin[INDEX(x + 1, y, z, ldx, ldy)]
                                    + ALPHA_ZNZ * origin[INDEX(x, y - 1, z, ldx, ldy)]
                                    + ALPHA_ZPZ * origin[INDEX(x, y + 1, z, ldx, ldy)]
                                    + ALPHA_ZZN * origin[INDEX(x, y, z - 1, ldx, ldy)]
                                    + ALPHA_ZZP * origin[INDEX(x, y, z + 1, ldx, ldy)]
                                    + ALPHA_NNZ * origin[INDEX(x - 1, y - 1, z, ldx, ldy)]
                                    + ALPHA_PNZ * origin[INDEX(x + 1, y - 1, z, ldx, ldy)]
                                    + ALPHA_NPZ * origin[INDEX(x - 1, y + 1, z, ldx, ldy)]
                                    + ALPHA_PPZ * origin[INDEX(x + 1, y + 1, z, ldx, ldy)]
                                    + ALPHA_NZN * origin[INDEX(x - 1, y, z - 1, ldx, ldy)]
                                    + ALPHA_PZN * origin[INDEX(x + 1, y, z - 1, ldx, ldy)]
                                    + ALPHA_NZP * origin[INDEX(x - 1, y, z + 1, ldx, ldy)]
                                    + ALPHA_PZP * origin[INDEX(x + 1, y, z + 1, ldx, ldy)]
                                    + ALPHA_ZNN * origin[INDEX(x, y - 1, z - 1, ldx, ldy)]
                                    + ALPHA_ZPN * origin[INDEX(x, y + 1, z - 1, ldx, ldy)]
                                    + ALPHA_ZNP * origin[INDEX(x, y - 1, z + 1, ldx, ldy)]
                                    + ALPHA_ZPP * origin[INDEX(x, y + 1, z + 1, ldx, ldy)]
                                    + ALPHA_NNN * origin[INDEX(x - 1, y - 1, z - 1, ldx, ldy)]
                                    + ALPHA_PNN * origin[INDEX(x + 1, y - 1, z - 1, ldx, ldy)]
                                    + ALPHA_NPN * origin[INDEX(x - 1, y + 1, z - 1, ldx, ldy)]
                                    + ALPHA_PPN * origin[INDEX(x + 1, y + 1, z - 1, ldx, ldy)]
                                    + ALPHA_NNP * origin[INDEX(x - 1, y - 1, z + 1, ldx, ldy)]
                                    + ALPHA_PNP * origin[INDEX(x + 1, y - 1, z + 1, ldx, ldy)]
                                    + ALPHA_NPP * origin[INDEX(x - 1, y + 1, z + 1, ldx, ldy)]
                                    + ALPHA_PPP * origin[INDEX(x + 1, y + 1, z + 1, ldx, ldy)];

                        }

                    }
                }

                put_reply = 0;
                athread_put(PE_MODE, &answer, &dest[INDEX(0, yy, zz, ldx, ldy)], ldx * 4 * sizeof(data_t),
                            &put_reply, ldx * (ldy - 2) * sizeof(data_t), ldx * 2 * sizeof(data_t));
                while (put_reply != 1);
            }
        }
        athread_syn(ARRAY_SCOPE, 0xffff);
        if(id == 0){
            *p->sync = 0;
        }
    }
    return;
}