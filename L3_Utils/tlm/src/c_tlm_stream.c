/*
 *     SocialLedge.com - Copyright (C) 2013
 *
 *     This file is part of free software framework for embedded processors.
 *     You can use it and/or distribute it as long as this copyright header
 *     remains unmodified.  The code is free for personal use and requires
 *     permission to use in a commercial product.
 *
 *      THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 *      OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 *      MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 *      I SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR
 *      CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 *     You can reach the author of this software at :
 *          p r e e t . w i k i @ g m a i l . c o m
 */
#include "sys_config.h"
#if ENABLE_TELEMETRY
#include "c_tlm_stream.h"
#include "c_tlm_var.h"
#include <string.h> /* strlen() etc. */
#include <stdlib.h> /* atoi() */
#include <ctype.h>  /* tolower() isdigit() etc. */

static uint8_t hex_to_byte(const char *two_digit_hex)
{
    uint8_t d1 = *(two_digit_hex + 0);
    uint8_t d2 = *(two_digit_hex + 1);
    uint8_t value = 0;

    if (isdigit(d1)) value += d1 - '0';
    else value += tolower(d1) - 'a' + 10;

    value <<= 4;

    if (isdigit(d2)) value += d2 - '0';
    else value += tolower(d2) - 'a' + 10;

    return value;
}

static void tlm_stream_file_ptr(const char *str, void *fptr)
{
    fwrite(str, sizeof(char), strlen(str), (FILE*)fptr);
}

/**
 * Callback function for each component's variables
 */
static c_bool tlm_stream_for_each_component_var(void *elm_ptr, void *arg1, void *arg2, void *arg3)
{
    char buff[16] = { 0 };
    tlm_reg_var_type *var = elm_ptr;
    stream_callback_type stream = arg1;
    void *stream_arg = arg2;
    char *p = (char*)(var->data_ptr);
    uint32_t i = 0;

    if (NULL == stream || NULL == var) {
        return c_false;
    }

    stream((var->name), stream_arg);
    stream(":", stream_arg);
    sprintf(buff, "%u:", var->elm_size_bytes);
    stream(buff, stream_arg);
    sprintf(buff, "%u:", var->elm_arr_size);
    stream(buff, stream_arg);
    sprintf(buff, "%u:", var->elm_type);
    stream(buff, stream_arg);

    /* A variable has at least one data byte */
    sprintf(buff, "%02X", ((*p++) & 0xFF));
    stream(buff, stream_arg);

    /* Stream rest of the data bytes */
    for(i=(var->elm_size_bytes) * (var->elm_arr_size); /* Total bytes */
        i > 1; i--) { /* note: 1 byte already streamed above */
        sprintf(buff, ",%02X", ((*p++) & 0xFF) );
        stream(buff, stream_arg);
    }
    stream("\n", stream_arg);

    return c_true;
}

/**
 * Callback function for each component
 * @param sca  Stream callback argument
 */
void tlm_stream_one(tlm_component *comp, stream_callback_type stream, void *sca)
{
    if (NULL == comp || NULL == stream) {
        return;
    }

    /* sca : stream callback argument */
    char buff[16] = { 0 };
    sprintf(buff, "%u\n", (unsigned int)c_list_node_count((comp->var_list)));

    /* Send: "START:<name>:<#>\n" */
    stream("START:", sca);
    stream(comp->name, sca);
    stream(":", sca);
    stream(buff, sca);

    /* Now for each variable list of this component, make a call-back to our
     * component for each function that will stream data of each variable
     */
    c_list_for_each_elm((comp->var_list), tlm_stream_for_each_component_var,
                        stream, sca, NULL);

    /* Send: "END:<name>\n" */
    stream("END:", sca);
    stream((comp->name), sca);
    stream("\n", sca);
}

void tlm_stream_all(stream_callback_type stream_func, void *arg)
{
    /*
     * Simply tell the telemetry component to call our call-back with our
     * stream function as the argument.  We will get the callback function
     * called with pointer to each telemetry component, along with our
     * argument which is the stream function itself
     */
    tlm_component_for_each((tlm_comp_callback)tlm_stream_one,
                           (void*)stream_func, arg);
}

void tlm_stream_one_file(tlm_component *comp_ptr, FILE *file)
{
    if(file) {
        tlm_stream_one(comp_ptr, tlm_stream_file_ptr, file);
    }
}

void tlm_stream_all_file(FILE *file)
{
    if(file) {
        tlm_component_for_each((tlm_comp_callback)tlm_stream_one,
                               (void*)tlm_stream_file_ptr, file);
    }
}

c_bool tlm_stream_decode(char *line)
{
    if (NULL == line) {
        return c_false;
    }

    static uint32_t num_vars_in_stream = 0;
    static tlm_component *p_comp = NULL;

    /* Only change num_vars_in_stream when we detect a valid start of a stream
     * Any time we detect failure, we reset this variable back to 0.
     */
    if (0 == num_vars_in_stream) {
        if (line == strstr(line, "START:")) {
            char *name = strstr(line, ":") + 1;
            char *count = strstr(name, ":");
            if (NULL != count) {
                *count++ = '\0';
                p_comp = tlm_component_get_by_name(name);
                if (NULL != p_comp) {
                    num_vars_in_stream = atoi(count);
                    return c_true;
                }
            }
        }
    }
    else {
        /* Stream is: <Description>:<Var Size in Bytes>:<Array size>:<HEX Bytes>\n */
        --num_vars_in_stream;
        char *itr = strstr(line, ":");
        char *name = NULL;
        uint32_t exp_byte_cnt = 0; /* expected byte count */

        if (NULL != itr) {
            *itr++ = '\0';
            name = line;

            /* itr should be at the var size */
            if (*itr != '\0') {
                exp_byte_cnt = atoi(itr);
                itr = strstr(itr, ":");
            } else { goto recover; }

            /* itr should be at var array size after ++itr */
            if (itr && *itr != '\0') {
                ++itr;
                exp_byte_cnt *= atoi(itr);
                itr = strstr(itr, ":");
            } else { goto recover; }

            /* itr should be at var type after ++itr */
            if (itr && *itr != '\0') {
                ++itr;
                itr = strstr(itr, ":");
            } else { goto recover; }

        } else { goto recover; }

        /* itr should be at the hex date so decode 01,02... */
        const tlm_reg_var_type *reg_var = tlm_variable_get_by_name(p_comp, name);
        if (NULL != itr &&
            NULL != reg_var &&
            (reg_var->elm_size_bytes) * (reg_var->elm_arr_size) == exp_byte_cnt)
        {
            char *dst = (char*)(reg_var->data_ptr);
            for ( ; exp_byte_cnt != 0 && strlen(itr) >= 3; --exp_byte_cnt) {
                itr++; /* Should be : or , */
                char byte = hex_to_byte(itr);
                itr += 2; /* Skip the 2 hex digits we just decoded */
                memcpy(dst, &byte, sizeof(byte));
                dst += sizeof(byte);
            }
        } else { goto recover; }
    }

    return c_false;

    recover :
        num_vars_in_stream = 0;
        return c_false;
}

c_bool tlm_stream_decode_file(FILE *file)
{
    char line[256] = { 0 };

    /* First line of a file should return true from tlm_stream_decode()
     * if the telemetry format matches, otherwise return with an error
     */
    if (fgets(line, sizeof(line)-1, file)) {
        if (!tlm_stream_decode(line)) {
            return c_false;
        }
    }
    else {
        return c_false;
    }

    while (fgets(line, sizeof(line)-1, file)) {
        tlm_stream_decode(line);
    }

    return c_true;
}
#endif
