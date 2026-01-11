/*
 * bsod.h
 * Interface for Black Screen of Death
 *  Created on: Jan 4, 2026
 *      Author: harma
 */

#ifndef BSOD_H_
#define BSOD_H_

#include "base.h"

// Forward declaration
typedef enum error_t error_t;

void bsod_show(error_t error_code, byte* regs_snapshot);

#endif /* BSOD_H_ */