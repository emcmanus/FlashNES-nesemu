#include "mappers/mapper.h"
#include "mappers/chips/vrc4.h"

static void vrc4a_init(int hard){vrc4_init(KONAMI_VRC4A);}
static void vrc4b_init(int hard){vrc4_init(KONAMI_VRC4B);}
static void vrc4c_init(int hard){vrc4_init(KONAMI_VRC4C);}
static void vrc4d_init(int hard){vrc4_init(KONAMI_VRC4D);}
static void vrc4e_init(int hard){vrc4_init(KONAMI_VRC4E);}

MAPPER_UNIF(konami_vrc_4, "KONAMI-VRC-4", vrc4b_init,0,vrc4_line,vrc4_state,INFO(256,256,8,0,0,0));
MAPPER_UNIF(konami_vrc_4a,"KONAMI-VRC-4A",vrc4a_init,0,vrc4_line,vrc4_state,INFO(256,256,0,0,0,0));
MAPPER_UNIF(konami_vrc_4b,"KONAMI-VRC-4B",vrc4b_init,0,vrc4_line,vrc4_state,INFO(256,256,8,0,0,0));
MAPPER_UNIF(konami_vrc_4c,"KONAMI-VRC-4C",vrc4c_init,0,vrc4_line,vrc4_state,INFO(256,256,8,0,0,0));
MAPPER_UNIF(konami_vrc_4d,"KONAMI-VRC-4D",vrc4d_init,0,vrc4_line,vrc4_state,INFO(256,256,8,0,0,0));
MAPPER_UNIF(konami_vrc_4e,"KONAMI-VRC-4E",vrc4e_init,0,vrc4_line,vrc4_state,INFO(256,256,8,0,0,0));
