#include "mock_template.h"

static int32_t pin_state = 0;

void mock_ir_recv_init(void)
{
    pin_state = 0;
}

void mock_ir_recv_pin_set(int32_t state)
{
    pin_state = state;
}

int32_t ir_recv_pin_get(void)
{
    return pin_state;
}