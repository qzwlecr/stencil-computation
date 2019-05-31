#include <string.h>
#include <slave.h>
#include <assert.h>
#include "cal.h"
#include "time.h"
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

extern volatile int non_runnable;

__thread_local volatile int runnable = 0;

void stencil_7_com(grid_param *p) {
#ifdef PROFILING
    lwpf_enter(TEST);
#endif
    ptr_t src, dest;

    int id = athread_get_id(-1);
    int pid = p->grid_info->p_id;
    volatile int get_reply = 0, put_reply = 0;

    int local_size_x = p->grid_info->local_size_x;
    int local_size_y = p->grid_info->local_size_y;
    int local_size_z = p->grid_info->local_size_z;
    int halo_size_x = p->grid_info->halo_size_x;
    int halo_size_y = p->grid_info->halo_size_y;
    int halo_size_z = p->grid_info->halo_size_z;

    int tot_size = local_size_y * local_size_z;
    int per_size = tot_size / THREAD_NUM;

    int x_begin = halo_size_x;
    int x_end = local_size_x + halo_size_x;

    int y_begin = per_size * id % local_size_y + halo_size_y;
    int y_end = per_size * (id + 1) % local_size_y + halo_size_y;

    int y_next_begin = halo_size_y;
    int y_next_end;
    if (y_end < y_begin) {
        y_next_end = y_end;
        y_end = local_size_y + halo_size_y;
    } else {
        y_next_end = halo_size_y;
    }

    int ldx = local_size_x + 2 * halo_size_x;
    int ldy = local_size_y + 2 * halo_size_y;

    int nt = p->nt;

    data_t origin[ldx * 3 * 3];
    data_t answer[ldx * 1 * 1];
    runnable = 0;
    for (int t = 0; t < nt; t++) {
        if (id == 0)
            while (runnable == 0);
        athread_syn(ARRAY_SCOPE, 0xffff);
        src = *p->src, dest = *p->dest;
#ifdef PROFILING
        lwpf_start(A);
#endif
        int zz = per_size * id / local_size_y + halo_size_z;
        int y0 = 0, y1 = 1, y2 = 2;
        for (int yy = y_begin; yy < y_end; yy += 1) {
            if (yy == y_begin) {
                get_reply = 0;
                athread_get(PE_MODE, &src[INDEX(0, yy - 1, zz, ldx, ldy)], &origin[INDEX(0, 0, 1, ldx, 3)],
                            ldx * 2 * sizeof(data_t), (void *) &get_reply, 0, 0, 0);
                while (get_reply != 1);
            }

            //use rotated array to avoid sloooooooow memcpy

            get_reply = 0;
            athread_get(PE_MODE, &src[INDEX(0, yy + 1, zz, ldx, ldy)], &origin[INDEX(0, y2, 1, ldx, 3)],
                        ldx * sizeof(data_t), (void *) &get_reply, 0, 0, 0);
            athread_get(PE_MODE, &src[INDEX(0, yy, zz - 1, ldx, ldy)], &origin[INDEX(0, y1, 0, ldx, 3)],
                        ldx * sizeof(data_t), (void *) &get_reply, 0, 0, 0);
            athread_get(PE_MODE, &src[INDEX(0, yy, zz + 1, ldx, ldy)], &origin[INDEX(0, y1, 2, ldx, 3)],
                        ldx * sizeof(data_t), (void *) &get_reply, 0, 0, 0);
            while (get_reply != 3);

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

            put_reply = 0;
            athread_put(PE_MODE, answer, &dest[INDEX(0, yy, zz, ldx, ldy)],
                        ldx * 1 * 1 * sizeof(data_t),
                        (void *) &put_reply, ldx * (ldy - 1) * sizeof(data_t),
                        ldx * 1 * sizeof(data_t));
            while (put_reply != 1);
            y0 = (y0 + 1) % 3;
            y1 = (y1 + 1) % 3;
            y2 = (y2 + 1) % 3;
        }
        zz++;

        y0 = 0, y1 = 1, y2 = 2;
        for (int yy = y_next_begin; yy < y_next_end; yy += 1) {
            if (yy == y_next_begin) {
                get_reply = 0;
                athread_get(PE_MODE, &src[INDEX(0, yy - 1, zz, ldx, ldy)], &origin[INDEX(0, 0, 1, ldx, 3)],
                            ldx * 2 * sizeof(data_t), (void *) &get_reply, 0, 0, 0);
                while (get_reply != 1);
            }

            //use rotated array to avoid sloooooooow memcpy

            get_reply = 0;
            athread_get(PE_MODE, &src[INDEX(0, yy + 1, zz, ldx, ldy)], &origin[INDEX(0, y2, 1, ldx, 3)],
                        ldx * sizeof(data_t), (void *) &get_reply, 0, 0, 0);
            athread_get(PE_MODE, &src[INDEX(0, yy, zz - 1, ldx, ldy)], &origin[INDEX(0, y1, 0, ldx, 3)],
                        ldx * sizeof(data_t), (void *) &get_reply, 0, 0, 0);
            athread_get(PE_MODE, &src[INDEX(0, yy, zz + 1, ldx, ldy)], &origin[INDEX(0, y1, 2, ldx, 3)],
                        ldx * sizeof(data_t), (void *) &get_reply, 0, 0, 0);
            while (get_reply != 3);

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

            put_reply = 0;
            athread_put(PE_MODE, answer, &dest[INDEX(0, yy, zz, ldx, ldy)],
                        ldx * 1 * 1 * sizeof(data_t),
                        (void *) &put_reply, ldx * (ldy - 1) * sizeof(data_t),
                        ldx * 1 * sizeof(data_t));
            y0 = (y0 + 1) % 3;
            y1 = (y1 + 1) % 3;
            y2 = (y2 + 1) % 3;
            while (put_reply != 1);
        }
#ifdef PROFILING
        lwpf_stop(A);
#endif
        athread_syn(ARRAY_SCOPE, 0xffff);
        if (id == 0) {
            runnable = 0;
            *p->non_runnable = 0;
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
    ptr_t src, dest;

    int id = athread_get_id(-1);
    int pid = p->grid_info->p_id;
    volatile int get_reply = 0, put_reply = 0;

    int local_size_x = p->grid_info->local_size_x;
    int local_size_y = p->grid_info->local_size_y;
    int local_size_z = p->grid_info->local_size_z;
    int halo_size_x = p->grid_info->halo_size_x;
    int halo_size_y = p->grid_info->halo_size_y;
    int halo_size_z = p->grid_info->halo_size_z;

    int tot_size = local_size_y * local_size_z;
    int per_size = tot_size / THREAD_NUM;

    int x_begin = halo_size_x;
    int x_end = local_size_x + halo_size_x;

    int y_begin = per_size * id % local_size_y + halo_size_y;
    int y_end = per_size * (id + 1) % local_size_y + halo_size_y;

    int y_next_begin = halo_size_y;
    int y_next_end;
    if (y_end < y_begin) {
        y_next_end = y_end;
        y_end = local_size_y + halo_size_y;
    } else {
        y_next_end = halo_size_y;
    }

    int ldx = local_size_x + 2 * halo_size_x;
    int ldy = local_size_y + 2 * halo_size_y;

    int nt = p->nt;

    data_t origin[ldx * 3 * 3];
    data_t answer[ldx * 1 * 1];
    runnable = 0;
    for (int t = 0; t < nt; t++) {
        if (id == 0)
            while (runnable == 0);
        athread_syn(ARRAY_SCOPE, 0xffff);
        src = *p->src, dest = *p->dest;
#ifdef PROFILING
        lwpf_start(A);
#endif
        int zz = per_size * id / local_size_y + halo_size_z;
        int y0 = 0, y1 = 1, y2 = 2;
        for (int yy = y_begin; yy < y_end; yy += 1) {
            if (yy == y_begin) {
                get_reply = 0;
                athread_get(PE_MODE, &src[INDEX(0, yy - 1, zz - 1, ldx, ldy)], &origin[INDEX(0, 0, 0, ldx, 3)],
                            ldx * 2 * sizeof(data_t), (void *) &get_reply, 0, 0, 0);
                athread_get(PE_MODE, &src[INDEX(0, yy - 1, zz, ldx, ldy)], &origin[INDEX(0, 0, 1, ldx, 3)],
                            ldx * 2 * sizeof(data_t), (void *) &get_reply, 0, 0, 0);
                athread_get(PE_MODE, &src[INDEX(0, yy - 1, zz + 1, ldx, ldy)], &origin[INDEX(0, 0, 2, ldx, 3)],
                            ldx * 2 * sizeof(data_t), (void *) &get_reply, 0, 0, 0);
                while (get_reply != 3);
            }

            //use rotated array to avoid sloooooooow memcpy

            get_reply = 0;
            athread_get(PE_MODE, &src[INDEX(0, yy + 1, zz - 1, ldx, ldy)], &origin[INDEX(0, y2, 0, ldx, 3)],
                        ldx * sizeof(data_t), (void *) &get_reply, 0, 0, 0);
            athread_get(PE_MODE, &src[INDEX(0, yy + 1, zz, ldx, ldy)], &origin[INDEX(0, y2, 1, ldx, 3)],
                        ldx * sizeof(data_t), (void *) &get_reply, 0, 0, 0);
            athread_get(PE_MODE, &src[INDEX(0, yy + 1, zz + 1, ldx, ldy)], &origin[INDEX(0, y2, 2, ldx, 3)],
                        ldx * sizeof(data_t), (void *) &get_reply, 0, 0, 0);
            while (get_reply != 3);

            for (int x = x_begin; x < x_end; ++x) {
                answer[INDEX(x, 0, 0, ldx, 1)] =
                        ALPHA_ZZZ * origin[INDEX(x, y1, 1, ldx, 3)]
                        + ALPHA_NZZ * origin[INDEX(x - 1, y1, 1, ldx, 3)]
                        + ALPHA_PZZ * origin[INDEX(x + 1, y1, 1, ldx, 3)]
                        + ALPHA_ZNZ * origin[INDEX(x, y0, 1, ldx, 3)]
                        + ALPHA_ZPZ * origin[INDEX(x, y2, 1, ldx, 3)]
                        + ALPHA_ZZN * origin[INDEX(x, y1, 0, ldx, 3)]
                        + ALPHA_ZZP * origin[INDEX(x, y1, 2, ldx, 3)]
                        + ALPHA_NNZ * origin[INDEX(x - 1, y0, 1, ldx, 3)]
                        + ALPHA_PNZ * origin[INDEX(x + 1, y0, 1, ldx, 3)]
                        + ALPHA_NPZ * origin[INDEX(x - 1, y2, 1, ldx, 3)]
                        + ALPHA_PPZ * origin[INDEX(x + 1, y2, 1, ldx, 3)]
                        + ALPHA_NZN * origin[INDEX(x - 1, y1, 0, ldx, 3)]
                        + ALPHA_PZN * origin[INDEX(x + 1, y1, 0, ldx, 3)]
                        + ALPHA_NZP * origin[INDEX(x - 1, y1, 2, ldx, 3)]
                        + ALPHA_PZP * origin[INDEX(x + 1, y1, 2, ldx, 3)]
                        + ALPHA_ZNN * origin[INDEX(x, y0, 0, ldx, 3)]
                        + ALPHA_ZPN * origin[INDEX(x, y2, 0, ldx, 3)]
                        + ALPHA_ZNP * origin[INDEX(x, y0, 2, ldx, 3)]
                        + ALPHA_ZPP * origin[INDEX(x, y2, 2, ldx, 3)]
                        + ALPHA_NNN * origin[INDEX(x - 1, y0, 0, ldx, 3)]
                        + ALPHA_PNN * origin[INDEX(x + 1, y0, 0, ldx, 3)]
                        + ALPHA_NPN * origin[INDEX(x - 1, y2, 0, ldx, 3)]
                        + ALPHA_PPN * origin[INDEX(x + 1, y2, 0, ldx, 3)]
                        + ALPHA_NNP * origin[INDEX(x - 1, y0, 2, ldx, 3)]
                        + ALPHA_PNP * origin[INDEX(x + 1, y0, 2, ldx, 3)]
                        + ALPHA_NPP * origin[INDEX(x - 1, y2, 2, ldx, 3)]
                        + ALPHA_PPP * origin[INDEX(x + 1, y2, 2, ldx, 3)];

            }

            put_reply = 0;
            athread_put(PE_MODE, answer, &dest[INDEX(0, yy, zz, ldx, ldy)],
                        ldx * 1 * 1 * sizeof(data_t),
                        (void *) &put_reply, ldx * (ldy - 1) * sizeof(data_t),
                        ldx * 1 * sizeof(data_t));
            while (put_reply != 1);
            y0 = (y0 + 1) % 3;
            y1 = (y1 + 1) % 3;
            y2 = (y2 + 1) % 3;
        }
        zz++;

        y0 = 0, y1 = 1, y2 = 2;
        for (int yy = y_next_begin; yy < y_next_end; yy += 1) {
            if (yy == y_next_begin) {
                get_reply = 0;
                athread_get(PE_MODE, &src[INDEX(0, yy - 1, zz - 1, ldx, ldy)], &origin[INDEX(0, 0, 0, ldx, 3)],
                            ldx * 2 * sizeof(data_t), (void *) &get_reply, 0, 0, 0);
                athread_get(PE_MODE, &src[INDEX(0, yy - 1, zz, ldx, ldy)], &origin[INDEX(0, 0, 1, ldx, 3)],
                            ldx * 2 * sizeof(data_t), (void *) &get_reply, 0, 0, 0);
                athread_get(PE_MODE, &src[INDEX(0, yy - 1, zz + 1, ldx, ldy)], &origin[INDEX(0, 0, 2, ldx, 3)],
                            ldx * 2 * sizeof(data_t), (void *) &get_reply, 0, 0, 0);
                while (get_reply != 3);
            }

            //use rotated array to avoid sloooooooow memcpy

            get_reply = 0;
            athread_get(PE_MODE, &src[INDEX(0, yy + 1, zz - 1, ldx, ldy)], &origin[INDEX(0, y2, 0, ldx, 3)],
                        ldx * sizeof(data_t), (void *) &get_reply, 0, 0, 0);
            athread_get(PE_MODE, &src[INDEX(0, yy + 1, zz, ldx, ldy)], &origin[INDEX(0, y2, 1, ldx, 3)],
                        ldx * sizeof(data_t), (void *) &get_reply, 0, 0, 0);
            athread_get(PE_MODE, &src[INDEX(0, yy + 1, zz + 1, ldx, ldy)], &origin[INDEX(0, y2, 2, ldx, 3)],
                        ldx * sizeof(data_t), (void *) &get_reply, 0, 0, 0);
            while (get_reply != 3);

            for (int x = x_begin; x < x_end; ++x) {
                answer[INDEX(x, 0, 0, ldx, 1)] =
                        ALPHA_ZZZ * origin[INDEX(x, y1, 1, ldx, 3)]
                        + ALPHA_NZZ * origin[INDEX(x - 1, y1, 1, ldx, 3)]
                        + ALPHA_PZZ * origin[INDEX(x + 1, y1, 1, ldx, 3)]
                        + ALPHA_ZNZ * origin[INDEX(x, y0, 1, ldx, 3)]
                        + ALPHA_ZPZ * origin[INDEX(x, y2, 1, ldx, 3)]
                        + ALPHA_ZZN * origin[INDEX(x, y1, 0, ldx, 3)]
                        + ALPHA_ZZP * origin[INDEX(x, y1, 2, ldx, 3)]
                        + ALPHA_NNZ * origin[INDEX(x - 1, y0, 1, ldx, 3)]
                        + ALPHA_PNZ * origin[INDEX(x + 1, y0, 1, ldx, 3)]
                        + ALPHA_NPZ * origin[INDEX(x - 1, y2, 1, ldx, 3)]
                        + ALPHA_PPZ * origin[INDEX(x + 1, y2, 1, ldx, 3)]
                        + ALPHA_NZN * origin[INDEX(x - 1, y1, 0, ldx, 3)]
                        + ALPHA_PZN * origin[INDEX(x + 1, y1, 0, ldx, 3)]
                        + ALPHA_NZP * origin[INDEX(x - 1, y1, 2, ldx, 3)]
                        + ALPHA_PZP * origin[INDEX(x + 1, y1, 2, ldx, 3)]
                        + ALPHA_ZNN * origin[INDEX(x, y0, 0, ldx, 3)]
                        + ALPHA_ZPN * origin[INDEX(x, y2, 0, ldx, 3)]
                        + ALPHA_ZNP * origin[INDEX(x, y0, 2, ldx, 3)]
                        + ALPHA_ZPP * origin[INDEX(x, y2, 2, ldx, 3)]
                        + ALPHA_NNN * origin[INDEX(x - 1, y0, 0, ldx, 3)]
                        + ALPHA_PNN * origin[INDEX(x + 1, y0, 0, ldx, 3)]
                        + ALPHA_NPN * origin[INDEX(x - 1, y2, 0, ldx, 3)]
                        + ALPHA_PPN * origin[INDEX(x + 1, y2, 0, ldx, 3)]
                        + ALPHA_NNP * origin[INDEX(x - 1, y0, 2, ldx, 3)]
                        + ALPHA_PNP * origin[INDEX(x + 1, y0, 2, ldx, 3)]
                        + ALPHA_NPP * origin[INDEX(x - 1, y2, 2, ldx, 3)]
                        + ALPHA_PPP * origin[INDEX(x + 1, y2, 2, ldx, 3)];

            }

            put_reply = 0;
            athread_put(PE_MODE, answer, &dest[INDEX(0, yy, zz, ldx, ldy)],
                        ldx * 1 * 1 * sizeof(data_t),
                        (void *) &put_reply, ldx * (ldy - 1) * sizeof(data_t),
                        ldx * 1 * sizeof(data_t));
            y0 = (y0 + 1) % 3;
            y1 = (y1 + 1) % 3;
            y2 = (y2 + 1) % 3;
            while (put_reply != 1);
        }
#ifdef PROFILING
        lwpf_stop(A);
#endif
        athread_syn(ARRAY_SCOPE, 0xffff);
        if (id == 0) {
            runnable = 0;
            *p->non_runnable = 0;
        }
        athread_syn(ARRAY_SCOPE, 0xffff);
    }

#ifdef PROFILING
    lwpf_exit(TEST);
#endif
    return;
}
