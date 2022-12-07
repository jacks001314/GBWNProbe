/*
 *
 *      Filename: gbw_util.h
 *
 *        Author: shajf,csp001314@163.com
 *   Description: ---
 *        Create: 2017-01-18 10:36:34
 * Last Modified: 2017-01-18 10:36:34
 */

#ifndef GBW_UTIL_H
#define GBW_UTIL_H

#define UNICODE_ERROR_CHARACTERS_MISSING    -1
#define UNICODE_ERROR_INVALID_ENCODING      -2
#define UNICODE_ERROR_OVERLONG_CHARACTER    -3
#define UNICODE_ERROR_RESTRICTED_CHARACTER  -4
#define UNICODE_ERROR_DECODING_ERROR        -5

#include "gbw_mpool.h"
#include "gbw_tables.h"

#define CREATEMODE_UNISTD ( S_IRUSR | S_IWUSR | S_IRGRP )
#define CREATEMODE ( GBW_UREAD | GBW_UWRITE | GBW_GREAD )



extern char  *gbw_utf8_unicode_inplace_ex(gbw_pool_t *mp, unsigned char *input, long int input_len, int *changed);

extern int  gbw_normalize_path_inplace(unsigned char *input, int len, int win, int *changed);

extern int  gbw_parse_boolean(const char *input);

extern char  *gbw_remove_quotes(gbw_pool_t *mptmp, const char *input, int input_len);


extern char  *gbw_remove_escape(gbw_pool_t *mptmp, const char *input, int input_len);

extern int  gbw_parse_name_eq_value(gbw_pool_t *mp, const char *input, char **name, char **value);

extern char  *gbw_url_encode(gbw_pool_t *mp, char *input, unsigned int input_len, int *changed);

extern char  *gbw_file_dirname(gbw_pool_t *p, const char *filename);

extern char  *gbw_file_basename(gbw_pool_t *p, const char *filename);

extern int  gbw_sql_hex2bytes_inplace(unsigned char *data, int len);

extern int  gbw_hex2bytes_inplace(unsigned char *data, int len);

extern char  *gbw_bytes2hex(gbw_pool_t *pool, unsigned char *data, int len);

extern char  *gbw_bytes2hex_malloc(unsigned char *data, int len);

extern int  gbw_is_token_char(unsigned char c);

extern int  gbw_remove_lf_crlf_inplace(char *text);

extern char  *gbw_guess_tmp_dir(gbw_pool_t *p);

extern char  *gbw_current_logtime(gbw_pool_t *mp);

extern char  *gbw_current_logtime_with_buf(char *buf,size_t buf_size);

extern char  *gbw_current_filetime(gbw_pool_t *mp);

extern unsigned long gbw_get_current_timems(void);

extern int  gbw_mkstemp_ex(char *templat, int mode);

extern int  gbw_mkstemp(char *templat);

extern char  *gbw_strtolower_inplace(unsigned char *str);

extern char  *gbw_log_escape_re(gbw_pool_t *p, const char *text);

extern char  *gbw_log_escape(gbw_pool_t *p, const char *text);

extern char  *gbw_log_escape_nq(gbw_pool_t *p, const char *text);

extern char  *gbw_log_escape_ex(gbw_pool_t *p, const char *text, unsigned long int text_length);

extern char  *gbw_log_escape_nq_ex(gbw_pool_t *p, const char *text, unsigned long int text_length);

extern char  *gbw_log_escape_hex(gbw_pool_t *mp, const unsigned char *text, unsigned long int text_length);

extern char  *gbw_log_escape_raw(gbw_pool_t *mp, const unsigned char *text, unsigned long int text_length);

extern char  *gbw_log_escape_nul(gbw_pool_t *mp, const unsigned char *text, unsigned long int text_length);

extern int  gbw_decode_base64_ext(char *plain_text, const unsigned char *input, int input_len);

extern unsigned char* gbw_encode_base64(gbw_pool_t *mp,const unsigned char *input,int input_len);

extern unsigned char* gbw_encode_base64_malloc(const unsigned char *input,int input_len);

extern int  gbw_convert_to_int(const char c);

extern int  gbw_js_decode_nonstrict_inplace(unsigned char *input, long int input_len);


extern int  gbw_urldecode_nonstrict_inplace_ex(unsigned char *input, long int input_length, int *invalid_count, int *changed);

extern int  gbw_html_entities_decode_inplace(gbw_pool_t *mp, unsigned char *input, int len);

extern int  gbw_ansi_c_sequences_decode_inplace(unsigned char *input, int len);


extern char  *gbw_resolve_relative_path(gbw_pool_t *pool, const char *parent_filename, const char *filename);

extern int  gbw_css_decode_inplace(unsigned char *input, long int input_len);

extern unsigned char gbw_is_netmask_v4(char *ip_strv4);

extern unsigned char gbw_is_netmask_v6(char *ip_strv6);

extern int  gbw_headers_to_buffer(const gbw_array_header_t *arr, char *buffer, int max_length);

extern int gbw_read_line(char *buff, int size, FILE *fp);

extern int gbw_dir_make_r(const char *orig_path);

static inline int gbw_port_equal(uint16_t src_port,uint16_t dst_port,uint16_t v){

    return src_port == v || dst_port == v;
}

static inline int gbw_ports_equal(uint16_t *ports,int n,uint16_t src_port,uint16_t dst_port){

    int i ;

    uint16_t v;

    for(i= 0;i<n;i++){
    
        v = ports[i];
		
		if(v == 0)
			break;

        if(gbw_port_equal(src_port,dst_port,v))
            return 1;
        
    }

    return 0;
}
#endif /* GBW_UTIL_H */
