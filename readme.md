# Brief

Some systems don't use interupt or DMA to output debug messages and it's unacceptable to wait for the completion of output in some critical place. So we need to save those messages in bufs and output them when cpu is idel. This is the main function of this component.

# How does it work?

1. Most mcu's ram is smaller than their rom. So I only put the format strings' pointer and other parameters in buf. For example, this piece of code below is used to output the string "hello world 1". "hello world %d\n"'s pointer and parameter "1" will be saved in ram buff, rather the format string "hello world %d\n".

```c
MINI_STR_BUF_WRITE_STRU("hello world %d\n", 1)
```

2. To minimize the ram used by saving the parameters, I use macros to generate stucts to save the parameters. 

```c
MINI_STR_BUF_WRITE_STRU("hello %d %d %d %d %d %d", t0, t1, t2, t3, t4, t5);
```

The code above will be transformed to the code below.

```c
do
{
    typeof(t0) temp_23_6;
    typeof(t1) temp_23_5;
    typeof(t2) temp_23_4;
    typeof(t3) temp_23_3;
    typeof(t4) temp_23_2;
    typeof(t5) temp_23_1;
    ;
    struct gen_auto23_stru
    {
        typeof(temp_23_6) temp_23_6;
        typeof(temp_23_5) temp_23_5;
        typeof(temp_23_4) temp_23_4;
        typeof(temp_23_3) temp_23_3;
        typeof(temp_23_2) temp_23_2;
        typeof(temp_23_1) temp_23_1;
        ;
    } __attribute__((__packed__)) gen_auto23;
    if (sizeof(struct gen_auto23_stru) > 20)
    {
        do
        {
            if (!(0))
            {
                os_kprintf("Assert failed. Condition(%s). [%s][%d]\r\n", "0", __FUNCTION__, 23);
                while (1)
                {
                    ;
                }
            }
        } while (0);
    }
    int macro_paras_num = 6;
    unsigned int macro_paras_size =
        (((sizeof(t0)) << 0) | ((sizeof(t1)) << 4) | ((sizeof(t2)) << 8) | ((sizeof(t3)) << 12) |
            ((sizeof(t4)) << 16) | ((sizeof(t5)) << 20) | ((0) << 24) | ((0) << 28));
    mini_print_stru_w_t mini_print_stru_w;
    static const mini_str_const_t str_const = {
        .str_buf = "L"
                    "23"
                    " "
                    "hello %d %d %d %d %d %d"
                    "\r\n",
        .para_c = 6,
        .para_size = (((sizeof(t0)) << 0) | ((sizeof(t1)) << 4) | ((sizeof(t2)) << 8) | ((sizeof(t3)) << 12) |
                        ((sizeof(t4)) << 16) | ((sizeof(t5)) << 20) | ((0) << 24) | ((0) << 28)),
        .paras_len = sizeof(struct gen_auto23_stru)};
    temp_23_6 = t0;
    temp_23_5 = t1;
    temp_23_4 = t2;
    temp_23_3 = t3;
    temp_23_2 = t4;
    temp_23_1 = t5;
    gen_auto23.temp_23_6 = temp_23_6;
    gen_auto23.temp_23_5 = temp_23_5;
    gen_auto23.temp_23_4 = temp_23_4;
    gen_auto23.temp_23_3 = temp_23_3;
    gen_auto23.temp_23_2 = temp_23_2;
    gen_auto23.temp_23_1 = temp_23_1;
    mini_print_stru_w.str_const_p = &str_const;
    int paras_start = mini_copy_para_into_buf((char *)&gen_auto23, sizeof(struct gen_auto23_stru));
    if (0 <= paras_start)
    {
        mini_print_stru_w.paras_start = paras_start;
    }
    else
    {
        break;
    }
    mini_str_buf_write_stru(&mini_print_stru_w);
} while (0);
```

So parameters "t0" to "t5" will be transformed into "struct gen_auto23_stru" and the values of gen_auto23_stru's members will finally be set as "t0" to "t5". Then I save the "struct gen_auto23_stru" to buff.

3. All parameter structs are save in "mini_paras_buf". By using one ring buf, we can save many ram.
```c
________________________________________________________________________________
↑         ↑_________________↑_____________________________↑                    ↑
start       gen_autoxx_stru   gen_autoxx_stru                                  end
```