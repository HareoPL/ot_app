#include "unity_fixture.h"

#include "mock_template.h"

TEST_GROUP(templateTest);

TEST_SETUP(templateTest)
{
    /* Init before every test */
}

TEST_TEAR_DOWN(templateTest)
{
    /* Cleanup after every test */
}

TEST(templateTest, test1)
{
    TEST_FAIL_MESSAGE("1: It is template for unit test. Use it to create your own test.");
}

TEST(templateTest, test2)
{
    TEST_FAIL_MESSAGE("2: It is template for unit test. Use it to create your own test.");
}
