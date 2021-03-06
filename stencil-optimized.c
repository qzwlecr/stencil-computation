#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <sys/time.h>
#include "athread.h"
#include "common.h"

//#define PROFILING
//#define TIMING

#ifdef PROFILING
#define MPE
#define LWPF_NOCOLOR
#define LWPF_UNITS U(TEST)
#include "lwpf2.h"
#endif

#define DUMP(varname) fprintf(stderr, "[%d]%s = %d\n", grid_info->p_id, #varname, varname);
#define DUMPF(varname) fprintf(stderr, "[%d]%s = %lf\n", grid_info->p_id, #varname, varname);

const char *version_name = "Optimized version";

extern SLAVE_FUN(stencil_7_com)(grid_param
*);

extern SLAVE_FUN(stencil_27_com)(grid_param
*);

extern volatile int __thread runnable0;
extern volatile int __thread runnable1;
volatile int non_runnable = 0;
unsigned long LDM_addr;

#ifdef PROFILING
perf_config_t conf; //以下是初始化采样选项的代码
#endif

double timer() {
    double t;
    struct timeval tv;
    gettimeofday(&tv, NULL);
    t = (double) tv.tv_sec * 1.0 + (double) tv.tv_usec * 1e-6;
    return t;
}

/* your implementation */
void create_dist_grid(dist_grid_info_t *grid_info, int stencil_type) {
    if (grid_info->p_num == 16) {
        grid_info->num_x = 1;
        grid_info->num_y = 1;
        grid_info->num_z = 16;
    } else {
        grid_info->num_x = 1;
        grid_info->num_y = 1;
        grid_info->num_z = 64;
    }
    grid_info->local_size_x = grid_info->global_size_x / grid_info->num_x;
    grid_info->local_size_y = grid_info->global_size_y / grid_info->num_y;
    grid_info->local_size_z = grid_info->global_size_z / grid_info->num_z;
    grid_info->offset_x = grid_info->local_size_x * (grid_info->p_id % grid_info->num_x);
    grid_info->offset_y = grid_info->local_size_y * ((grid_info->p_id / grid_info->num_x) % grid_info->num_y);
    grid_info->offset_z = grid_info->local_size_z * (grid_info->p_id / grid_info->num_x / grid_info->num_y);
    grid_info->halo_size_x = 1;
    grid_info->halo_size_y = 1;
    grid_info->halo_size_z = 1;
    athread_init();
    int spe_cnt = athread_get_max_threads();
    if (spe_cnt != 64) {
        fprintf(stderr, "This CG cannot afford 64 SPEs (%d only).\n", spe_cnt);
    }
#ifdef PROFILING
    conf.pcrc = PCRC_ALL;
    conf.pcr0 = PC0_CYCLE;
    conf.pcr1 = PC1_CYCLE;
    conf.pcr2 = PC2_CNT_GST;
    lwpf_init(&conf); //调用lwpf_init对计数器数据和选项进行初始化.
#endif

}

/* your implementation */
void destroy_dist_grid(dist_grid_info_t *grid_info) {
    athread_halt();
}

/* your implementation */
ptr_t stencil_7(ptr_t grid, ptr_t aux, const dist_grid_info_t *grid_info, int nt) {
    ptr_t buffer[2] = {grid, aux};
    int x_start = grid_info->halo_size_x,
            x_end = grid_info->local_size_x + grid_info->halo_size_x;
    int y_start = grid_info->halo_size_y,
            y_end = grid_info->local_size_y + grid_info->halo_size_y;
    int z_start = grid_info->halo_size_z,
            z_end = grid_info->local_size_z + grid_info->halo_size_z;
    int ldx = grid_info->local_size_x + 2 * grid_info->halo_size_x;
    int ldy = grid_info->local_size_y + 2 * grid_info->halo_size_y;
    int ldz = grid_info->local_size_z + 2 * grid_info->halo_size_z;

    MPI_Datatype yzplane, xzplane, xyplane;
    MPI_Type_vector(ldy * ldz, 1, ldx, MPI_DOUBLE, &yzplane);
    MPI_Type_commit(&yzplane);
    MPI_Type_vector(ldz, ldx, ldx * ldy, MPI_DOUBLE, &xzplane);
    MPI_Type_commit(&xzplane);
    MPI_Type_vector(1, ldx * ldy, 0, MPI_DOUBLE, &xyplane);
    MPI_Type_commit(&xyplane);

    int pid = grid_info->p_id;
    non_runnable = 0;
    ptr_t a0, a1;
    grid_param p = {
            .grid = grid,
            .aux = aux,
            .nt = nt,
            .non_runnable = &non_runnable,
            .grid_info = grid_info
    };
    athread_spawn(stencil_7_com, &p);
#ifdef TIMING
    double time_consumed = 0;
#endif

    for (int t = 0; t < nt; ++t) {
        a0 = buffer[t % 2];
        a1 = buffer[(t + 1) % 2];
        MPI_Status status[2][6];
        MPI_Request request[2][6];
        int count0 = 0;
        int count1 = 0;
#ifdef TIMING
        double temp = timer(), temp2;
        if (pid == 1) {
            fprintf(stderr, "[%d]Iter %d mpi start : %lf\n", pid, t, temp);
        }
#endif
        if (grid_info->num_x != 1) {
            if (pid % grid_info->num_x == 0) { // yz
                MPI_Isend((void *) (a0 + x_end - 1), 1, yzplane, pid + 1, pid, MPI_COMM_WORLD, &request[0][count0++]);
                MPI_Irecv((void *) (a0 + x_end), 1, yzplane, pid + 1, pid + 1, MPI_COMM_WORLD, &request[0][count0++]);
            } else if (pid % grid_info->num_x == grid_info->num_x - 1) {
                MPI_Isend((void *) (a0 + x_start), 1, yzplane, pid - 1, pid, MPI_COMM_WORLD, &request[1][count1++]);
                MPI_Irecv((void *) (a0 + x_start - 1), 1, yzplane, pid - 1, pid - 1, MPI_COMM_WORLD,
                          &request[1][count1++]);
            } else {
                MPI_Isend((void *) (a0 + x_start), 1, yzplane, pid - 1, pid, MPI_COMM_WORLD, &request[1][count1++]);
                MPI_Irecv((void *) (a0 + x_start - 1), 1, yzplane, pid - 1, pid - 1, MPI_COMM_WORLD,
                          &request[1][count1++]);
                MPI_Isend((void *) (a0 + x_end - 1), 1, yzplane, pid + 1, pid, MPI_COMM_WORLD, &request[0][count0++]);
                MPI_Irecv((void *) (a0 + x_end), 1, yzplane, pid + 1, pid + 1, MPI_COMM_WORLD, &request[0][count0++]);
            }
        }

        if (grid_info->num_y != 1) {
            if ((pid / grid_info->num_x) % grid_info->num_y == 0) { // xz
                MPI_Isend((void *) (a0 + (y_end - 1) * ldx), 1, xzplane,
                          pid + grid_info->num_x,
                          pid,
                          MPI_COMM_WORLD, &request[0][count0++]);
                MPI_Irecv((void *) (a0 + y_end * ldx), 1, xzplane,
                          pid + grid_info->num_x,
                          pid + grid_info->num_x,
                          MPI_COMM_WORLD, &request[0][count0++]);
            } else if ((pid / grid_info->num_x) % grid_info->num_y == grid_info->num_y - 1) {
                MPI_Isend((void *) (a0 + y_start * ldx), 1, xzplane,
                          pid - grid_info->num_x,
                          pid,
                          MPI_COMM_WORLD, &request[1][count1++]);
                MPI_Irecv((void *) (a0 + (y_start - 1) * ldx), 1, xzplane,
                          pid - grid_info->num_x,
                          pid - grid_info->num_x,
                          MPI_COMM_WORLD, &request[1][count1++]);
            } else {
                MPI_Isend((void *) (a0 + (y_end - 1) * ldx), 1, xzplane,
                          pid + grid_info->num_x,
                          pid,
                          MPI_COMM_WORLD, &request[0][count0++]);
                MPI_Irecv((void *) (a0 + y_end * ldx), 1, xzplane,
                          pid + grid_info->num_x,
                          pid + grid_info->num_x,
                          MPI_COMM_WORLD, &request[0][count0++]);
                MPI_Isend((void *) (a0 + y_start * ldx), 1, xzplane,
                          pid - grid_info->num_x,
                          pid,
                          MPI_COMM_WORLD, &request[1][count1++]);
                MPI_Irecv((void *) (a0 + (y_start - 1) * ldx), 1, xzplane,
                          pid - grid_info->num_x,
                          pid - grid_info->num_x,
                          MPI_COMM_WORLD, &request[1][count1++]);
            }

        }

        if (grid_info->num_z != 1) {
            if (pid / grid_info->num_x / grid_info->num_y == 0) { // xy
                MPI_Isend((void *) (a0 + (z_end - 1) * ldx * ldy), 1, xyplane,
                          pid + grid_info->num_x * grid_info->num_y,
                          pid,
                          MPI_COMM_WORLD, &request[0][count0++]);
                MPI_Irecv((void *) (a0 + z_end * ldx * ldy), 1, xyplane,
                          pid + grid_info->num_x * grid_info->num_y,
                          pid + grid_info->num_x * grid_info->num_y,
                          MPI_COMM_WORLD, &request[0][count0++]);
            } else if (pid / grid_info->num_x / grid_info->num_y == grid_info->num_z - 1) {
                MPI_Isend((void *) (a0 + z_start * ldx * ldy), 1, xyplane,
                          pid - grid_info->num_x * grid_info->num_y,
                          pid,
                          MPI_COMM_WORLD, &request[1][count1++]);
                MPI_Irecv((void *) (a0 + (z_start - 1) * ldx * ldy), 1, xyplane,
                          pid - grid_info->num_x * grid_info->num_y,
                          pid - grid_info->num_x * grid_info->num_y,
                          MPI_COMM_WORLD, &request[1][count1++]);
            } else {
                MPI_Isend((void *) (a0 + (z_end - 1) * ldx * ldy), 1, xyplane,
                          pid + grid_info->num_x * grid_info->num_y,
                          pid,
                          MPI_COMM_WORLD, &request[0][count0++]);
                MPI_Irecv((void *) (a0 + z_end * ldx * ldy), 1, xyplane,
                          pid + grid_info->num_x * grid_info->num_y,
                          pid + grid_info->num_x * grid_info->num_y,
                          MPI_COMM_WORLD, &request[0][count0++]);
                MPI_Isend((void *) (a0 + z_start * ldx * ldy), 1, xyplane,
                          pid - grid_info->num_x * grid_info->num_y,
                          pid,
                          MPI_COMM_WORLD, &request[1][count1++]);
                MPI_Irecv((void *) (a0 + (z_start - 1) * ldx * ldy), 1, xyplane,
                          pid - grid_info->num_x * grid_info->num_y,
                          pid - grid_info->num_x * grid_info->num_y,
                          MPI_COMM_WORLD, &request[1][count1++]);
            }
        }
        non_runnable = 1;
        //assume that pid % 4 == cgid
        int flag0 = 0;
        int flag1 = 0;
        int runnable0_flag = 0;
        int runnable1_flag = 0;
        if (count0 == 0) {
            flag0 = 2;
            runnable0_flag = 2;
            for (int i = 0; i < 64; i++) h2ldm(runnable0, i, pid % 4) = 2;
            // h2ldm(runnable0, 0, pid % 4) = 2;
        }
        if (count1 == 0) {
            flag1 = 2;
            runnable1_flag = 2;
            for (int i = 0; i < 64; i++) h2ldm(runnable1, i, pid % 4) = 2;
            // h2ldm(runnable1, 0, pid % 4) = 2;
        }

        while (1) {
            if (runnable0_flag == 0) {
                MPI_Testall(count0, request[0], &flag0, status[0]);
            }

            if (runnable1_flag == 0) {
                MPI_Testall(count1, request[1], &flag1, status[1]);
            }

            if (flag0 != 0 && runnable0_flag == 0) {
                runnable0_flag = 1;
                for (int i = 0; i < 64; i++)
                    h2ldm(runnable0, i, pid % 4) = 1;
            }
            if (flag1 != 0 && runnable1_flag == 0) {
                runnable1_flag = 1;
                for (int i = 0; i < 64; i++)
                    h2ldm(runnable1, i, pid % 4) = 1;
            }
            if (runnable0_flag && runnable1_flag)
                break;
        }
#ifdef TIMING
        temp2 = timer();
        if (pid == 1) {
            fprintf(stderr, "[%d]Iter %d mpi end : %lf\n", pid, t, temp2);
            fprintf(stderr, "[%d]Iter %d mpi consume : %lf\n", pid, t, temp2 - temp);
        }
        time_consumed += temp2 - temp;
#endif
        while (non_runnable == 1);

    }
    athread_join();
#ifdef PROFILING
    if(pid == 0){
        lwpf_report_detail(stdout, &conf); //输出最详细的数据
    }
#endif
#ifdef TIMING
    fprintf(stderr, "[%d]Iter mpi consume all : %lf\n", pid, time_consumed);
#endif
//    fprintf(stderr, "[%d]Stencil 7 computing done\n", grid_info->p_id);
    return buffer[nt % 2];
}

/* your implementation */
ptr_t stencil_27(ptr_t grid, ptr_t aux, const dist_grid_info_t *grid_info, int nt) {
    ptr_t buffer[2] = {grid, aux};
    int x_start = grid_info->halo_size_x,
            x_end = grid_info->local_size_x + grid_info->halo_size_x;
    int y_start = grid_info->halo_size_y,
            y_end = grid_info->local_size_y + grid_info->halo_size_y;
    int z_start = grid_info->halo_size_z,
            z_end = grid_info->local_size_z + grid_info->halo_size_z;
    int ldx = grid_info->local_size_x + 2 * grid_info->halo_size_x;
    int ldy = grid_info->local_size_y + 2 * grid_info->halo_size_y;
    int ldz = grid_info->local_size_z + 2 * grid_info->halo_size_z;


//    fprintf(stderr, "[%d]Stencil 27 computing start\n", grid_info->p_id);

    MPI_Datatype yzplane, xzplane, xyplane;
    MPI_Type_vector(ldy * ldz, 1, ldx, MPI_DOUBLE, &yzplane);
    MPI_Type_commit(&yzplane);
    MPI_Type_vector(ldz, ldx, ldx * ldy, MPI_DOUBLE, &xzplane);
    MPI_Type_commit(&xzplane);
    MPI_Type_vector(1, ldx * ldy, 0, MPI_DOUBLE, &xyplane);
    MPI_Type_commit(&xyplane);

    int pid = grid_info->p_id;
    non_runnable = 0;
    ptr_t a0, a1;
    grid_param p = {
            .grid = grid,
            .aux = aux,
            .nt = nt,
            .non_runnable = &non_runnable,
            .grid_info = grid_info
    };
    athread_spawn(stencil_27_com, &p);
#ifdef TIMING
    double time_consumed = 0;
#endif

    for (int t = 0; t < nt; ++t) {
        a0 = buffer[t % 2];
        a1 = buffer[(t + 1) % 2];
        MPI_Status status[2][6];
        MPI_Request request[2][6];
        int count0 = 0;
        int count1 = 0;
#ifdef TIMING
        double temp = timer(), temp2;
        if (pid == 1) {
            fprintf(stderr, "[%d]Iter %d mpi start : %lf\n", pid, t, temp);
        }
#endif
        if (grid_info->num_x != 1) {
            if (pid % grid_info->num_x == 0) { // yz
                MPI_Isend((void *) (a0 + x_end - 1), 1, yzplane, pid + 1, pid, MPI_COMM_WORLD, &request[0][count0++]);
                MPI_Irecv((void *) (a0 + x_end), 1, yzplane, pid + 1, pid + 1, MPI_COMM_WORLD, &request[0][count0++]);
            } else if (pid % grid_info->num_x == grid_info->num_x - 1) {
                MPI_Isend((void *) (a0 + x_start), 1, yzplane, pid - 1, pid, MPI_COMM_WORLD, &request[1][count1++]);
                MPI_Irecv((void *) (a0 + x_start - 1), 1, yzplane, pid - 1, pid - 1, MPI_COMM_WORLD,
                          &request[1][count1++]);
            } else {
                MPI_Isend((void *) (a0 + x_start), 1, yzplane, pid - 1, pid, MPI_COMM_WORLD, &request[1][count1++]);
                MPI_Irecv((void *) (a0 + x_start - 1), 1, yzplane, pid - 1, pid - 1, MPI_COMM_WORLD,
                          &request[1][count1++]);
                MPI_Isend((void *) (a0 + x_end - 1), 1, yzplane, pid + 1, pid, MPI_COMM_WORLD, &request[0][count0++]);
                MPI_Irecv((void *) (a0 + x_end), 1, yzplane, pid + 1, pid + 1, MPI_COMM_WORLD, &request[0][count0++]);
            }
        }

        if (grid_info->num_y != 1) {
            if ((pid / grid_info->num_x) % grid_info->num_y == 0) { // xz
                MPI_Isend((void *) (a0 + (y_end - 1) * ldx), 1, xzplane,
                          pid + grid_info->num_x,
                          pid,
                          MPI_COMM_WORLD, &request[0][count0++]);
                MPI_Irecv((void *) (a0 + y_end * ldx), 1, xzplane,
                          pid + grid_info->num_x,
                          pid + grid_info->num_x,
                          MPI_COMM_WORLD, &request[0][count0++]);
            } else if ((pid / grid_info->num_x) % grid_info->num_y == grid_info->num_y - 1) {
                MPI_Isend((void *) (a0 + y_start * ldx), 1, xzplane,
                          pid - grid_info->num_x,
                          pid,
                          MPI_COMM_WORLD, &request[1][count1++]);
                MPI_Irecv((void *) (a0 + (y_start - 1) * ldx), 1, xzplane,
                          pid - grid_info->num_x,
                          pid - grid_info->num_x,
                          MPI_COMM_WORLD, &request[1][count1++]);
            } else {
                MPI_Isend((void *) (a0 + (y_end - 1) * ldx), 1, xzplane,
                          pid + grid_info->num_x,
                          pid,
                          MPI_COMM_WORLD, &request[0][count0++]);
                MPI_Irecv((void *) (a0 + y_end * ldx), 1, xzplane,
                          pid + grid_info->num_x,
                          pid + grid_info->num_x,
                          MPI_COMM_WORLD, &request[0][count0++]);
                MPI_Isend((void *) (a0 + y_start * ldx), 1, xzplane,
                          pid - grid_info->num_x,
                          pid,
                          MPI_COMM_WORLD, &request[1][count1++]);
                MPI_Irecv((void *) (a0 + (y_start - 1) * ldx), 1, xzplane,
                          pid - grid_info->num_x,
                          pid - grid_info->num_x,
                          MPI_COMM_WORLD, &request[1][count1++]);
            }

        }

        if (grid_info->num_z != 1) {
            if (pid / grid_info->num_x / grid_info->num_y == 0) { // xy
                MPI_Isend((void *) (a0 + (z_end - 1) * ldx * ldy), 1, xyplane,
                          pid + grid_info->num_x * grid_info->num_y,
                          pid,
                          MPI_COMM_WORLD, &request[0][count0++]);
                MPI_Irecv((void *) (a0 + z_end * ldx * ldy), 1, xyplane,
                          pid + grid_info->num_x * grid_info->num_y,
                          pid + grid_info->num_x * grid_info->num_y,
                          MPI_COMM_WORLD, &request[0][count0++]);
            } else if (pid / grid_info->num_x / grid_info->num_y == grid_info->num_z - 1) {
                MPI_Isend((void *) (a0 + z_start * ldx * ldy), 1, xyplane,
                          pid - grid_info->num_x * grid_info->num_y,
                          pid,
                          MPI_COMM_WORLD, &request[1][count1++]);
                MPI_Irecv((void *) (a0 + (z_start - 1) * ldx * ldy), 1, xyplane,
                          pid - grid_info->num_x * grid_info->num_y,
                          pid - grid_info->num_x * grid_info->num_y,
                          MPI_COMM_WORLD, &request[1][count1++]);
            } else {
                MPI_Isend((void *) (a0 + (z_end - 1) * ldx * ldy), 1, xyplane,
                          pid + grid_info->num_x * grid_info->num_y,
                          pid,
                          MPI_COMM_WORLD, &request[0][count0++]);
                MPI_Irecv((void *) (a0 + z_end * ldx * ldy), 1, xyplane,
                          pid + grid_info->num_x * grid_info->num_y,
                          pid + grid_info->num_x * grid_info->num_y,
                          MPI_COMM_WORLD, &request[0][count0++]);
                MPI_Isend((void *) (a0 + z_start * ldx * ldy), 1, xyplane,
                          pid - grid_info->num_x * grid_info->num_y,
                          pid,
                          MPI_COMM_WORLD, &request[1][count1++]);
                MPI_Irecv((void *) (a0 + (z_start - 1) * ldx * ldy), 1, xyplane,
                          pid - grid_info->num_x * grid_info->num_y,
                          pid - grid_info->num_x * grid_info->num_y,
                          MPI_COMM_WORLD, &request[1][count1++]);
            }
            //MPI_Waitall(count, request, status);
        }
        non_runnable = 1;
        //assume that pid % 4 == cgid
        int flag0 = 0;
        int flag1 = 0;
        int runnable0_flag = 0;
        int runnable1_flag = 0;
        if (count0 == 0) {
            flag0 = 2;
            runnable0_flag = 2;
            for (int i = 0; i < 64; i++) h2ldm(runnable0, i, pid % 4) = 2;
            // h2ldm(runnable0, 0, pid % 4) = 2;
        }
        if (count1 == 0) {
            flag1 = 2;
            runnable1_flag = 2;
            for (int i = 0; i < 64; i++) h2ldm(runnable1, i, pid % 4) = 2;
            // h2ldm(runnable1, 0, pid % 4) = 2;
        }

        while (1) {
            if (runnable0_flag == 0) {
                MPI_Testall(count0, request[0], &flag0, status[0]);
            }

            if (runnable1_flag == 0) {
                MPI_Testall(count1, request[1], &flag1, status[1]);
            }

            if (flag0 != 0 && runnable0_flag == 0) {
                runnable0_flag = 1;
                for (int i = 0; i < 64; i++)
                    h2ldm(runnable0, i, pid % 4) = 1;
            }
            if (flag1 != 0 && runnable1_flag == 0) {
                runnable1_flag = 1;
                for (int i = 0; i < 64; i++)
                    h2ldm(runnable1, i, pid % 4) = 1;
            }
            if (runnable0_flag && runnable1_flag)
                break;
        }
#ifdef TIMING
        temp2 = timer();
        if (pid == 1) {
            fprintf(stderr, "[%d]Iter %d mpi end : %lf\n", pid, t, temp2);
            fprintf(stderr, "[%d]Iter %d mpi consume : %lf\n", pid, t, temp2 - temp);
        }
        time_consumed += temp2 - temp;
#endif
        while (non_runnable == 1);
    }
    athread_join();
#ifdef TIMING
    fprintf(stderr, "[%d]Iter mpi consume all : %lf\n", pid, time_consumed);
#endif
//    fprintf(stderr, "[%d]Stencil 27 computing done\n", grid_info->p_id);
    return buffer[nt % 2];
}