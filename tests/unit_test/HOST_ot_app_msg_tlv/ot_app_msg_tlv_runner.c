#include "unity_fixture.h"

TEST_GROUP_RUNNER(ot_app_msg_tlv)
{
   /* Test cases to run */
   RUN_TEST_CASE(ot_app_msg_tlv, GivenNullPtr_WhenCallKeyAdd_ThenReturnError);
   RUN_TEST_CASE(ot_app_msg_tlv, GivenFalseBufSize_WhenCallKeyAdd_ThenReturnError);
   RUN_TEST_CASE(ot_app_msg_tlv, GivenSmallerBufSizeThenValue_WhenCallKeyAdd_ThenReturnError);
   RUN_TEST_CASE(ot_app_msg_tlv, GivenPerfectBufSizeForValueLength_WhenCallKeyAdd_ThenReturnOK);
   RUN_TEST_CASE(ot_app_msg_tlv, GivenBufSizeForOneKey_WhenCallKeyAddTwice_ThenReturnErrorNoSpace);
   RUN_TEST_CASE(ot_app_msg_tlv, GivenBufSizeForTwoDiffrentKeys_WhenCallKeyAddTwice_ThenReturnOk);
   RUN_TEST_CASE(ot_app_msg_tlv, GivenBufSizeForTwoSameKeys_WhenCallKeyAddTwice_ThenReturnKeyExist);
   RUN_TEST_CASE(ot_app_msg_tlv, GivenBufSizeFor10DiffrentKeys_WhenCallKeyAddTwice_ThenReturnOk);

   RUN_TEST_CASE(ot_app_msg_tlv, GivenNullPtr_WhenCallKeyGet_ThenReturnError);
   RUN_TEST_CASE(ot_app_msg_tlv, GivenTooSmallBufSize_WhenCallKeyGet_ThenReturnError);
   RUN_TEST_CASE(ot_app_msg_tlv, GivenTrueKey_WhenCallKeyGet_ThenReturnKeyExist);
   RUN_TEST_CASE(ot_app_msg_tlv, GivenTrueKeyAndReadNotExistKey_WhenCallKeyGet_ThenReturnKeyNoExist);
   RUN_TEST_CASE(ot_app_msg_tlv, GivenTrueKeyAndReadExistKeyWithoutReadValueAndValSize_WhenCallKeyGet_ThenReturnKeyExist);
   RUN_TEST_CASE(ot_app_msg_tlv, GivenEmptyBuffer_WhenCallKeyGet_ThenReturn);

   RUN_TEST_CASE(ot_app_msg_tlv, GivenKeyDataLen_WhenCallalCualeBuffer_ThenReturnOK);
   RUN_TEST_CASE(ot_app_msg_tlv, GivenKeyDataLenDouble_WhenCallalCualeBuffer_ThenReturnOK);
}


// TEST_GROUP_RUNNER(templateTest2)
// {
//    RUN_TEST_CASE(remote_rx, FirstTest);
// }
