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
#ifndef SRSLTE_UL_RS_TABLES_H
#define SRSLTE_UL_RS_TABLES_H

#include <stdint.h>

// Phi values for M_sc=12 Table 5.5.1.2-1 in 36.211
extern int phi_M_sc_12[30][12];

// Phi values for M_sc=24 Table 5.5.1.2-2 in 36.211
extern int phi_M_sc_24[30][24];

// Prime numbers used for Section 5.5.1.1 of 36.211
#define NOF_PRIME_NUMBERS 196
extern uint32_t prime_numbers[NOF_PRIME_NUMBERS];

#endif