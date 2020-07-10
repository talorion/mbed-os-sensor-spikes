#include "W25X40CL.h"
#include "rtos/ThisThread.h"

using namespace mbed;

// Read/write/erase sizes
constexpr bd_size_t SPIF_READ_SIZE = 1;
constexpr bd_size_t SPIF_PROG_SIZE = 1;
constexpr bd_size_t SPIF_SE_SIZE  =  4096; 
constexpr int  SPIF_TIMEOUT =   10000;

// Debug available
#define SPIF_DEBUG      0

// Legacy SFDP Instruction Table.

constexpr uint8_t SPIF_SFDP = 0x5A; // Read SFDP

constexpr uint8_t SPIF_NOP  = 0x00; // No operation
constexpr uint8_t SPIF_WRITE_STATUS_REGISTER = 0x01;
constexpr uint8_t SPIF_PAGE_PROGRAM = 0x02;
constexpr uint8_t SPIF_READ_DATA = 0x03;
constexpr uint8_t SPIF_WRITE_DISABLE = 0x04;
constexpr uint8_t SPIF_READ_STATUS_REGISTER = 0x05;
constexpr uint8_t SPIF_WRITE_ENABLE = 0x06;
constexpr uint8_t SPIF_FAST_READ = 0x0B;
constexpr uint8_t SPIF_SECTOR_ERASE = 0x20;
constexpr uint8_t SPIF_FAST_READ_DUAL_OUTPUT = 0x3B;
constexpr uint8_t SPIF_READ_UNIQUE_ID = 0x4B;
constexpr uint8_t SPIF_WRITE_ENABLE_FOR_VOATILE_STATUS_REGISTER = 0x50;
constexpr uint8_t SPIF_BLOCK_ERASE_32KB = 0x52;
constexpr uint8_t SPIF_CHIP_ERASE_2 = 0x60;
constexpr uint8_t SPIF_READ_MANUFATURER_DEVICE_ID = 0x90;
constexpr uint8_t SPIF_READ_MANUFACTURER_DEVICE_ID_BY_DUAL_IO = 0x92;
constexpr uint8_t SPIF_READ_JDEDEC_ID = 0x9F;
constexpr uint8_t SPIF_RELEASE_POWER_DOWN = 0xAB;
constexpr uint8_t SPIF_POWER_DOWN = 0xB9;
constexpr uint8_t SPIF_FAST_READ_DUAL_IO = 0xBB;
constexpr uint8_t SPIF_CHIP_ERASE = 0xC7;
constexpr uint8_t SPIF_BLOCK_ERASE_64KB = 0xD8;




// Status register from RDSR
// [---------| wel | wip ]
// [-   6   -|  1  |  1  ]
#define SPIF_WEL 0x2
#define SPIF_WIP 0x1


std::unique_ptr<W25X40CL> W25X40CL::_singleton(nullptr);


W25X40CL & W25X40CL::getInstance() {
    if(!_singleton) {
        _singleton.reset(new W25X40CL);
    }

    return *_singleton;
}

W25X40CL::W25X40CL(): W25X40CL(MBED_CONF_W25X40CL_DRIVER_SPI_MOSI, MBED_CONF_W25X40CL_DRIVER_SPI_MISO, MBED_CONF_W25X40CL_DRIVER_SPI_CLK, MBED_CONF_W25X40CL_DRIVER_SPI_CS, MBED_CONF_W25X40CL_DRIVER_SPI_FREQ) {

}

W25X40CL::W25X40CL(
    PinName mosi, PinName miso, PinName sclk, PinName cs, int freq)
    : _spi(mosi, miso, sclk), _cs(cs), _size(0)
{
    _cs = 1;
    _spi.frequency(freq);
}

void W25X40CL::deselectAllSpiCs()
{
    // gpio_t temp_cs;
    // gpio_init_out_ex(&temp_cs, SPI1_nCS0, 1);
    // gpio_init_out_ex(&temp_cs, SPI1_nCS1, 1);
    // gpio_init_out_ex(&temp_cs, SPI1_nCS2, 1);
    // gpio_init_out_ex(&temp_cs, SPI1_nCS3, 1);
    // gpio_init_out_ex(&temp_cs, SPI1_nCS4, 1);
}

int W25X40CL::init()
{
    uint8_t dev_id=0;
    _cmdread(SPIF_RELEASE_POWER_DOWN, 3, 1, 0x00, &dev_id);
    if(dev_id == 0){
        return BD_ERROR_DEVICE_ERROR;
    }

    // Check for vendor specific hacks, these should move into more general
    // handling when possible. RDID is not used to verify a device is attached.
    uint8_t id[3];
    _cmdread(SPIF_READ_MANUFATURER_DEVICE_ID, 2, 3, 0x00, id);
    if(dev_id != id[2]){
        return BD_ERROR_DEVICE_ERROR;
    }


    // Check that device is doing ok
    int err = _sync();
    if (err) {
        return BD_ERROR_DEVICE_ERROR;
    }

    _size = 524288;

    return 0;
}

int W25X40CL::read_unique_id(uint64_t* buffer)
{
    deselectAllSpiCs();
    uint8_t uid[8]={};
    _cmdread(SPIF_READ_UNIQUE_ID, 4, 8, 0x0, uid);

    *buffer = (((uint64_t)uid[0]) << 56) | (((uint64_t)uid[1]) << 48) | (((uint64_t)uid[2]) << 40) |
              (((uint64_t)uid[3]) << 32) | (((uint64_t)uid[4]) << 24) | (((uint64_t)uid[5]) << 16) |
              (((uint64_t)uid[6]) << 8) | ((uint64_t)uid[7]);

    return 0;
}

int W25X40CL::deinit()
{
    // Latch write disable just to keep noise
    // from changing the device
    _cmdwrite(SPIF_WRITE_DISABLE, 0, 0, 0x0, NULL);

    _cmdwrite(SPIF_POWER_DOWN, 0, 0, 0x0, NULL);

    return 0;
}

void W25X40CL::_cmdread(
    uint8_t op, uint32_t addrc, uint32_t retc,
    uint32_t addr, uint8_t *rets)
{
    _cs = 0;
    _spi.write(op);

    for (uint32_t i = 0; i < addrc; i++) {
        _spi.write(0xff & (addr >> 8 * (addrc - 1 - i)));
    }

    for (uint32_t i = 0; i < retc; i++) {
        rets[i] = _spi.write(0);
    }
    _cs = 1;

    if (SPIF_DEBUG) {
        printf("spif <- %02x", op);
        for (uint32_t i = 0; i < addrc; i++) {
            if (i < addrc) {
                printf("%02lx", 0xff & (addr >> 8 * (addrc - 1 - i)));
            } else {
                printf("  ");
            }
        }
        printf(" ");
        for (uint32_t i = 0; i < 16 && i < retc; i++) {
            printf("%02x", rets[i]);
        }
        if (retc > 16) {
            printf("...");
        }
        printf("\n");
    }
}

void W25X40CL::_cmdwrite(
    uint8_t op, uint32_t addrc, uint32_t argc,
    uint32_t addr, const uint8_t *args)
{
    _cs = 0;
    _spi.write(op);

    for (uint32_t i = 0; i < addrc; i++) {
        _spi.write(0xff & (addr >> 8 * (addrc - 1 - i)));
    }

    for (uint32_t i = 0; i < argc; i++) {
        _spi.write(args[i]);
    }
    _cs = 1;

    if (SPIF_DEBUG) {
        printf("spif -> %02x", op);
        for (uint32_t i = 0; i < addrc; i++) {
            if (i < addrc) {
                printf("%02lx", 0xff & (addr >> 8 * (addrc - 1 - i)));
            } else {
                printf("  ");
            }
        }
        printf(" ");
        for (uint32_t i = 0; i < 16 && i < argc; i++) {
            printf("%02x", args[i]);
        }
        if (argc > 16) {
            printf("...");
        }
        printf("\n");
    }
}

int W25X40CL::_sync()
{
    for (int i = 0; i < SPIF_TIMEOUT; i++) {
        // Read status register until write not-in-progress
        uint8_t status;
        _cmdread(SPIF_READ_STATUS_REGISTER, 0, 1, 0x0, &status);

        // Check WIP bit
        if (!(status & SPIF_WIP)) {
            return 0;
        }

        rtos::ThisThread::sleep_for(1);
    }

    return BD_ERROR_DEVICE_ERROR;
}

int W25X40CL::_wren()
{
    _cmdwrite(SPIF_WRITE_ENABLE, 0, 0, 0x0, NULL);

    for (int i = 0; i < SPIF_TIMEOUT; i++) {
        // Read status register until write latch is enabled
        uint8_t status;
        _cmdread(SPIF_READ_STATUS_REGISTER, 0, 1, 0x0, &status);

        // Check WEL bit
        if (status & SPIF_WEL) {
            return 0;
        }

        rtos::ThisThread::sleep_for(1);
    }

    return BD_ERROR_DEVICE_ERROR;
}

int W25X40CL::read(void *buffer, bd_addr_t addr, bd_size_t size)
{
    // Check the address and size fit onto the chip.
    MBED_ASSERT(is_valid_read(addr, size));

    _cmdread(SPIF_READ_DATA, 3, size, addr, static_cast<uint8_t *>(buffer));
    return 0;
}

int W25X40CL::program(const void *buffer, bd_addr_t addr, bd_size_t size)
{
    // Check the address and size fit onto the chip.
    MBED_ASSERT(is_valid_program(addr, size));

    while (size > 0) {
        int err = _wren();
        if (err) {
            return err;
        }

        // Write up to 256 bytes a page
        uint32_t off = addr % 256;
        uint32_t chunk = (off + size < 256) ? size : (256 - off);
        _cmdwrite(SPIF_PAGE_PROGRAM, 3, chunk, addr, static_cast<const uint8_t *>(buffer));
        buffer = static_cast<const uint8_t *>(buffer) + chunk;
        addr += chunk;
        size -= chunk;

        rtos::ThisThread::sleep_for(1);

        err = _sync();
        if (err) {
            return err;
        }
    }

    return 0;
}

int W25X40CL::erase(bd_addr_t addr, bd_size_t size)
{
    // Check the address and size fit onto the chip.
    MBED_ASSERT(is_valid_erase(addr, size));

    while (size > 0) {
        int err = _wren();
        if (err) {
            return err;
        }

        // Erase 4kbyte sectors
        uint32_t chunk = 4096;
        _cmdwrite(SPIF_SECTOR_ERASE, 3, 0, addr, NULL);
        addr += chunk;
        size -= chunk;

        err = _sync();
        if (err) {
            return err;
        }
    }

    return 0;
}

bd_size_t W25X40CL::get_read_size() const
{
    return SPIF_READ_SIZE;
}

bd_size_t W25X40CL::get_program_size() const
{
    return SPIF_PROG_SIZE;
}

bd_size_t W25X40CL::get_erase_size() const
{
    return SPIF_SE_SIZE;
}

bd_size_t W25X40CL::get_erase_size(bd_addr_t addr) const
{
    return SPIF_SE_SIZE;
}

int W25X40CL::get_erase_value() const
{
    return 0xFF;
}

bd_size_t W25X40CL::size() const
{
    return _size;
}

const char *W25X40CL::get_type() const
{
    return "SPIFR";
}

