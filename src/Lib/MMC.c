/***************************************************************************
**                                                                        **
**  FlySight firmware                                                     **
**  Copyright 2018 Michael Cooper                                         **
**  MMC interface copyright 2013 Dean Camera                              **
**                                                                        **
**  This program is free software: you can redistribute it and/or modify  **
**  it under the terms of the GNU General Public License as published by  **
**  the Free Software Foundation, either version 3 of the License, or     **
**  (at your option) any later version.                                   **
**                                                                        **
**  This program is distributed in the hope that it will be useful,       **
**  but WITHOUT ANY WARRANTY; without even the implied warranty of        **
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         **
**  GNU General Public License for more details.                          **
**                                                                        **
**  You should have received a copy of the GNU General Public License     **
**  along with this program.  If not, see <http://www.gnu.org/licenses/>. **
**                                                                        **
****************************************************************************
**  Contact: Michael Cooper                                               **
**  Website: http://flysight.ca/                                          **
****************************************************************************/

#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>

#include "../../vendor/FatFS/diskio.h"
#include "../../vendor/FatFS/ff.h"
#include "Main.h"
#include "MMC.h"

static uint32_t MMC_mediaBlocks;

ISR(TIMER0_COMPA_vect)
{
	disk_timerproc();
}

uint8_t MMC_Init(void)
{
	DSTATUS stat;

	TCCR0A |= (1 << WGM01);
	TCCR0B |= (1 << CS02) | (1 << CS00);
	OCR0A   = 78;
	TIMSK0 |= (1 << OCIE0A);

	sei();

	DDRB  |= (1 << 2) | (1 << 1) | (1 << 0);
	PORTB |= (1 << 3) | (1 << 2) | (1 << 0);

	SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR1);
	SPSR = (1 << SPI2X);
	
	stat = disk_initialize(0);
	if (stat & STA_NOINIT)
	{
		return 0;
	}
	else
	{
		disk_ioctl(0, GET_SECTOR_COUNT, &MMC_mediaBlocks);
		return 1;
	}
}

void MMC_WriteBlocks(
	USB_ClassInfo_MS_Device_t *MSInterfaceInfo, 
	uint32_t                  BlockAddress, 
	uint16_t                  TotalBlocks)
{
	uint8_t  CurrDFPageByteDiv16 = 0;

	/* Wait until endpoint is ready before continuing */
	if (Endpoint_WaitUntilReady()) return;

	while (TotalBlocks)
	{
		uint8_t BytesInBlockDiv16 = 0;
		uint8_t *buf = Main_buffer;
		
		/* Write an endpoint packet sized data block to the dataflash */
		while (BytesInBlockDiv16 < (VIRTUAL_MEMORY_BLOCK_SIZE >> 4))
		{
			/* Check if the endpoint is currently empty */
			if (!(Endpoint_IsReadWriteAllowed()))
			{
				/* Clear the current endpoint bank */
				Endpoint_ClearOUT();
				
				/* Wait until the host has sent another packet */
				if (Endpoint_WaitUntilReady()) return;
			}

			/* Write one 16-byte chunk of data to the dataflash */
			*(buf++) = Endpoint_Read_8();
			*(buf++) = Endpoint_Read_8();
			*(buf++) = Endpoint_Read_8();
			*(buf++) = Endpoint_Read_8();
			*(buf++) = Endpoint_Read_8();
			*(buf++) = Endpoint_Read_8();
			*(buf++) = Endpoint_Read_8();
			*(buf++) = Endpoint_Read_8();
			*(buf++) = Endpoint_Read_8();
			*(buf++) = Endpoint_Read_8();
			*(buf++) = Endpoint_Read_8();
			*(buf++) = Endpoint_Read_8();
			*(buf++) = Endpoint_Read_8();
			*(buf++) = Endpoint_Read_8();
			*(buf++) = Endpoint_Read_8();
			*(buf++) = Endpoint_Read_8();
			
			/* Increment the dataflash page 16 byte block counter */
			CurrDFPageByteDiv16++;

			/* Increment the block 16 byte block counter */
			BytesInBlockDiv16++;

			/* Check if the current command is being aborted by the host */
			if (MSInterfaceInfo->State.IsMassStoreReset) return;			
		}

		/* Write a single sector */
		disk_write(0, Main_buffer, BlockAddress, 1);
			
		/* Increment the block address */
		BlockAddress++;
			
		/* Decrement the blocks remaining counter and reset the sub block counter */
		TotalBlocks--;
	}

	/* If the endpoint is empty, clear it ready for the next packet from the host */
	if (!(Endpoint_IsReadWriteAllowed()))
	{
		Endpoint_ClearOUT();
	}
}

void MMC_ReadBlocks(
	USB_ClassInfo_MS_Device_t *MSInterfaceInfo, 
	uint32_t                  BlockAddress, 
	uint16_t                  TotalBlocks)
{
	uint8_t  CurrDFPageByteDiv16 = 0;

	/* Wait until endpoint is ready before continuing */
	if (Endpoint_WaitUntilReady()) return;
	
	while (TotalBlocks)
	{
		uint8_t BytesInBlockDiv16 = 0;
		uint8_t *buf = Main_buffer;
		
		/* read a single sector */
		disk_read(0, Main_buffer, BlockAddress, 1);
			
		/* Increment the block address */
		BlockAddress++;
			
		/* Write an endpoint packet sized data block to the dataflash */
		while (BytesInBlockDiv16 < (VIRTUAL_MEMORY_BLOCK_SIZE >> 4))
		{
			/* Check if the endpoint is currently full */
			if (!(Endpoint_IsReadWriteAllowed()))
			{
				/* Clear the endpoint bank to send its contents to the host */
				Endpoint_ClearIN();
				
				/* Wait until the endpoint is ready for more data */
				if (Endpoint_WaitUntilReady()) return;
			}

			/* Read one 16-byte chunk of data from the dataflash */
			Endpoint_Write_8(*(buf++));
			Endpoint_Write_8(*(buf++));
			Endpoint_Write_8(*(buf++));
			Endpoint_Write_8(*(buf++));
			Endpoint_Write_8(*(buf++));
			Endpoint_Write_8(*(buf++));
			Endpoint_Write_8(*(buf++));
			Endpoint_Write_8(*(buf++));
			Endpoint_Write_8(*(buf++));
			Endpoint_Write_8(*(buf++));
			Endpoint_Write_8(*(buf++));
			Endpoint_Write_8(*(buf++));
			Endpoint_Write_8(*(buf++));
			Endpoint_Write_8(*(buf++));
			Endpoint_Write_8(*(buf++));
			Endpoint_Write_8(*(buf++));
			
			/* Increment the dataflash page 16 byte block counter */
			CurrDFPageByteDiv16++;
			
			/* Increment the block 16 byte block counter */
			BytesInBlockDiv16++;

			/* Check if the current command is being aborted by the host */
			if (MSInterfaceInfo->State.IsMassStoreReset) return;
		}
		
		/* Decrement the blocks remaining counter */
		TotalBlocks--;
	}
	
	/* If the endpoint is full, send its contents to the host */
	if (!(Endpoint_IsReadWriteAllowed()))
	{
		Endpoint_ClearIN();
	}
}

bool MMC_CheckDataflashOperation(void)
{
	return true;
}

uint32_t MMC_MediaBlocks(void)
{
	return MMC_mediaBlocks;
}
