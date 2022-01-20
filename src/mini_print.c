#include <stdarg.h>
#include <stdio.h>

#include "mini_print.h"

#define MINI_PARAS_BUF_MAX 0x80
static char mini_paras_buf[MINI_PARAS_BUF_MAX];
static char mini_paras_buf_write;
static char mini_paras_buf_read;

#define MINI_PARAS_BUF_EMPTY (mini_paras_buf_read == mini_paras_buf_write)
#define MINI_PARAS_BUF_LEAVE                                                                                           \
    ((mini_paras_buf_read + MINI_PARAS_BUF_MAX - mini_paras_buf_write) & (MINI_PARAS_BUF_MAX - 1))
#define MINI_PARAS_BUF_USED                                                                                            \
    ((mini_paras_buf_write + MINI_PARAS_BUF_MAX - mini_paras_buf_read) & (MINI_PARAS_BUF_MAX - 1))

#define STR_PRINT_R_PARAS_MAX 8

#define os_irq_lock(...)   0
#define os_irq_unlock(...)
#define os_tick_get(...)    0

struct str_print_r
{
    const char *str_buf;
    unsigned char order;
    unsigned char para_c;
    unsigned int time;
    unsigned int para[STR_PRINT_R_PARAS_MAX];
};

#define MINI_PRINT_STRU_W_BUF_LEN 0x20

static mini_print_stru_w_t s_mini_print_stru_w_buf[MINI_PRINT_STRU_W_BUF_LEN];

static int s_mini_str_stru_w_buf_num_w = 0;
static int s_mini_str_stru_w_buf_num_r = 0;

static unsigned char print_order;

int mini_copy_para_into_buf(char *paras, char paras_len)
{
    if (!paras_len)
    {
        return 0;
    }

    unsigned int level = os_irq_lock();

    if ((!MINI_PARAS_BUF_EMPTY) && (paras_len >= MINI_PARAS_BUF_LEAVE))
    {
        os_irq_unlock(level);
        return -1;
    }

    unsigned char paras_buf_write_old = mini_paras_buf_write;
    unsigned char buf_left_len = MINI_PARAS_BUF_MAX - paras_buf_write_old;

    if (paras_len > buf_left_len)
    {
        unsigned char paras_len_leave = paras_len - buf_left_len;
        mini_paras_buf_write = paras_len_leave;
        os_irq_unlock(level);
        memcpy(&mini_paras_buf[paras_buf_write_old], &paras[0], buf_left_len);
        memcpy(&mini_paras_buf[0], &paras[buf_left_len], paras_len_leave);
    }
    else
    {
        mini_paras_buf_write = mini_paras_buf_write + paras_len;
        os_irq_unlock(level);
        memcpy(&mini_paras_buf[paras_buf_write_old], &paras[0], paras_len);
    }
    return paras_buf_write_old;
}

void mini_copy_para_out_buf(char *paras, unsigned char paras_start, unsigned char paras_len)
{
    if (!paras_len)
    {
        return;
    }

    unsigned char buf_left_len = MINI_PARAS_BUF_MAX - paras_start;

    if (paras_len > buf_left_len)
    {
        unsigned char para_len_leave = paras_len - buf_left_len;
        mini_paras_buf_write = para_len_leave;
        memcpy(&paras[0], &mini_paras_buf[paras_start], buf_left_len);
        memcpy(&paras[buf_left_len], &mini_paras_buf[0], para_len_leave);
    }
    else
    {
        mini_paras_buf_write = mini_paras_buf_write + paras_len;
        memcpy(&paras[0], &mini_paras_buf[paras_start], paras_len);
    }
}

void mini_str_buf_write_stru(mini_print_stru_w_t *str_print_stru_w)
{
    unsigned int time = os_tick_get();

    unsigned int level = os_irq_lock();
    int str_stru_w_buf_num_w_get = s_mini_str_stru_w_buf_num_w;
    s_mini_str_stru_w_buf_num_w++;
    if (MINI_PRINT_STRU_W_BUF_LEN == s_mini_str_stru_w_buf_num_w)
    {
        s_mini_str_stru_w_buf_num_w = 0;
    }
    int print_order_get = print_order++;
    os_irq_unlock(level);

    mini_print_stru_w_t *str_print_stru_w_p = &s_mini_print_stru_w_buf[str_stru_w_buf_num_w_get];

    memcpy(str_print_stru_w_p, str_print_stru_w, sizeof(mini_print_stru_w_t));
    str_print_stru_w_p->order = print_order_get;
    str_print_stru_w_p->time = time;

    // level = os_irq_lock();
    str_print_stru_w_p->flag = 1;
    // os_irq_unlock(level);
    return;
}

void mini_str_stru_buf_read(struct str_print_r *str_print_r)
{
    int i;
    int size;
    int y;
    int offset;

    mini_print_stru_w_t *str_print_stru_r_p = &s_mini_print_stru_w_buf[s_mini_str_stru_w_buf_num_r];
    s_mini_str_stru_w_buf_num_r++;
    if (MINI_PRINT_STRU_W_BUF_LEN == s_mini_str_stru_w_buf_num_r)
    {
        s_mini_str_stru_w_buf_num_r = 0;
    }

    str_print_r->str_buf = str_print_stru_r_p->str_const_p->str_buf;
    str_print_r->order = str_print_stru_r_p->order;
    str_print_r->para_c = str_print_stru_r_p->str_const_p->para_c;
    str_print_r->time = str_print_stru_r_p->time;

    str_print_stru_r_p->flag = 0;

    char paras[32];
    mini_copy_para_out_buf(paras, str_print_stru_r_p->paras_start, str_print_stru_r_p->str_const_p->paras_len);

    for (i = 0, offset = 0; i < str_print_r->para_c; i++)
    {
        y = (i < 8) ? i : (i - 8);
        size = ((str_print_stru_r_p->str_const_p->para_size) >> (y << 2)) & 0x0F;
        switch (size)
        {
        case 1:
            str_print_r->para[i] = *(unsigned char *)&paras[offset];
            break;
        case 2:
            str_print_r->para[i] = *(unsigned short *)&paras[offset];
            break;
        case 4:
            str_print_r->para[i] = *(unsigned int *)&paras[offset];
            break;
        // case 8:
        //     break;
        default:
            break;
        }
        offset += size;
    }

    return;
}

int mini_str_stru_buf_is_empty(void)
{
    int level = os_irq_lock();
    mini_print_stru_w_t *str_print_stru_r_p = &s_mini_print_stru_w_buf[s_mini_str_stru_w_buf_num_r];
    int ret = (s_mini_str_stru_w_buf_num_r == s_mini_str_stru_w_buf_num_w) ? 1 : !str_print_stru_r_p->flag;
    os_irq_unlock(level);

    return ret;
}

void mini_print_str_stru_buf(void)
{
    unsigned char order;
    struct str_print_r str_print_r;

    while (!mini_str_stru_buf_is_empty())
    {
        mini_str_stru_buf_read(&str_print_r);
        printf("%03d\t%05d\t", str_print_r.order, str_print_r.time);
        printf(str_print_r.str_buf,
               str_print_r.para[0],
               str_print_r.para[1],
               str_print_r.para[2],
               str_print_r.para[3],
               str_print_r.para[4],
               str_print_r.para[5]);
    }
    return;
}

