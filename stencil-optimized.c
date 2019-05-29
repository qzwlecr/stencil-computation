#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include "common.h"

#define DUMP(varname) fprintf(stderr, "[%d]%s = %d\n", grid_info->p_id, #varname, varname);

const char *version_name = "Optimized version";

/* your implementation */
void create_dist_grid(dist_grid_info_t *grid_info, int stencil_type) {
    if (grid_info->p_num == 16) {
        grid_info->num_x = 2;
        grid_info->num_y = 2;
        grid_info->num_z = 4;
    } else {
        grid_info->num_x = 4;
        grid_info->num_y = 4;
        grid_info->num_z = 4;
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

    fprintf(stderr, "Init dist grid: %d\n", grid_info->p_id);
    //DUMP(grid_info->offset_x);
    //DUMP(grid_info->offset_y);
    //DUMP(grid_info->offset_z);
}

/* your implementation */
void destroy_dist_grid(dist_grid_info_t *grid_info) {

}

/* your implementation */
ptr_t stencil_7(ptr_t grid, ptr_t aux, const dist_grid_info_t *grid_info,
                int nt) {
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

    fprintf(stderr, "[%d]Stencil 7 computing start\n", grid_info->p_id);
//    DUMP(x_start);
//    DUMP(y_start);
//    DUMP(z_start);
//    DUMP(x_end);
//    DUMP(y_end);
//    DUMP(z_end);
//    DUMP(ldx);
//    DUMP(ldy);
//    DUMP(ldz);

    MPI_Datatype yzplane, xzplane, xyplane;
    MPI_Type_vector(ldy * ldz, 1, ldx, MPI_DOUBLE, &yzplane);
    MPI_Type_commit(&yzplane);
    MPI_Type_vector(ldz, ldx, ldx * ldy, MPI_DOUBLE, &xzplane);
    MPI_Type_commit(&xzplane);
    MPI_Type_vector(1, ldx * ldy, 0, MPI_DOUBLE, &xyplane);
    MPI_Type_commit(&xyplane);
int t;
    int pid = grid_info->p_id;
    for ( t = 0; t < nt; ++t) {
        cptr_t a0 = buffer[t % 2];
        ptr_t a1 = buffer[(t + 1) % 2];
        MPI_Status status;
        if (pid % grid_info->num_x == 0) { // yz
            MPI_Sendrecv((void *) (a0 + x_end - 1), 1, yzplane, pid + 1, pid,
                         (void *) (a0 + x_end), 1, yzplane, pid + 1, pid + 1,
                         MPI_COMM_WORLD, &status);
        } else if (pid % grid_info->num_x == grid_info->num_x - 1) {
            MPI_Sendrecv((void *) (a0 + x_start), 1, yzplane, pid - 1, pid,
                         (void *) (a0 + x_start - 1), 1, yzplane, pid - 1, pid - 1,
                         MPI_COMM_WORLD, &status);
        } else {
            MPI_Sendrecv((void *) (a0 + x_end - 1), 1, yzplane, pid + 1, pid,
                         (void *) (a0 + x_end), 1, yzplane, pid + 1, pid + 1,
                         MPI_COMM_WORLD, &status);
            MPI_Sendrecv((void *) (a0 + x_start), 1, yzplane, pid - 1, pid,
                         (void *) (a0 + x_start - 1), 1, yzplane, pid - 1, pid - 1,
                         MPI_COMM_WORLD, &status);
        }

        if ((pid / grid_info->num_x) % grid_info->num_y == 0) { // xz
            MPI_Sendrecv((void *) (a0 + (y_end - 1) * ldx), 1, xzplane, pid + grid_info->num_x, pid,
                         (void *) (a0 + y_end * ldx), 1, xzplane, pid + grid_info->num_x, pid + grid_info->num_x,
                         MPI_COMM_WORLD, &status);
        } else if ((pid / grid_info->num_x) % grid_info->num_y == grid_info->num_y - 1) {
            MPI_Sendrecv((void *) (a0 + y_start * ldx), 1, xzplane, pid - grid_info->num_x, pid,
                         (void *) (a0 + (y_start - 1) * ldx), 1, xzplane, pid - grid_info->num_x,
                         pid - grid_info->num_x, MPI_COMM_WORLD, &status);
        } else {
            MPI_Sendrecv((void *) (a0 + y_start * ldx), 1, xzplane, pid - grid_info->num_x, pid,
                         (void *) (a0 + (y_start - 1) * ldx), 1, xzplane, pid - grid_info->num_x,
                         pid - grid_info->num_x, MPI_COMM_WORLD, &status);
            MPI_Sendrecv((void *) (a0 + (y_end - 1) * ldx), 1, xzplane, pid + grid_info->num_x, pid,
                         (void *) (a0 + y_end * ldx), 1, xzplane, pid + grid_info->num_x, pid + grid_info->num_x,
                         MPI_COMM_WORLD, &status);

        }

        if (pid / grid_info->num_x / grid_info->num_y == 0) { // xy
            MPI_Sendrecv((void *) (a0 + (z_end - 1) * ldx * ldy), 1, xyplane, pid + grid_info->num_x * grid_info->num_y,
                         pid, (void *) (a0 + z_end * ldx * ldy), 1, xyplane, pid + grid_info->num_x * grid_info->num_y,
                         pid + grid_info->num_x * grid_info->num_y, MPI_COMM_WORLD, &status);
        } else if (pid / grid_info->num_x / grid_info->num_y == grid_info->num_z - 1) {
            MPI_Sendrecv((void *) (a0 + z_start * ldx * ldy), 1, xyplane, pid - grid_info->num_x * grid_info->num_y,
                         pid,
                         (void *) (a0 + (z_start - 1) * ldx * ldy), 1, xyplane,
                         pid - grid_info->num_x * grid_info->num_y, pid - grid_info->num_x * grid_info->num_y,
                         MPI_COMM_WORLD, &status);
        } else {
            MPI_Sendrecv((void *) (a0 + (z_end - 1) * ldx * ldy), 1, xyplane, pid + grid_info->num_x * grid_info->num_y,
                         pid, (void *) (a0 + z_end * ldx * ldy), 1, xyplane, pid + grid_info->num_x * grid_info->num_y,
                         pid + grid_info->num_x * grid_info->num_y, MPI_COMM_WORLD, &status);
            MPI_Sendrecv((void *) (a0 + z_start * ldx * ldy), 1, xyplane, pid - grid_info->num_x * grid_info->num_y,
                         pid,
                         (void *) (a0 + (z_start - 1) * ldx * ldy), 1, xyplane,
                         pid - grid_info->num_x * grid_info->num_y, pid - grid_info->num_x * grid_info->num_y,
                         MPI_COMM_WORLD, &status);

        }
        
//todo matrix [][][]
int z,y,x;
// #pragma acc parallel loop  local(z,y,x)  cache(p:200,a0) copyout(a1) collapse(3) tile(z:1,y:1,x:10) //annotate(co_compute)
//         for ( z = z_start; z < z_end; ++z) {
//             for ( y = y_start; y < y_end; ++y) {
//                 for ( x = x_start; x < x_end; ++x) {
//                     a1[INDEX(x, y, z, ldx, ldy)]
//                             = ALPHA_ZZZ * a0[INDEX(x, y, z, ldx, ldy)]
//                               + ALPHA_NZZ * a0[INDEX(x - 1, y, z, ldx, ldy)]
//                               + ALPHA_PZZ * a0[INDEX(x + 1, y, z, ldx, ldy)]
//                               + ALPHA_ZNZ * a0[INDEX(x, y - 1, z, ldx, ldy)]
//                               + ALPHA_ZPZ * a0[INDEX(x, y + 1, z, ldx, ldy)]
//                               + ALPHA_ZZN * a0[INDEX(x, y, z - 1, ldx, ldy)]
//                               + ALPHA_ZZP * a0[INDEX(x, y, z + 1, ldx, ldy)];
//                 }
//             }
//         }

// #pragma acc parallel loop  local(z,y,x)  copyin(a0) copyout(a1) collapse(2) tile(z:1,y:1) //annotate(co_compute)
//         for ( z = z_start; z < z_end; ++z) {
//             for ( y = y_start; y < y_end; ++y) {
//                 for ( x = x_start; x < x_end; ++x) {
//                     a1[INDEX(x, y, z, ldx, ldy)]
//                             = ALPHA_ZZZ * a0[INDEX(x, y, z, ldx, ldy)]
//                               + ALPHA_NZZ * a0[INDEX(x - 1, y, z, ldx, ldy)]
//                               + ALPHA_PZZ * a0[INDEX(x + 1, y, z, ldx, ldy)]
//                               + ALPHA_ZNZ * a0[INDEX(x, y - 1, z, ldx, ldy)]
//                               + ALPHA_ZPZ * a0[INDEX(x, y + 1, z, ldx, ldy)]
//                               + ALPHA_ZZN * a0[INDEX(x, y, z - 1, ldx, ldy)]
//                               + ALPHA_ZZP * a0[INDEX(x, y, z + 1, ldx, ldy)];
//                 }
//             }
//         }

#pragma acc parallel loop  local(z,y,x)  copyin(a0) copyout(a1) collapse(3) tile(z:1,y:1,x:3) //annotate(co_compute)
        for ( z = z_start; z < z_end; ++z) {
            for ( y = y_start; y < y_end; ++y) {
                for ( x = x_start; x < x_end; ++x) {
                    a1[INDEX(x, y, z, ldx, ldy)]
                            = ALPHA_ZZZ * a0[INDEX(x, y, z, ldx, ldy)]
                              + ALPHA_NZZ * a0[INDEX(x - 1, y, z, ldx, ldy)]
                              + ALPHA_PZZ * a0[INDEX(x + 1, y, z, ldx, ldy)]
                              + ALPHA_ZNZ * a0[INDEX(x, y - 1, z, ldx, ldy)]
                              + ALPHA_ZPZ * a0[INDEX(x, y + 1, z, ldx, ldy)]
                              + ALPHA_ZZN * a0[INDEX(x, y, z - 1, ldx, ldy)]
                              + ALPHA_ZZP * a0[INDEX(x, y, z + 1, ldx, ldy)];
                }
            }
        }


    }
    fprintf(stderr, "[%d]Stencil 7 computing done\n", grid_info->p_id);
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

    fprintf(stderr, "[%d]Stencil 27 computing start\n", grid_info->p_id);
//    DUMP(x_start);
//    DUMP(y_start);
//    DUMP(z_start);
//    DUMP(x_end);
//    DUMP(y_end);
//    DUMP(z_end);
//    DUMP(ldx);
//    DUMP(ldy);
//    DUMP(ldz);

    MPI_Datatype yzplane, xzplane, xyplane;
    MPI_Type_vector(ldy * ldz, 1, ldx, MPI_DOUBLE, &yzplane);
    MPI_Type_commit(&yzplane);
    MPI_Type_vector(ldz, ldx, ldx * ldy, MPI_DOUBLE, &xzplane);
    MPI_Type_commit(&xzplane);
    MPI_Type_vector(1, ldx * ldy, 0, MPI_DOUBLE, &xyplane);
    MPI_Type_commit(&xyplane);
int t;
    int pid = grid_info->p_id;
    for ( t = 0; t < nt; ++t) {
        cptr_t a0 = buffer[t % 2];
        ptr_t a1 = buffer[(t + 1) % 2];
        MPI_Status status;
        if (pid % grid_info->num_x == 0) { // yz
            MPI_Sendrecv((void *) (a0 + x_end - 1), 1, yzplane, pid + 1, pid,
                         (void *) (a0 + x_end), 1, yzplane, pid + 1, pid + 1,
                         MPI_COMM_WORLD, &status);
        } else if (pid % grid_info->num_x == grid_info->num_x - 1) {
            MPI_Sendrecv((void *) (a0 + x_start), 1, yzplane, pid - 1, pid,
                         (void *) (a0 + x_start - 1), 1, yzplane, pid - 1, pid - 1,
                         MPI_COMM_WORLD, &status);
        } else {
            MPI_Sendrecv((void *) (a0 + x_end - 1), 1, yzplane, pid + 1, pid,
                         (void *) (a0 + x_end), 1, yzplane, pid + 1, pid + 1,
                         MPI_COMM_WORLD, &status);
            MPI_Sendrecv((void *) (a0 + x_start), 1, yzplane, pid - 1, pid,
                         (void *) (a0 + x_start - 1), 1, yzplane, pid - 1, pid - 1,
                         MPI_COMM_WORLD, &status);
        }

        if ((pid / grid_info->num_x) % grid_info->num_y == 0) { // xz
            MPI_Sendrecv((void *) (a0 + (y_end - 1) * ldx), 1, xzplane, pid + grid_info->num_x, pid,
                         (void *) (a0 + y_end * ldx), 1, xzplane, pid + grid_info->num_x, pid + grid_info->num_x,
                         MPI_COMM_WORLD, &status);
        } else if ((pid / grid_info->num_x) % grid_info->num_y == grid_info->num_y - 1) {
            MPI_Sendrecv((void *) (a0 + y_start * ldx), 1, xzplane, pid - grid_info->num_x, pid,
                         (void *) (a0 + (y_start - 1) * ldx), 1, xzplane, pid - grid_info->num_x,
                         pid - grid_info->num_x, MPI_COMM_WORLD, &status);
        } else {
            MPI_Sendrecv((void *) (a0 + y_start * ldx), 1, xzplane, pid - grid_info->num_x, pid,
                         (void *) (a0 + (y_start - 1) * ldx), 1, xzplane, pid - grid_info->num_x,
                         pid - grid_info->num_x, MPI_COMM_WORLD, &status);
            MPI_Sendrecv((void *) (a0 + (y_end - 1) * ldx), 1, xzplane, pid + grid_info->num_x, pid,
                         (void *) (a0 + y_end * ldx), 1, xzplane, pid + grid_info->num_x, pid + grid_info->num_x,
                         MPI_COMM_WORLD, &status);

        }

        if (pid / grid_info->num_x / grid_info->num_y == 0) { // xy
            MPI_Sendrecv((void *) (a0 + (z_end - 1) * ldx * ldy), 1, xyplane, pid + grid_info->num_x * grid_info->num_y,
                         pid, (void *) (a0 + z_end * ldx * ldy), 1, xyplane, pid + grid_info->num_x * grid_info->num_y,
                         pid + grid_info->num_x * grid_info->num_y, MPI_COMM_WORLD, &status);
        } else if (pid / grid_info->num_x / grid_info->num_y == grid_info->num_z - 1) {
            MPI_Sendrecv((void *) (a0 + z_start * ldx * ldy), 1, xyplane, pid - grid_info->num_x * grid_info->num_y,
                         pid,
                         (void *) (a0 + (z_start - 1) * ldx * ldy), 1, xyplane,
                         pid - grid_info->num_x * grid_info->num_y, pid - grid_info->num_x * grid_info->num_y,
                         MPI_COMM_WORLD, &status);
        } else {
            MPI_Sendrecv((void *) (a0 + (z_end - 1) * ldx * ldy), 1, xyplane, pid + grid_info->num_x * grid_info->num_y,
                         pid, (void *) (a0 + z_end * ldx * ldy), 1, xyplane, pid + grid_info->num_x * grid_info->num_y,
                         pid + grid_info->num_x * grid_info->num_y, MPI_COMM_WORLD, &status);
            MPI_Sendrecv((void *) (a0 + z_start * ldx * ldy), 1, xyplane, pid - grid_info->num_x * grid_info->num_y,
                         pid,
                         (void *) (a0 + (z_start - 1) * ldx * ldy), 1, xyplane,
                         pid - grid_info->num_x * grid_info->num_y, pid - grid_info->num_x * grid_info->num_y,
                         MPI_COMM_WORLD, &status);

        }
int z,y,x;

        //y or x can be expand manually
        for ( z = z_start; z < z_end; ++z) {
            for ( y = y_start; y < y_end; ++y) {
                for ( x = x_start; x < x_end; ++x) {
                    a1[INDEX(x, y, z, ldx, ldy)]
                            = ALPHA_ZZZ * a0[INDEX(x, y, z, ldx, ldy)]
                              + ALPHA_NZZ * a0[INDEX(x - 1, y, z, ldx, ldy)]
                              + ALPHA_PZZ * a0[INDEX(x + 1, y, z, ldx, ldy)]
                              + ALPHA_ZNZ * a0[INDEX(x, y - 1, z, ldx, ldy)]
                              + ALPHA_ZPZ * a0[INDEX(x, y + 1, z, ldx, ldy)]
                              + ALPHA_ZZN * a0[INDEX(x, y, z - 1, ldx, ldy)]
                              + ALPHA_ZZP * a0[INDEX(x, y, z + 1, ldx, ldy)]
                              + ALPHA_NNZ * a0[INDEX(x - 1, y - 1, z, ldx, ldy)]
                              + ALPHA_PNZ * a0[INDEX(x + 1, y - 1, z, ldx, ldy)]
                              + ALPHA_NPZ * a0[INDEX(x - 1, y + 1, z, ldx, ldy)]
                              + ALPHA_PPZ * a0[INDEX(x + 1, y + 1, z, ldx, ldy)]
                              + ALPHA_NZN * a0[INDEX(x - 1, y, z - 1, ldx, ldy)]
                              + ALPHA_PZN * a0[INDEX(x + 1, y, z - 1, ldx, ldy)]
                              + ALPHA_NZP * a0[INDEX(x - 1, y, z + 1, ldx, ldy)]
                              + ALPHA_PZP * a0[INDEX(x + 1, y, z + 1, ldx, ldy)]
                              + ALPHA_ZNN * a0[INDEX(x, y - 1, z - 1, ldx, ldy)]
                              + ALPHA_ZPN * a0[INDEX(x, y + 1, z - 1, ldx, ldy)]
                              + ALPHA_ZNP * a0[INDEX(x, y - 1, z + 1, ldx, ldy)]
                              + ALPHA_ZPP * a0[INDEX(x, y + 1, z + 1, ldx, ldy)]
                              + ALPHA_NNN * a0[INDEX(x - 1, y - 1, z - 1, ldx, ldy)]
                              + ALPHA_PNN * a0[INDEX(x + 1, y - 1, z - 1, ldx, ldy)]
                              + ALPHA_NPN * a0[INDEX(x - 1, y + 1, z - 1, ldx, ldy)]
                              + ALPHA_PPN * a0[INDEX(x + 1, y + 1, z - 1, ldx, ldy)]
                              + ALPHA_NNP * a0[INDEX(x - 1, y - 1, z + 1, ldx, ldy)]
                              + ALPHA_PNP * a0[INDEX(x + 1, y - 1, z + 1, ldx, ldy)]
                              + ALPHA_NPP * a0[INDEX(x - 1, y + 1, z + 1, ldx, ldy)]
                              + ALPHA_PPP * a0[INDEX(x + 1, y + 1, z + 1, ldx, ldy)];
                }
            }
        }
    }
    fprintf(stderr, "[%d]Stencil 27 computing done\n", grid_info->p_id);
    return buffer[nt % 2];
}