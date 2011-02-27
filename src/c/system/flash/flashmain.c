#include "AS3.h"

#include "../../nesemu.h"

int main(int argc,char *argv[])
{
	// Flash API
	AS3_Val setup = AS3_Function(NULL, f_setup);
	AS3_Val loop = AS3_Function(NULL, f_loop);
	AS3_Val teardown = AS3_Function(NULL, f_teardown);
	AS3_Val screen = AS3_Function(NULL, f_screen);
	
    AS3_Val nes = AS3_Object( "setup:AS3ValType, loop:AS3ValType, teardown:AS3ValType, screen:AS3ValType", setup, loop, teardown, screen );
    
	AS3_Release( setup );
	AS3_Release( loop );
	AS3_Release( teardown );
	AS3_Release( screen );
	
    AS3_LibInit( nes );
	
	return 0;
}