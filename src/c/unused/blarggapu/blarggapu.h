#ifndef __blarggapu_h__
#define __blarggapu_h__

#include "defines.h"

extern u32 ppucycles,ppuframes;

#define pputime (ppucycles + (ppuframes * 341 * 262))
#define cputime (pputime / 3)

typedef struct apuext_s {
   void	(*init)();
   void	(*kill)();
   void	(*reset)();
   int	(*process)(int);
   void	*mem_read;
   void	*mem_write;
} apuext_t;

typedef struct apu_s {
	double	base_freq;
	int		sample_rate;
	int		refresh_rate;
	int		sample_bits;
	void		(*irq)();
	apuext_t	*ext;
} apu_t;

#ifdef __cplusplus
extern "C" {
#endif

void apu_setcontext(apu_t *src_apu);
void apu_getcontext(apu_t *dest_apu);

void apu_setparams(double base_freq, int sample_rate, int refresh_rate, int sample_bits);
apu_t *apu_create(double base_freq, int sample_rate, int refresh_rate, int sample_bits,void (*irq)());
void apu_destroy(apu_t **apu);

void apu_endframe();
void apu_process(void *buffer, int num_samples);
void apu_reset(int hard);

void apu_setext(apu_t *apu, apuext_t *ext);
void apu_setfilter(int filter_type);
void apu_setchan(int chan, bool enabled);

u8 apu_read(u32 address);
void apu_write(u32 address, u8 value);

#ifdef __cplusplus
};
#endif

#endif
