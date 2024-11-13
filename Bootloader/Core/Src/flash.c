#include "flash.h"

/* Function pointer for jumping to user application. */
typedef void (*fnc_ptr)(void);

static uint32_t GetSector(uint32_t address);

/**
 * @brief   This function erases the flash memory sector based on the address.
 * @param   address: First address in the sector to be erased.
 * @return  flash_status: Status of the erase operation.
 */
flash_status flash_erase(uint32_t address) {
    HAL_StatusTypeDef hal_status;

    // Unlock the Flash memory for write operations
    HAL_FLASH_Unlock();

    // Determine the sector to erase based on the address
    FLASH_EraseInitTypeDef erase_init = {
        .TypeErase = FLASH_TYPEERASE_SECTORS,
        .Sector = GetSector(address),
        .Banks = FLASH_BANK_1,
        .NbSectors = 1
    };

    // Perform the erase operation
    hal_status = HAL_FLASHEx_Erase(&erase_init, NULL);

    // Lock the Flash memory
    HAL_FLASH_Lock();

    // Handle the result
    if (hal_status == HAL_OK) {
        return FLASH_OK;
    } else {
        return FLASH_ERROR;
    }
}

/**
 * @brief   This function writes data to flash memory.
 * @param   address: Address to start writing to.
 * @param   *data:   Pointer to the data array to write.
 * @param   length:  Number of 32-bit words to write.
 * @return  flash_status: Status of the write operation.
 */
flash_status flash_write(uint32_t address, uint32_t *data, uint32_t length)
{
    flash_status status = FLASH_OK;

    // Unlock the Flash memory for write operations
    HAL_FLASH_Unlock();

    // Loop through the array and write each word
    for (uint32_t i = 0u; (i < length) && (FLASH_OK == status); i++)
    {
        // Program the flash memory word by word
        if (HAL_OK != HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, address, data[i]))
        {
            status |= FLASH_ERROR_WRITE;
        }

        // Read back the content of the memory to confirm
        if (data[i] != (*(volatile uint32_t*)address))
        {
            status |= FLASH_ERROR_READBACK;
        }

        // Move to the next word
        address += 4u;
    }

    // Lock the Flash memory
    HAL_FLASH_Lock();

    return status;
}

/**
 * @brief   Jump to the user application in flash memory.
 */
void flash_jump_to_app(void)
{
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);

    /* Function pointer to the address of the user application. */
    fnc_ptr jump_to_app;
    jump_to_app = (fnc_ptr)(*(volatile uint32_t*) (FLASH_APP_START_ADDRESS + 4u));

    // De-initialize HAL and reset peripherals
    HAL_DeInit();

    // Set the main stack pointer
    __set_MSP(*(volatile uint32_t*)FLASH_APP_START_ADDRESS);
    jump_to_app();
}

/**
 * @brief   Determine the flash memory sector for a given address.
 * @param   address: Flash memory address.
 * @return  uint32_t: Sector number corresponding to the address.
 */
static uint32_t GetSector(uint32_t address) {
    if (address >= 0x08000000 && address < 0x08004000) {
        return FLASH_SECTOR_0;
    } else if (address >= 0x08004000 && address < 0x08008000) {
        return FLASH_SECTOR_1;
    } else if (address >= 0x08008000 && address < 0x0800C000) {
        return FLASH_SECTOR_2;
    } else if (address >= 0x0800C000 && address < 0x08010000) {
        return FLASH_SECTOR_3;
    } else if (address >= 0x08010000 && address < 0x08020000) {
        return FLASH_SECTOR_4;
    } else if (address >= 0x08020000 && address < 0x08040000) {
        return FLASH_SECTOR_5;
    } else if (address >= 0x08040000 && address < 0x08060000) {
        return FLASH_SECTOR_6;
    } else if (address >= 0x08060000 && address < 0x08080000) {
        return FLASH_SECTOR_7;
    } else if (address >= 0x08080000 && address < 0x080A0000) {
        return FLASH_SECTOR_8;
    } else if (address >= 0x080A0000 && address < 0x080C0000) {
        return FLASH_SECTOR_9;
    } else if (address >= 0x080C0000 && address < 0x080E0000) {
        return FLASH_SECTOR_10;
    } else if (address >= 0x080E0000 && address < 0x08100000) {
        return FLASH_SECTOR_11;
    } else {
        return FLASH_SECTOR_11; // Default to last sector if address is out of range
    }
}

