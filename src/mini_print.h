#ifndef __EASY_PRINT_H__
#define __EASY_PRINT_H__

#include <string.h>
#include "macro_paras_opt.h"
#include "macro_struct.h"
#include "macro_type.h"

#define STR_PRINT_STRU_W_PARAS_MAX 20

typedef struct mini_str_const
{
    const char *str_buf;
    unsigned int para_size;
    unsigned char para_c;
    unsigned char paras_len;
} mini_str_const_t;

typedef struct mini_print_stru_w
{
    const mini_str_const_t *str_const_p;
    unsigned int time : 31;
    unsigned int flag : 1;
    unsigned char order;
    unsigned char paras_start;
} mini_print_stru_w_t;

#define _GET_TYPE_SIZE(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9)                                                         \
    (((_0) << 0) | ((_1) << 4) | ((_2) << 8) | ((_3) << 12) | ((_4) << 16) | ((_5) << 20) | ((_6) << 24) | ((_7) << 28))
#define GET_TYPE_SIZE(...) _GET_TYPE_SIZE(__VA_ARGS__)

#define _TEMP_PARAS(num, para) para;
#define TEMP_PARAS(num, para)  MACRO_PARAS_NOTDEAL_0PARAS(_TEMP_PARAS, para)

#define MINI_STR_BUF_WRITE_STRU(str, ...)                                                                              \
    do                                                                                                                 \
    {                                                                                                                  \
        MACRO_PARAS_ENUM_OPT_WITHOUT_LAST(TEMP_PARAS, GEN_TEMP_VARS(__VA_ARGS__));                                     \
        GEN_STRUCT_AUTO(GET_TEMP_VARS(__VA_ARGS__));                                                                   \
        if (sizeof(GET_STRUCT_AUTO_TYPE()) > STR_PRINT_STRU_W_PARAS_MAX)                                               \
        {                                                                                                              \
            ;                                                                                              \
        }                                                                                                              \
        int macro_paras_num = GET_MACRO_PARAS_TOTAL_NUM(__VA_ARGS__);                                                  \
        unsigned int macro_paras_size = GET_TYPE_SIZE(GET_MACRO_PARAS_SIZE_FIX(__VA_ARGS__));                          \
        mini_print_stru_w_t mini_print_stru_w;                                                                         \
        static const mini_str_const_t str_const = {.str_buf = "L" MACRO_STR_LINE_NUM " " str "\r\n",                   \
                                                   .para_c = GET_MACRO_PARAS_TOTAL_NUM(__VA_ARGS__),                   \
                                                   .para_size = GET_TYPE_SIZE(GET_MACRO_PARAS_SIZE_FIX(__VA_ARGS__)),  \
                                                   .paras_len = sizeof(GET_STRUCT_AUTO_TYPE())};                       \
        SET_TEMP_VARS(__VA_ARGS__);                                                                                    \
        SET_STRUCT_AUTO(GET_TEMP_VARS(__VA_ARGS__));                                                                   \
        mini_print_stru_w.str_const_p = &str_const;                                                                    \
        int paras_start = mini_copy_para_into_buf((char *)&GET_STRUCT_AUTO_VAR(), sizeof(GET_STRUCT_AUTO_TYPE()));     \
        if (0 <= paras_start)                                                                                          \
        {                                                                                                              \
            mini_print_stru_w.paras_start = paras_start;                                                               \
        }                                                                                                              \
        else                                                                                                           \
        {                                                                                                              \
            break;                                                                                                     \
        }                                                                                                              \
        mini_str_buf_write_stru(&mini_print_stru_w);                                                                   \
    } while (0)

int mini_copy_para_into_buf(char *paras, char paras_len);

void mini_str_buf_write_stru(mini_print_stru_w_t *mini_print_stru_w);

void mini_print_str_stru_buf(void);

#endif
