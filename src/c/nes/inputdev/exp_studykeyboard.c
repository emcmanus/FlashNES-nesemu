#include "exp_studykeyboard.h"
#include "system/system.h"
#include "system/keys.h"

//keyboard data
static u8 rowdata[16];

//keyboard clock bit and current row
static u8 clock,row;

static u8 read_kb()
{
	u8 ret = 0;

	if(clock == 0)	//read lower nibble
		ret = (rowdata[row] & 0xF) << 1;
	else
		ret = ((rowdata[row] >> 4) & 0xF) << 1;
//	log_message("keyboard read (clock=%d, row=%d) = $%02X\n",clock,row,ret);
	return(ret ^ 0x1E);
}

static void write_kb(u8 data)
{
//	log_message("keyboard write = $%02X\n",data);
	if((data & 2) == 0 && clock)	//increment row
		row = (row + 1) & 0xF;
	clock = data & 2;
}

static void strobe_kb()
{
	int i;

//	log_message("keyboard strobe\n");
	clock = 0;
	row = 0;
	for(i=0;i<16;i++)		rowdata[i] = 0;
	if(joykeys[SDLK_4])					rowdata[0] |= 0x01;
	if(joykeys[SDLK_g])					rowdata[0] |= 0x02;
	if(joykeys[SDLK_f])					rowdata[0] |= 0x04;
	if(joykeys[SDLK_c])					rowdata[0] |= 0x08;
	if(joykeys[SDLK_F2])					rowdata[0] |= 0x10;
	if(joykeys[SDLK_e])					rowdata[0] |= 0x20;
	if(joykeys[SDLK_5])					rowdata[0] |= 0x40;
	if(joykeys[SDLK_v])					rowdata[0] |= 0x80;

	if(joykeys[SDLK_2])					rowdata[1] |= 0x01;
	if(joykeys[SDLK_d])					rowdata[1] |= 0x02;
	if(joykeys[SDLK_s])					rowdata[1] |= 0x04;
	if(joykeys[SDLK_END])				rowdata[1] |= 0x08;
	if(joykeys[SDLK_F1])					rowdata[1] |= 0x10;
	if(joykeys[SDLK_w])					rowdata[1] |= 0x20;
	if(joykeys[SDLK_3])					rowdata[1] |= 0x40;
	if(joykeys[SDLK_x])					rowdata[1] |= 0x80;

	if(joykeys[SDLK_INSERT])			rowdata[2] |= 0x01;
	if(joykeys[SDLK_BACKSPACE])		rowdata[2] |= 0x02;
	if(joykeys[SDLK_PAGEDOWN])			rowdata[2] |= 0x04;
	if(joykeys[SDLK_RIGHT])				rowdata[2] |= 0x08;
	if(joykeys[SDLK_F8])					rowdata[2] |= 0x10;
	if(joykeys[SDLK_PAGEUP])			rowdata[2] |= 0x20;
	if(joykeys[SDLK_ESCAPE])			rowdata[2] |= 0x40;
	if(joykeys[SDLK_HOME])				rowdata[2] |= 0x80;

	if(joykeys[SDLK_9])					rowdata[3] |= 0x01;
	if(joykeys[SDLK_i])					rowdata[3] |= 0x02;
	if(joykeys[SDLK_l])					rowdata[3] |= 0x04;
	if(joykeys[SDLK_COMMA])				rowdata[3] |= 0x08;
	if(joykeys[SDLK_F5])					rowdata[3] |= 0x10;
	if(joykeys[SDLK_o])					rowdata[3] |= 0x20;
	if(joykeys[SDLK_0])					rowdata[3] |= 0x40;
	if(joykeys[SDLK_PERIOD])			rowdata[3] |= 0x80;

	if(joykeys[SDLK_RIGHTBRACKET])	rowdata[4] |= 0x01;
	if(joykeys[SDLK_RETURN])			rowdata[4] |= 0x02;
	if(joykeys[SDLK_UP])					rowdata[4] |= 0x04;
	if(joykeys[SDLK_LEFT])				rowdata[4] |= 0x08;
	if(joykeys[SDLK_F7])					rowdata[4] |= 0x10;
	if(joykeys[SDLK_LEFTBRACKET])		rowdata[4] |= 0x20;
	if(joykeys[SDLK_BACKSLASH])		rowdata[4] |= 0x40;
	if(joykeys[SDLK_DOWN])				rowdata[4] |= 0x80;

	if(joykeys[SDLK_q])					rowdata[5] |= 0x01;
	if(joykeys[SDLK_CAPSLOCK])			rowdata[5] |= 0x02;
	if(joykeys[SDLK_z])					rowdata[5] |= 0x04;
//	if(joykeys[SDLK_END])				rowdata[5] |= 0x08;
	if(joykeys[SDLK_ESCAPE])			rowdata[5] |= 0x10;
	if(joykeys[SDLK_a])					rowdata[5] |= 0x20;
	if(joykeys[SDLK_1])					rowdata[5] |= 0x40;
	if(joykeys[SDLK_LCTRL])				rowdata[5] |= 0x80;
	if(joykeys[SDLK_RCTRL])				rowdata[5] |= 0x80;

	if(joykeys[SDLK_7])					rowdata[6] |= 0x01;
	if(joykeys[SDLK_y])					rowdata[6] |= 0x02;
	if(joykeys[SDLK_k])					rowdata[6] |= 0x04;
	if(joykeys[SDLK_m])					rowdata[6] |= 0x08;
	if(joykeys[SDLK_F4])					rowdata[6] |= 0x10;
	if(joykeys[SDLK_u])					rowdata[6] |= 0x20;
	if(joykeys[SDLK_8])					rowdata[6] |= 0x40;
	if(joykeys[SDLK_j])					rowdata[6] |= 0x80;

	if(joykeys[SDLK_MINUS])				rowdata[7] |= 0x01;
	if(joykeys[SDLK_SEMICOLON])		rowdata[7] |= 0x02;
	if(joykeys[SDLK_QUOTE])				rowdata[7] |= 0x04;
	if(joykeys[SDLK_SLASH])				rowdata[7] |= 0x08;
	if(joykeys[SDLK_F6])					rowdata[7] |= 0x10;
	if(joykeys[SDLK_p])					rowdata[7] |= 0x20;
	if(joykeys[SDLK_EQUALS])			rowdata[7] |= 0x40;
	if(joykeys[SDLK_LSHIFT])			rowdata[7] |= 0x80;
	if(joykeys[SDLK_RSHIFT])			rowdata[7] |= 0x80;

	if(joykeys[SDLK_t])					rowdata[8] |= 0x01;
	if(joykeys[SDLK_h])					rowdata[8] |= 0x02;
	if(joykeys[SDLK_n])					rowdata[8] |= 0x04;
	if(joykeys[SDLK_SPACE])				rowdata[8] |= 0x08;
	if(joykeys[SDLK_F3])					rowdata[8] |= 0x10;
	if(joykeys[SDLK_r])					rowdata[8] |= 0x20;
	if(joykeys[SDLK_6])					rowdata[8] |= 0x40;
	if(joykeys[SDLK_b])					rowdata[8] |= 0x80;

}

inputdev_t exp_studykeyboard = {read_kb,write_kb,strobe_kb,0};
