#include <stdio.h>
#include <string.h>
#include "blarggapu.h"
#include "Simple_Apu.h"

static apu_t *apu;
//static Blip_Buffer buf;
static Simple_Apu nesapu;

const size_t out_size = 4096;
blip_sample_t out_buf[out_size];

void apu_setext(apu_t *apu,apuext_t *ext)
{
	apu->ext = ext;
	ext->init();
	ext->reset();
}

void apu_process(void *buf,int len)
{
	int rlen = nesapu.read_samples((sample_t*)buf,len);

//	if(apu && rlen == 0) {
//		nesapu.end_frame();
//		rlen = nesapu.read_samples((sample_t*)buf,len);
//	}
	printf("read %d (of %d wanted) samples\n",rlen,len);
}

int dmc_reader(void *user,cpu_addr_t addr)
{
	return(dead6502_read(addr));
}

extern "C" void osd_setsound(void (*playfunc)(void *buffer, int length));

apu_t *apu_create(double base_freq, int sample_rate, int refresh_rate, int sample_bits,void (*irq)())
{
	apu_t *ret = 0;

	ret = (apu_t*)malloc(sizeof(apu_t));
	memset(ret,0,sizeof(apu_t));

	ret->base_freq = base_freq;
	ret->sample_rate = sample_rate;
	ret->refresh_rate = refresh_rate;
	ret->sample_bits = sample_bits;
	ret->irq = irq;

	nesapu.sample_rate(SOUND_HZ);
	nesapu.dmc_reader(dmc_reader);

	osd_setsound(apu_process);

	printf("blargg apu created ok\n");

	//set our apu pointer and return
	apu = ret;
	return(ret);
}

void apu_destroy(apu_t **apu)
{
	free(*apu);
	*apu = 0;
}

void apu_endframe()
{
	nesapu.clearbuf();
	nesapu.end_frame();
}

void apu_reset(int hard)
{
//	nesapu.reset();
}

u8 apu_read(u32 addr)
{
	if(addr == 0x4015)
		return(nesapu.read_status());
	return(0);
}

void apu_write(u32 addr,u8 data)
{
	nesapu.write_register(addr,data);
}
