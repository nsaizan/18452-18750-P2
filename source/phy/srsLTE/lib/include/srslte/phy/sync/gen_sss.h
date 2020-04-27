#ifndef SRSLTE_GEN_SSS_H
#define SRSLTE_GEN_SSS_H

#include <stdbool.h>
#include <stdint.h>
#include "srslte/config.h"
#include "sss.h"

void generate_zsc_tilde(int* z_tilde, int* s_tilde, int* c_tilde);

void generate_m0m1(uint32_t N_id_1, uint32_t* m0, uint32_t* m1);

void generate_N_id_1_table(uint32_t table[30][30]);

void generate_s(int* s, int* s_tilde, uint32_t m0_m1);

void generate_s_all(int s[SRSLTE_SSS_N][SRSLTE_SSS_N], int* s_tilde);

void generate_c(int* c, int* c_tilde, uint32_t N_id_2, bool is_c0);

void generate_z(int* z, int* z_tilde, uint32_t m0_m1);

void generate_z_all(int z[SRSLTE_SSS_N][SRSLTE_SSS_N], int* z_tilde);

void generate_sss_all_tables(srslte_sss_tables_t* tables, uint32_t N_id_2);

SRSLTE_API void srslte_sss_generate(float* signal0, float* signal5, uint32_t cell_id);

#endif