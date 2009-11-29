#ifndef __txc_22211_h__
#define __txc_22211_h__

#define TXC_22211A 0
#define TXC_22211B 1
#define TXC_22211C 2

void txc_22211_reset(int hard,int revision);
void txc_22211_state(int mode,u8 *data);

#endif
