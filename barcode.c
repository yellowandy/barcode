/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2010 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author:                                                              |
  +----------------------------------------------------------------------+
*/

/* $Id: header 297205 2010-03-30 21:09:07Z johannes $ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_barcode.h"

#include <zbar.h>
#include <png.h>


/* If you declare any globals in php_barcode.h uncomment this:
ZEND_DECLARE_MODULE_GLOBALS(barcode)
*/

/* True global resources - no need for thread safety here */
static int le_barcode;

/* {{{ barcode_functions[]
 *
 * Every user visible function must have an entry in barcode_functions[].
 */
const zend_function_entry barcode_functions[] = {
	PHP_FE(confirm_barcode_compiled,	NULL)		/* For testing, remove later. */
	PHP_FE(scan_image,	NULL)		
	{NULL, NULL, NULL}	/* Must be the last line in barcode_functions[] */
};
/* }}} */

/* {{{ barcode_module_entry
 */
zend_module_entry barcode_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
	STANDARD_MODULE_HEADER,
#endif
	"barcode",
	barcode_functions,
	PHP_MINIT(barcode),
	PHP_MSHUTDOWN(barcode),
	PHP_RINIT(barcode),		/* Replace with NULL if there's nothing to do at request start */
	PHP_RSHUTDOWN(barcode),	/* Replace with NULL if there's nothing to do at request end */
	PHP_MINFO(barcode),
#if ZEND_MODULE_API_NO >= 20010901
	"0.1", /* Replace with version number for your extension */
#endif
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_BARCODE
ZEND_GET_MODULE(barcode)
#endif

/* {{{ PHP_INI
 */
/* Remove comments and fill if you need to have entries in php.ini
PHP_INI_BEGIN()
    STD_PHP_INI_ENTRY("barcode.global_value",      "42", PHP_INI_ALL, OnUpdateLong, global_value, zend_barcode_globals, barcode_globals)
    STD_PHP_INI_ENTRY("barcode.global_string", "foobar", PHP_INI_ALL, OnUpdateString, global_string, zend_barcode_globals, barcode_globals)
PHP_INI_END()
*/
/* }}} */

/* {{{ php_barcode_init_globals
 */
/* Uncomment this function if you have INI entries
static void php_barcode_init_globals(zend_barcode_globals *barcode_globals)
{
	barcode_globals->global_value = 0;
	barcode_globals->global_string = NULL;
}
*/
/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(barcode)
{
	/* If you have INI entries, uncomment these lines 
	REGISTER_INI_ENTRIES();
	*/
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(barcode)
{
	/* uncomment this line if you have INI entries
	UNREGISTER_INI_ENTRIES();
	*/
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request start */
/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(barcode)
{
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request end */
/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(barcode)
{
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(barcode)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "barcode support", "enabled");
	php_info_print_table_end();

	/* Remove comments if you have entries in php.ini
	DISPLAY_INI_ENTRIES();
	*/
}
/* }}} */


/* Remove the following function when you have succesfully modified config.m4
   so that your module can be compiled into PHP, it exists only for testing
   purposes. */

/* Every user-visible function in PHP should document itself in the source */
/* {{{ proto string confirm_barcode_compiled(string arg)
   Return a string to confirm that the module is compiled in */
PHP_FUNCTION(confirm_barcode_compiled)
{
	char *arg = NULL;
	int arg_len, len;
	char *strg;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &arg, &arg_len) == FAILURE) {
		return;
	}

	len = spprintf(&strg, 0, "Congratulations! You have successfully modified ext/%.78s/config.m4. Module %.78s is now compiled into PHP.", "barcode", arg);
	RETURN_STRINGL(strg, len, 0);
}
/* }}} */

PHP_FUNCTION(scan_image)
{
	char *arg = NULL;
	int arg_len, len;
	char *strg;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &arg, &arg_len) == FAILURE) {
		return;
	}
	
	scan_image(arg, return_value);
	//len = spprintf(&strg, 0, "Image Scanned!! You have successfully modified ext/%.78s/config.m4. Module %.78s is now compiled into PHP.", "barcode", arg);
	//RETURN_STRINGL(strg, len, 0);
	
//	RETURN_TRUE;
}

void scan_image(const char * fileName, zval * results)
{
	/* create a reader */
	zbar_image_scanner_t * scanner;
    scanner = zbar_image_scanner_create();
	
    /* configure the reader */
    zbar_image_scanner_set_config(scanner, 0, ZBAR_CFG_ENABLE, 1);
	
    /* obtain image data */
    int width = 0, height = 0;
    void *raw = NULL;
    get_data(fileName, &width, &height, &raw);
	
    /* wrap image data */
    zbar_image_t *image = zbar_image_create();
    zbar_image_set_format(image, *(int*)"Y800");
    zbar_image_set_size(image, width, height);
    zbar_image_set_data(image, raw, width * height, zbar_image_free_data);
	
    /* scan the image for barcodes */
    int n = zbar_scan_image(scanner, image);
	
    /* extract results */
	array_init(results);
    const zbar_symbol_t *symbol = zbar_image_first_symbol(image);
    for(; symbol; symbol = zbar_symbol_next(symbol)) 
	{
		zval *sub_array;
		MAKE_STD_ZVAL(sub_array);
		array_init(sub_array);
		
        /* do something useful with results */
        zbar_symbol_type_t typ = zbar_symbol_get_type(symbol);
        const char *data = zbar_symbol_get_data(symbol);
		
		add_assoc_string(sub_array, "symbol", zbar_get_symbol_name(typ), 1); 
		add_assoc_string(sub_array, "value", data, 1); 
		
		add_next_index_zval(results, sub_array);
    }
	
    /* clean up */
    zbar_image_destroy(image);
	
}

static void get_data (const char *name, int *width, int *height, void **data)
{
    FILE *file = fopen(name, "rb");
    if(!file) 
		exit(2);
    png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING,NULL, NULL, NULL);
	
    if(!png) 
		exit(3);
	
    if(setjmp(png_jmpbuf(png))) 
		exit(4);
    
	png_infop info = png_create_info_struct(png);
    if(!info) 
		exit(5);
    
	png_init_io(png, file);
    png_read_info(png, info);
    
	/* configure for 8bpp grayscale input */
    int color = png_get_color_type(png, info);
    int bits = png_get_bit_depth(png, info);
	
    if(color & PNG_COLOR_TYPE_PALETTE)
        png_set_palette_to_rgb(png);
	
    if(color == PNG_COLOR_TYPE_GRAY && bits < 8)
        png_set_expand_gray_1_2_4_to_8(png);
	
    if(bits == 16)
        png_set_strip_16(png);
	
    if(color & PNG_COLOR_MASK_ALPHA)
        png_set_strip_alpha(png);
	
    if(color & PNG_COLOR_MASK_COLOR)
        png_set_rgb_to_gray_fixed(png, 1, -1, -1);
	
    /* allocate image */
    *width = png_get_image_width(png, info);
    *height = png_get_image_height(png, info);
    *data = malloc(*width * *height);
    png_bytep rows[*height];
    int i;
    for(i = 0; i < *height; i++)
        rows[i] = *data + (*width * i);
    png_read_image(png, rows);
}




/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
