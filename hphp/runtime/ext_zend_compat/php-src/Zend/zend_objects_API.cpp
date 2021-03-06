/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2013 Zend Technologies Ltd. (http://www.zend.com) |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.zend.com/license/2_00.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Authors: Andi Gutmans <andi@zend.com>                                |
   |          Zeev Suraski <zeev@zend.com>                                |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#include "zend.h"
#include "zend_globals.h"
#include "zend_variables.h"
// has to be before zend_API since that defines getThis()
#include "hphp/runtime/ext_zend_compat/hhvm/zend-request-local.h"
#include "zend_API.h"
#include "zend_objects_API.h"
#include "hphp/runtime/ext_zend_compat/hhvm/zend-object.h"
#include "hphp/runtime/ext_zend_compat/hhvm/zend-object-store.h"
#include "hphp/runtime/vm/native-data.h"

#define ZEND_DEBUG_OBJECTS 0

ZEND_API void zend_objects_store_add_ref(zval *object TSRMLS_DC) {
  Z_OBJVAL_P(object)->incRefCount();
}

ZEND_API void zend_objects_store_del_ref(zval *zobject TSRMLS_DC) {
  Z_OBJVAL_P(zobject)->decRefCount();
}

ZEND_API void *zend_object_store_get_object(const zval *zobject TSRMLS_DC) {
  const HPHP::ZendObject * zod = HPHP::Native::data<HPHP::ZendObject>(Z_OBJVAL_P(zobject));
  return zend_object_store_get_object_by_handle(zod->getHandle() TSRMLS_CC);
}

ZEND_API void *zend_object_store_get_object_by_handle(zend_object_handle handle TSRMLS_DC) {
  return HPHP::ZendObjectStore::getInstance().getObject(handle);
}

ZEND_API zend_object_handle zend_objects_store_put(void *object, zend_objects_store_dtor_t dtor, zend_objects_free_object_storage_t free_storage, zend_objects_store_clone_t clone TSRMLS_DC) {
  return HPHP::ZendObjectStore::getInstance().insertObject(
      object, dtor, free_storage, clone);
}

ZEND_API zend_object_value zend_objects_store_clone_obj(zval *zobject TSRMLS_DC)
{
  const HPHP::ZendObject * zod =
    HPHP::Native::data<HPHP::ZendObject>(Z_OBJVAL_P(zobject));
  zend_object_value retval;
  retval.handle = HPHP::ZendObjectStore::getInstance().cloneObject(zod->getHandle());
  if (retval.handle == 0) {
    HPHP::raise_error("Trying to clone uncloneable object of class %s",
        Z_OBJVAL_P(zobject)->getVMClass()->name()->data());
  }
  retval.handlers = zod->getHandlers();
  return retval;
}

ZEND_API zend_object_handlers *zend_get_std_object_handlers(void) {
    return &std_object_handlers;
}

/* Called when the ctor was terminated by an exception */
ZEND_API void zend_object_store_ctor_failed(zval *zobject TSRMLS_DC) {
}
