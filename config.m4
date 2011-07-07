dnl $Id$
dnl config.m4 for extension barcode

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary. This file will not work
dnl without editing.

dnl If your extension references something external, use with:

PHP_ARG_WITH(barcode, for barcode support,
Make sure that the comment is aligned:
[  --with-barcode             Include barcode support])

dnl Otherwise use enable:

dnl PHP_ARG_ENABLE(barcode, whether to enable barcode support,
dnl Make sure that the comment is aligned:
dnl [  --enable-barcode           Enable barcode support])

if test "$PHP_BARCODE" != "no"; then
  dnl check for libpng and libzbar
  AC_MSG_CHECKING([for zbar library])
  for i in /usr /usr/local /opt; do
     if test -f $i/include/zbar.h; then
	    ZBARLIB_DIR=$i
	 fi
  done
  AC_MSG_RESULT([found libzbar in $ZBARLIB_DIR])
  
  if test -z "$ZBARLIB_DIR"; then
     AC_MSG_RESULT([not found])
	 AC_MSG_ERROR([Please install the zbar library])	   
  fi
  
  AC_MSG_CHECKING([for png library])
  for i in /usr /usr/local /opt; do
     if test -f $i/include/png.h; then
	    PNGLIB_DIR=$i
	 fi 
  done
  AC_MSG_RESULT([found libpng in $PNGLIB_DIR])
  
  if test -z "$PNGLIB_DIR"; then
     AC_MSG_RESULT([not found])
	 AC_MSG_ERROR([Please install the png library])	   
  fi
  
  PHP_ADD_LIBRARY_WITH_PATH(png, $PNGLIB_DIR/lib, BARCODE_SHARED_LIBADD)
  PHP_ADD_LIBRARY_WITH_PATH(zbar, $ZBARLIB_DIR/lib, BARCODE_SHARED_LIBADD)
  
  PHP_ADD_INCLUDE($PNGLIB_DIR/include)
  PHP_ADD_INCLUDE($ZBARLIB_DIR/include)
  
  AC_DEFINE(HAVE_ZBARLIB,1, [libzar found and included] )
  AC_DEFINE(HAVE_PNGLIB,1, [libpng found and included] )
  
  dnl # --with-barcode -> check with-path
  dnl SEARCH_PATH="/usr/local /usr"     # you might want to change this
  dnl SEARCH_FOR="/include/barcode.h"  # you most likely want to change this
  dnl if test -r $PHP_BARCODE/$SEARCH_FOR; then # path given as parameter
  dnl   BARCODE_DIR=$PHP_BARCODE
  dnl else # search default path list
  dnl   AC_MSG_CHECKING([for barcode files in default path])
  dnl   for i in $SEARCH_PATH ; do
  dnl     if test -r $i/$SEARCH_FOR; then
  dnl       BARCODE_DIR=$i
  dnl       AC_MSG_RESULT(found in $i)
  dnl     fi
  dnl   done
  dnl fi
  dnl
  dnl if test -z "$BARCODE_DIR"; then
  dnl   AC_MSG_RESULT([not found])
  dnl   AC_MSG_ERROR([Please reinstall the barcode distribution])
  dnl fi

  dnl # --with-barcode -> add include path
  dnl PHP_ADD_INCLUDE($BARCODE_DIR/include)

  dnl # --with-barcode -> check for lib and symbol presence
  dnl LIBNAME=barcode # you may want to change this
  dnl LIBSYMBOL=barcode # you most likely want to change this 

  dnl PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
  dnl [
  dnl   PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $BARCODE_DIR/lib, BARCODE_SHARED_LIBADD)
  dnl   AC_DEFINE(HAVE_BARCODELIB,1,[ ])
  dnl ],[
  dnl   AC_MSG_ERROR([wrong barcode lib version or lib not found])
  dnl ],[
  dnl   -L$BARCODE_DIR/lib -lm
  dnl ])
  dnl
  PHP_SUBST(BARCODE_SHARED_LIBADD)

  PHP_NEW_EXTENSION(barcode, barcode.c, $ext_shared)
fi
