#include <string.h>
#include <slave.h>
#include <assert.h>
#include "cal.h"
#include "time.h"
#include "simd.h"
#include "common.h"

//#define PROFILING
//#define TIMING

#ifdef PROFILING
#define CPE
#define LWPF_KERNELS K(A) K(B) K(C)
#define LWPF_UNIT U(TEST)
#include "lwpf2.h"
#endif

#ifdef TIMING
#define TIMING_COMPUTE_BEGIN rtc_(&rpcc_begin);
#else
#define TIMING_COMPUTE_BEGIN ;
#endif
#ifdef TIMING
#define TIMING_COMPUTE_END rtc_(&rpcc_end); compute += rpcc_end - rpcc_begin;
#else
#define TIMING_COMPUTE_END ;
#endif
#ifdef TIMING
#define TIMING_COMMUNI_BEGIN rtc_(&rpcc_begin);
#else
#define TIMING_COMMUNI_BEGIN ;
#endif
#ifdef TIMING
#define TIMING_COMMUNI_END rtc_(&rpcc_end); communicate += rpcc_end - rpcc_begin;
#else
#define TIMING_COMMUNI_END ;
#endif
#ifdef TIMING
#define TIMING_MEMCPY_BEGIN rtc_(&rpcc_begin_in);
#else
#define TIMING_MEMCPY_BEGIN ;
#endif
#ifdef TIMING
#define TIMING_MEMCPY_END rtc_(&rpcc_end_in); memcpy_time += rpcc_end_in - rpcc_begin_in;
#else
#define TIMING_MEMCPY_END ;
#endif

#define DUMP(varname) cal_locked_printf("[%d][%d]%s = %d\n", pid, id, #varname, varname);
#define DUMPF(varname) cal_locked_printf("[%d][%d]%s = %lf\n", pid, id, #varname, varname);

#define THREAD_NUM 64

extern volatile int non_runnable;

__thread_local volatile int runnable0 = 0;
__thread_local volatile int runnable1 = 0;


#define _memcpy(dst, src, len) { for (int i = 0; i < len; i++) *(dst + i) = *(src + i);}

static inline void rtc_(unsigned long *counter) {
    unsigned long rpcc;
    asm volatile("rcsr %0, 4":"=r"(rpcc));
    *counter = rpcc;
}

void stencil_7_com(grid_param *p) {
#ifdef PROFILING
    lwpf_enter(TEST);
#endif
    int id = athread_get_id(-1);
    int pid = p->grid_info->p_id;
    volatile int get_reply = 0, put_reply = 0;

    int local_size_x = p->grid_info->local_size_x;
    int local_size_y = p->grid_info->local_size_y;
    int local_size_z = p->grid_info->local_size_z;
    int halo_size_x = p->grid_info->halo_size_x;
    int halo_size_y = p->grid_info->halo_size_y;
    int halo_size_z = p->grid_info->halo_size_z;

    int tot_size = local_size_y * (local_size_z - 2);
    //the highest and lowest layer wouldn't be computed first.
    int per_size = tot_size / THREAD_NUM;

    int x_begin = halo_size_x;
    int x_end = local_size_x + halo_size_x;

    int z_loc[4];
    int y_begin[4];
    int y_end[4];
    y_begin[0] = per_size * id % local_size_y + halo_size_y;
    y_end[0] = per_size * (id + 1) % local_size_y + halo_size_y;

    y_begin[1] = halo_size_y;
    if (y_end[0] < y_begin[0]) {
        y_end[1] = y_end[0];
        y_end[0] = local_size_y + halo_size_y;
    } else {
        y_end[1] = halo_size_y;
    }

    //compute the highest and lowest layer.

    y_begin[2] = local_size_y / THREAD_NUM * id + halo_size_y;
    y_end[2] = local_size_y / THREAD_NUM * (id + 1) + halo_size_y;

    y_begin[3] = local_size_y / THREAD_NUM * id + halo_size_y;
    y_end[3] = local_size_y / THREAD_NUM * (id + 1) + halo_size_y;

    z_loc[0] = per_size * id / local_size_y + halo_size_z + 1;
    z_loc[1] = per_size * id / local_size_y + halo_size_z + 2;
    z_loc[2] = 1;
    z_loc[3] = local_size_z;

    int ldx = local_size_x + 2 * halo_size_x;
    int ldy = local_size_y + 2 * halo_size_y;

    int nt = p->nt;

    ptr_t src, dest;
    ptr_t buffer[2] = {p->grid, p->aux};

    data_t origin[ldx * 3 * 3];
    data_t answer[ldx * 1 * 1];
    runnable0 = 0;
    runnable1 = 0;

#ifdef TIMING
    unsigned long rpcc_begin, rpcc_end;
    unsigned long compute = 0, communicate = 0;
#endif

    for (int t = 0; t < nt; t++) {
        src = buffer[t % 2];
        dest = buffer[(t + 1) % 2];
#ifdef PROFILING
        lwpf_start(A);
#endif
        //deal with main part
        for (int z = 0; z <= 1; z++) {
            int y0 = 0, y1 = 1, y2 = 2;
            int zz = z_loc[z];
            for (int yy = y_begin[z]; yy < y_end[z]; yy++) {
                if (yy == y_begin[z]) {
                    TIMING_COMMUNI_BEGIN;
                    get_reply = 0;
                    athread_get(PE_MODE, &src[INDEX(0, yy - 1, zz, ldx, ldy)], &origin[INDEX(0, 0, 1, ldx, 3)],
                                ldx * 2 * sizeof(data_t), (void *) &get_reply, 0, 0, 0);
                    while (get_reply != 1);
                    TIMING_COMMUNI_END;
                }

                //use rotated array to avoid sloooooooow memcpy


                TIMING_COMMUNI_BEGIN;
                get_reply = 0;
                athread_get(PE_MODE, &src[INDEX(0, yy + 1, zz, ldx, ldy)], &origin[INDEX(0, y2, 1, ldx, 3)],
                            ldx * sizeof(data_t), (void *) &get_reply, 0, 0, 0);
                athread_get(PE_MODE, &src[INDEX(0, yy, zz - 1, ldx, ldy)], &origin[INDEX(0, y1, 0, ldx, 3)],
                            ldx * sizeof(data_t), (void *) &get_reply, 0, 0, 0);
                athread_get(PE_MODE, &src[INDEX(0, yy, zz + 1, ldx, ldy)], &origin[INDEX(0, y1, 2, ldx, 3)],
                            ldx * sizeof(data_t), (void *) &get_reply, 0, 0, 0);
                while (get_reply != 3);
                TIMING_COMMUNI_END;

                TIMING_COMPUTE_BEGIN;
                for (int x = x_begin; x < x_end; ++x) {
                    answer[INDEX(x, 0, 0, ldx, 1)] =
                            ALPHA_ZZZ * origin[INDEX(x, y1, 1, ldx, 3)]
                            + ALPHA_NZZ * origin[INDEX(x - 1, y1, 1, ldx, 3)]
                            + ALPHA_PZZ * origin[INDEX(x + 1, y1, 1, ldx, 3)]
                            + ALPHA_ZNZ * origin[INDEX(x, y0, 1, ldx, 3)]
                            + ALPHA_ZPZ * origin[INDEX(x, y2, 1, ldx, 3)]
                            + ALPHA_ZZN * origin[INDEX(x, y1, 0, ldx, 3)]
                            + ALPHA_ZZP * origin[INDEX(x, y1, 2, ldx, 3)];

                }
                TIMING_COMPUTE_END;

                TIMING_COMMUNI_BEGIN;
                put_reply = 0;
                athread_put(PE_MODE, answer, &dest[INDEX(0, yy, zz, ldx, ldy)],
                            ldx * 1 * 1 * sizeof(data_t),
                            (void *) &put_reply, ldx * (ldy - 1) * sizeof(data_t),
                            ldx * 1 * sizeof(data_t));
                y0 = (y0 + 1) % 3;
                y1 = (y1 + 1) % 3;
                y2 = (y2 + 1) % 3;
                while (put_reply != 1);
                TIMING_COMMUNI_END;
            }

        }
        int finish0 = 0, finish1 = 0;

        int z = -1;
        //int zz = z_loc[z];

        while (1) {
            if (finish0 && finish1) break;
            //if (runnable0 == 2) finish0 = 1;
            //if (runnable1 == 2) finish1 = 1;
            if (runnable0 && finish0 == 0) {
                z = 3;
                finish0 = 1;
            } else if (runnable1 && finish1 == 0) {
                z = 2;
                finish1 = 1;
            } else {
                continue;
            }
            int zz = z_loc[z];
            int y0 = 0, y1 = 1, y2 = 2;
            for (int yy = y_begin[z]; yy < y_end[z]; yy++) {
                if (yy == y_begin[z]) {
                    TIMING_COMMUNI_BEGIN;
                    get_reply = 0;
                    athread_get(PE_MODE, &src[INDEX(0, yy - 1, zz, ldx, ldy)], &origin[INDEX(0, 0, 1, ldx, 3)],
                                ldx * 2 * sizeof(data_t), (void *) &get_reply, 0, 0, 0);
                    while (get_reply != 1);
                    TIMING_COMMUNI_END;
                }

                //use rotated array to avoid sloooooooow memcpy

                TIMING_COMMUNI_BEGIN;
                get_reply = 0;
                athread_get(PE_MODE, &src[INDEX(0, yy + 1, zz, ldx, ldy)], &origin[INDEX(0, y2, 1, ldx, 3)],
                            ldx * sizeof(data_t), (void *) &get_reply, 0, 0, 0);
                athread_get(PE_MODE, &src[INDEX(0, yy, zz - 1, ldx, ldy)], &origin[INDEX(0, y1, 0, ldx, 3)],
                            ldx * sizeof(data_t), (void *) &get_reply, 0, 0, 0);
                athread_get(PE_MODE, &src[INDEX(0, yy, zz + 1, ldx, ldy)], &origin[INDEX(0, y1, 2, ldx, 3)],
                            ldx * sizeof(data_t), (void *) &get_reply, 0, 0, 0);
                while (get_reply != 3);
                TIMING_COMMUNI_END;

                TIMING_COMPUTE_BEGIN;
                for (int x = x_begin; x < x_end; ++x) {
                    answer[INDEX(x, 0, 0, ldx, 1)] =
                            ALPHA_ZZZ * origin[INDEX(x, y1, 1, ldx, 3)]
                            + ALPHA_NZZ * origin[INDEX(x - 1, y1, 1, ldx, 3)]
                            + ALPHA_PZZ * origin[INDEX(x + 1, y1, 1, ldx, 3)]
                            + ALPHA_ZNZ * origin[INDEX(x, y0, 1, ldx, 3)]
                            + ALPHA_ZPZ * origin[INDEX(x, y2, 1, ldx, 3)]
                            + ALPHA_ZZN * origin[INDEX(x, y1, 0, ldx, 3)]
                            + ALPHA_ZZP * origin[INDEX(x, y1, 2, ldx, 3)];

                }
                TIMING_COMPUTE_END;

                TIMING_COMMUNI_BEGIN;
                put_reply = 0;
                athread_put(PE_MODE, answer, &dest[INDEX(0, yy, zz, ldx, ldy)],
                            ldx * 1 * 1 * sizeof(data_t),
                            (void *) &put_reply, ldx * (ldy - 1) * sizeof(data_t),
                            ldx * 1 * sizeof(data_t));
                y0 = (y0 + 1) % 3;
                y1 = (y1 + 1) % 3;
                y2 = (y2 + 1) % 3;
                while (put_reply != 1);
                TIMING_COMMUNI_END;
            }
        }

#ifdef PROFILING
        lwpf_stop(A);
#endif
        athread_syn(ARRAY_SCOPE, 0xffff);
        runnable0 = 0;
        runnable1 = 0;
        if (id == 0) {
            *p->non_runnable = 0;
        }
        athread_syn(ARRAY_SCOPE, 0xffff);
    }

#ifdef PROFILING
    lwpf_exit(TEST);
#endif
#ifdef TIMING
    if (pid == 1) {
        cal_locked_printf("compute:%d, communicate:%d\n", compute, communicate);
    }
#endif
    return;
}

void stencil_27_com(grid_param *p) {
#ifdef PROFILING
    lwpf_enter(TEST);
#endif
    int id = athread_get_id(-1);
    int pid = p->grid_info->p_id;
    volatile int get_reply = 0, put_reply = 0;

    int local_size_x = p->grid_info->local_size_x;
    int local_size_y = p->grid_info->local_size_y;
    int local_size_z = p->grid_info->local_size_z;
    int halo_size_x = p->grid_info->halo_size_x;
    int halo_size_y = p->grid_info->halo_size_y;
    int halo_size_z = p->grid_info->halo_size_z;

    int tot_size = local_size_y * (local_size_z - 2);
    //the highest and lowest layer wouldn't be computed first.
    int per_size = tot_size / THREAD_NUM;

    int x_begin = halo_size_x;
    int x_end = local_size_x + halo_size_x;

    int z_loc[4];
    int y_begin[4];
    int y_end[4];
    y_begin[0] = per_size * id % local_size_y + halo_size_y;
    y_end[0] = per_size * (id + 1) % local_size_y + halo_size_y;

    y_begin[1] = halo_size_y;
    if (y_end[0] < y_begin[0]) {
        y_end[1] = y_end[0];
        y_end[0] = local_size_y + halo_size_y;
    } else {
        y_end[1] = halo_size_y;
    }

    //compute the highest and lowest layer.

    y_begin[2] = local_size_y / THREAD_NUM * id + halo_size_y;
    y_end[2] = local_size_y / THREAD_NUM * (id + 1) + halo_size_y;

    y_begin[3] = local_size_y / THREAD_NUM * id + halo_size_y;
    y_end[3] = local_size_y / THREAD_NUM * (id + 1) + halo_size_y;

    z_loc[0] = per_size * id / local_size_y + halo_size_z + 1;
    z_loc[1] = per_size * id / local_size_y + halo_size_z + 2;
    z_loc[2] = 1;
    z_loc[3] = local_size_z;

    int ldx = local_size_x + 2 * halo_size_x;
    int ldy = local_size_y + 2 * halo_size_y;

    int nt = p->nt;

    ptr_t src, dest;
    ptr_t buffer[2] = {p->grid, p->aux};

    data_t origin[ldx * 3 * 3];
    data_t answer[ldx * 1 * 1];
    //runnable = 0;
    doublev4 alpha_vector[9];
    doublev4 data_vector[9];
    data_t data[28];

    alpha_vector[0] = simd_set_doublev4(ALPHA_NNN, ALPHA_ZNN, ALPHA_PNN, ALPHA_NZN);
    alpha_vector[1] = simd_set_doublev4(ALPHA_ZZN, ALPHA_PZN, ALPHA_NPN, ALPHA_ZPN);
    alpha_vector[2] = simd_set_doublev4(ALPHA_PPN, ALPHA_NNZ, ALPHA_ZNZ, ALPHA_PNZ);
    alpha_vector[3] = simd_set_doublev4(ALPHA_NZZ, ALPHA_ZZZ, ALPHA_PZZ, ALPHA_NPZ);
    alpha_vector[4] = simd_set_doublev4(ALPHA_ZPZ, ALPHA_PPZ, ALPHA_NNP, ALPHA_ZNP);
    alpha_vector[5] = simd_set_doublev4(ALPHA_PNP, ALPHA_NZP, ALPHA_ZZP, ALPHA_PZP);
    alpha_vector[6] = simd_set_doublev4(ALPHA_NPP, ALPHA_ZPP, ALPHA_PPP, 0);

#ifdef TIMING
    unsigned long rpcc_begin, rpcc_end;
    unsigned long rpcc_begin_in, rpcc_end_in;
    unsigned long compute = 0, communicate = 0, memcpy_time = 0;
#endif

    for (int t = 0; t < nt; t++) {
        src = buffer[t % 2];
        dest = buffer[(t + 1) % 2];
#ifdef PROFILING
        lwpf_start(A);
#endif
        //deal with main part
        for (int z = 0; z <= 1; z++) {
            int y0 = 0, y1 = 1, y2 = 2;
            int zz = z_loc[z];
            for (int yy = y_begin[z]; yy < y_end[z]; yy++) {
                if (yy == y_begin[z]) {
                    TIMING_COMMUNI_BEGIN;
                    get_reply = 0;
                    athread_get(PE_MODE, &src[INDEX(0, yy - 1, zz - 1, ldx, ldy)], &origin[INDEX(0, 0, 0, ldx, 3)],
                                ldx * 2 * sizeof(data_t), (void *) &get_reply, 0, 0, 0);
                    athread_get(PE_MODE, &src[INDEX(0, yy - 1, zz, ldx, ldy)], &origin[INDEX(0, 0, 1, ldx, 3)],
                                ldx * 2 * sizeof(data_t), (void *) &get_reply, 0, 0, 0);
                    athread_get(PE_MODE, &src[INDEX(0, yy - 1, zz + 1, ldx, ldy)], &origin[INDEX(0, 0, 2, ldx, 3)],
                                ldx * 2 * sizeof(data_t), (void *) &get_reply, 0, 0, 0);
                    while (get_reply != 3);
                    TIMING_COMMUNI_END;
                }

                //use rotated array to avoid sloooooooow memcpy

                TIMING_COMMUNI_BEGIN;
                get_reply = 0;
                athread_get(PE_MODE, &src[INDEX(0, yy + 1, zz - 1, ldx, ldy)], &origin[INDEX(0, y2, 0, ldx, 3)],
                            ldx * sizeof(data_t), (void *) &get_reply, 0, 0, 0);
                athread_get(PE_MODE, &src[INDEX(0, yy + 1, zz, ldx, ldy)], &origin[INDEX(0, y2, 1, ldx, 3)],
                            ldx * sizeof(data_t), (void *) &get_reply, 0, 0, 0);
                athread_get(PE_MODE, &src[INDEX(0, yy + 1, zz + 1, ldx, ldy)], &origin[INDEX(0, y2, 2, ldx, 3)],
                            ldx * sizeof(data_t), (void *) &get_reply, 0, 0, 0);
                while (get_reply != 3);
                TIMING_COMMUNI_END;

                TIMING_COMPUTE_BEGIN;
                for (int x = x_begin; x < x_end; ++x) {
                    TIMING_MEMCPY_BEGIN;
                    _memcpy(data, &origin[INDEX(x - 1, y0, 0, ldx, 3)], 3);
                    _memcpy(data + 3, &origin[INDEX(x - 1, y1, 0, ldx, 3)], 3);
                    _memcpy(data + 6, &origin[INDEX(x - 1, y2, 0, ldx, 3)], 3);
                    _memcpy(data + 9, &origin[INDEX(x - 1, y0, 1, ldx, 3)], 3);
                    _memcpy(data + 12, &origin[INDEX(x - 1, y1, 1, ldx, 3)], 3);
                    _memcpy(data + 15, &origin[INDEX(x - 1, y2, 1, ldx, 3)], 3);
                    _memcpy(data + 18, &origin[INDEX(x - 1, y0, 2, ldx, 3)], 3);
                    _memcpy(data + 21, &origin[INDEX(x - 1, y1, 2, ldx, 3)], 3);
                    _memcpy(data + 24, &origin[INDEX(x - 1, y2, 2, ldx, 3)], 3);
                    TIMING_MEMCPY_END;

                    simd_load(data_vector[0], data);
                    simd_load(data_vector[1], data + 4);
                    simd_load(data_vector[2], data + 8);
                    simd_load(data_vector[3], data + 12);
                    simd_load(data_vector[4], data + 16);
                    simd_load(data_vector[5], data + 20);
                    simd_load(data_vector[6], data + 24);


                    for (int j = 0; j < 7; j++) data_vector[j] = data_vector[j] * alpha_vector[j];
                    for (int j = 1; j < 7; j++) data_vector[0] = data_vector[0] + data_vector[j];
                    simd_store(data_vector[0], data);
                    answer[INDEX(x, 0, 0, ldx, 1)] = data[0] + data[1] + data[2] + data[3];
                }
                TIMING_COMPUTE_END;

                TIMING_COMMUNI_BEGIN;
                put_reply = 0;
                athread_put(PE_MODE, answer, &dest[INDEX(0, yy, zz, ldx, ldy)],
                            ldx * 1 * 1 * sizeof(data_t),
                            (void *) &put_reply, ldx * (ldy - 1) * sizeof(data_t),
                            ldx * 1 * sizeof(data_t));
                y0 = (y0 + 1) % 3;
                y1 = (y1 + 1) % 3;
                y2 = (y2 + 1) % 3;
                while (put_reply != 1);
                TIMING_COMMUNI_END;
            }

        }

        // if (id == 0)
        //     while (runnable == 0);
        //athread_syn(ARRAY_SCOPE, 0xffff);
        int finish0 = 0, finish1 = 0;
        int z = -1;
        while (1) {
            if (finish0 && finish1) break;
            if (runnable0 && finish0 == 0) {
                z = 3;
                finish0 = 1;
            } else if (runnable1 && finish1 == 0) {
                z = 2;
                finish1 = 1;
            } else {
                continue;
            }
            int zz = z_loc[z];
            int y0 = 0, y1 = 1, y2 = 2;
            for (int yy = y_begin[z]; yy < y_end[z]; yy++) {
                if (yy == y_begin[z]) {
                    TIMING_COMMUNI_BEGIN;
                    get_reply = 0;
                    athread_get(PE_MODE, &src[INDEX(0, yy - 1, zz - 1, ldx, ldy)], &origin[INDEX(0, 0, 0, ldx, 3)],
                                ldx * 2 * sizeof(data_t), (void *) &get_reply, 0, 0, 0);
                    athread_get(PE_MODE, &src[INDEX(0, yy - 1, zz, ldx, ldy)], &origin[INDEX(0, 0, 1, ldx, 3)],
                                ldx * 2 * sizeof(data_t), (void *) &get_reply, 0, 0, 0);
                    athread_get(PE_MODE, &src[INDEX(0, yy - 1, zz + 1, ldx, ldy)], &origin[INDEX(0, 0, 2, ldx, 3)],
                                ldx * 2 * sizeof(data_t), (void *) &get_reply, 0, 0, 0);
                    while (get_reply != 3);
                    TIMING_COMMUNI_END;
                }
                TIMING_COMMUNI_BEGIN;
                get_reply = 0;
                athread_get(PE_MODE, &src[INDEX(0, yy + 1, zz - 1, ldx, ldy)], &origin[INDEX(0, y2, 0, ldx, 3)],
                            ldx * sizeof(data_t), (void *) &get_reply, 0, 0, 0);
                athread_get(PE_MODE, &src[INDEX(0, yy + 1, zz, ldx, ldy)], &origin[INDEX(0, y2, 1, ldx, 3)],
                            ldx * sizeof(data_t), (void *) &get_reply, 0, 0, 0);
                athread_get(PE_MODE, &src[INDEX(0, yy + 1, zz + 1, ldx, ldy)], &origin[INDEX(0, y2, 2, ldx, 3)],
                            ldx * sizeof(data_t), (void *) &get_reply, 0, 0, 0);
                while (get_reply != 3);
                TIMING_COMMUNI_END;

                TIMING_COMPUTE_BEGIN;
                for (int x = x_begin; x < x_end; ++x) {
                    TIMING_MEMCPY_BEGIN;
                    _memcpy(data, &origin[INDEX(x - 1, y0, 0, ldx, 3)], 3);
                    _memcpy(data + 3, &origin[INDEX(x - 1, y1, 0, ldx, 3)], 3);
                    _memcpy(data + 6, &origin[INDEX(x - 1, y2, 0, ldx, 3)], 3);
                    _memcpy(data + 9, &origin[INDEX(x - 1, y0, 1, ldx, 3)], 3);
                    _memcpy(data + 12, &origin[INDEX(x - 1, y1, 1, ldx, 3)], 3);
                    _memcpy(data + 15, &origin[INDEX(x - 1, y2, 1, ldx, 3)], 3);
                    _memcpy(data + 18, &origin[INDEX(x - 1, y0, 2, ldx, 3)], 3);
                    _memcpy(data + 21, &origin[INDEX(x - 1, y1, 2, ldx, 3)], 3);
                    _memcpy(data + 24, &origin[INDEX(x - 1, y2, 2, ldx, 3)], 3);
                    TIMING_MEMCPY_END;

                    simd_load(data_vector[0], data);
                    simd_load(data_vector[1], data + 4);
                    simd_load(data_vector[2], data + 8);
                    simd_load(data_vector[3], data + 12);
                    simd_load(data_vector[4], data + 16);
                    simd_load(data_vector[5], data + 20);
                    simd_load(data_vector[6], data + 24);

                    for (int j = 0; j < 7; j++) data_vector[j] = data_vector[j] * alpha_vector[j];
                    for (int j = 1; j < 7; j++) data_vector[0] = data_vector[0] + data_vector[j];
                    simd_store(data_vector[0], data);
                    answer[INDEX(x, 0, 0, ldx, 1)] = data[0] + data[1] + data[2] + data[3];
                }
                TIMING_COMPUTE_END;

                TIMING_COMMUNI_BEGIN;
                put_reply = 0;
                athread_put(PE_MODE, answer, &dest[INDEX(0, yy, zz, ldx, ldy)],
                            ldx * 1 * 1 * sizeof(data_t),
                            (void *) &put_reply, ldx * (ldy - 1) * sizeof(data_t),
                            ldx * 1 * sizeof(data_t));
                y0 = (y0 + 1) % 3;
                y1 = (y1 + 1) % 3;
                y2 = (y2 + 1) % 3;
                while (put_reply != 1);
                TIMING_COMMUNI_END;
            }
        }

#ifdef PROFILING
        lwpf_stop(A);
#endif
        athread_syn(ARRAY_SCOPE, 0xffff);
        runnable0 = 0;
        runnable1 = 0;
        if (id == 0) {
            *p->non_runnable = 0;
        }
        athread_syn(ARRAY_SCOPE, 0xffff);
    }
#ifdef TIMING
    if (pid == 1) {
        cal_locked_printf("compute:%d, communicate:%d, memcpy:%d\n", compute, communicate, memcpy_time);
    }
#endif

#ifdef PROFILING
    lwpf_exit(TEST);
#endif
    return;
}
