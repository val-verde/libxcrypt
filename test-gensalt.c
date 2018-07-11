#include "crypt-port.h"
#include "crypt-base.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

static const char *const entropy[] =
{
  "\x58\x35\xcd\x26\x03\xab\x2c\x14\x92\x13\x1e\x59\xb0\xbc\xfe\xd5",
  "\x9b\x35\xa2\x45\xeb\x68\x9e\x8f\xd9\xa9\x09\x71\xcc\x4d\x21\x44",
  "\x25\x13\xc5\x94\xc3\x93\x1d\xf4\xfd\xd4\x4f\xbd\x10\xe5\x28\x08",
  "\xa0\x2d\x35\x70\xa8\x0b\xc3\xad\xdf\x61\x69\xb3\x19\xda\x7e\x8d",
  0
};

#if INCLUDE_des || INCLUDE_des_big
static const char *const des_expected_output[] = { "Mp", "Pp", "ZH", "Uh"};
#endif
#if INCLUDE_des_xbsd
static const char *const bsdi_expected_output[] =
{
  "_J9..MJHn",
  "_J9..PKXc",
  "_J9..ZAFl",
  "_J9..UqGB"
};
static const char *const bsdi_expected_output_r[] =
{
  "_DT0.MJHn",
  "_DT0.PKXc",
  "_DT0.ZAFl",
  "_DT0.UqGB"
};
#endif
#if INCLUDE_md5
static const char *const md5_expected_output[] =
{
  "$1$MJHnaAke",
  "$1$PKXc3hCO",
  "$1$ZAFlICwY",
  "$1$UqGBkVu0"
};
#endif
#if INCLUDE_nthash
static const char *const nthash_expected_output[] =
{
  "$3$__not_used__c809a450df09a3",
  "$3$__not_used__30d0d6f834c0c3",
  "$3$__not_used__0eeeebb83d6fe4",
  "$3$__not_used__1c690d6a9ef88c"
};
#endif
#if INCLUDE_sunmd5
static const char *const sunmd5_expected_output[] =
{
  "$md5,rounds=55349$BPm.fm03$",
  "$md5,rounds=72501$WKoucttX$",
  "$md5,rounds=42259$3HtkHq/x$",
  "$md5,rounds=73773$p.5e9AQf$",
};
static const char *const sunmd5_expected_output_r[] =
{
  "$md5,rounds=4294920244$BPm.fm03$",
  "$md5,rounds=4294937396$WKoucttX$",
  "$md5,rounds=4294907154$3HtkHq/x$",
  "$md5,rounds=4294938668$p.5e9AQf$",
};
#endif
#if INCLUDE_sha1
static const char *const sha1_expected_output[] =
{
  "$sha1$248488$ggu.H673kaZ5$",
  "$sha1$248421$SWqudaxXA5L0$",
  "$sha1$257243$RAtkIrDxEovH$",
  "$sha1$250464$1j.eVxRfNAPO$",
};
static const char *const sha1_expected_output_r[] =
{
  "$sha1$3643984551$ggu.H673kaZ5$",
  "$sha1$4200450659$SWqudaxXA5L0$",
  "$sha1$3946507480$RAtkIrDxEovH$",
  "$sha1$3486175838$1j.eVxRfNAPO$",
};
#endif
#if INCLUDE_sha256
static const char *const sha256_expected_output[] =
{
  "$5$MJHnaAkegEVYHsFK",
  "$5$PKXc3hCOSyMqdaEQ",
  "$5$ZAFlICwYRETzIzIj",
  "$5$UqGBkVu01rurVZqg"
};
static const char *const sha256_expected_output_r[] =
{
  "$5$rounds=10191$MJHnaAkegEVYHsFK",
  "$5$rounds=10191$PKXc3hCOSyMqdaEQ",
  "$5$rounds=10191$ZAFlICwYRETzIzIj",
  "$5$rounds=10191$UqGBkVu01rurVZqg"
};
#endif
#if INCLUDE_sha512
static const char *const sha512_expected_output[] =
{
  "$6$MJHnaAkegEVYHsFK",
  "$6$PKXc3hCOSyMqdaEQ",
  "$6$ZAFlICwYRETzIzIj",
  "$6$UqGBkVu01rurVZqg"
};
static const char *const sha512_expected_output_r[] =
{
  "$6$rounds=10191$MJHnaAkegEVYHsFK",
  "$6$rounds=10191$PKXc3hCOSyMqdaEQ",
  "$6$rounds=10191$ZAFlICwYRETzIzIj",
  "$6$rounds=10191$UqGBkVu01rurVZqg"
};
#endif
#if INCLUDE_bcrypt
static const char *const bcrypt_a_expected_output[] =
{
  "$2a$05$UBVLHeMpJ/QQCv3XqJx8zO",
  "$2a$05$kxUgPcrmlm9XoOjvxCyfP.",
  "$2a$05$HPNDjKMRFdR7zC87CMSmA.",
  "$2a$05$mAyzaIeJu41dWUkxEbn8hO"
};
static const char *const bcrypt_b_expected_output[] =
{
  "$2b$05$UBVLHeMpJ/QQCv3XqJx8zO",
  "$2b$05$kxUgPcrmlm9XoOjvxCyfP.",
  "$2b$05$HPNDjKMRFdR7zC87CMSmA.",
  "$2b$05$mAyzaIeJu41dWUkxEbn8hO"
};
static const char *const bcrypt_x_expected_output[] =
{
  "$2x$05$UBVLHeMpJ/QQCv3XqJx8zO",
  "$2x$05$kxUgPcrmlm9XoOjvxCyfP.",
  "$2x$05$HPNDjKMRFdR7zC87CMSmA.",
  "$2x$05$mAyzaIeJu41dWUkxEbn8hO"
};
static const char *const bcrypt_y_expected_output[] =
{
  "$2y$05$UBVLHeMpJ/QQCv3XqJx8zO",
  "$2y$05$kxUgPcrmlm9XoOjvxCyfP.",
  "$2y$05$HPNDjKMRFdR7zC87CMSmA.",
  "$2y$05$mAyzaIeJu41dWUkxEbn8hO"
};
#endif

struct testcase
{
  const char *prefix;
  const char *const *expected_output;
  unsigned int expected_len;
  unsigned int expected_auto_len;
  unsigned long rounds;
};

static const struct testcase testcases[] =
{
#if INCLUDE_des || INCLUDE_des_big
  { "",      des_expected_output,       2,  0, 0 },
  // DES doesn't have variable round count.
#endif
#if INCLUDE_des_xbsd
  { "_",     bsdi_expected_output,      9,  0, 0 },
  { "_",     bsdi_expected_output_r,    9,  0, 10191 },
#endif
#if INCLUDE_md5
  { "$1$",   md5_expected_output,      11,  0, 0 },
  // MD5/BSD doesn't have variable round count.
#endif
#if INCLUDE_nthash
  { "$3$",   nthash_expected_output,   29,  0, 0 },
  // NTHASH doesn't have variable round count.
#endif
#if INCLUDE_sunmd5
  { "$md5",  sunmd5_expected_output,   27,  0, 0 },
  // SHA1/PBKDF always emits a round count, but we need to test its
  // behavior on very large inputs.  (This number is the largest
  // supported round count.)
  { "$md5", sunmd5_expected_output_r,  32, 0, 4294963199 },
#endif
#if INCLUDE_sha1
  { "$sha1", sha1_expected_output,     26, 34, 0 },
  // SHA1/PBKDF always emits a round count, but we need to test its
  // behavior on very large inputs.  (The behavior should be the
  // same whether or not ULONG_MAX > UINT32_MAX.)
  { "$sha1", sha1_expected_output_r,   30, 38, ULONG_MAX },
#endif
#if INCLUDE_sha256
  { "$5$",   sha256_expected_output,   19,  0, 0 },
  { "$5$",   sha256_expected_output_r, 32,  0, 10191 },
#endif
#if INCLUDE_sha512
  { "$6$",   sha512_expected_output,   19,  0, 0 },
  { "$6$",   sha512_expected_output_r, 32,  0, 10191 },
#endif
#if INCLUDE_bcrypt
  { "$2a$",  bcrypt_a_expected_output, 29,  0, 0 },
  { "$2b$",  bcrypt_b_expected_output, 29,  0, 0 },
  { "$2x$",  bcrypt_x_expected_output, 29,  0, 0 },
  { "$2y$",  bcrypt_y_expected_output, 29,  0, 0 },
  // bcrypt gensalt always emits a round count.
#endif
  { 0, 0, 0, 0, 0 }
};

int
main (void)
{
  int status = 0;
  unsigned int ent;
  const struct testcase *tcase;
  char output[CRYPT_GENSALT_OUTPUT_SIZE];
  char prev_output[CRYPT_GENSALT_OUTPUT_SIZE];

  for (tcase = testcases; tcase->prefix; tcase++)
    {
      XCRYPT_SECURE_MEMSET (prev_output, CRYPT_GENSALT_OUTPUT_SIZE);
      for (ent = 0; ent < ARRAY_SIZE (entropy); ent++)
        {
          XCRYPT_SECURE_MEMSET (output, CRYPT_GENSALT_OUTPUT_SIZE);
          char *salt = crypt_gensalt_rn (tcase->prefix, tcase->rounds,
                                         entropy[ent], 16,
                                         output, CRYPT_GENSALT_OUTPUT_SIZE);
          if (salt == 0)
            {
              if (entropy[ent] == 0 && errno == ENOSYS)
                {
                  fprintf (stderr, "UNSUPPORTED: %s/auto-entropy -> ENOSYS\n",
                           tcase->prefix);
                }
              else
                {
                  fprintf (stderr, "ERROR: %s/%u -> 0\n", tcase->prefix, ent);
                  status = 1;
                }
              continue;
            }
          size_t slen = strlen (salt);
          unsigned int expected_len =
            (!entropy[ent] && tcase->expected_auto_len) ?
            tcase->expected_auto_len : tcase->expected_len;
          if (slen != expected_len)
            {
              fprintf (stderr,
                       "ERROR: %s/%u -> %s (expected len=%u got %zu)\n",
                       tcase->prefix, ent, salt, expected_len, slen);
              status = 1;
            }
          else if (strncmp (salt, tcase->prefix, strlen (tcase->prefix)))
            {
              fprintf (stderr, "ERROR: %s/%u -> %s (prefix wrong)\n",
                       tcase->prefix, ent, salt);
              status = 1;
            }
          else if (!strcmp (salt, prev_output))
            {
              fprintf (stderr, "ERROR: %s/%u -> %s (same as prev)\n",
                       tcase->prefix, ent, salt);
              status = 1;
            }
          else if (entropy[ent] &&  strcmp (salt, tcase->expected_output[ent]))
            {
              fprintf (stderr, "ERROR: %s/%u -> %s (expected %s)\n",
                       tcase->prefix, ent, salt, tcase->expected_output[ent]);
              status = 1;
            }
          else
            fprintf (stderr, "   ok: %s/%u -> %s\n",
                     tcase->prefix, ent, salt);

          XCRYPT_SECURE_MEMSET (prev_output, CRYPT_GENSALT_OUTPUT_SIZE);
          strncpy (prev_output, salt, CRYPT_GENSALT_OUTPUT_SIZE -1 );
        }
    }

  /* Currently, passing a null pointer as the prefix argument to
     crypt_gensalt is supposed to produce a bcrypt-mode-2b setting
     string.  */
  {
    char *setting1, *setting2;
    setting1 = crypt_gensalt_ra ("$2b$", 0, entropy[0], 16);
    setting2 = crypt_gensalt_ra (0, 0, entropy[0], 16);
    if ((setting1 == 0 && setting2 != 0) ||
        (setting1 != 0 && setting2 == 0) ||
        (setting1 != 0 && setting2 != 0 && strcmp (setting1, setting2)))
      {
        printf ("FAILED: crypt_gensalt defaulting to $2b$\n"
                "  $2b$ -> %s\n"
                "  null -> %s\n",
                setting1, setting2);
        status = 1;
      }
    free (setting1);
    free (setting2);
  }

#if INCLUDE_bcrypt
  /* FIXME: This test is a little too specific.  It used to be in
     test-bcrypt.c and I'm not sure what it's meant to be testing.  */
  {
    char *setting1, *setting2;
    const char *which = "$2a$05$CCCCCCCCCCCCCCCCCCCCC.E5YPO9kmyuRGyh0XouQYb4YMJKvyOeW";
    setting1 = crypt_gensalt (which, 12, "CCCCCCCCCCCCCCCCCCCCC", 21);
    if (!setting1 || strncmp (setting1, "$2a$12$", 7))
      {
        printf ("FAILED (crypt_gensalt: wrong prefix) s1=%s\n", setting1);
        status = 1;
      }

    setting2 = crypt_gensalt_ra (setting1, 12, "CCCCCCCCCCCCCCCCCCCCC", 21);
    if (strcmp (setting1, setting2))
      {
        printf ("FAILED (crypt_gensalt_ra/1: s1=%s s2=%s)\n", setting1, setting2);
        status = 1;
      }

    setting1 = crypt_gensalt_ra (setting2, 12, "DCCCCCCCCCCCCCCCCCCCC", 21);
    if (!strcmp (setting1, setting2))
      {
        printf ("FAILED (crypt_gensalt_ra/2: s1=%s s2=%s)\n", setting1, setting2);
        status = 1;
      }

    free (setting1);
    free (setting2);
  }
#endif
  return status;
}
