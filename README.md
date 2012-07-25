# strlib

String functions for SA-MP PAWN scripting.

# Functions

* `sprintf` - Returns a formatted string.
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
