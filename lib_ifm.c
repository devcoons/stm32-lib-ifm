/*!
	@file   lib_ifm.c
	@brief  <brief description here>
	@t.odo	-
	---------------------------------------------------------------------------
	
	MIT License
	
	Copyright (c) 2021 Ioannis Deligiannis
	
	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:
	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.
	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.
*/
/******************************************************************************
* Preprocessor Definitions & Macros
******************************************************************************/

/******************************************************************************
* Includes
******************************************************************************/

#include "lib_ifm.h"

/******************************************************************************
* Enumerations, structures & Variables
******************************************************************************/
#ifdef LIB_IFM_ENABLED

#if defined(IFM_AS_BOOTLOADER) || defined(IFM_AS_APPLICATION)

const uint8_t SOFTINFO[64] __attribute__ ((section (".flag"))) __attribute__ ((__used__)) =
{
		(IFM_SOFTINFO_CRC & 0xFF00) >> 8,
		(IFM_SOFTINFO_CRC & 0x00FF) >> 0,
		IFM_SOFTINFO_SI,
		IFM_SOFTINFO_SISZ,
		IFM_SOFTINFO_SIDT0,
		IFM_SOFTINFO_SIDT1,
		IFM_SOFTINFO_SIDT2,
		IFM_SOFTINFO_SIDT3,
		IFM_SOFTINFO_SIDT4,
		IFM_SOFTINFO_SIDT5,
		IFM_SOFTINFO_SIDT6,
		IFM_SOFTINFO_SIDT7,
		IFM_SOFTINFO_SIDT8,
		IFM_SOFTINFO_SIDT9,
		IFM_SOFTINFO_SIDT10,
		IFM_SOFTINFO_SIDT11,
		IFM_SOFTINFO_SIDT12,
		IFM_SOFTINFO_SIDT13,
		#if defined(IFM_AS_BOOTLOADER)
		IFM_SOFTINFO_CA,
		IFM_SOFTINFO_CASZ,
		IFM_SOFTINFO_CADT0,
		IFM_SOFTINFO_CADT1,
		IFM_SOFTINFO_CADT2,
		IFM_SOFTINFO_CADT3,
		IFM_SOFTINFO_CADT4,
		IFM_SOFTINFO_CADT5,
		IFM_SOFTINFO_CADT6,
		IFM_SOFTINFO_CADT7,
		IFM_SOFTINFO_CADT8,
		IFM_SOFTINFO_CADT9,
		#else
		0,
		10,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		#endif
		IFM_SOFTINFO_UV,
		IFM_SOFTINFO_UVSZ,
		IFM_SOFTINFO_UVDT0,
		IFM_SOFTINFO_UVDT1,
		IFM_SOFTINFO_UVDT2,
		IFM_SOFTINFO_UVDT3,
		IFM_SOFTINFO_CB,
		IFM_SOFTINFO_CBSZ,
		IFM_SOFTINFO_CBDT0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		'I',
		'O',
		'D'
};

#endif

/******************************************************************************
* Declaration | Static Functions
******************************************************************************/

/******************************************************************************
* Definition  | Static Functions
******************************************************************************/

/******************************************************************************
* Definition  | Public Functions
******************************************************************************/

i_status ifm_area_is_valid(ifm_area area)
{
#if defined(IFM_AS_BOOTLOADER) || defined(IFM_AS_APPLICATION)

	if(area != IFM_BOOT_SOFTINFO && area != IFM_APP_SOFTINFO && area != IFM_DEVINFO && area != IFM_APPINFO)
		return I_NOTEXISTS;

	uint32_t area_sz = 0;
	uint32_t area_address = 0;

	switch(area)
	{
	case IFM_BOOT_SOFTINFO:
		area_address = IFMBOOT_SOFTINFO;
		area_sz = 64;
		break;
	case IFM_APP_SOFTINFO:
		area_address = IFMAPP_SOFTINFO;
		area_sz = 64;
		break;
	case IFM_DEVINFO:
		area_address = IFMDEVINFO;
		area_sz = 256;
		break;
	case IFM_APPINFO:
		area_address = IFMAPPINFO;
		area_sz = 32;
		break;
	default:
		area_address = 0;
		area_sz = 0;
	}
#if defined(HAL_ICACHE_MODULE_ENABLED)
	HAL_ICACHE_Invalidate();
#endif

	uint16_t calc_crc = 0xFFFF;
	uint16_t sect_crc = (*((volatile uint8_t*)area_address)) << 8 | (*((volatile uint8_t*)area_address+1)) << 0;

	for(register int i = 0; i < (area_sz-2); i++)
		calc_crc = crc16_ccitt(calc_crc, (uint8_t*)(2+area_address+i), 1);

	if(calc_crc != sect_crc)
		return I_INVALID;

	return I_OK;
#else
	return I_NOTIMPLEMENTED;
#endif
}

i_status ifm_invalidate_application_upgrade_info()
{
	uint8_t data[16] = {0xFF};

	return flash_write(0x8020000, data, 16);
}

i_status ifm_retrieve(ifm_field field, uint8_t* data, uint8_t* data_sz)
{
#if defined(IFM_AS_BOOTLOADER) || defined(IFM_AS_APPLICATION)
#if defined(HAL_ICACHE_MODULE_ENABLED)
	HAL_ICACHE_Invalidate();
#endif

	*data_sz = 0;

	uint8_t area = field & 0xF0;

	uint32_t area_sz = 0;
	uint32_t area_address = 0;

	switch(area)
	{
	case IFM_BOOT_SOFTINFO:
		area_address = IFMBOOT_SOFTINFO;
		area_sz = 64;
		break;
	case IFM_APP_SOFTINFO:
		area_address = IFMAPP_SOFTINFO;
		area_sz = 64;
		break;
	case IFM_DEVINFO:
		area_address = IFMDEVINFO;
		area_sz = 256;
		break;
	case IFM_APPINFO:
		area_address = IFMAPPINFO;
		area_sz = 32;
		break;
	default:
		area_address = 0;
		area_sz = 0;
	}

	uint16_t current_pos = 2;

	while(current_pos <area_sz)
	{
		uint8_t cur_field = (*((volatile uint8_t*)(area_address+current_pos)));
		uint8_t cur_field_sz = (*((volatile uint8_t*)(area_address+current_pos+1)));
		current_pos+=2;

		if(cur_field == field)
		{
			for(int i = 0;i<cur_field_sz;i++)
				data[i] = (*((volatile uint8_t*)(area_address+current_pos+i)));
			*data_sz = cur_field_sz;
			return I_OK;
		}

		current_pos+=cur_field_sz;
	}
	return I_NOTEXISTS;
#else
	return I_NOTIMPLEMENTED;
#endif
}
/******************************************************************************
* EOF - NO CODE AFTER THIS LINE
******************************************************************************/
#endif
