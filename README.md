# strlib

String functions for SA-MP Pawn scripting.

# Functions

* `strgetfirstc` - Get the first character of a string
* `strgetc` - Get a character from a specific index in a string.
* `isempty` - Find out if a string is empty.
* `isequal` - Compare two strings.
* `strexplode` - Split a string by a given delimiter.
* `strimplode` - Glue together strings into one.
* `strreplace` - Replace occurrences of the search string with the replacement string.
* `strtrim` - Trim whitespace or a specific group of characters from a string.
* `strcount` - Count substrings.
* `strfromliteral` - Read a string from a PAWN string literal.
* `strtoliteral` - Build a PAWN string literal from a given string.
* `strfrombin` - Convert an array to a string.
* `strtobin` - Convert a string to an array.
* `strcatmid` - Concatenate a part of another string.
* `strurlencode` - URL encode a string.
* `strurldecode` - Decode an encoded URL.
* `strpad` - Pad edge(s) of a string with spaces.
* `strwrap` - Wrap a string inside two other strings.
* `utf8encode` - Encode a string into UTF-8 (assuming it's ISO-8859-1).
* `utf8decode` - Decode a UTF-8 encoded string.

## Output as return value

All of the functions above require one argument to be the output variable - the functions listed below do not.

Note, however, the max size of returned strings is `STRLIB_RETURN_SIZE` (default 128). Using large sizes will add lots of heap usage.

* `sprintf(format[], ...)` - Same as the native `format`, but it returns the output.
* `ret_utf8encode(input[])`
* `ret_utf8decode(input[])`
* `ret_strpad(string[], length, substr[] = !" ", edge = edge_both, trim_first = true, trim_chars[] = "")`
* `ret_strwrap(left[], string[], right[])`
* `ret_strcatmid(string[], source[], start = 0, end = -1)`
* `ret_strfrombin(input[], inputlength = sizeof(input))`
* `ret_strimplode(glue[], ...)`
* `ret_strreplace(string[], search[], replacement[], ignorecase = false, pos = 0, limit = -1)`
* `ret_strfromliteral(input[], &pos = 0)`
* `ret_strtoliteral(input[])`
* `ret_strtrim(string[], chars[] = "", trim_edges:edges = trim_both)`
* `ret_strurldecode(input[])`
* `ret_strurlencode(input[], pack = false)`
* `ret_strpack(source[])`
* `ret_strunpack(source[])`
* `ret_strcat(string1[], string2[])`
* `ret_strmid(source[], start, end)`
* `ret_strins(string[], substr[], pos)`
* `ret_strdel(string[], start, end)`
* `ret_valstr(value, pack = false)`
* `ret_GetPlayerName(playerid, pack = false)`

# Examples

## `sprintf`

```cpp
SetGameModeText(sprintf("Hello %s. %d %d %d.", "world", 1, 2, 3));

// GameMode text is now: Hello world. 1 2 3.
```

## `isequal`

```cpp
new str1[] = "HELLO", str2[] = "hello";

if (isequal(str1, str2)) // false
if (isequal(str1, str2, .ignorecase = true)) // true
```

## `strexplode`

```cpp
new output[10][10], count;

count = strexplode(output, "I, like, jolly, ranchers", ",");

for (new i = 0; i < count; i++)
	print(output[i]);

/* Output:
     I
     like
     jolly
     ranchers
*/
```

## `strimplode`

```cpp
new output[128];

strimplode(" ~~ ", output, sizeof(output), "I", "like", "jolly", "ranchers");

// output = "I ~~ like ~~ jolly ~~ ranchers"
```

## `strreplace`

```cpp
new string[128] = "Hello world";

strreplace(string, "world", "earth"); // string = "Hello earth"
strreplace(string, "HELLO", "Hola"); // string = "Hello earth" (no match for "HELLO")
strreplace(string, "HELLO", "Hola", .ignorecase = true); // string = "Hola earth"
```

## `strtrim`

```cpp
new string[128] = "   XXbla    blaY    ";

strtrim(string); // string = "XXbla    blaY"
strtrim(string, "XY"); // string = "bla    bla"

string = "/Users/home/bla/bla/";

strtrim(string, "/\\", .edges = trim_right); // string = "/Users/home/bla/bla"
```

## `strcount`

```cpp
new string[] = "cow COW cow COW sheep cow", count;

count = strcount(strong, "cow"); // count = 3
count = strcount(strong, "cow", .ignorecase = true); // count = 5
```

## `strfrombin`

```cpp
new data[] = {0x11223344, 0x55667788, 0x99AABBCC, 0xDDEEFF00};
new string[128];

strfrombin(string, data); // string = "112233445566778899AABBCCDDEEFF00"
```
