/***************************************************************************
 *   Copyright (C) 2006-2009 by Dead_Body   *
 *   jamesholodnak@gmail.com   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <string.h>
#include "defines.h"
#include "mapper.h"
#include "nes/ppu/ppu.h"
#include "nes/nes.h"

#define MAPPER(n) \
	extern mapper_unif_t mapper_##n; \
	if(unifsearch) { \
		unifcounter++; \
		if(unifcounter == unifsearch) { \
			unifcounter = 0; \
			unifsearch++; \
			return(&mapper_##n); \
		} \
	} \
	else if(stricmp(board,mapper_##n.board) == 0) \
		return(&mapper_##n);

#ifdef __cplusplus
extern "C" 
#endif
mapper_unif_t *mapper_init_unif(char *board)
{
	static int unifsearch = 0;
	static int unifcounter = 0;

	//begin unif mapper search
	if(board == 0) {
		if(unifsearch == 0) {
			unifsearch = 1;
			unifcounter = 0;
		}
	}
	else
		unifsearch = 0;

	//konami mappers
	MAPPER(konami_vrc_1);
	MAPPER(konami_vrc_2);
	MAPPER(konami_vrc_2a);
	MAPPER(konami_vrc_2b);
	MAPPER(konami_vrc_3);
	MAPPER(konami_vrc_4);
	MAPPER(konami_vrc_4a);
	MAPPER(konami_vrc_4b);
	MAPPER(konami_vrc_4c);
	MAPPER(konami_vrc_4d);
	MAPPER(konami_vrc_4e);
	MAPPER(konami_vrc_6);
	MAPPER(konami_vrc_6a);
	MAPPER(konami_vrc_6b);
	MAPPER(konami_vrc_7);
	MAPPER(konami_vrc_7a);
	MAPPER(konami_vrc_7b);

	//famicom mappers
	MAPPER(hvc_un1rom);

	//nintendo mappers
	MAPPER(nes_nrom_128);
	MAPPER(nes_nrom_256);

	MAPPER(nes_amrom);
	MAPPER(nes_anrom);
	MAPPER(nes_an1rom);
	MAPPER(nes_aorom);

	MAPPER(nes_bnrom);
	MAPPER(irem_bnrom);

	MAPPER(nes_cnrom);
	MAPPER(taito_cnrom);
	MAPPER(konami_cnrom);

	MAPPER(nes_fjrom);
	MAPPER(nes_fkrom);

	MAPPER(nes_jlrom);
	MAPPER(nes_jsrom);

	MAPPER(nes_pnrom);
	MAPPER(nes_peeorom);

	MAPPER(nes_sarom);
	MAPPER(nes_sbrom);
	MAPPER(nes_scrom);
	MAPPER(nes_serom);
	MAPPER(nes_sfrom);
	MAPPER(nes_sgrom);
	MAPPER(nes_shrom);
	MAPPER(nes_sjrom);
	MAPPER(nes_skrom);
	MAPPER(nes_slrom);
	MAPPER(nes_snrom);
	MAPPER(nes_sorom);
	MAPPER(nes_surom);
	MAPPER(nes_sxrom);

	MAPPER(nes_terom);
	MAPPER(nes_tbrom);
	MAPPER(nes_tfrom);
	MAPPER(nes_tgrom);
	MAPPER(nes_tkrom);
	MAPPER(nes_tksrom);
	MAPPER(nes_tlrom);
	MAPPER(nes_tl1rom);
	MAPPER(nes_tl2rom);
	MAPPER(nes_tlsrom);
	MAPPER(nes_tnrom);
	MAPPER(nes_tr1rom);
	MAPPER(nes_tsrom);
	MAPPER(nes_tvrom);
	MAPPER(nes_tqrom);

	//unrom variations
	MAPPER(nes_unrom);
	MAPPER(hvc_unrom);
	MAPPER(konami_unrom);

	//uorom variations
	MAPPER(nes_uorom);
	MAPPER(hvc_uorom);
	MAPPER(jaleco_uorom);

	//sunsoft mappers
	MAPPER(sunsoft_fme_7);

	//namco mappers
	MAPPER(namco_106);
	MAPPER(namco_108);
	MAPPER(namco_109);
	MAPPER(namco_118);

	//bootleg boards
	MAPPER(mlt_caltron6in1);
	MAPPER(btl_biomiraclea);
	MAPPER(btl_tek2a);

	MAPPER(unl_22211);
	MAPPER(unl_h2288);

	MAPPER(unl_sa_nrom);

	MAPPER(unl_603_5052);

	MAPPER(colordreams_74377);

	//unif only
	MAPPER(btl_mario1_malee2);
	MAPPER(bmc_fk23c);
	MAPPER(bmc_generic42in1);
	MAPPER(bmc_super24in1sc03);
	MAPPER(unl_kof97);
	MAPPER(unl_sachen_74ls374n);
	MAPPER(unl_sachen_8259a);
	MAPPER(unl_sachen_8259b);
	MAPPER(unl_sachen_8259c);
	MAPPER(unl_sachen_8259d);
	MAPPER(unl_sa_0036);
	MAPPER(unl_sa_72007);
	MAPPER(unl_sa_72008);
	MAPPER(bmc_70in1);
	MAPPER(bmc_70in1a);
	MAPPER(bmc_70in1b);

	//board not found, return error
	return(0);
}
