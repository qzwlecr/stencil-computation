#include <string.h>
#include <slave.h>
#include <assert.h>
#include "cal.h"
#include "time.h"
#include "simd.h"
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

__thread_local volatile int runnable[6] = {0};// 0:left_x 1:right_x 2:left_y 3:right_y 4:left_z 5:right_z 没有其实不应该按左右来分但是明白我意思就行
//__thread_local volatile int runnable1 = 0;

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
    int y_loc[4];

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

    z_loc[0] = per_size * id / local_size_y + halo_size_z + 1;//
    z_loc[1] = per_size * id / local_size_y + halo_size_z + 2;//
    z_loc[2] = 1;//
    z_loc[3] = local_size_z;//



    int ldx = local_size_x + 2 * halo_size_x;
    int ldy = local_size_y + 2 * halo_size_y;

    int nt = p->nt;

    ptr_t src, dest;
    ptr_t buffer[2] = {p->grid, p->aux};

    data_t origin[ldx * 3 * 3];
    data_t answer[ldx * 1 * 1];
    for (int i = 0; i < 6; i++) {
        runnable[i] = 0;
    }

    for (int t = 0; t < nt; t++) {
        src = buffer[t % 2];
        dest = buffer[(t + 1) % 2];
#ifdef PROFILING
        lwpf_start(A);
#endif
        //deal with main part
        //这部分中
        for (int z = 0; z <= 1; z++) {
            int y0 = 0, y1 = 1, y2 = 2;
            int zz = z_loc[z];
            for (int yy = y_begin[z]; yy < y_end[z]; yy++) {
                if (yy == y_begin[z]) {
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

        }
        int finish[6] = {0};//0:left_x 1:right_x 2:left_y 3:right_y 4:left_z 5:right_z 没有其实不应该按左右来分但是明白我意思就行

        int z = -1;
        int y = -1;
        int x = -1;
        //int zz = z_loc[z];

        while (1) {
            if (finish[0] && finish[1] && finish[2] && finish[3] && finish[4] && finish[5]) break;
            //if (runnable0 == 2) finish0 = 1;
            //if (runnable1 == 2) finish1 = 1;
            if (runnable[4]  && finish[4] == 0) {
                z = 3;
                finish[4] = 1;
            } else if (runnable[5]  && finish[5] == 0) {
                z = 2;
                finish[5] = 1;
            } else if (runnable[2] && finish[2] == 0) {
                y = 3;
                finish[2] = 1;
            } else if (runnable[3] && finish[3] == 0) {
                y = 2;
                finish[3] = 1;
            } else if (runnable[0] && finish[0] == 0) {
                x = 3;
                finish[0] = 1;
            } else if (runnable[1] && finish[1] == 0) {
                x = 2;
                finish[1] = 1;
            } else {
                continue;
            }

            if (z == 2 || z == 3) {//compute z 的 上面 或者 下面 两条
                int zz = z_loc[z];
                int y0 = 0, y1 = 1, y2 = 2;
                for (int yy = y_begin[z]; yy < y_end[z]; yy++) {
                    if (yy == y_begin[z]) {
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
                z = -1;
            }

            if (y == 2 || y == 3) {//compute y 的两头 还没改可能存在的y_loc,在改了.jpg
                int zz = z_loc[z];
                int y0 = 0, y1 = 1, y2 = 2;
                for (int yy = y_begin[z]; yy < y_end[z]; yy++) {
                    if (yy == y_begin[z]) {
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
                y = -1;
            }

            if (x == 2 || x == 3) {//compute x 的两头 搞不懂怎么切的，再改
                int zz = z_loc[z];
                int y0 = 0, y1 = 1, y2 = 2;
                for (int yy = y_begin[z]; yy < y_end[z]; yy++) {
                    if (yy == y_begin[z]) {
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
                x = -1;
            }
        }

#ifdef PROFILING
        lwpf_stop(A);
#endif
        athread_syn(ARRAY_SCOPE, 0xffff);
        for (int i = 0; i < 6; i++) {
            runnable[i] = 0;
        }
        if (id == 0) {
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

        }

        // if (id == 0)
        //     while (runnable == 0);
        athread_syn(ARRAY_SCOPE, 0xffff);

        for (int z = 2; z <= 3; z++) {
            int y0 = 0, y1 = 1, y2 = 2;
            int zz = z_loc[z];
            for (int yy = y_begin[z]; yy < y_end[z]; yy++) {
                if (yy == y_begin[z]) {
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
        }

#ifdef PROFILING
        lwpf_stop(A);
#endif
        athread_syn(ARRAY_SCOPE, 0xffff);
        if (id == 0) {
            // runnable = 0;
            *p->non_runnable = 0;
        }
        athread_syn(ARRAY_SCOPE, 0xffff);
    }

#ifdef PROFILING
    lwpf_exit(TEST);
#endif
    return;
}
