#include "storage.h"
#include "central_logic.h"
#include "common.h"
#include "safetable.h"
#include "state_storage.h"
#include "hardware.h"

#define PACK    1
#define UNPACK  0


/* Auxiliary functions */
static void pack_unpack_colors(uint8_t what);
static uint8_t check_side_states(void);

extern Side_State sides_states[SIDE_COUNT];

static volatile uint8_t is_saving_states = FALSE;
static uint8_t colors_packed_buff[STATE_DATA_STORAGE_LEN + 1];


uint8_t is_saving(void)
{
    return is_saving_states;
}

void save_state(void)
{
    uint8_t i;
    uint32_t record_num;

    // Prevent saving in parallel
    is_saving_states = TRUE;

#ifndef DEBUG_DONT_STORE
    USART_TRANSMIT_BYTE(0xAA);

    // Get the current bank from the safetale
    record_num = read_safetable_record_num();
    record_num++;

    // Make a copy of the cube state
    pack_unpack_colors(PACK);

    USART_TRANSMIT_BYTE(get_bank_num_storage(record_num));
#endif

#ifdef USART_DEBUG
    if (!check_side_states())
    {
        USART_transmit(0xEC);
        USART_transmit(0xEC);
        USART_transmit(0xEC);
        USART_transmit(0xEC);
    }
#endif

    // Enable interrupts, because storing takes long time
    ENABLE_GLOBAL_INTERRUPTS();

#ifndef DEBUG_DONT_STORE
    // Store state of the cube from the copy
    store_side_states(colors_packed_buff, get_bank_num_storage(record_num));
    
    // Make a record about successful storing into the safetable
    store_safetable_record_num(record_num);

    USART_TRANSMIT_BYTE(0xBB);
#endif

    // Unset 'is_saving' indicator
    is_saving_states = FALSE;
}

uint8_t load_state(void)
{
    uint8_t i;
    uint32_t record_num;

    DISABLE_GLOBAL_INTERRUPTS();
    
    record_num = read_safetable_record_num();

    for (i = STATE_BANK_COUNT; i > 0 && record_num > 0; i--)
    {
        read_side_states(colors_packed_buff, get_bank_num_storage(record_num));
        pack_unpack_colors(UNPACK);

        if (check_side_states())
            break;
        else
            record_num--;
    }
    ENABLE_GLOBAL_INTERRUPTS();

    if (i == 0 || record_num == 0)
        return FALSE;
    else 
        return TRUE;
}

static void pack_unpack_colors(uint8_t what)
{
    uint8_t i, j, byte_bit_index;
    uint16_t buff_bit_index, buff_byte_index, data;
    uint8_t *colors;

    for (i = 0; i < SIDE_COUNT; i++)
    {
        colors = sides_states[i].colors;
        buff_bit_index = (uint16_t) i * SIDE_STICKER_COUNT * 3;

        for (j = 0; j < SIDE_STICKER_COUNT; j++)
        {
            //   0          1           2          3           4           5
            // |xxx,xxx,xx|x,xxx,xxx,x|xx,xxx,xxx,|xxx,xxx,xx|x,xxx,xxx,x|xx,...
            //   0   1   2    3   4   5    6   7    8   9  10   11  12   13   
            buff_byte_index = buff_bit_index / 8;
            byte_bit_index = buff_bit_index % 8;

            // 'data' will contain this and the next byte
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

            buff_bit_index += 3;
        } 
    }
}

static uint8_t check_side_states(void)
{
    uint8_t i, j;
    uint8_t color_counts[MAIN_COLOR_COUNT] = {0};

    for (i = 0; i < SIDE_COUNT; i++)
    {
        for (j = 0; j < SIDE_STICKER_COUNT; j++)
        {
            color_counts[sides_states[i].colors[j]]++;
        }
    }
    for (i = 0; i < MAIN_COLOR_COUNT; i++)
    {
        // There should be exactly SIDE_CUBES_COUNT stickers of each color
        if (color_counts[i] != SIDE_STICKER_COUNT)
            return FALSE;
    }
    return TRUE;
}
