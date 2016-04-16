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
//http://www.walu.cc/phpbook/9.1.md
/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_qsf.h"

#define QSF_PUT_OK 			0
#define QSF_PUT_ERROR 		1
#define QSF_PUT_END 		2
#define QSF_GET_END 		3
#define QSF_RESET_OK 		4
#define QSF_RESET_ERROR 	5
#define QSF_MAXQUEUE_OK		6
#define QSF_MAXQUEUE_CANCEL 7
#define QSF_SYNCTIME_OK		8
#define QSF_SYNCTIME_CANCEL 9
#define QSF_ERROR			10



ZEND_DECLARE_MODULE_GLOBALS(qsf)

/* True global resources - no need for thread safety here */
static int le_qsf;

/* {{{ qsf_functions[]
 *
 * Every user visible function must have an entry in qsf_functions[].
 */
const zend_function_entry qsf_functions[] = {
	PHP_FE(confirm_qsf_compiled,	NULL)		/* For testing, remove later. */
	PHP_FE(qsf_test,	NULL)/* For testing, remove later. */
	PHP_FE(qsf_hello,	NULL)/* For testing, remove later. */
        {NULL, NULL, NULL} 
	//PHP_FE_END	/* Must be the last line in qsf_functions[] */
};
/* }}} */



zend_class_entry *qsfclass_ce;
static zend_function_entry qsfclass_method[] = {
    ZEND_ME(qsfclass,    public_method,     NULL,   ZEND_ACC_PUBLIC)
    ZEND_ME(qsfclass,    private_method,    NULL,   ZEND_ACC_PRIVATE)
    ZEND_ME(qsfclass,    protected_method,  NULL,   ZEND_ACC_PROTECTED)
    ZEND_ME(qsfclass,    __construct,       NULL,   ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
    PHP_FALIAS(test,     qsf_test, NULL)
    PHP_FALIAS(connect,  qsfclass_connect, NULL)
    PHP_FALIAS(get,      qsf_get, NULL)
    { NULL, NULL, NULL }
};



/* {{{ qsf_module_entry
 */
zend_module_entry qsf_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
	STANDARD_MODULE_HEADER,
#endif
	"qsf",
	qsf_functions,
	PHP_MINIT(qsf),
	PHP_MSHUTDOWN(qsf),
	PHP_RINIT(qsf),		/* Replace with NULL if there's nothing to do at request start */
	PHP_RSHUTDOWN(qsf),	/* Replace with NULL if there's nothing to do at request end */
	PHP_MINFO(qsf),
#if ZEND_MODULE_API_NO >= 20010901
	PHP_QSF_VERSION,
#endif
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_QSF
ZEND_GET_MODULE(qsf)
#endif

//{{{ PHP_INI

// Remove comments and fill if you need to have entries in php.ini
PHP_INI_BEGIN()
    STD_PHP_INI_ENTRY("qsf.qsf_int",      "42", PHP_INI_ALL, OnUpdateLong, qsf_int, zend_qsf_globals, qsf_globals)
    STD_PHP_INI_ENTRY("qsf.qsf_str",  "foobar", PHP_INI_ALL, OnUpdateString, qsf_str, zend_qsf_globals, qsf_globals)
PHP_INI_END()
// }}} 

/* {{{ php_qsf_init_globals
 */
//Uncomment this function if you have INI entries
//static void php_qsf_init_globals(zend_qsf_globals *qsf_globals TSRMLS_DC)
static void php_qsf_init_globals(zend_qsf_globals *qsf_globals_p TSRMLS_DC)
{
	//qsf_globals->qsf_int = 0;
	//qsf_globals->qsf_str = NULL;
	QSF_G(qsf_int) = 0;
	QSF_G(qsf_str) = NULL;
	QSF_G(default_port) = 1218;
	QSF_G(default_charset) = "utf-8";
	QSF_G(default_host) = "127.0.0.1";
	QSF_G(timeout) = 20;
}

static void php_qsf_dtor(zend_rsrc_list_entry *rsrc TSRMLS_DC) {
	qsf_t *qsf = (qsf_t *)rsrc->ptr;
	efree(qsf->hostname);
	efree(qsf);
}

/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(qsf)
{
        zend_class_entry ce;
        INIT_CLASS_ENTRY(ce, "qsfclass", qsfclass_method);
        qsfclass_ce = zend_register_internal_class(&ce TSRMLS_CC);
        zend_declare_property_null(qsfclass_ce, "link", strlen("link"), ZEND_ACC_PUBLIC TSRMLS_CC);
        zend_declare_property_null(qsfclass_ce, "public_var", strlen("public_var"), ZEND_ACC_PUBLIC TSRMLS_CC);
        zend_declare_property_null(qsfclass_ce, "private_var", strlen("private_var"), ZEND_ACC_PRIVATE TSRMLS_CC);
        zend_declare_property_null(qsfclass_ce, "protected_var", strlen("protected_var"), ZEND_ACC_PROTECTED TSRMLS_CC);

#ifdef ZTS
	ts_allocate_id(&qsf_globals_id, sizeof(zend_qsf_globals), (ts_allocate_ctor) php_qsf_init_globals, NULL);
#else
	php_qsf_init_globals(&qsf_globals TSRMLS_CC);
#endif

	le_qsf = zend_register_list_destructors_ex(php_qsf_dtor, NULL, QSF_RES_NAME, module_number);

 
	//* If you have INI entries, uncomment these lines 
	REGISTER_INI_ENTRIES();
	//*/
	return SUCCESS;
}

//ZEND_MINIT_FUNCTION(qsf)
//{
//        zend_class_entry ce;
//        INIT_CLASS_ENTRY(ce, "qsfclass", qsfclass_method);
//        qsfclass_ce = zend_register_internal_class(&ce TSRMLS_CC);
//        zend_declare_property_null(qsfclass_ce, "public_var", strlen("public_var"), ZEND_ACC_PUBLIC TSRMLS_CC);
//        zend_declare_property_null(qsfclass_ce, "private_var", strlen("private_var"), ZEND_ACC_PRIVATE TSRMLS_CC);
//        zend_declare_property_null(qsfclass_ce, "protected_var", strlen("protected_var"), ZEND_ACC_PROTECTED TSRMLS_CC);
//        return SUCCESS;
//}

// }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(qsf)
{
	///* uncomment this line if you have INI entries
	UNREGISTER_INI_ENTRIES();
	//*/
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request start */
/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(qsf)
{
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request end */
/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(qsf)
{
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(qsf)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "qsf support", "enabled");
	php_info_print_table_end();

	//* Remove comments if you have entries in php.ini
	DISPLAY_INI_ENTRIES();
	//*/
}
/* }}} */


/* Remove the following function when you have successfully modified config.m4
   so that your module can be compiled into PHP, it exists only for testing
   purposes. */

/* Every user-visible function in PHP should document itself in the source */
/* {{{ proto string confirm_qsf_compiled(string arg)
   Return a string to confirm that the module is compiled in */
PHP_FUNCTION(confirm_qsf_compiled)
{
	char *arg = NULL;
	int arg_len, len;
	char *strg;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &arg, &arg_len) == FAILURE) {
		return;
	}

	len = spprintf(&strg, 0, "Congratulations! You have successfully modified ext/%.78s/config.m4. Module %.78s is now compiled into PHP.", "qsf", arg);
	RETURN_STRINGL(strg, len, 0);
}


PHP_FUNCTION(qsf_test)
{
	QSF_G(qsf_int) = 1000000;
        char *qsf_str = NULL;
        int qsf_str_len, len, qsf_int;
        char *strg;

        if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sl", &qsf_str, &qsf_str_len, &qsf_int) == FAILURE) {
                return;
        }

        len = spprintf(&strg, 0, "result %s & %d", qsf_str, qsf_int);
        php_printf("Hello ");
        RETURN_STRINGL(strg, len, 0);
}

PHP_FUNCTION(qsf_hello)
{
        char *qsf_str = NULL;
        int qsf_str_len, len;
        char *strg;

        if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &qsf_str, &qsf_str_len) == FAILURE) {
                return;
        }

        len = spprintf(&strg, 0, "result %s & %d", qsf_str, QSF_G(qsf_int));
        php_printf("Hello ");
        RETURN_STRINGL(strg, len, 0);
}

char *chg(char *input, char *out) {
        //php_printf("1>>>>>>>>>>>>\n");
        if (input == NULL) {
                input = (char *)emalloc(strlen(out) * sizeof(char));
                memcpy(input, out, strlen(out));
                return input;
        }

        //php_printf("2>>>>>>>>>>>>\n");
        if (out == NULL) {
                return input;
        }

        //php_printf("3>>>>>>>>>>>>\n");
        if ( input == NULL && out == NULL ) {
                return NULL;
        }

        //php_printf("%s\n", input);
        //php_printf("%s\n", out);
        //php_printf("###########\n");
        input = (char *)erealloc(input, (strlen(input) + strlen(out) + 1) * sizeof(char));
        strcat(input, out);
        return input;
}

PHP_FUNCTION(qsfclass_connect) {
    zval *object = getThis();
    qsf_t *qsf;
    char *hostname, *host = QSF_G(default_host);
    long port = QSF_G(default_port);
    int hostname_len, host_len, list_id;

    zval *link;
    zend_class_entry *ce;
    ce = Z_OBJCE_P(getThis());
    
    php_stream *stream;
    char *errstr = NULL;
    int errcode = 0;
    int options = (ENFORCE_SAFE_MODE | REPORT_ERRORS, STREAM_XPORT_CLIENT | STREAM_XPORT_CONNECT);
    int flags = STREAM_XPORT_CLIENT | STREAM_XPORT_CONNECT;
    struct timeval tv;
    tv.tv_sec = QSF_G(timeout);
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sl", &host, &host_len, &port) == FAILURE) {
        return;
    }
    
    php_printf("3>>>>>>>>>>>>\n");

    hostname_len = spprintf(&hostname, 0, "%s%s:%d", "tcp://", host, port);
    php_printf("4:%s#\n", hostname);
    qsf = emalloc(sizeof(qsf_t));
    qsf->hostname = (char *)emalloc((strlen(hostname) + 1) * sizeof(char));
    memcpy(qsf->hostname, hostname, (strlen(hostname) + 1));
    php_printf("5:%s#\n", qsf->hostname);
    qsf->hostname_len = hostname_len;
    efree(hostname);

    if (!object) {
            ZEND_REGISTER_RESOURCE(return_value, qsf, le_qsf);
    } else {
            list_id = zend_list_insert(qsf, le_qsf);
            add_property_resource(object, "connection", list_id);
    }

    ////link = stream;
    ////qsf_connect(INTERNAL_FUNCTION_PARAM_PASSTHRU);
    ////zend_update_property(ce, getThis(), "link", sizeof("link")-1, stream  TSRMLS_CC);
    //    //qsf_connect(INTERNAL_FUNCTION_PARAM_PASSTHRU);
    ////link = zend_read_property(ce, getThis(), "link", sizeof("link")-1, 0 TSRMLS_DC);
    //ZEND_REGISTER_RESOURCE(return_value, stream, le_qsf);
    //php_printf("6>>>>>>>>>>>>\n");


}

PHP_FUNCTION(qsf_get) {
        qsf_t *qsf;
        qsf_t *retstr;
	zval *hr, **connection, *object = getThis();
	char *key;
	int key_len;

	if (object) { 
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &key, &key_len) == FAILURE) {
			RETURN_FALSE;
		}
		if (zend_hash_find(Z_OBJPROP_P(object), "connection", sizeof("connection"), (void **) &connection) == FAILURE) {
			RETURN_FALSE;
		}

		MAKE_STD_ZVAL(hr);
		ZVAL_RESOURCE(hr, Z_LVAL_PP(connection));
	} else {
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs", &hr, &key, &key_len) == FAILURE) {
			RETURN_NULL();
		}
	}

	ZEND_FETCH_RESOURCE(qsf, qsf_t*, &hr, -1, QSF_RES_NAME, le_qsf);
        char *query;
        int query_len;
        query_len = spprintf(&query, 0, "get %s\r\n", key);
        php_printf("9:%s#\n", qsf->hostname);
	retstr = get_content(qsf, query, query_len);
        RETURN_STRINGL(retstr, strlen(retstr), 0);	
        
}

char *get_content(qsf_t *qsf, char *query, int query_len TSRMLS_DC) {
    php_stream *stream;
    char *host, *transport, *errstr = NULL;
    int host_len, transport_len, implicit_tcp = 1, errcode = 0;
    long port = 0;
    int options = ENFORCE_SAFE_MODE;
    int flags = STREAM_XPORT_CLIENT | STREAM_XPORT_CONNECT;
    struct timeval tv;
    tv.tv_sec = QSF_G(timeout);
    php_printf("10:%s#\n", qsf->hostname);
    stream = php_stream_xport_create(qsf->hostname, strlen(qsf->hostname), options, flags, NULL, &tv, NULL, &errstr, &errcode);
    if (errstr) {
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "[%d] %s", errcode, errstr);
        efree(errstr);
    }
    if (!stream) {
    	php_printf("failed to php_stream_xport_create\n");
    }
    //php_stream_to_zval(stream, return_value);
    php_stream_write(stream, query, query_len);
    //efree(query);
    char buf[4096], *response, *ret, *eptr;
    int response_len = 0;
    char *retstr = NULL;
    int flag_value = 0;
    while (!stream->eof) {
        response = php_stream_get_line(stream, buf, 1024, &response_len);
	if (response == NULL) {
		break;
	}
	php_printf("!!!%s#%d\n", response, response_len);
	if (strstr(response, "VALUE")) {
		flag_value++;
		continue;
	}
	if (strstr(response, "END")) {
		php_stream_close(stream);
		break;
	}
	if ( flag_value == 1) {
		retstr = (char *)emalloc((strlen(response)+1) * sizeof(char));
        	memcpy(retstr, response, strlen(response) - 2);
	}
    }
    php_printf("get result:#%s#\n", retstr);
    //efree(retstr);
    //RETURN_STRINGL(retstr, strlen(retstr), 0);	
    return retstr;	
}




ZEND_METHOD(qsfclass , public_method )
{
    php_stream *stream;
    char *host, *transport, *errstr = NULL;
    int host_len, transport_len, implicit_tcp = 1, errcode = 0;
    long port = 0;
    int options = ENFORCE_SAFE_MODE;
    int flags = STREAM_XPORT_CLIENT | STREAM_XPORT_CONNECT;
    struct timeval tv;
    tv.tv_sec = QSF_G(timeout);
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|l", &host, &host_len, &port) == FAILURE) {
        return;
    }
    if (port) {
        int implicit_tcp = 1;
        if (strstr(host, "://")) {
                /* A protocol was specified,
   *                * no need to fall back on tcp:// */
            implicit_tcp = 0;
        }
        transport_len = spprintf(&transport, 0, "%s%s:%d", implicit_tcp ? "tcp://" : "", host, port);
    } else {
      /* When port isn't specified
 *        * we can safely assume that a protocol was
 *                 * (e.g. unix:// or udg://) */
      transport = host;
      transport_len = host_len;
    }
    php_printf("8:%s#\n", transport);
    stream = php_stream_xport_create(transport, transport_len, options, flags, NULL, &tv, NULL, &errstr, &errcode);
    if (transport != host) {
        efree(transport);
    }
    if (errstr) {
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "[%d] %s", errcode, errstr);
        efree(errstr);
    }
    if (!stream) {
        RETURN_FALSE;
    }
    //php_stream_to_zval(stream, return_value);
    char *query;
    int query_len;
    query_len = spprintf(&query, 0, "user\r\n\r\n");
    php_stream_write(stream, query, query_len);
    efree(query);
    char buf[4096], *response, *ret, *eptr;
    int response_len = 0;
    int bodyflag = 0;
    char *retstr = NULL;
    while (!stream->eof) {
    	response = php_stream_get_line(stream, buf, 1024, &response_len);
        retstr = chg(retstr, response);
    	
    }
    php_stream_close(stream);
    //php_printf("%s\n", retstr);
    //efree(retstr);
    php_stream_close(stream);
    php_printf("我是public类型的方法\n");
    RETURN_STRINGL(retstr, strlen(retstr), 0);
}
 
ZEND_METHOD(qsfclass , private_method )
{
    php_printf("我是private类型的方法\n");
}

ZEND_METHOD(qsfclass , protected_method )
{
    php_printf("我是protected类型的方法\n");
}


ZEND_METHOD(qsfclass , __construct )
{
    php_printf("我是__construct方法\n");
}
 

/* }}} */
/* The previous line is meant for vim and emacs, so it can correctly fold and 
   unfold functions in source code. See the corresponding marks just before 
   function definition, where the functions purpose is also documented. Please 
   follow this convention for the convenience of others editing your code.
*/


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
