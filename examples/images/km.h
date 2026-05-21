#ifndef KM_H
#define KM_H

#include "gliv.h"

static const uint32_t km_image_data[] = {
    0xfff3fbdb, 0xdbdb7f6c, 0x6dbdb6f6, 0x3
};

static const gliv_image_res_t km_image_res = {
    .width  = 14,
    .height = 7,
    .data   = km_image_data
};

#endif /* KM_H */