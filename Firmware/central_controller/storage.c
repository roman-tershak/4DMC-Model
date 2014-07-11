#include "storage.h"
#include "central_logic.h"
#include "common.h"
#include "safetable.h"
#include "state_storage.h"
#include "driver.h"


#define CANNOT_BE_SAVED (ROTATING)
#define PACK    1
#define UNPACK  0


/* Auxiliary functions */
static void unpack_colors(uint8_t *colors_packed, uint8_t *colors);

static void pack_colors(uint8_t *colors, uint8_t *colors_packed);


extern volatile Side_State sides_states[SIDE_COUNT];

static volatile uint8_t is_saving_states = FALSE;
static uint8_t colors_packed_buff[STATE_DATA_STORAGE_LEN];


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

    // Prevent saving in parallel
    is_saving_states = TRUE;
    
    // Get the current bank from the safetale
    record_num = read_safetable_record_num();
    record_num++;
    
    // Make a copy of the cube state
    pack_unpack_colors(PACK);

    // 'Release' sides that were waiting for saving
    for (i = 0; i < SIDE_COUNT; i++)
    {
        if (sides_states[i].status == WAITING_FOR_SAVING)
            sides_states[i].status = SIDE_IDLE;
    }

    // Enable interrupts, because storing takes long time
    ENABLE_GLOBAL_INTERRUPTS;

    // Store state of the cube from the copy
    store_side_state(colors_packed_buff, get_bank_num_storage(record_num));
    
    // Make a record about successful storing into the safetable
    store_safetable_record_num(record_num);

    // Unset 'is_saving' indicator
    is_saving_states = FALSE;
}

static void pack_unpack_colors(uint8_t what)
{
    uint8_t i, j, byte_bit_index;
    uint16_t buff_bit_index, buff_byte_index, data;
    uint8_t *colors;

    for (i = 0; i < SIDE_COUNT; i++)
    {
        colors = sides_states[i].colors;
        buff_bit_index = (uint16_t) i * SIDE_CUBES_COUNT * 3;

        for (j = 0; j < SIDE_CUBES_COUNT; j++)
        {
            //   0          1           2          3           4           5
            // |xxx,xxx,xx|x,xxx,xxx,x|xx,xxx,xxx|,xxx,xxx,xx|x,xxx,xxx,x|xx,...
            //   0   1   2    3   4   5    6   7    8   9  10   11  12   13   
            buff_bit_index += j * 3;

            buff_byte_index = buff_bit_index / 8;
            byte_bit_index = buff_bit_index % 8;

            // 'data' will contain this and the subsequent bytes
            data = 0;
            data |= colors_packed_buff[buff_byte_index];
            data |= ((uint16_t) colors_packed_buff[buff_byte_index + 1]) << 8;

            if (what)
            {
                // Write/pack block
                // Clear the place
                data &= ~((uint16_t) 0x7 << byte_bit_index);
                // Put the color from j'th cell into the place
                data |= ((uint16_t) colors[j] & 0x7) << byte_bit_index;

                // Write 'data' var back to colors_packed_buff
                colors_packed_buff[buff_byte_index] = (uint8_t) data;
                colors_packed_buff[buff_byte_index + 1] = (uint8_t) (data >> 8);
            }
            else
            {
                // read/unpack section
                colors[j] = (uint8_t) ((data & ((uint16_t) 0x7 << byte_bit_index)) >> byte_bit_index);
            }
        }
    }
}

