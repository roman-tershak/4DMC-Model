#include "storage.h"
#include "central_logic.h"
#include "common.h"
#include "safetable.h"
#include "state_storage.h"
#include "driver.h"


#define CANNOT_BE_SAVED (ROTATING)


/* Auxiliary functions */
void unpack_colors(uint8_t *colors_packed, uint8_t *colors);

void pack_colors(uint8_t *colors, uint8_t *colors_packed);


extern volatile Side_State sides_states[SIDE_COUNT];

static volatile uint8_t is_saving_states = FALSE;


uint8_t can_save(void)
{
    uint8_t i;
    if (is_saving_states)
        return FALSE;

    for (i = 0; i < SIDE_COUNT; i++)
    {
        if (sides_states[i].status & CANNOT_BE_SAVED)
            return FALSE;
    }
    return TRUE;
}

void save_state(void)
{
    uint8_t i;
    uint32_t record_num;

    static uint8_t back_side_copy_packed[SIDE_CUBES_COUNT];
    
    // Prevent saving in parallel
    is_saving_states = TRUE;
    // Get the current bank from the safetale
    record_num = read_safetable_record_num();
    record_num++;
    
    // TODO Make a copy of the hidden side
    // TODO pack_colors(sides_states[SIDE_CB].colors, back_side_copy_packed);
    // TODO Store state of the side from the copy
    // TODO store_side_state(back_side_copy_packed, get_bank_num_for_hidden_side(record_num));
    

    // 'Release' sides that were waiting for saving
    for (i = 0; i < SIDE_COUNT; i++)
    {
        if (sides_states[i].status == WAITING_FOR_SAVING)
            sides_states[i].status = SIDE_IDLE;
    }

    // Enable interrupts, because storing takes long time
    ENABLE_GLOBAL_INTERRUPTS;

    // Make a record about successful storing into the safetable
    store_safetable_record_num(record_num);

    // Unset 'is_saving' indicator
    is_saving_states = FALSE;
}

void unpack_colors(uint8_t *colors_packed, uint8_t *colors)
{
    uint8_t i, j, data;

    for (i = 0, j = 0; i < SIDE_CUBES_COUNT; i++)
    {
        if (i % 2 == 0)
        {
            data = colors_packed[j++];
            colors[i] = data & 0xF;
        }
        else
        {
            colors[i] = (data >> 4) & 0xF;
        }
    }
}

void pack_colors(uint8_t *colors, uint8_t *colors_packed)
{
    uint8_t i, j;

    for (i = 0, j = 0; i < SIDE_CUBES_COUNT; i++)
    {
        if (i % 2)
        {
            colors_packed[j] |= (colors[i] << 4);
            j++;
        }
        else
        {
            colors_packed[j] = colors[i];
        }
    }
}
