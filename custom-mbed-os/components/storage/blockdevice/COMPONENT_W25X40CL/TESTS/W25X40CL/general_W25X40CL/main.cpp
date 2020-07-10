#include "utest/utest.h"
#include "unity/unity.h"
#include "greentea-client/test_env.h"
#include "mbed.h"
#include "W25X40CL.h"

using namespace utest::v1;

W25X40CL* get_bd_instance(){
    static W25X40CL& default_bd = W25X40CL::getInstance();
    return &default_bd;

}

static W25X40CL* block_device = nullptr;

static void test_init_w25x40cl()
{
    utest_printf("\nTest Init block device.\n");

    block_device = get_bd_instance();
    TEST_SKIP_UNLESS_MESSAGE(block_device != NULL, "no block device found.");

    int err = block_device->init();
    TEST_ASSERT_EQUAL(0, err);
}

static void test_uid_can_be_read_w25x40cl()
{
    int err = block_device->init();
    TEST_ASSERT_EQUAL(0, err);

    uint64_t uid=0;
    err = block_device->read_unique_id(&uid);
    TEST_ASSERT_EQUAL(0, err);
}

static void test_uid_not_nullt_w25x40cl()
{
    int err = block_device->init();
    TEST_ASSERT_EQUAL(0, err);

    uint64_t uid=0;
    err = block_device->read_unique_id(&uid);
    TEST_ASSERT_NOT_EQUAL(0x0000000000000000, uid);

}

static void test_uid_is_valid_w25x40cl()
{
    int err = block_device->init();
    TEST_ASSERT_EQUAL(0, err);

    uint64_t uid=0;
    err = block_device->read_unique_id(&uid);
    TEST_ASSERT_NOT_EQUAL(0x0101010101010101, uid);
    TEST_ASSERT_NOT_EQUAL(0x1010101010101010, uid);
    TEST_ASSERT_NOT_EQUAL(0x0202020202020202, uid);
    TEST_ASSERT_NOT_EQUAL(0x2020202020202020, uid);
    TEST_ASSERT_NOT_EQUAL(0x0404040404040404, uid);
    TEST_ASSERT_NOT_EQUAL(0x4040404040404040, uid);
    TEST_ASSERT_NOT_EQUAL(0x0808080808080808, uid);
    TEST_ASSERT_NOT_EQUAL(0x8080808080808080, uid);
//TEST_ASSERT_EQUAL_HEX64(0x0, uid);
//TEST_ASSERT_EQUAL_UINT64(0x0, uid);
}

static void test_deinit_w25x40cl()
{
    utest_printf("\nTest deinit block device.\n");

    TEST_SKIP_UNLESS_MESSAGE(block_device != NULL, "no block device found.");

    int err = block_device->deinit();
    TEST_ASSERT_EQUAL(0, err);

    block_device = NULL;
}

utest::v1::status_t test_setup(const size_t number_of_cases)
{
    GREENTEA_SETUP(20, "default_auto");
    return verbose_test_setup_handler(number_of_cases);
}

#define DEFAULT_HANDLERS NULL,NULL,greentea_case_setup_handler,greentea_case_teardown_handler,greentea_case_failure_abort_handler

static const case_t cases[] = {
    {"W25X40CL - Testing Init block device", test_init_w25x40cl, DEFAULT_HANDLERS},
    {"W25X40CL - UID can be read", test_uid_can_be_read_w25x40cl, DEFAULT_HANDLERS},
    {"W25X40CL - UID not ull", test_uid_not_nullt_w25x40cl, DEFAULT_HANDLERS},
    {"W25X40CL - UID is valid", test_uid_is_valid_w25x40cl, DEFAULT_HANDLERS},
    {"W25X40CL - Testing Deinit block device", test_deinit_w25x40cl, DEFAULT_HANDLERS}
};

Specification specification(test_setup, cases);

int main()
{
    return !Harness::run(specification);
}
