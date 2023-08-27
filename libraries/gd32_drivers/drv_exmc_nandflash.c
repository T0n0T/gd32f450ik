#include "gd32f4xx.h"
#include "drv_exmc_nandflash.h"
#include <rtthread.h>

#define delay_1ms(count) rt_thread_mdelay(count)
/* defines the physical address of nand flash, and it is determined by the hardware */
#define BANK1_NAND_ADDR ((uint32_t)0x70000000)
#define BANK_NAND_ADDR  BANK1_NAND_ADDR

/* define operating nand flash macro */
#define NAND_CMD_AREA  *(__IO uint8_t *)(BANK_NAND_ADDR | EXMC_CMD_AREA)
#define NAND_ADDR_AREA *(__IO uint8_t *)(BANK_NAND_ADDR | EXMC_ADDR_AREA)
#define NAND_DATA_AREA *(__IO uint8_t *)(BANK_NAND_ADDR | EXMC_DATA_AREA)

/* the macro of calculate nand flash operating address */
#define ROW_ADDRESS (address.page + (address.block + (address.zone * NAND_ZONE_SIZE)) * NAND_BLOCK_SIZE)

/* page bit count per block */
#define PAGE_BIT 6

/* function prototypes */
static uint8_t exmc_nand_getstatus(void);
static uint8_t exmc_nand_writedata(uint8_t *pbuffer, nand_address_struct physicaladdress, uint16_t bytecount);
static uint8_t exmc_nand_readdata(uint8_t *pbuffer, nand_address_struct phyaddress, uint16_t bytecount);
static uint8_t exmc_nand_writepage(uint8_t *pbuffer, nand_address_struct address, uint16_t bytecount);
static uint8_t exmc_nand_readpage(uint8_t *pbuffer, nand_address_struct address, uint16_t bytecount);
static uint8_t exmc_nand_eraseblock(uint32_t blocknum);
static uint8_t exmc_nand_readstatus(void);

/*!
    \brief      nand flash peripheral initialize
    \param[in]  nand_bank: specify the NAND bank
    \param[out] none
    \retval     none
*/
void exmc_nandflash_init(uint32_t nand_bank)
{
    exmc_nand_parameter_struct nand_init_struct;
    exmc_nand_pccard_timing_parameter_struct nand_timing_init_struct;

    /* enable EXMC clock*/
    rcu_periph_clock_enable(RCU_EXMC);
    rcu_periph_clock_enable(RCU_GPIOD);
    rcu_periph_clock_enable(RCU_GPIOE);
    rcu_periph_clock_enable(RCU_GPIOF);
    rcu_periph_clock_enable(RCU_GPIOG);

    /* common GPIO configuration */
    /* D2(PD0),D3(PD1),D0(PD14),D1(PD15) pin configuration */
    gpio_af_set(GPIOD, GPIO_AF_12, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_14 | GPIO_PIN_15);
    gpio_mode_set(GPIOD, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_14 | GPIO_PIN_15);
    gpio_output_options_set(GPIOD, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_14 | GPIO_PIN_15);

    /* D4(PE7),D5(PE8),D6(PE9),D7(PE10) pin configuration */
    gpio_af_set(GPIOE, GPIO_AF_12, GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10);
    gpio_mode_set(GPIOE, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10);
    gpio_output_options_set(GPIOE, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10);

    /* CLE(PD11),ALE(PD12) pin configuration */
    gpio_af_set(GPIOD, GPIO_AF_12, GPIO_PIN_11 | GPIO_PIN_12);
    gpio_mode_set(GPIOD, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_11 | GPIO_PIN_12);
    gpio_output_options_set(GPIOD, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_11 | GPIO_PIN_12);

    /* NOE(PD4),NWE(PD5) pin configuration */
    gpio_af_set(GPIOD, GPIO_AF_12, GPIO_PIN_4 | GPIO_PIN_5);
    gpio_mode_set(GPIOD, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_4 | GPIO_PIN_5);
    gpio_output_options_set(GPIOD, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_4 | GPIO_PIN_5);

    /* NWAIT(PD6) pin configuration */
    gpio_mode_set(GPIOD, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, GPIO_PIN_6);

    /* NCE1(PD7) pin configuration */
    gpio_af_set(GPIOD, GPIO_AF_12, GPIO_PIN_7);
    gpio_mode_set(GPIOD, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_7);
    gpio_output_options_set(GPIOD, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_7);

    /* EXMC configuration */
    nand_timing_init_struct.setuptime       = 6;
    nand_timing_init_struct.waittime        = 5;
    nand_timing_init_struct.holdtime        = 3;
    nand_timing_init_struct.databus_hiztime = 2;

    nand_init_struct.nand_bank              = EXMC_BANK1_NAND;
    nand_init_struct.ecc_size               = EXMC_ECC_SIZE_2048BYTES;
    nand_init_struct.atr_latency            = EXMC_ALE_RE_DELAY_2_HCLK;
    nand_init_struct.ctr_latency            = EXMC_CLE_RE_DELAY_2_HCLK;
    nand_init_struct.ecc_logic              = ENABLE;
    nand_init_struct.databus_width          = EXMC_NAND_DATABUS_WIDTH_8B;
    nand_init_struct.wait_feature           = ENABLE;
    nand_init_struct.common_space_timing    = &nand_timing_init_struct;
    nand_init_struct.attribute_space_timing = &nand_timing_init_struct;

    exmc_nand_init(&nand_init_struct);

    /* enable EXMC NAND bank1 */
    exmc_nand_enable(EXMC_BANK1_NAND);
}

int rt_hw_nandflash_init(void)
{
    exmc_nandflash_init(EXMC_BANK1_NAND);
    printf("nandflash init OK\n");
    return 0;
}
INIT_DEVICE_EXPORT(rt_hw_nandflash_init);

/*!
    \brief      read NAND flash ID
    \param[in]  nand_id: structure of nand flash ID
    \param[out] none
    \retval     none
*/
void nand_read_id(nand_id_struct *nand_id)
{
    uint32_t data = 0;

    /* send command to the command area */
    NAND_CMD_AREA = NAND_CMD_READID;

    /* send address to the address area */
    NAND_ADDR_AREA = 0x00;

    /* read id from NAND flash */
    data = *(__IO uint32_t *)(BANK_NAND_ADDR | EXMC_DATA_AREA);

    nand_id->maker_id  = ADDR_1ST_CYCLE(data);
    nand_id->device_id = ADDR_2ND_CYCLE(data);
    nand_id->third_id  = ADDR_3RD_CYCLE(data);
    nand_id->fourth_id = ADDR_4TH_CYCLE(data);
}

/*!
    \brief      write a set of data to nand flash for the specified pages addresses
    \param[in]  pbuffer: pointer on the buffer containing data to be written
    \param[in]  address: the address of the data to be written
    \param[in]  bytecount: byte count to be written(bytecount + address.page_in_offset <= nand_page_total_size)
    \param[out] none
    \retval     NAND_OK, NAND_FAIL
*/
static uint8_t exmc_nand_writepage(uint8_t *pbuffer, nand_address_struct address, uint16_t bytecount)
{
    uint16_t i;

    /* send 1st cycle page programming command to the command area */
    NAND_CMD_AREA = NAND_CMD_WRITE_1ST;

    /* send address to the address area
                    bit7 bit6 bit5 bit4 bit3 bit2 bit1 bit0
       first byte:  A7   A6   A5   A4   A3   A2   A1   A0    (it7 - bit0 of page address)
       second byte: 0    0    0    0    A11  A10  A9   A8    (bit11 - bit8 of page address, high 4bit must be zero)
       third byte: A19  A18  A17  A16  A15  A14  A13  A12
       fourth byte: A27  A26  A25  A24  A23  A22  A21  A20
    */
    NAND_ADDR_AREA = address.page_in_offset;
    NAND_ADDR_AREA = address.page_in_offset >> 8;
    NAND_ADDR_AREA = ADDR_1ST_CYCLE(ROW_ADDRESS);
    NAND_ADDR_AREA = ADDR_2ND_CYCLE(ROW_ADDRESS);

    /* write data to data area */
    for (i = 0; i < bytecount; i++) {
        NAND_DATA_AREA = pbuffer[i];
    }

    /* send 2nd cycle page programming command to the command area */
    NAND_CMD_AREA = NAND_CMD_WRITE_2ND;

    /* check operation status */
    if (NAND_READY == exmc_nand_getstatus()) {
        return NAND_OK;
    }
    return NAND_FAIL;
}

/*!
    \brief      read a set of data from nand flash for the specified pages addresses
    \param[in]  pbuffer: pointer on the buffer filling data to be read
    \param[in]  address: the address of the data to be read
    \param[in]  bytecount: byte count to be read(bytecount + address.page_in_offset <= NAND_PAGE_TOTAL_SIZE)
    \param[out] none
    \retval     NAND_OK, NAND_FAIL
*/
static uint8_t exmc_nand_readpage(uint8_t *pbuffer, nand_address_struct address, uint16_t bytecount)
{
    uint16_t i;

    /* send 1st cycle read command to the command area */
    NAND_CMD_AREA = NAND_CMD_READ1_1ST;

    /* send address to the address area
                    bit7 bit6 bit5 bit4 bit3 bit2 bit1 bit0
       first byte:  A7   A6   A5   A4   A3   A2   A1   A0    (it7 - bit0 of page address)
       second byte: 0    0    0    0    A11  A10  A9   A8    (bit11 - bit8 of page address, high 4bit must be zero)
       third byte: A19  A18  A17  A16  A15  A14  A13  A12
       fourth byte: A27  A26  A25  A24  A23  A22  A21  A20
    */
    NAND_ADDR_AREA = address.page_in_offset;
    NAND_ADDR_AREA = address.page_in_offset >> 8;
    NAND_ADDR_AREA = ADDR_1ST_CYCLE(ROW_ADDRESS);
    NAND_ADDR_AREA = ADDR_2ND_CYCLE(ROW_ADDRESS);

    /* send 2nd cycle read command to the command area */
    NAND_CMD_AREA = NAND_CMD_READ1_2ND;

    /* read data to pbuffer */
    for (i = 0; i < bytecount; i++) {
        pbuffer[i] = NAND_DATA_AREA;
    }

    /* check operation status */
    if (NAND_READY == exmc_nand_getstatus()) {
        return NAND_OK;
    }

    return NAND_FAIL;
}

/*!
    \brief      write the spare area information for the specified pages addresses
    \param[in]  pbuffer: pointer on the buffer containing data to be written
    \param[in]  address: the address of the data to be written
    \param[in]  bytecount: byte count to be written(bytecount + (address.page_in_offset - NAND_PAGE_SIZE) <= NAND_SPARE_AREA_SIZE)
    \param[out] none
    \retval     NAND_OK, NAND_FAIL
*/
uint8_t exmc_nand_writespare(uint8_t *pbuffer, nand_address_struct address, uint16_t bytecount)
{
    /* address.page_in_offset > NAND_PAGE_SIZE */
    if (address.page_in_offset <= NAND_PAGE_SIZE) {
        return NAND_FAIL;
    }

    /* bytecount + address.page_in_offset < NAND_PAGE_TOTAL_SIZE */
    if (bytecount + address.page_in_offset >= NAND_PAGE_TOTAL_SIZE) {
        return NAND_FAIL;
    }

    /* write spare area */
    return exmc_nand_writepage(pbuffer, address, bytecount);
}

/*!
    \brief      read the spare area information for the specified pages addresses
    \param[in]  pbuffer: pointer on the buffer containing data to be read
    \param[in]  address: the address of the data to be read
    \param[in]  bytecount: byte count to be read(bytecount + (address.page_in_offset - NAND_PAGE_SIZE) <= NAND_SPARE_AREA_SIZE)
    \param[out] none
    \retval     NAND_OK, NAND_FAIL
*/
uint8_t exmc_nand_readspare(uint8_t *pbuffer, nand_address_struct address, uint16_t bytecount)
{
    /* address.page_in_offset > NAND_PAGE_SIZE */
    if (address.page_in_offset <= NAND_PAGE_SIZE) {
        return NAND_FAIL;
    }

    /* bytecount + address.page_in_offset < NAND_PAGE_TOTAL_SIZE */
    if (bytecount + address.page_in_offset >= NAND_PAGE_TOTAL_SIZE) {
        return NAND_FAIL;
    }

    /* read spare area */
    return exmc_nand_readpage(pbuffer, address, bytecount);
}

/*!
    \brief      write the main area information for the specified pages addresses
    \param[in]  pbuffer: pointer on the buffer containing data to be written
    \param[in]  address: the address of the data to be written
    \param[in]  bytecount: byte count to be written
    \param[out] none
    \retval     NAND_OK, NAND_FAIL
*/
static uint8_t exmc_nand_writedata(uint8_t *pbuffer, nand_address_struct physicaladdress, uint16_t bytecount)
{
    uint8_t *temp_pbuffer = pbuffer;

    /* erase block before write data based on the feature of nand flash*/
    exmc_nand_eraseblock(physicaladdress.block);

    /* if the number of data bytes to be written plus the offset is greater than the page size,
       the automatic next page. */
    while (bytecount + physicaladdress.page_in_offset > NAND_PAGE_SIZE) {
        if (exmc_nand_writepage(temp_pbuffer, physicaladdress, NAND_PAGE_SIZE - physicaladdress.page_in_offset) != NAND_OK) {
            return NAND_FAIL;
        }
        /* compute address of the next block */
        bytecount -= NAND_PAGE_SIZE - physicaladdress.page_in_offset;
        temp_pbuffer += NAND_PAGE_SIZE - physicaladdress.page_in_offset;
        physicaladdress.page++;
        physicaladdress.page_in_offset = 0;
    }

    /* write the last less than one block of data */
    if (bytecount > 0) {
        if (exmc_nand_writepage(temp_pbuffer, physicaladdress, bytecount) != NAND_OK) {
            return NAND_FAIL;
        }
    }

    return NAND_OK;
}

/*!
    \brief      read the main area information for the specified pages addresses
    \param[in]  pbuffer: pointer on the buffer containing data to be read
    \param[in]  address: the address of the data to be read
    \param[in]  bytecount: byte count to be read
    \param[out] none
    \retval     NAND_OK, NAND_FAIL
*/
static uint8_t exmc_nand_readdata(uint8_t *pbuffer, nand_address_struct phyaddress, uint16_t bytecount)
{
    uint8_t *temp_pbuffer = pbuffer;
    /* if the number of data bytes to be read plus the offset is greater than the page size, the automatic next page. */
    while (bytecount + phyaddress.page_in_offset > NAND_PAGE_SIZE) {
        if (exmc_nand_readpage(temp_pbuffer, phyaddress, NAND_PAGE_SIZE - phyaddress.page_in_offset) != NAND_OK) {
            return NAND_FAIL;
        }
        phyaddress.page++;
        temp_pbuffer += NAND_PAGE_SIZE - phyaddress.page_in_offset;
        bytecount -= NAND_PAGE_SIZE - phyaddress.page_in_offset;
        phyaddress.page_in_offset = 0;
    }

    if (bytecount > 0) {
        if (exmc_nand_readpage(temp_pbuffer, phyaddress, bytecount) != NAND_OK) {
            return NAND_FAIL;
        }
    }

    return NAND_OK;
}

/*!
    \brief      erase data specified block
    \param[in]  blocknum: block number to be erased data
    \param[out] none
    \retval     NAND memory status
*/
static uint8_t exmc_nand_eraseblock(uint32_t blocknum)
{
    /* send 1st cycle erase command to command area */
    NAND_CMD_AREA = NAND_CMD_ERASE_1ST;

    /* block number into a block number and the page number */
    blocknum <<= PAGE_BIT;
    NAND_ADDR_AREA = ADDR_1ST_CYCLE(blocknum);
    NAND_ADDR_AREA = ADDR_2ND_CYCLE(blocknum);

    /* send 2nd cycle erase command to command area */
    NAND_CMD_AREA = NAND_CMD_ERASE_2ND;

    return (exmc_nand_getstatus());
}

/*!
    \brief      reset nand flash
    \param[in]  none
    \param[out] none
    \retval     NAND_OK, NAND_FAIL
*/
uint8_t nand_reset(void)
{
    NAND_CMD_AREA = NAND_CMD_RESET;

    /* check operation status */
    if (NAND_READY == exmc_nand_getstatus()) {
        return NAND_OK;
    }

    return NAND_FAIL;
}

/*!
    \brief      reads the NAND memory status
    \param[in]  none
    \param[out] none
    \retval     NAND memory status
*/
static uint8_t exmc_nand_readstatus(void)
{
    uint8_t data;
    uint8_t status = NAND_BUSY;

    /* send read status command to the command area */
    NAND_CMD_AREA = NAND_CMD_STATUS;

    data = NAND_DATA_AREA;

    if ((data & NAND_ERROR) == NAND_ERROR) {
        status = NAND_ERROR;
    } else if ((data & NAND_READY) == NAND_READY) {
        status = NAND_READY;
    } else {
        status = NAND_BUSY;
    }

    return (status);
}

/*!
    \brief      get the NAND operation status
    \param[in]  none
    \param[out] none
    \retval     new status of the NAND operation
*/
static uint8_t exmc_nand_getstatus(void)
{
    uint32_t timeout = 0x10000;
    uint8_t status   = NAND_READY;

    status = exmc_nand_readstatus();

    /* waiting for NAND operation over, it will exit after a timeout. */
    while ((status != NAND_READY) && (timeout != 0x00)) {
        status = exmc_nand_readstatus();
        timeout--;
    }

    if (timeout == 0x00) {
        status = NAND_TIMEOUT_ERROR;
    }

    return (status);
}

/*!
    \brief      write the main area information for the specified logic addresses
    \param[in]  memaddr: the logic address of the data to be written
    \param[in]  pwritebuf: pointer on the buffer containing data to be written
    \param[in]  bytecount: byte count to be written
    \param[out] none
    \retval     NAND_OK, NAND_FAIL
*/
uint8_t nand_write(uint32_t memaddr, uint8_t *pwritebuf, uint16_t bytecount)
{
    uint32_t temp_blockremainsize;
    nand_address_struct physicaladdress;
    uint32_t temp;

    temp = memaddr % (NAND_BLOCK_SIZE * NAND_PAGE_SIZE);

    /* compute physical zone number */
    physicaladdress.zone = memaddr / (NAND_BLOCK_SIZE * NAND_PAGE_SIZE * NAND_ZONE_SIZE);

    /* compute physical block number */
    physicaladdress.block = memaddr / (NAND_BLOCK_SIZE * NAND_PAGE_SIZE);

    /* compute physical page number */
    physicaladdress.page = temp / NAND_PAGE_SIZE;

    /* compute physical offset into page  */
    physicaladdress.page_in_offset = temp % NAND_PAGE_SIZE;
    temp_blockremainsize           = (NAND_BLOCK_SIZE * NAND_PAGE_SIZE) - (NAND_PAGE_SIZE * physicaladdress.page +
                                                                 physicaladdress.page_in_offset);

    /* if the number of data bytes to be written plus the offset is greater than the block size, the automatic next block. */
    while (bytecount > temp_blockremainsize) {
        if (NAND_FAIL == exmc_nand_writedata(pwritebuf, physicaladdress, temp_blockremainsize)) {
            return NAND_FAIL;
        }

        physicaladdress.block++;
        pwritebuf += temp_blockremainsize;
        bytecount -= temp_blockremainsize;
        physicaladdress.page           = 0;
        physicaladdress.page_in_offset = 0;
        temp_blockremainsize           = (NAND_BLOCK_SIZE * NAND_PAGE_SIZE);
    }

    if (bytecount > 0) {
        if (NAND_FAIL == exmc_nand_writedata(pwritebuf, physicaladdress, bytecount)) {
            return NAND_FAIL;
        }
    }

    return NAND_OK;
}

/*!
    \brief      read the main area information for the specified logic addresses
    \param[in]  memaddr: the logic address of the data to be read
    \param[in]  preadbuf: pointer on the buffer containing data to be read
    \param[in]  bytecount: byte count to be reas
    \param[out] none
    \retval     NAND_OK, NAND_FAIL
*/
uint8_t nand_read(uint32_t memaddr, uint8_t *preadbuf, uint16_t bytecount)
{
    uint32_t temp_blockremainsize;
    nand_address_struct physicaladdress;
    uint32_t temp;

    temp = memaddr % (NAND_BLOCK_SIZE * NAND_PAGE_SIZE);

    /* compute physical zone number */
    physicaladdress.zone = memaddr / (NAND_BLOCK_SIZE * NAND_PAGE_SIZE * NAND_ZONE_SIZE);

    /* compute physical block number */
    physicaladdress.block = memaddr / (NAND_BLOCK_SIZE * NAND_PAGE_SIZE);

    /* compute physical page number */
    physicaladdress.page = temp / NAND_PAGE_SIZE;

    /* compute physical offset into page  */
    physicaladdress.page_in_offset = temp % NAND_PAGE_SIZE;

    temp_blockremainsize = (NAND_BLOCK_SIZE * NAND_PAGE_SIZE) - (NAND_PAGE_SIZE * physicaladdress.page + physicaladdress.page_in_offset);

    /* if the number of data bytes to be read plus the offset is greater than the block size, the automatic next block. */
    while (bytecount > temp_blockremainsize) {
        if (NAND_FAIL == exmc_nand_readdata(preadbuf, physicaladdress, temp_blockremainsize)) {
            return NAND_FAIL;
        }
        physicaladdress.block++;
        preadbuf += temp_blockremainsize;
        bytecount -= temp_blockremainsize;
        physicaladdress.page           = 0;
        physicaladdress.page_in_offset = 0;
        temp_blockremainsize           = (NAND_BLOCK_SIZE * NAND_PAGE_SIZE);
    }

    if (bytecount > 0) {
        if (exmc_nand_readdata(preadbuf, physicaladdress, bytecount) == NAND_FAIL) {
            return NAND_FAIL;
        }
    }

    return NAND_OK;
}

/*!
    \brief      format nand flash
    \param[in]  none
    \param[out] none
    \retval     NAND_OK, NAND_FAIL
*/
uint8_t nand_format(void)
{
    uint16_t i;

    for (i = 0; i < NAND_BLOCK_COUNT; i++) {
        if (NAND_READY != exmc_nand_eraseblock(i)) {
            return NAND_FAIL;
        }
    }

    return NAND_OK;
}

/*!
    \brief      fill the buffer with specified value
    \param[in]  pbuffer: pointer on the buffer to fill
    \param[in]  buffer_lenght: size of the buffer to fill
    \param[in]  value: value to fill on the buffer
    \param[out] none
    \retval     none
*/
void fill_buffer_nand(uint8_t *pbuffer, uint16_t buffer_lenght, uint32_t value)
{
    uint16_t index = 0;

    /* put in global buffer same values */
    for (index = 0; index < buffer_lenght; index++) {
        pbuffer[index] = value + index;
    }
}

#define BUFFER_SIZE     (0x100U)
#define NAND_GD_MAKERID (0xC8U)
#define NAND_HY_MAKERID (0xADU)
#define NAND_DEVICEID   (0xF1U)

static void nandflash_test(void)
{
    nand_id_struct nand_id;
    uint8_t txbuffer[BUFFER_SIZE], rxbuffer[BUFFER_SIZE];
    __IO uint32_t writereadstatus = 0, status = 0;
    uint32_t writereadaddr;
    uint16_t zone, block, page, pageoffset;
    /* read NAND ID */
    nand_read_id(&nand_id);

    printf("\r\nRead NAND ID!");
    delay_1ms(1000);
    /* print NAND ID */
    printf("\r\nNand flash ID:0x%X 0x%X 0x%X 0x%X\r\n", nand_id.maker_id, nand_id.device_id,
           nand_id.third_id, nand_id.fourth_id);
    delay_1ms(1000);

    if (((NAND_GD_MAKERID == nand_id.maker_id) || (NAND_HY_MAKERID == nand_id.maker_id)) && (NAND_DEVICEID == nand_id.device_id)) {
        /* set the read and write the address */
        zone          = 0;
        block         = 10;
        page          = 0;
        pageoffset    = 1200;
        writereadaddr = ((zone * NAND_ZONE_SIZE + block) * NAND_BLOCK_SIZE + page) * NAND_PAGE_SIZE + pageoffset;
        printf("%ld\n", writereadaddr);
        /* whether address cross-border */
        if ((writereadaddr + BUFFER_SIZE) > NAND_MAX_ADDRESS) {
            printf("\r\nAddress cross-border!");
        }

        /* fill writebuffer with 0x00.. */
        fill_buffer_nand(txbuffer, BUFFER_SIZE, 0x00);

        /* write data to nand flash */
        status = nand_write(writereadaddr, txbuffer, BUFFER_SIZE);
        if (NAND_OK == status) {
            printf("\r\nWrite data successfully!");
            delay_1ms(1000);
        } else {
            printf("\r\nWrite data failure!");
        }

        /* read data from nand flash */
        status = nand_read(writereadaddr, rxbuffer, BUFFER_SIZE);
        if (NAND_OK == status) {
            printf("\r\nRead data successfully!");
            delay_1ms(1000);
        } else {
            printf("\r\nRead data failure!");
        }

        printf("\r\nCheck the data!");
        delay_1ms(1000);
        /* read and write data comparison for equality */
        writereadstatus = 0;
        for (uint32_t j = 0; j < BUFFER_SIZE; j++) {
            if (txbuffer[j] != rxbuffer[j]) {
                writereadstatus++;
                break;
            }
        }

        if (writereadstatus == 0) {
            printf("\r\nAccess NAND flash successfully!");
            delay_1ms(1000);
        } else {
            printf("\r\nAccess NAND flash failure!");
        }
        printf("\r\nThe data to be read:\r\n");
        delay_1ms(1000);
        for (uint32_t k = 0; k < BUFFER_SIZE; k++) {
            printf("%5x ", rxbuffer[k]);
            if (((k + 1) % 16) == 0) {
                printf("\r\n");
            }
        }
    } else {
        printf("\r\nRead NAND ID failure!");
    }
}
MSH_CMD_EXPORT(nandflash_test, test);

#if defined(RT_USING_FAL)

#include "fal.h"
static int fal_flash_read(long offset, rt_uint8_t *buf, size_t size)
{
    rt_kprintf("read\n");
    if (nand_read(nand_flash.addr + (uint32_t)offset, buf, (uint16_t)size) != NAND_OK) {
        printf("nand_read failed.\n");
        return -1;
    }
    return (int)size;
}

static int fal_flash_write(long offset, const rt_uint8_t *buf, size_t size)
{
    rt_kprintf("write\n");
    if (nand_write(nand_flash.addr + (uint32_t)offset, buf, (uint16_t)size) != NAND_OK) {
        printf("nand_write failed.\n");
        return -1;
    }
    for (size_t i = 0; i < size; i++) {
        printf("%02x", *(buf + i));
        if (i % 64 == 63) {
            printf("\n");
        }
    }
    return (int)size;
}

static int fal_flash_erase(long offset, size_t size)
{
    return size;
}

const struct fal_flash_dev nand_flash =
    {
        .name       = NAND_FLASH_DEV_NAME,
        .addr       = 0,
        .len        = NAND_MAX_ADDRESS, /* 1*1024*64*2048 */
        .blk_size   = NAND_PAGE_SIZE,   /* 2048 */
        .ops        = {NULL, fal_flash_read, fal_flash_write, fal_flash_erase},
        .write_gran = 0};
#endif