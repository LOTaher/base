#include "base.h"
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

String8 string_substring(String8 str, U64 start, U64 end)
{
    end = Min(end, str.length);
    start = Min(start, end);

    return (String8){str.str + start, end - start};
}

String8 string_cstring(char *str)
{
    return (String8){(U8 *)str, strlen(str)};
}

String8 string_copy(String8 str, Arena *arena)
{
    U8 *bytes = (U8 *)arena_push(arena, str.length);
    if (bytes == NULL)
    {
        return (String8){0};
    }

    memcpy(bytes, str.str, str.length);

    return (String8){bytes, str.length};
}

String8 string_concat(String8 str1, String8 str2, Arena *arena)
{
    U8 *bytes = (U8 *)arena_push(arena, str1.length + str2.length);
    if (bytes == NULL)
    {
        return (String8){0};
    }

    memcpy(bytes, str1.str, str1.length);
    memcpy(bytes + str1.length, str2.str, str2.length);

    return (String8){bytes, str1.length + str2.length};
}

B8 string_compare(String8 str1, String8 str2)
{
    if (str1.length != str2.length) return 0;

    return memcmp(str1.str, str2.str, str1.length) == 0 ? 1 : 0;
}

String8 string_create_fmt(Arena *arena, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    va_list args_copy;
    va_copy(args_copy, args);
    I32 length = vsnprintf(NULL, 0, fmt, args);
    va_end(args);

    if (length < 0)
    {
        va_end(args_copy);
        return (String8){0};
    }

    U8 *bytes = (U8 *)arena_push(arena, (U64)length + 1);
    if (bytes == NULL)
    {
        va_end(args_copy);
        return (String8){0};
    }

    vsnprintf((char *)bytes, (U64)length + 1, fmt, args_copy);
    va_end(args_copy);

    return (String8){bytes, (U64)length};
}

B8 string_contains(String8 str, String8 substr)
{
  if (substr.length > str.length)
    return 0;

  for (U64 i = 0; i < str.length; i++) {
    String8 sub = string_substring(str, i, substr.length + i);

    if (string_compare(sub, substr))
      return 1;
  }

  return 0;
}

