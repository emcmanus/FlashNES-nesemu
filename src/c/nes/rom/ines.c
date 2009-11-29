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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "defines.h"
#include "nes/nes.h"
#include "system/file.h"

static void load_ines_header(rom_t *ret,u8 *header)
{
	ret->prgsize = header[4] * 0x4000;
	ret->chrsize = header[5] * 0x2000;
	ret->prgmask = rom_createmask(ret->prgsize);
	ret->chrmask = rom_createmask(ret->chrsize);
	ret->vrammask = rom_createmask(0);				//start with no vram, let mapper control this
	ret->mapper = (header[6] & 0xF0) >> 4;
	if(memcmp(&header[8],"\0\0\0\0\0\0\0\0",8) != 0)
		log_message("load_ines_header:  dirty header! (%c%c%c%c%c%c%c%c%c)\n",header[7],header[8],header[9],header[10],header[11],header[12],header[13],header[14],header[15]);
	else
		ret->mapper |= header[7] & 0xF0;
	if(ret->mirroring & 4)
		ret->mirroring = 4;
	else
		ret->mirroring = header[6] & 1;
	if(header[6] & 2)
		rom_setsramsize(ret,2);
	log_message("load_ines_header:  %dkb prg, %dkb chr, mapper %d, %s mirroring\n",
		ret->prgsize / 1024,ret->chrsize / 1024,ret->mapper,
		(ret->mirroring == 4) ? "four screen" :
		((ret->mirroring == 0) ? "horizontal" : "vertical"));
}

#if 0
				switch (mapper)
				{
					case 10:

						if (chr == SIZE_8K || chr == SIZE_16K || chr == SIZE_32K || chr == SIZE_64K || chr >= SIZE_128K)
						{
							if (prg == SIZE_128K)
							{
								name = "FJROM";
								id = Type::STD_FJROM;
								break;
							}
							else if (prg >= SIZE_256K)
							{
								name = "FKROM";
								id = Type::STD_FKROM;
								break;
							}
						}

						name = "FxROM (non-standard)";
						id = Type::STD_FKROM;
						break;

					case 11:

						name = "COLORDREAMS 74*377";
						id = Type::DISCRETE_74_377;
						break;

					case 12:

						name = "REX DBZ5";
						id = Type::REXSOFT_DBZ5;
						break;

					case 13:

						name = "CPROM";
						id = Type::STD_CPROM;
						break;

					case 14:

						name = "SOMERITEAM SL-1632";
						id = Type::REXSOFT_SL1632;
						break;

					case 15:

						name = "WAIXING / BMC CONTRA 100-IN-1";

						if (wram || useWramAuto)
						{
							wramAuto = useWramAuto;
							id = Type::WAIXING_PS2_1;
						}
						else
						{
							id = Type::WAIXING_PS2_0;
						}
						break;

					case 16:

						if (!wram && prg <= SIZE_256K)
						{
							name = "BANDAI LZ93D50 +24C02";
							id = Type::BANDAI_LZ93D50_24C02;
							break;
						}

					case 153:

						if (prg == SIZE_512K)
						{
							name = "BANDAI BA-JUMP2";
							id = Type::BANDAI_BAJUMP2;
						}
						else
						{
							name = "BANDAI FCG-1/FCG-2";
							id = Type::BANDAI_FCG1;
						}
						break;

					case 17:

						name = "FFE F8xxx / SMxxxx";
						id = Type::CUSTOM_FFE8;
						break;

					case 18:

						name = "JALECO SS88006";
						id = Type::JALECO_SS88006;
						break;

					case 19:

						name = "NAMCOT 163";

						if (wram || useWramAuto)
						{
							wramAuto = useWramAuto;

							if (mmcBattery)
								id = Type::NAMCOT_163_S_1;
							else
								id = Type::NAMCOT_163_1;
						}
						else
						{
							if (mmcBattery)
								id = Type::NAMCOT_163_S_0;
							else
								id = Type::NAMCOT_163_0;
						}
						break;

					case 21:
					case 25:

						if (!this->chips.Has(L"Konami VRC IV"))
							return false;

						name = "KONAMI VRC4";
						id = Type::KONAMI_VRC4_2;
						break;

					case 22:

						if (!this->chips.Has(L"Konami VRC II"))
						{
							Chips::Type& chip = chips.Add(L"Konami VRC II");

							chip.Pin(3)  = L"PRG A0";
							chip.Pin(4)  = L"PRG A1";
							chip.Pin(21) = (chr >= SIZE_256K ? L"CHR A17" : L"NC");
							chip.Pin(22) = L"CHR A15";
							chip.Pin(23) = L"CHR A10";
							chip.Pin(24) = L"CHR A12";
							chip.Pin(25) = L"CHR A13";
							chip.Pin(26) = L"CHR A11";
							chip.Pin(27) = L"CHR A14";
							chip.Pin(28) = L"CHR A16";
						}

						name = "KONAMI VRC2";
						id = Type::KONAMI_VRC2;
						break;

					case 23:

						if (prg >= SIZE_512K)
						{
							if (!this->chips.Has(L"Konami VRC IV"))
							{
								Chips::Type& chip = chips.Add(L"Konami VRC IV");

								chip.Pin(3) = L"PRG A3";
								chip.Pin(4) = L"PRG A2";
							}

							name = "BMC VRC4";
							id = Type::BMC_VRC4;
						}
						else if (this->chips.Has(L"Konami VRC II"))
						{
							name = "KONAMI VRC2";
							id = Type::KONAMI_VRC2;
						}
						else if (this->chips.Has(L"Konami VRC IV"))
						{
							name = "KONAMI VRC4";
							id = Type::KONAMI_VRC4_2;
						}
						else
						{
							return false;
						}
						break;

					case 24:
					case 26:

						if (!this->chips.Has(L"Konami VRC VI"))
						{
							Chips::Type& chip = chips.Add(L"Konami VRC VI");

							if (mapper == 24)
							{
								chip.Pin(9)  = L"PRG A1";
								chip.Pin(10) = L"PRG A0";

								name = "KONAMI VRC6 (a)";
							}
							else
							{
								chip.Pin(9)  = L"PRG A0";
								chip.Pin(10) = L"PRG A1";

								name = "KONAMI VRC6 (b)";
							}
						}

						if (wram || useWramAuto)
						{
							wramAuto = useWramAuto;
							id = Type::KONAMI_VRC6_1;
						}
						else
						{
							id = Type::KONAMI_VRC6_0;
						}
						break;

					case 27:

						name = "UNL WORLDHERO";
						id = Type::UNL_WORLDHERO;
						break;

					case 32:

						name = "IREM G-101";

						if (wram || useWramAuto)
						{
							wramAuto = useWramAuto;
							id = Type::IREM_G101A_1;
						}
						else
						{
							id = Type::IREM_G101A_0;
						}
						break;

					case 33:

						name = "TAITO TC0190FMC";
						id = Type::TAITO_TC0190FMC;
						break;

					case 34:

						if (chr)
						{
							name = "AVE NINA-001";
							id = Type::AVE_NINA001;
						}
						else if (prg == SIZE_128K && !wram && (nmt == Type::NMT_HORIZONTAL || nmt == Type::NMT_VERTICAL))
						{
							name = "BNROM";
							id = Type::STD_BNROM;
						}
						else
						{
							name = "BxROM (non-standard)";
							id = Type::UNL_BXROM;
						}
						break;

					case 36:

						name = "TXC 01-22000-400";
						id = Type::TXC_POLICEMAN;
						break;

					case 37:

						name = "ZZ";
						id = Type::CUSTOM_ZZ;
						break;

					case 38:

						name = "BIT CORP 74*161/138";
						id = Type::DISCRETE_74_161_138;
						break;

					case 39:

						name = "STUDY & GAME 32-IN-1";
						id = Type::SUBOR_STUDYNGAME;
						break;

					case 40:

						name = "BTL SMB2 (a)";
						id = Type::BTL_SMB2_A;
						break;

					case 41:

						name = "CALTRON 6-IN-1";
						id = Type::CALTRON_6IN1;
						break;

					case 42:

						if (chr)
						{
							name = "BTL UPA";
							id = Type::BTL_MARIOBABY;
						}
						else
						{
							name = "BTL AI SENSHI NICOL";
							id = Type::BTL_AISENSHINICOL;
						}
						break;

					case 43:

						name = "BTL SMB2 (c)";
						id = Type::BTL_SMB2_C;
						break;

					case 44:

						name = "BMC SUPERBIG 7-IN-1";
						id = Type::BMC_SUPERBIG_7IN1;
						break;

					case 45:

						name = "BMC SUPER/HERO X-IN-1";
						id = Type::BMC_HERO;
						break;

					case 46:

						name = "RUMBLESTATION 15-IN-1";
						id = Type::CUSTOM_RUMBLESTATION;
						break;

					case 47:

						name = "QJ";
						id = Type::CUSTOM_QJ;
						break;

					case 48:

						name = "TAITO TC0190FMC +PAL16R4";
						id = Type::TAITO_TC0190FMC_PAL16R4;
						break;

					case 49:

						name = "BMC SUPERHIK 4-IN-1";
						id = Type::BMC_SUPERHIK_4IN1;
						break;

					case 50:

						name = "BTL SMB2 (b)";
						id = Type::BTL_SMB2_B;
						break;

					case 51:

						name = "BMC 11-IN-1 BALLGAMES";
						id = Type::BMC_BALLGAMES_11IN1;
						break;

					case 52:

						name = "BMC MARIOPARTY 7-IN-1";
						id = Type::BMC_MARIOPARTY_7IN1;
						break;

					case 53:

						name = "BMC SUPERVISION 16-IN-1";
						id = Type::BMC_SUPERVISION_16IN1;
						break;

					case 54:

						name = "BMC NOVELDIAMOND 9999999-IN-1";
						id = Type::BMC_NOVELDIAMOND;
						break;

					case 55:

						name = "BTL GENIUS MERIO BROS";
						id = Type::BTL_GENIUSMERIOBROS;
						break;

					case 56:

						name = "KAISER KS-202";
						id = Type::KAISER_KS202;
						break;

					case 57:

						name = "BMC GAMESTAR (a)";
						id = Type::BMC_GKA;
						break;

					case 58:

						name = "BMC GAMESTAR (b)";
						id = Type::BMC_GKB;
						break;

					case 60:

						if (prg == SIZE_64K && chr == SIZE_32K)
						{
							name = "BMC RESETBASED 4-IN-1";
							id = Type::BMC_RESETBASED_4IN1;
						}
						else
						{
							name = "BMC VT5201";
							id = Type::BMC_VT5201;
						}
						break;

					case 61:

						name = "TXC / BMC 20-IN-1";
						id = Type::RCM_TETRISFAMILY;
						break;

					case 62:

						name = "BMC SUPER 700-IN-1";
						id = Type::BMC_SUPER_700IN1;
						break;

					case 63:

						name = "BMC CH-001";
						id = Type::BMC_CH001;
						break;

					case 64:

						name = "TENGEN 800032";
						id = Type::TENGEN_800032;
						break;

					case 65:

						name = "IREM H-3001";
						id = Type::IREM_H3001;
						break;

					case 66:

						if ((chr == SIZE_8K || chr == SIZE_16K || chr == SIZE_32K) && !wram && (nmt == Type::NMT_HORIZONTAL || nmt == Type::NMT_VERTICAL))
						{
							if (prg == SIZE_64K)
							{
								name = "GNROM / MHROM";
								id = Type::STD_GNROM;
								break;
							}
							else if (prg == SIZE_32K || prg == SIZE_128K)
							{
								name = "GNROM";
								id = Type::STD_GNROM;
								break;
							}
						}

						name = "GxROM (non-standard)";
						id = Type::UNL_GXROM;
						break;

					case 67:

						name = "SUNSOFT 3";
						id = Type::SUNSOFT_3;
						break;

					case 68:

						if (prg > SIZE_128K)
						{
							name = "SUNSOFT DOUBLE CASETTE SYSTEM";
							id = Type::SUNSOFT_DCS;
						}
						else
						{
							name = "SUNSOFT 4";

							if (wram || useWramAuto)
							{
								wramAuto = useWramAuto;
								id = Type::SUNSOFT_4_1;
							}
							else
							{
								id = Type::SUNSOFT_4_0;
							}
						}
						break;

					case 69:

						name = "SUNSOFT 5B/FME7";

						if (wram || useWramAuto)
						{
							wramAuto = useWramAuto;
							id = Type::SUNSOFT_5B_1;
						}
						else
						{
							id = Type::SUNSOFT_5B_0;
						}
						break;

					case 70:

						name = "74*161/161/32 (a)";
						id = Type::DISCRETE_74_161_161_32_A;
						break;

					case 71:

						if (prg >= SIZE_256K)
						{
							name = "CAMERICA BF9093";
							id = Type::CAMERICA_BF9093;
						}
						else
						{
							name = "CAMERICA BF9093/BF9097";
							id = Type::CAMERICA_BF909X;
						}
						break;

					case 72:

						name = "JALECO JF-26/28";
						id = Type::JALECO_JF26;
						break;

					case 73:

						name = "KONAMI VRC3";
						id = Type::KONAMI_VRC3;
						break;

					case 74:

						name = "WAIXING (a)";
						id = Type::WAIXING_TYPE_A;
						break;

					case 75:

						name = "KONAMI VRC1 / JALECO JF-22";
						id = Type::KONAMI_VRC1;
						break;

					case 76:

						name = "NAMCOT 3446";
						id = Type::NAMCOT_3446;
						break;

					case 77:

						name = "IREM 74*161/161/21/138";
						id = Type::IREM_LROG017;
						break;

					case 78:

						name = "JALECO JF-16";
						id = Type::JALECO_JF16;
						break;

					case 79:

						name = "AVE NINA-03 / NINA-06 / MB-91";
						id = Type::AVE_NINA06;
						break;

					case 80:

						name = "TAITO X1-005 (a)";
						id = Type::TAITO_X1005;
						break;

					case 82:

						name = "TAITO X1-017";
						id = Type::TAITO_X1017;
						break;

					case 83:

						if (prg >= SIZE_1024K || chr >= SIZE_1024K)
						{
							name = "BMC DRAGONBALLPARTY 4-IN-1";
							id = Type::BMC_DRAGONBOLLPARTY;
						}
						else
						{
							name = "CONY";
							id = Type::CONY_STD;
						}
						break;

					case 85:

						name = "KONAMI VRC7";

						if (wram || useWramAuto)
						{
							wramAuto = useWramAuto;
							id = Type::KONAMI_VRC7_1;
						}
						else
						{
							id = Type::KONAMI_VRC7_0;
						}
						break;

					case 86:

						name = "JALECO JF-13";
						id = Type::JALECO_JF13;
						break;

					case 87:

						name = "74*139/74";
						id = Type::DISCRETE_74_139_74;
						break;

					case 88:

						if (nmt == Type::NMT_HORIZONTAL)
						{
							name = "NAMCOT 3433";
							id = Type::NAMCOT_3433;
						}
						else
						{
							name = "NAMCOT 3443";
							id = Type::NAMCOT_3443;
						}
						break;

					case 89:

						name = "SUNSOFT 2 (b)";
						id = Type::SUNSOFT_2B;
						break;

					case 90:

						name = "J.Y.COMPANY (a)";
						id = Type::JYCOMPANY_TYPE_A;
						break;

					case 91:

						name = "UNL MK2/SF3/SMKR";
						id = Type::UNL_MORTALKOMBAT2;
						break;

					case 92:

						name = "JALECO JF-21";
						id = Type::JALECO_JF21;
						break;

					case 93:

						name = "SUNSOFT 2 (a)";
						id = Type::SUNSOFT_2A;
						break;

					case 94:

						name = "UN1ROM";
						id = Type::STD_UN1ROM;
						break;

					case 95:

						name = "NAMCOT 3425";
						id = Type::NAMCOT_3425;
						break;

					case 96:

						name = "BANDAI OEKAKIDS";
						id = Type::BANDAI_OEKAKIDS;
						break;

					case 97:

						name = "IREM KAIKETSU";
						id = Type::IREM_KAIKETSU;
						break;

					case 99:

						name = "VS.SYSTEM";

						if (wram || useWramAuto)
						{
							wramAuto = useWramAuto;
							id = Type::CUSTOM_VSSYSTEM_1;
						}
						else
						{
							id = Type::CUSTOM_VSSYSTEM_0;
						}
						break;

					case 103:

						name = "BTL 2708";
						id = Type::BTL_2708;
						break;

					case 104:

						name = "BIC PEGASUS GOLDEN FIVE";
						id = Type::CAMERICA_GOLDENFIVE;
						break;

					case 105:

						name = "EVENT";
						id = Type::CUSTOM_EVENT;
						break;

					case 106:

						name = "BTL SMB3";
						id = Type::BTL_SMB3;
						break;

					case 107:

						name = "MAGICSERIES";
						id = Type::MAGICSERIES_MAGICDRAGON;
						break;

					case 108:

						name = "WHIRLWIND 2706";
						id = Type::WHIRLWIND_2706;
						break;

					case 112:

						name = "NTDEC / ASDER";

						if (wram || useWramAuto)
						{
							wramAuto = useWramAuto;
							id = Type::NTDEC_ASDER_1;
						}
						else
						{
							id = Type::NTDEC_ASDER_0;
						}
						break;

					case 113:

						name = "HES";
						id = Type::HES_STD;
						break;

					case 114:

						name = "SUPERGAME LIONKING";
						id = Type::SUPERGAME_LIONKING;
						break;

					case 115:

						name = "KASING";
						id = Type::KASING_STD;
						break;

					case 116:

						name = "SOMERITEAM SL-12";
						id = Type::SOMERITEAM_SL12;
						break;

					case 117:

						name = "FUTUREMEDIA";
						id = Type::FUTUREMEDIA_STD;
						break;

					case 118:

						if (wram || useWramAuto)
						{
							wramAuto = useWramAuto;
							name = "TKSROM";
							id = Type::STD_TKSROM;
						}
						else
						{
							name = "TLSROM";
							id = Type::STD_TLSROM;
						}
						break;

					case 119:

						name = "TQROM";
						id = Type::STD_TQROM;
						break;

					case 120:

						name = "BTL TOBIDASE DAISAKUSEN";
						id = Type::BTL_TOBIDASEDAISAKUSEN;
						break;

					case 121:

						name = "K PANDAPRINCE";
						id = Type::KAY_PANDAPRINCE;
						break;

					case 123:

						name = "K H2288";
						id = Type::KAY_H2288;
						break;

					case 126:

						name = "BMC POWERJOY 84-IN-1";
						id = Type::BMC_POWERJOY_84IN1;
						break;

					case 132:

						name = "TXC 22211 (a)";
						id = Type::TXC_22211A;
						break;

					case 133:

						name = "SACHEN SA72008";
						id = Type::SACHEN_SA72008;
						break;

					case 134:

						name = "BMC FAMILY 4646B";
						id = Type::BMC_FAMILY_4646B;
						break;

					case 136:

						name = "SACHEN TCU02";
						id = Type::SACHEN_TCU02;
						break;

					case 137:

						name = "SACHEN 8259D";
						id = Type::SACHEN_8259D;
						break;

					case 138:

						name = "SACHEN 8259B";
						id = Type::SACHEN_8259B;
						break;

					case 139:

						name = "SACHEN 8259C";
						id = Type::SACHEN_8259C;
						break;

					case 140:

						name = "JALECO JF-11/12/14";
						id = Type::JALECO_JF14;
						break;

					case 141:

						name = "SACHEN 8259A";
						id = Type::SACHEN_8259A;
						break;

					case 142:

						name = "KAISER KS-7032";
						id = Type::KAISER_KS7032;
						break;

					case 143:

						name = "SACHEN TCA01";
						id = Type::SACHEN_TCA01;
						break;

					case 144:

						name = "AGCI 50282";
						id = Type::AGCI_50282;
						break;

					case 145:

						name = "SACHEN SA72007";
						id = Type::SACHEN_SA72007;
						break;

					case 146:

						name = "SACHEN SA0161M";
						id = Type::SACHEN_SA0161M;
						break;

					case 147:

						name = "SACHEN TCU01";
						id = Type::SACHEN_TCU01;
						break;

					case 148:

						name = "AVE 74*161 / SACHEN SA0037";
						id = Type::SACHEN_SA0037;
						break;

					case 149:

						name = "SACHEN SA0036";
						id = Type::SACHEN_SA0036;
						break;

					case 150:

						name = "SACHEN 74*374 (b)";
						id = Type::SACHEN_74_374B;
						break;

					case 151:

						name = "KONAMI VS.SYSTEM";
						id = Type::KONAMI_VSSYSTEM;
						break;

					case 152:

						name = "74*161/161/32 (b)";
						id = Type::DISCRETE_74_161_161_32_B;
						break;

					case 154:

						name = "NAMCOT 108/109/118";
						id = Type::NAMCOT_34XX;
						break;

					case 156:

						name = "DAOU 306";
						id = Type::OPENCORP_DAOU306;
						break;

					case 157:

						name = "BANDAI DATACH JOINT SYSTEM";
						id = Type::BANDAI_DATACH;
						break;

					case 158:

						name = "TENGEN 800037";
						id = Type::TENGEN_800037;
						break;

					case 159:

						name = "BANDAI LZ93D50 +24C01";
						id = Type::BANDAI_LZ93D50_24C01;
						break;

					case 163:

						name = "NANJING";
						id = Type::NANJING_STD;
						break;

					case 164:

						name = "WAIXING FFV";

						if (wram || useWramAuto)
						{
							wramAuto = useWramAuto;
							id = Type::WAIXING_FFV_1;
						}
						else
						{
							id = Type::WAIXING_FFV_0;
						}
						break;

					case 165:

						name = "WAIXING SH2";

						if (wram || useWramAuto)
						{
							wramAuto = useWramAuto;
							id = Type::WAIXING_SH2_1;
						}
						else
						{
							id = Type::WAIXING_SH2_0;
						}
						break;

					case 166:

						name = "SUBOR (b)";
						id = Type::SUBOR_TYPE1;
						break;

					case 167:

						name = "SUBOR (a)";
						id = Type::SUBOR_TYPE0;
						break;

					case 170:

						name = "FUJIYA NROM +SECURITY";
						id = Type::FUJIYA_STD;
						break;

					case 171:

						name = "KAISER KS-7058";
						id = Type::KAISER_KS7058;
						break;

					case 172:

						name = "IDEA-TEK 22211";
						id = Type::TXC_22211B;
						break;

					case 173:

						name = "NEI-HU 22211";
						id = Type::TXC_22211C;
						break;

					case 175:

						name = "KAISER KS-7022";
						id = Type::KAISER_KS7022;
						break;

					case 176:

						name = "XIAO ZHUAN YUAN";
						id = Type::UNL_XZY;
						break;

					case 177:

						name = "HENGGEDIANZI";
						id = Type::HENGEDIANZI_STD;
						break;

					case 178:

						name = "WAIXING SAN GUO ZHONG LIE ZHUAN";
						id = Type::WAIXING_SGZLZ;
						break;

					case 179:

						name = "HENGGEDIANZI XING HE ZHAN SHI";
						id = Type::HENGEDIANZI_XJZB;
						break;

					case 180:

						name = "NIHON UNROM M5";
						id = Type::NIHON_UNROM_M5;
						break;

					case 182:

						name = "HOSENKAN ELECTRONICS";
						id = Type::HOSENKAN_STD;
						break;

					case 183:

						name = "BTL SHUI GUAN PIPE";
						id = Type::BTL_SHUIGUANPIPE;
						break;

					case 184:

						name = "SUNSOFT 1";
						id = Type::SUNSOFT_1;
						break;

					case 186:

						name = "SBX";
						id = Type::FUKUTAKE_SBX;
						break;

					case 187:

						name = "UNL SF3/KOF96";
						id = Type::UNL_KINGOFFIGHTERS96;
						break;

					case 188:

						name = "BANDAI KARAOKESTUDIO";
						id = Type::BANDAI_KARAOKESTUDIO;
						break;

					case 189:

						name = "YOKOSOFT / TXC";
						id = Type::TXC_TW;
						break;

					case 191:

						name = "WAIXING (b)";
						id = Type::WAIXING_TYPE_B;
						break;

					case 192:

						name = "WAIXING (c)";
						id = Type::WAIXING_TYPE_C;
						break;

					case 193:

						name = "NTDEC / MEGA SOFT";
						id = Type::NTDEC_FIGHTINGHERO;
						break;

					case 194:

						name = "WAIXING (d)";
						id = Type::WAIXING_TYPE_D;
						break;

					case 195:

						name = "WAIXING (e)";
						id = Type::WAIXING_TYPE_E;
						break;

					case 196:

						name = "BTL SUPER BROS 11";
						id = Type::BTL_SUPERBROS11;
						break;

					case 197:

						name = "HD-1047-A/AX5202P";
						id = Type::UNL_SUPERFIGHTER3;
						break;

					case 198:

						name = "WAIXING (f)";
						id = Type::WAIXING_TYPE_F;
						break;

					case 199:

						name = "WAIXING (g)";
						id = Type::WAIXING_TYPE_G;
						break;

					case 200:

						name = "BMC 1200/36-IN-1";
						id = Type::BMC_36IN1;
						break;

					case 201:

						name = "BMC 21/8-IN-1";
						id = Type::BMC_21IN1;
						break;

					case 202:

						name = "BMC 150-IN-1";
						id = Type::BMC_150IN1;
						break;

					case 203:

						name = "BMC 35-IN-1";
						id = Type::BMC_35IN1;
						break;

					case 204:

						name = "BMC 64-IN-1";
						id = Type::BMC_64IN1;
						break;

					case 205:

						name = "BMC 15/3-IN-1";
						id = Type::BMC_15IN1;
						break;

					case 206:

						if (nmt == Type::NMT_FOURSCREEN)
						{
							name = "DRROM";
							id = Type::STD_DRROM;
						}
						else if (prg >= SIZE_128K)
						{
							name = "DE1ROM / NAMCOT 34xx";
							id = Type::STD_DE1ROM;
						}
						else
						{
							name = "DEROM / TENGEN MIMIC-1 / NAMCOT 34xx";
							id = Type::STD_DEROM;
						}
						break;

					case 207:
					{
						Chips::Type& chip = chips.Add(L"X1-005");

						chip.Pin(17) = L"CIRAM A10";
						chip.Pin(31) = L"NC";

						name = "TAITO X1-005 (b)";
						id = Type::TAITO_X1005;
						break;
					}

					case 208:

						name = "GOUDER 37017";
						id = Type::GOUDER_37017;
						break;

					case 209:

						name = "J.Y.COMPANY (b)";
						id = Type::JYCOMPANY_TYPE_B;
						break;

					case 211:

						name = "J.Y.COMPANY (c)";
						id = Type::JYCOMPANY_TYPE_C;
						break;

					case 212:

						name = "BMC SUPERHIK 300-IN-1";
						id = Type::BMC_SUPERHIK_300IN1;
						break;

					case 213:

						name = "BMC 9999999-IN-1";
						id = Type::BMC_9999999IN1;
						break;

					case 214:

						name = "BMC SUPERGUN 20-IN-1";
						id = Type::BMC_SUPERGUN_20IN1;
						break;

					case 215:

						if (prg == SIZE_256K && chr == SIZE_512K)
						{
							name = "SUPERGAME MK3E";
							id = Type::SUPERGAME_MK3E;
						}
						else
						{
							name = "SUPERGAME BOOGERMAN";
							id = Type::SUPERGAME_BOOGERMAN;
						}
						break;

					case 216:

						name = "RCM GS-2015";
						id = Type::RCM_GS2015;
						break;

					case 217:

						name = "BMC SPC009";
						id = Type::BMC_GOLDENCARD_6IN1;
						break;

					case 219:

						name = "UNL A9746";
						id = Type::UNL_A9746;
						break;

					case 221:

						name = "UNL N625092";
						id = Type::UNL_N625092;
						break;

					case 222:

						name = "BTL DRAGON NINJA";
						id = Type::BTL_DRAGONNINJA;
						break;

					case 223:

						name = "WAIXING TANG MU LI XIAN JI";
						id = Type::WAIXING_TYPE_I;
						break;

					case 224:

						name = "WAIXING YING XIONG CHUAN QI";
						id = Type::WAIXING_TYPE_J;
						break;

					case 225:

						name = "BMC 58/64/72-IN-1";
						id = Type::BMC_72IN1;
						break;

					case 226:

						if (prg == SIZE_1024K)
						{
							name = "BMC SUPER 42-IN-1";
							id = Type::BMC_SUPER_42IN1;
						}
						else
						{
							name = "BMC 76-IN-1";
							id = Type::BMC_76IN1;
						}
						break;

					case 227:

						name = "BMC 1200-IN-1";
						id = Type::BMC_1200IN1;
						break;

					case 228:

						name = "ACTIVE ENTERTAINMENT";
						id = Type::AE_STD;
						break;

					case 229:

						name = "BMC 31-IN-1";
						id = Type::BMC_31IN1;
						break;

					case 230:

						name = "BMC SUPER 22 GAMES / 20-IN-1";
						id = Type::BMC_22GAMES;
						break;

					case 231:

						name = "BMC 20-IN-1";
						id = Type::BMC_20IN1;
						break;

					case 232:

						name = "CAMERICA BF9096";
						id = Type::CAMERICA_BF9096;
						break;

					case 233:

						name = "BMC SUPER 22 GAMES";
						id = Type::BMC_SUPER_22GAMES;
						break;

					case 234:

						name = "AVE D-1012";
						id = Type::AVE_D1012;
						break;

					case 235:

						if (prg <= SIZE_2048K)
						{
							name = "GOLDENGAME 150-IN-1";
							id = Type::BMC_GOLDENGAME_150IN1;
						}
						else
						{
							name = "GOLDENGAME 260-IN-1";
							id = Type::BMC_GOLDENGAME_260IN1;
						}
						break;

					case 236:

						name = "BMC 70/800-IN-1";
						id = Type::BMC_GAME_800IN1;
						break;

					case 240:

						name = "CNE SHLZ";
						id = Type::CNE_SHLZ;
						break;

					case 241:

						name = "MXMDHTWO / TXC";
						id = Type::TXC_MXMDHTWO;
						break;

					case 242:

						if (nmt == Type::NMT_CONTROLLED)
						{
							name = "WAIXING ZS";
							id = Type::WAIXING_ZS;
						}
						else
						{
							name = "WAIXING DQVII";
							id = Type::WAIXING_DQVII;
						}
						break;

					case 243:

						name = "SACHEN 74*374 (a)";
						id = Type::SACHEN_74_374A;
						break;

					case 244:

						name = "CNE DECATHLON";
						id = Type::CNE_DECATHLON;
						break;

					case 245:

						name = "WAIXING (h)";
						id = Type::WAIXING_TYPE_H;
						break;

					case 246:

						name = "CNE PHONE SERM BERM";
						id = Type::CNE_PSB;
						break;

					case 249:

						name = "WAIXING +SECURITY";

						if (wram || useWramAuto)
						{
							wramAuto = useWramAuto;
							id = Type::WAIXING_SECURITY_1;
						}
						else
						{
							id = Type::WAIXING_SECURITY_0;
						}
						break;

					case 250:

						name = "NITRA";
						id = Type::NITRA_TDA;
						break;

					case 252:

						name = "WAIXING SAN GUO ZHI";
						id = Type::WAIXING_SGZ;
						break;

					case 254:

						name = "BTL PIKACHU Y2K";
						id = Type::BTL_PIKACHUY2K;
						break;

					case 255:

						name = "BMC 110/115-IN-1";
						id = Type::BMC_110IN1;
						break;

					default:

						return false;
				}

				type = Type( id, this->prg, this->chr, nmt, wramBattery || mmcBattery, wramAuto );
#endif
static char *getboardname(rom_t *rom)
{
	static char ret[32];

	strcpy(ret,"?");
	switch(rom->mapper) {
		case 0:
			if(rom->chrsize) {
				if(rom->prgsize == KB(16))
					strcpy(ret,"NES-NROM-128");
				else
					strcpy(ret,"NES-NROM-256");
			}
			else
				log_error("mapper 0: rom must have chr rom\n");
		case 2:
			if(rom->prgsize == KB(128))
				strcpy(ret,"NES-UNROM");
			else
				strcpy(ret,"NES-UOROM");
			break;
		case 3: strcpy(ret,"NES-CNROM"); break;
		case 4:
			if(rom->mirroring & 4) {
				if(rom->prgsize == KB(64))
					strcpy(ret,"NES-TVROM");
				else if(rom->prgsize >= KB(128) && rom->prgsize <= KB(512))
					strcpy(ret,"NES-TR1ROM");
				else {
					strcpy(ret,"NES-TR1ROM");
					log_warning("mapper 4: bad prg size, defaulting to TR1ROM\n");
				}
			}
			else {
				if(rom->prgsize == KB(32))
					strcpy(ret,"NES-TEROM");
				if(rom->prgsize == KB(64))
					strcpy(ret,"NES-TBROM");
				else if(rom->prgsize >= KB(128) && rom->prgsize <= KB(512)) {
					if(rom->chrsize == 0) {
						if(rom->sramsize == 0)
							strcpy(ret,"NES-TGROM");
						else
							strcpy(ret,"NES-TNROM");
					}
					else if(rom->chrsize <= KB(64))
						strcpy(ret,"NES-TFROM");
					else if(rom->chrsize <= KB(256)) {
						if(rom->sramsize == 0)
							strcpy(ret,"NES-TLROM");
						else
							strcpy(ret,"NES-TSROM");
					}
					else {
						strcpy(ret,"NES-TSROM");
						log_error("mapper 7: bad chr size, defaulting to TSROM\n");
					}
				}
				else {
					strcpy(ret,"NES-TSROM");
					log_error("mapper 7: bad prg size, defaulting to TSROM\n");
				}
			}
			break;
		//mapper 6 has no boards (ffe)
		case 7:
			if(rom->chrsize == 0) {
				if(rom->prgsize == KB(256))
					strcpy(ret,"NES-AOROM");
				else if(rom->prgsize == KB(128))
					strcpy(ret,"NES-ANROM");
				else if(rom->prgsize == KB(64))
					strcpy(ret,"NES-AN1ROM");
				else {
					strcpy(ret,"NES-AOROM");
					log_warning("mapper 7: bad prg size, defaulting to AOROM\n");
				}
			}
			else
				log_error("mapper 7: AxROM boards always have chrrom\n");
			break;
		//mapper 8 has no boards (ffe)
		case 9: strcpy(ret,"NES-PNROM"); break;
		case 10:
			if(rom->prgsize == KB(128))
				strcpy(ret,"NES-FJROM"); 
			else
				strcpy(ret,"NES-FKROM"); 
			break;
		case 11: strcpy(ret,"COLORDREAMS-74*377"); break;
	}
	return(ret);
}

rom_t *rom_load_ines(int fd,rom_t *ret)
{
	u8 header[16];

	//read header (again...)
	file_read(fd,header,16);

	//load ines header
	load_ines_header(ret,header);

	//allocate memory for the prg rom
	ret->prg = (u8*)malloc(ret->prgsize);

	//read in the prg rom
	if(file_read(fd,ret->prg,ret->prgsize) != ret->prgsize) {
		log_message("rom_load: error reading prg data\n");
		rom_unload(ret);
		return(0);
	}

	//if chr exists...
	if(ret->chrsize) {

		//allocate memory for chr rom
		ret->chr = (u8*)malloc(ret->chrsize);

		//read in the chr rom data
		if(file_read(fd,ret->chr,ret->chrsize) != ret->chrsize) {
			log_message("rom_load: error reading chr data\n");
			rom_unload(ret);
			return(0);
		}

		//allocate memory for the tile cache
		ret->cache = (cache_t*)malloc(ret->chrsize);
		ret->cache_hflip = (cache_t*)malloc(ret->chrsize);

		//convert all chr tiles to cache tiles
		convert_tiles(ret->chr,ret->cache,ret->chrsize / 16,0);
		convert_tiles(ret->chr,ret->cache_hflip,ret->chrsize / 16,1);
	}

	//if no chr exists, go ahead and set default vram size of 8kb
	else
		rom_setvramsize(ret,1);

	//look for rom in database and update its mapper info
	if(rom_checkdb(ret,0) == 0) {
		int fd;
		char str[512];
#ifdef WIN32
#define NEW_CRCS "c:\\new_crcs.c"
#else
#define NEW_CRCS "new_crcs.c"
#endif
		if((fd = file_open(NEW_CRCS,"uat")) >= 0) {
			sprintf(str,"\t{\"%s\",0x%08X,0x%08X,\"%s\",%s},\n",ret->filename,rom_prgcrc32(ret),rom_chrcrc32(ret),getboardname(ret),((ret->chrsize == 0) ? "ROM_NOCHR" : "0"));
			file_write(fd,str,strlen(str));
			file_close(fd);
		}
	}

	return(ret);
}
