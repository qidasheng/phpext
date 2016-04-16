/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2013 The PHP Group                                |
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

/* $Id$ */

#ifndef PHP_QSF_H
#define PHP_QSF_H

extern zend_module_entry qsf_module_entry;
#define phpext_qsf_ptr &qsf_module_entry

#define PHP_QSF_VERSION "0.1.0" /* Replace with version number for your extension */

#ifdef PHP_WIN32
#	define PHP_QSF_API __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >= 4
#	define PHP_QSF_API __attribute__ ((visibility("default")))
#else
#	define PHP_QSF_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

#define QSF_RES_NAME "Qsf Connection"

PHP_MINIT_FUNCTION(qsf);
PHP_MSHUTDOWN_FUNCTION(qsf);
PHP_RINIT_FUNCTION(qsf);
PHP_RSHUTDOWN_FUNCTION(qsf);
PHP_MINFO_FUNCTION(qsf);

PHP_FUNCTION(confirm_qsf_compiled);	/* For testing, remove later. */
PHP_FUNCTION(qsf_test);	/* For testing, remove later. */
PHP_FUNCTION(qsf_hello);	/* For testing, remove later. */
PHP_FUNCTION(qsfclass_connect);	/* For testing, remove later. */
PHP_FUNCTION(qsf_get);	/* For testing, remove later. */

PHP_METHOD(qsfclass,    public_method);  
PHP_METHOD(qsfclass,    private_method);  
PHP_METHOD(qsfclass,    protected_method);
PHP_METHOD(qsfclass,    __construct);     

typedef struct qsf {
	char *hostname;
	int hostname_len;
	int response_code;
	int response_pos;
	char *response_body;
} qsf_t;

 
//  	Declare any global variables you may need between the BEGIN
//	and END macros here:     

ZEND_BEGIN_MODULE_GLOBALS(qsf)
	long  qsf_int;
	char *qsf_str;
	long default_port;
	char *default_charset;
	char *default_host;
	long timeout;
ZEND_END_MODULE_GLOBALS(qsf)

char *get_content(qsf_t *qsf, char *query, int query_len TSRMLS_DC);

/* In every utility function you add that needs to use variables 
   in php_qsf_globals, call TSRMLS_FETCH(); after declaring other 
   variables used by that function, or better yet, pass in TSRMLS_CC
   after the last function argument and declare your utility function
   with TSRMLS_DC after the last declared argument.  Always refer to
   the globals in your function as QSF_G(variable).  You are 
   encouraged to rename these macros something shorter, see
   examples in any other php module directory.
*/

#ifdef ZTS
#define QSF_G(v) TSRMG(qsf_globals_id, zend_qsf_globals *, v)
#else
#define QSF_G(v) (qsf_globals.v)
#endif

#endif	/* PHP_QSF_H */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
