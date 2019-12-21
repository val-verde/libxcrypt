/*-
 * Copyright 2020 Björn Esser <besser82@fedoraproject.org>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/* Some versions of the <unistd.h> header file may declare the
   prototype of the crypt() function attributing some parameters
   passed into the function *MUST NOT* be NULL.

   In unity builds this attribute can instruct several compilers
   to delete some of the needed NULL-pointer checks inside of our
   code, as we cannot include <unistd.h> in just those source files,
   which need some of the prototypes and macro definitions the
   <unistd.h> header file provides.

   When including <unistd.h> we need to make sure such a prototype
   will not be used by the compiler to apply code optimizations,
   and thus potential deletion of some needed NULL-pointer checks,
   to our implemetation of the crypt() function.  As we use the
   preprocessor to rename the prototype, we must include <unistd.h>
   within <crypt-port.h>, when compiling a unity build, *before* we
   define our macros used for symbol-versioning purposes.

   Anyways, using a prototype of the crypt() function, which is
   declared with those non-NULL attributes, for compiling external
   code, that gets linked against a binary copy of the libxcrypt
   library, will not cause any such problems.  */

#ifndef _CRYPT_WRAP_UNISTD_H
#define _CRYPT_WRAP_UNISTD_H 1

/* For the reasons explained below, this header file *MUST* be included
   *BEFORE* <crypt-symbol-vers.h> …  */
#ifdef _CRYPT_SYMBOL_VERS_H
#error <__FILE__> *MUST* be included *BEFORE* <crypt-symbol-vers.h>.
#endif

/* … but *AFTER*, or inside of, <crypt-port.h>.  */
#ifndef _CRYPT_PORT_H
#error <__FILE__> *MUST* be included *AFTER* <crypt-port.h>.
#endif

/* The prototype of the crypt() function in <unistd.h> needs to be
   renamed if it seems incompatible with our declaration. Defining
   this macro *AFTER* <crypt-symbol-vers.h>, which also defines the
   same macro for symbol-versioning purposes, was included, would
   lead to a clashing redefinition of this macro, and thus cause
   our symbol-versioning would not work properly.  */
#ifdef HAVE_INCOMPATIBLE_CRYPT_PROTOTYPE_IN_UNISTD_H
#define crypt unistd_crypt_is_incompatible
#endif

/* Include <unistd.h>.  The macro conditionally defined previously
   will ensure the compiler will not use an incompatible declaration
   of the crypt() function.  */
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

/* Clean-up, so the macro does not clash with <crypt-symbol-vers.h>.  */
#ifdef crypt
#undef crypt
#endif

#endif /* crypt-wrap-unistd.h */
