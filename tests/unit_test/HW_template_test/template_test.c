

#include "unity_config.h"
#include "unity_fixture.h"

#include <stdint.h>

extern int value;

TEST_GROUP(template);

TEST_SETUP(template)
{
    value = 5;
}

TEST_TEAR_DOWN(template)
{
    
}

TEST(template, template_fn_name)
{
    TEST_FAIL_MESSAGE("It is template for unit test. Use it to create your own test.");
}
