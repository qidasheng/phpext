dnl $Id$
dnl config.m4 for extension qsf

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary. This file will not work
dnl without editing.

dnl If your extension references something external, use with:

PHP_ARG_WITH(qsf, for qsf support,
Make sure that the comment is aligned:
[  --with-qsf             Include qsf support])

dnl Otherwise use enable:

dnl PHP_ARG_ENABLE(qsf, whether to enable qsf support,
dnl Make sure that the comment is aligned:
dnl [  --enable-qsf           Enable qsf support])

if test -z "$PHP_DEBUG"; then   
    AC_ARG_ENABLE(debug,  
    [   --enable-debug          compile with debugging symbols],[  
        PHP_DEBUG=$enableval  
    ],[ PHP_DEBUG=no  
    ])  
fi  

if test "$PHP_QSF" != "no"; then
  dnl Write more examples of tests here...

  dnl # --with-qsf -> check with-path
  dnl SEARCH_PATH="/usr/local /usr"     # you might want to change this
  dnl SEARCH_FOR="/include/qsf.h"  # you most likely want to change this
  dnl if test -r $PHP_QSF/$SEARCH_FOR; then # path given as parameter
  dnl   QSF_DIR=$PHP_QSF
  dnl else # search default path list
  dnl   AC_MSG_CHECKING([for qsf files in default path])
  dnl   for i in $SEARCH_PATH ; do
  dnl     if test -r $i/$SEARCH_FOR; then
  dnl       QSF_DIR=$i
  dnl       AC_MSG_RESULT(found in $i)
  dnl     fi
  dnl   done
  dnl fi
  dnl
  dnl if test -z "$QSF_DIR"; then
  dnl   AC_MSG_RESULT([not found])
  dnl   AC_MSG_ERROR([Please reinstall the qsf distribution])
  dnl fi

  dnl # --with-qsf -> add include path
  dnl PHP_ADD_INCLUDE($QSF_DIR/include)

  dnl # --with-qsf -> check for lib and symbol presence
  dnl LIBNAME=qsf # you may want to change this
  dnl LIBSYMBOL=qsf # you most likely want to change this 

  dnl PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
  dnl [
  dnl   PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $QSF_DIR/lib, QSF_SHARED_LIBADD)
  dnl   AC_DEFINE(HAVE_QSFLIB,1,[ ])
  dnl ],[
  dnl   AC_MSG_ERROR([wrong qsf lib version or lib not found])
  dnl ],[
  dnl   -L$QSF_DIR/lib -lm
  dnl ])
  dnl
  dnl PHP_SUBST(QSF_SHARED_LIBADD)

  PHP_NEW_EXTENSION(qsf, qsf.c, $ext_shared)
fi
