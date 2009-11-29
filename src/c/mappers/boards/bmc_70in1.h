#ifndef __bmc_70in1_h__
#define __bmc_70in1_h__

#define BMC_70IN1A	0
#define BMC_70IN1B	1

void bmc_70in1_reset(int revision,int hard);
void bmc_70in1_state(int mode,u8 *data);

#endif
