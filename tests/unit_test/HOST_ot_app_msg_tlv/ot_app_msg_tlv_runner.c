#include "unity_fixture.h"

TEST_GROUP_RUNNER(ot_app_msg_tlv)
{
   /* Test cases to run */
   RUN_TEST_CASE(ot_app_msg_tlv, GivenNullPtr_WhenCallKeyAdd_ThenReturnError);
   RUN_TEST_CASE(ot_app_msg_tlv, GivenFalseBufSize_WhenCallKeyAdd_ThenReturnError);
   RUN_TEST_CASE(ot_app_msg_tlv, GivenSmallerBufSizeThenValue_WhenCallKeyAdd_ThenReturnError);
   RUN_TEST_CASE(ot_app_msg_tlv, GivenPerfectBufSizeForValueLength_WhenCallKeyAdd_ThenReturnOK);
}


// TEST_GROUP_RUNNER(templateTest2)
// {
//    RUN_TEST_CASE(remote_rx, FirstTest);
// }
