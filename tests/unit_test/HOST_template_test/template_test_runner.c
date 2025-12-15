#include "unity_fixture.h"

TEST_GROUP_RUNNER(templateTest)
{
   /* Test cases to run */
   RUN_TEST_CASE(templateTest, test1);
   RUN_TEST_CASE(templateTest, test2);
}


// TEST_GROUP_RUNNER(templateTest2)
// {
//    RUN_TEST_CASE(remote_rx, FirstTest);
// }
