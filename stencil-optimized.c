#include <stdio.h>
#include <stdlib.h>
#include "common.h"

const char* version_name = "Optimized version";

/* your implementation */
void create_dist_grid(dist_grid_info_t *grid_info, int stencil_type) {
    puts("not implemented");
    exit(1);
}

/* your implementation */
void destroy_dist_grid(dist_grid_info_t *grid_info) {

}

/* your implementation */
ptr_t stencil_7(ptr_t grid, ptr_t aux, const dist_grid_info_t *grid_info, int nt) {
    return grid;
}

/* your implementation */
ptr_t stencil_27(ptr_t grid, ptr_t aux, const dist_grid_info_t *grid_info, int nt) {
    return grid;
}