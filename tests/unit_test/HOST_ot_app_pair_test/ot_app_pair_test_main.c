#include "unity_fixture.h"

static void run_all_tests(void);

int main(int argc, const char **argv)
{
   UnityMain(argc, argv, run_all_tests);

   return 0;
}

static void run_all_tests(void)
{
   RUN_TEST_GROUP(ot_app_pair);
   RUN_TEST_GROUP(ot_app_pair_UriIndex);
   RUN_TEST_GROUP(ot_app_pair_rtos);
}
