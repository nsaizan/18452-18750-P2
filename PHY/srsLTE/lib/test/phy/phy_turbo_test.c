/*
 * Copyright 2013-2019 Software Radio Systems Limited
 *
 * This file is part of srsLTE.
 *
 * srsLTE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsLTE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#include <srslte/phy/utils/random.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "srslte/srslte.h"

#define MAX_DATABUFFER_SIZE (6144 * 16 * 3 / 8)

srslte_cell_t cell = {.nof_prb         = 100,
                      .nof_ports       = 1,
                      .id              = 1,
                      .cp              = SRSLTE_CP_NORM,
                      .phich_resources = SRSLTE_PHICH_R_1,
                      .phich_length    = SRSLTE_PHICH_NORM};

static uint32_t transmission_mode = 1;
static uint32_t cfi               = 1;
static uint32_t nof_rx_ant        = 1;
static uint32_t nof_subframes     = 0;
static bool     print_dci_table;
static uint32_t mcs                     = 20;
static int      cross_carrier_indicator = -1;
static bool     enable_256qam           = false;

void usage(char* prog)
{
  printf("Usage: %s [cfpndvs]\n", prog);
  printf("\t-c cell id [Default %d]\n", cell.id);
  printf("\t-f cfi [Default %d]\n", cfi);
  printf("\t-p cell.nof_prb [Default %d]\n", cell.nof_prb);
  printf("\t-s number of subframes to simulate [Default %d]\n", nof_subframes);
  printf("\t-d Print DCI table [Default %s]\n", print_dci_table ? "yes" : "no");
  printf("\t-t Transmission mode: 1,2,3,4 [Default %d]\n", transmission_mode);
  printf("\t-m mcs [Default %d]\n", mcs);
  printf("\tAdvanced parameters:\n");
  if (cross_carrier_indicator >= 0) {
    printf("\t\t-a carrier-indicator [Default %d]\n", cross_carrier_indicator);
  } else {
    printf("\t\t-a carrier-indicator [Default none]\n");
  }
  printf("\t-v [set srslte_verbose to debug, default none]\n");
  printf("\t-q Enable/Disable 256QAM modulation (default %s)\n", enable_256qam ? "enabled" : "disabled");
}

void parse_extensive_param(char* param, char* arg)
{
  int ext_code = SRSLTE_SUCCESS;
  if (!strcmp(param, "carrier-indicator")) {
    cross_carrier_indicator = (uint32_t)strtol(arg, NULL, 10);
  } else {
    ext_code = SRSLTE_ERROR;
  }

  if (ext_code) {
    ERROR("Error parsing parameter '%s' and argument '%s'\n", param, arg);
    exit(ext_code);
  }
}

void parse_args(int argc, char** argv)
{
  int opt;

  // Load default transmission mode to avoid wrong number of ports/antennas
  if (transmission_mode == 0) {
    cell.nof_ports = 1;
    nof_rx_ant     = 1;
  } else if (transmission_mode < 4) {
    cell.nof_ports = 2;
    nof_rx_ant     = 2;
  }

  while ((opt = getopt(argc, argv, "cfapndvqstm")) != -1) {
    switch (opt) {
      case 't':
        transmission_mode = (uint32_t)strtol(argv[optind], NULL, 10) - 1;
        if (transmission_mode == 0) {
          cell.nof_ports = 1;
          nof_rx_ant     = 1;
        } else if (transmission_mode < 4) {
          cell.nof_ports = 2;
          nof_rx_ant     = 2;
        }
        break;
      case 'f':
        cfi = (uint32_t)(uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'm':
        mcs = (uint32_t)(uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'p':
        cell.nof_prb = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'c':
        cell.id = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 's':
        nof_subframes = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'd':
        print_dci_table = true;
        break;
      case 'a':
        parse_extensive_param(argv[optind], argv[optind + 1]);
        optind++;
        break;
      case 'v':
        srslte_verbose++;
        break;
      case 'q':
        enable_256qam = (enable_256qam) ? false : true;
        break;
      default:
        usage(argv[0]);
        exit(-1);
    }
  }
}

srslte_softbuffer_tx_t* softbuffer_tx[SRSLTE_MAX_TB] = {};
srslte_softbuffer_rx_t* softbuffer_rx[SRSLTE_MAX_TB] = {};
uint8_t*                data_tx[SRSLTE_MAX_TB]       = {};
uint8_t*                data_rx[SRSLTE_MAX_TB]       = {};
uint8_t*                e_bits[SRSLTE_MAX_TB]        = {};

int main(int argc, char** argv)
{
    for (int i = 0; i < SRSLTE_MAX_TB; i++) {
        softbuffer_tx[i] = (srslte_softbuffer_tx_t*)calloc(sizeof(srslte_softbuffer_tx_t), 1);
        if (!softbuffer_tx[i]) {
        ERROR("Error allocating softbuffer_tx\n");
        exit(1);
        }

        if (srslte_softbuffer_tx_init(softbuffer_tx[i], cell.nof_prb)) {
        ERROR("Error initiating softbuffer_tx\n");
        exit(1);
        }

        softbuffer_rx[i] = (srslte_softbuffer_rx_t*)calloc(sizeof(srslte_softbuffer_rx_t), 1);
        if (!softbuffer_rx[i]) {
        ERROR("Error allocating softbuffer_rx\n");
        exit(1);
        }

        if (srslte_softbuffer_rx_init(softbuffer_rx[i], cell.nof_prb)) {
        ERROR("Error initiating softbuffer_rx\n");
        exit(1);
        }

        data_tx[i] = srslte_vec_malloc(sizeof(uint8_t) * MAX_DATABUFFER_SIZE);
        if (!data_tx[i]) {
        ERROR("Error allocating data tx\n");
        exit(1);
        }

        data_rx[i] = srslte_vec_malloc(sizeof(uint8_t) * MAX_DATABUFFER_SIZE);
        if (!data_rx[i]) {
        ERROR("Error allocating data tx\n");
        exit(1);
        }

        e_bits[i] = srslte_vec_malloc(sizeof(uint8_t) * 2048);
        if (!e_bits[i]) {
        ERROR("Error allocating data tx\n");
        exit(1);
        }
    }

    srslte_pdsch_t pdsch_enb;
    srslte_pdsch_init_enb(&pdsch_enb, 100);
    srslte_pdsch_t pdsch_ue;
    srslte_pdsch_init_ue(&pdsch_ue, 100, 1);

    srslte_random_t         random      = srslte_random_init(0);
    for (int j = 0; j < SRSLTE_MAX_TB; j++) {
      for (int i = 0; i < MAX_DATABUFFER_SIZE; i++) {
        data_tx[j][i] = (uint8_t)srslte_random_uniform_int_dist(random, 0, 255);
      }
    }

    srslte_pdsch_cfg_t cfg;
    cfg.rnti = 0x1234;
    cfg.grant.nof_layers = 1;
    cfg.grant.nof_layers = 1;
    cfg.grant.tx_scheme = SRSLTE_TXSCHEME_PORT0;
    cfg.grant.nof_re = 200; // Num Symbols
    cfg.grant.tb[0].cw_idx = 0;
    cfg.grant.tb[0].enabled = true;
    cfg.grant.tb[0].mod = SRSLTE_MOD_QPSK;
    cfg.grant.tb[0].tbs = 256; // Size in bits
    cfg.grant.tb[0].nof_bits = cfg.grant.nof_re * srslte_mod_bits_x_symbol(cfg.grant.tb[0].mod);
    cfg.grant.tb[0].rv = 0;
    cfg.softbuffers.tx[0] = softbuffer_tx[0];
    cfg.softbuffers.tx[1] = softbuffer_tx[1];

    srslte_sch_t dl_sch;
    srslte_sch_init(&dl_sch);

    printf("Data Bytes: ");
    for (int i = 0; i < cfg.grant.tb[0].tbs/8; i++){
      printf("%d ", (int)data_tx[0][i]);
    }
    printf("\n");

    local_pdsch_codeword_encode(&pdsch_enb, &cfg, data_tx[0]);

    for (int i = 0; i < cfg.grant.nof_re; i++){
      pdsch_ue.d[0][i] = pdsch_enb.d[0][i];
    }

    pdsch_ue.d[0][0] = (cf_t)(0 + 0 * _Complex_I);
    pdsch_ue.d[0][4] = (cf_t)(0 + 0 * _Complex_I);
    pdsch_ue.d[0][8] = (cf_t)(0 + 0 * _Complex_I);
    pdsch_ue.d[0][16] = (cf_t)(0 + 0 * _Complex_I);
    pdsch_ue.d[0][17] = (cf_t)(0 + 0 * _Complex_I);
    pdsch_ue.d[0][18] = (cf_t)(0 + 0 * _Complex_I);
    pdsch_ue.d[0][20] = (cf_t)(0 + 0 * _Complex_I);
    pdsch_ue.d[0][25] = (cf_t)(0 + 0 * _Complex_I);
    pdsch_ue.d[0][26] = (cf_t)(0 + 0 * _Complex_I);
    for (int i = 170; i < cfg.grant.nof_re; i++){
      pdsch_ue.d[0][i] = 0;
    }

    cfg.softbuffers.rx[0] = softbuffer_rx[0];
    cfg.softbuffers.rx[1] = softbuffer_rx[1];
    srslte_softbuffer_rx_reset(cfg.softbuffers.rx[0]);
    srslte_softbuffer_rx_reset(cfg.softbuffers.rx[1]);
    int ret = local_pdsch_codeword_decode(&pdsch_ue, &cfg, &dl_sch, data_rx[0]);

    // local_dlsch_encode(&dl_sch, softbuffer_tx[0], 16, SRSLTE_MOD_16QAM, data_tx[0], 128, e_bits[0]);

    // local_dlsch_decode(&dl_sch, softbuffer_rx[0], SRSLTE_MOD_16QAM, 128, e_bits[0], 16, data_rx[0]);
    
    srslte_sch_free(&dl_sch);

    printf("Error Bytes: ");
    for (int i = 0; i < cfg.grant.nof_re * srslte_mod_bits_x_symbol(cfg.grant.tb[0].mod) / 8; i++){
      printf("%d ", (int)((uint8_t*)(pdsch_enb.e[0]))[i]);
    }
    printf("\n");

    printf(ret ? "Bad Decode :(\n" : "Good Decode!\n");
    printf("Data Bytes: ");
    for (int i = 0; i < cfg.grant.tb[0].tbs/8; i++){
      printf("%d ", (int)data_rx[0][i]);
    }
    printf("\n");
}
