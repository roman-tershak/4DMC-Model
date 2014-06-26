#include "utils.h"
#include "central_logic.h"

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
