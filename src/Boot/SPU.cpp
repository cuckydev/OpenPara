/*
	[ OpenPara ]
	Copyright Regan "CKDEV" Green 2022
	
	- SPU.h -
	PSX SPU
*/

#include "Boot/SPU.h"

namespace OpenPara
{
	namespace SPU
	{
		// SPU functions
		void Init()
		{
			// Initialize SPU
			::SpuInit();

			// Set SPU common attributes
			::SpuCommonAttr spu_common_attr;
			spu_common_attr.mask =
				SPU_COMMON_MVOLL | SPU_COMMON_MVOLR |
				SPU_COMMON_CDMIX | SPU_COMMON_CDREV | SPU_COMMON_CDVOLL | SPU_COMMON_CDVOLR |
				SPU_COMMON_EXTMIX | SPU_COMMON_EXTREV | SPU_COMMON_EXTVOLL | SPU_COMMON_EXTVOLR;
			spu_common_attr.mvol.left = 0x3FFF;
			spu_common_attr.mvol.right = 0x3FFF;
			spu_common_attr.cd.mix = SPU_ON;
			spu_common_attr.cd.reverb = SPU_OFF;
			spu_common_attr.cd.volume.left = 0x3FFF;
			spu_common_attr.cd.volume.right = 0x3FFF;
			spu_common_attr.ext.mix = SPU_OFF;
			spu_common_attr.ext.reverb = SPU_OFF;
			spu_common_attr.ext.volume.left = 0;
			spu_common_attr.ext.volume.right = 0;

			::SpuSetCommonAttr(&spu_common_attr);
		}
	}
}
