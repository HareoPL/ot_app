#include "unity_fixture.h"

TEST_GROUP_RUNNER(ot_app_buffer)
{
   /* Test cases to run */

   // otapp_buf_append
   RUN_TEST_CASE(ot_app_buffer, given_false_args_when_call_append_return_error);
   RUN_TEST_CASE(ot_app_buffer, given_key_no_exist_when_call_append_return_error);
   RUN_TEST_CASE(ot_app_buffer, given_too_large_buf_size_when_call_append_return_overflow);
   RUN_TEST_CASE(ot_app_buffer, given_tru_args_when_call_append_return_ok);
   RUN_TEST_CASE(ot_app_buffer, given_tru_args_when_call_append_twice_return_ok);

   // otapp_buf_getData
   RUN_TEST_CASE(ot_app_buffer, given_false_args_when_call_getData_return_error);
   RUN_TEST_CASE(ot_app_buffer, given_too_small_buf_when_call_getData_return_error);
   RUN_TEST_CASE(ot_app_buffer, given_correct_args_when_call_getData_return_ok);

   // otapp_buf_getReadOnly_ptr
   RUN_TEST_CASE(ot_app_buffer, given_false_args_when_call_getReadOnly_ptr_return_error);
   RUN_TEST_CASE(ot_app_buffer, given_true_args_when_call_getReadOnly_ptr_return_not_null_ptr);
   RUN_TEST_CASE(ot_app_buffer, given_append_when_call_getReadOnly_ptr_return_ok);

   //otapp_buf_getWriteOnly_ptr
   RUN_TEST_CASE(ot_app_buffer, given_false_args_when_call_getWriteOnly_ptr_return_error);
   RUN_TEST_CASE(ot_app_buffer, given_too_large_size_when_call_getWriteOnly_ptr_return_error);
   RUN_TEST_CASE(ot_app_buffer, check_lock_when_call_getWriteOnly_ptr_and_append_return_error);
   RUN_TEST_CASE(ot_app_buffer, check_unlock_when_call_getWriteOnly_ptr_and_append_return_ok);
   RUN_TEST_CASE(ot_app_buffer, given_true_args_when_call_getWriteOnly_ptr_return_ok);



   // multithread race condidion
   RUN_TEST_CASE(ot_app_buffer, buffer_should_survive_multithreaded_race_condition_when_mutex_on_retorn_ok);
   RUN_TEST_CASE(ot_app_buffer, buffer_should_not_survive_multithreaded_race_condition_when_mutex_off_return_error);

   
}

