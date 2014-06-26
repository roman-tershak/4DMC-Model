#include "init_logic.h"
#include "common.h"
#include "central_logic.h"
#include "mpcomm.h"
#include "safetable.h"
#include "state_storage.h"


static void unpack_colors(uint8_t *colors_packed_src, uint8_t *colors_dest);


extern volatile Side_State sides_states[SIDE_COUNT];


void init_central_logic(void)
{
    uint8_t i;
    Side_State *side_state_ptr;
    
    for (i = 0; i < SIDE_COUNT; i++)
    {
        side_state_ptr = &(sides_states[i]);
        
        side_state_ptr->status = SIDE_IDLE;
        side_state_ptr->cycle_ct = 0;
        side_state_ptr->phase_len = 0;
        side_state_ptr->rotation_func_ptr = NULL;
        side_state_ptr->colors_changed = FALSE;
    }
}

/*
 * | 1 | 0 || 3 | 2 || 5 | 4 |
 */
void load_sides_states(void)
{
    uint8_t i, j, bank_num;
    uint8_t colors_packed[SIDE_STATE_LEN] = {0};
    uint32_t record_num;

    record_num = read_safetable_record_num();

    bank_num = get_bank_num_for_sides(record_num);
    for (i = 0; i < 2/*SIDE_CB*/; i++)
    {
        send_one_byte_command(GET_MCU_ADDRESS_FROM_SIDE_NUM(i), USART_INIT_LOAD, bank_num);

        for (j = 0; j < SIDE_STATE_LEN; j++)
        {
            colors_packed[j] = USART_receive(100000); // TODO
        }
        unpack_colors(colors_packed, sides_states[i].colors);
    }

    read_side_state(colors_packed, get_bank_num_for_hidden_side(record_num));
    unpack_colors(colors_packed, sides_states[SIDE_CB].colors);

}

static void unpack_colors(uint8_t *colors_packed_src, uint8_t *colors_dest)
{
    uint8_t i, j, data;

    for (i = 0, j = 0; i < SIDE_CUBES_COUNT; i++)
    {
        if (i % 2 == 0)
        {
            data = colors_packed_src[j++];
            colors_dest[i] = data & 0xF;
        }
        else
        {
            colors_dest[i] = (data >> 4) & 0xF;
        }
    }
}

