#include <a_samp>

#if !defined STRLIB_BUFFER_SIZE
	#define STRLIB_BUFFER_SIZE  2048
#endif

// Used in strtrim()
enum trim_edges {
	trim_left  = 1,
	trim_right = 2,
	trim_both = trim_left | trim_right
};

stock
	// Used throughout the library
	g_StrlibBuffer[2048]
;

// Forward-declarations
forward bool:isequal(const str1[], const str2[], bool:ignorecase = false);
forward bool:isempty(const string[]);
forward strexplode(output[][], const input[], const delimiter[] = !",", limit = cellmax, bool:trim = true, bool:ignorecase = false, size1 = sizeof(output), size2 = sizeof(output[]));
forward strgetc(const string[], index);
forward strgetfirstc(const string[]);
forward strimplode(const glue[], output[], maxlength = sizeof(output), ...);
forward strreplace(string[], const search[], const replacement[], bool:ignorecase = false, pos = 0, limit = -1, maxlength = sizeof(string));
forward strtrim(string[], const chars[] = !"", trim_edges:edges = trim_both);
forward strcount(const string[], const sub[], bool:ignorecase = false);
forward bool:strfromliteral(output[], const input[], &pos = 0, maxlength = sizeof(output));
forward strtoliteral(output[], const input[], maxlength = sizeof(output));
forward strfrombin(output[], const input[], inputlength = sizeof(input), maxlength = sizeof(output));
forward strtobin(output[], const input[], maxlength = sizeof(output));

// Workaround for compiler bug
forward _strlib_funcinc();
public _strlib_funcinc() {
	new temp[1];
	
	strcat(temp, temp);
}

/*
 * Returns a formatted string.
 *
 * Parameters:
 *   format[] - The format string.
 *   ... - The format variables.
 *
 * Returns:
 *   The formatted string.
 *
 * Limitations:
 *   Can only be used once in the same function call.
 */
#define sprintf(%1) \
	(format(g_StrlibBuffer, sizeof(g_StrlibBuffer), %1), g_StrlibBuffer)

/*
 * Macro - quicker than the native version.
 *
 * Parameters:
 *   string[] - The string to check.
 *
 * Returns:
 *   True if packed, false if empty or not packed.
 */
#define ispacked(%1) \
	((%1)[0] > 255)

/*
 * Get the first character of a string
 *
 * Parameters:
 *   string[] - The string.
 *
 * Returns:
 *   The first character of the string.
 */
stock strgetfirstc(const string[])
	return ispacked(string) ? string{0} : string[0];

/*
 * Get a character from a specific index in a string.
 *
 * Parameters:
 *   string[] - The string.
 *   index - The position in the string.
 *
 * Returns:
 *   The character at that index, or '\0' if out of range.
 */
stock strgetc(const string[], index) {
	if (index < 0)
		return '\0';
	
	new len = strlen(string);
	
	if (index >= len)
		return '\0';
	
	return ispacked(string) ? string{index} : string[index];
}

/*
 * Find out if a string is empty.
 *
 * Parameters:
 *   string[] - The string.
 *
 * Returns:
 *   True if empty, otherwise false.
 */
stock bool:isempty(const string[]) {
	if (ispacked(string))
		return string{0} == '\0';
	else
		return string[0] == '\0';
}

/*
 * Glue together strings into one.
 *
 * Parameters:
 *   glue[] - The string that will be between all other strings.
 *   output[] - The output string.
 *   maxlength - The size of "output". Defaults to sizeof(output).
 *   ...[] - Strings to glue together.
 *
 * Returns:
 *   Nothing
 */
stock strimplode(const glue[], output[], maxlength = sizeof(output), ...) {
	new args = numargs();
	
	// Null-out "output"
	output[0] = '\0';
	
	// Loop the variable arguments (the ones after "maxlength").
	for (new arg = 3; arg < args; arg++) {
		// If this isn't the first string, append the glue.
		if (arg != 3)
			strcat(output, glue, maxlength);
		
		// Wrap these in braces or they will be a part of the above if statement (compiler bug)
		{
			// Get the address of argument no. <arg>
			#emit LCTRL       5
			#emit ADD.C       12
			#emit LOAD.S.alt  arg
			#emit SHL.C.alt   2
			#emit ADD
			#emit LOAD.I
			
			// Push the maxlength, arg address, and output address
			#emit PUSH.S      maxlength
			#emit PUSH.pri
			#emit PUSH.S      output
			
			// Push the argument count
			#emit PUSH.C      12
			
			// call strcat
			#emit SYSREQ.C    strcat
			
			// Restore the stack
			#emit STACK       16
		}
	}
}

/*
 * Split a string by a given delimiter.
 * 
 * Parameters:
 *   output[][] - A multi-dimensional array that will be filled with substrings.
 *   input[] - The input string to split.
 *   delimiter[] - The delimiter to split by. Defaults to ",".
 *   limit - The max. no. substrings.
 *   trim - Whether to trim the substrings from whitespace. Defaults to true.
 *   ignorecase - Whether the search for "delimiter" should be case-insensitive.
 *   size1 - The size of the 1st dimension of output (otput[this][]). Defaults to sizeof(output).
 *   size2 - The size of the 2nd dimension of output (otput[][this]). Defaults to sizeof(output[]).
 *
 * Returns:
 *   The number of substrings that were copied into the array.
 */
stock strexplode(output[][], const input[], const delimiter[] = !",", limit = cellmax, bool:trim = true, bool:ignorecase = false, size1 = sizeof(output), size2 = sizeof(output[])) {
	if (!size1 || !size2) {
		printf("(strexplode) ERROR: size1 = %d, size2 = %d. Can't be 0.", size1, size2);
		
		return 0;
	}
	
	if (isempty(delimiter)) {
		print(!"(strexplode) ERROR: delimiter is empty.");
		
		return 0;
	}
	
	if (trim) {
		new i = -1;
		
		if (ispacked(input)) {
			while (input{++i}) {
				if (input{i} > ' ') {
					i = -1;
					
					break;
				}
			}
		} else {
			while (input[++i]) {
				if (input[i] > ' ') {
					i = -1;
					
					break;
				}
			}
		}
		
		if (i != -1)
			return 0;
	} else if (isempty(input)) {
		return 0;
	}
	
	if (limit == 0) {
		return 0;
	} else if (limit == cellmax) {
		limit = 0;
	}
	
	new
		     pos = 0,
		     next,
		bool:packed = ispacked(input),
		     dlen = strlen(delimiter),
		     count = 0,
		     end
	;
	
	while (pos != -1) {
		++count;
		
		if (limit > 0 && count >= limit) {
			next = -1;
		} else {
			next = strfind(input, delimiter, ignorecase, pos);
		}
		
		end = (next == -1) ? cellmax : next;
		
		if (trim) {
			if (end == cellmax)
				end = strlen(input);
			
			if (packed) {
				while (0 < input{pos} <= ' ') pos++;
				while (end > 0 && input{end - 1} <= ' ') end--;
			} else {
				while (0 < input[pos] <= ' ') pos++;
				while (end > 0 && input[end - 1] <= ' ') end--;
			}
		}
		
		strmid(output[count - 1], input, pos, end, size2);
		
		if (count >= size1 || next == -1 || (limit < 0 && count >= -limit))
			break;
		
		pos = next + dlen;
	}
	
	return count;
}

/*
 * Replace occurrences of the search string with the replacement string.
 *
 * Parameters:
 *   string[] - The string to perform the replacing in.
 *   search[] - The string to look for.
 *   replacement[] - The string to put instead of "search".
 *   ignorecase - Whether the search for "search" should be case-insensitive. Defaults to false.
 *   pos - The position to start at. Defaults to 0 (the beginning).
 *   limit - Limit the number of replacements. Defaults to -1 (no limit).
 *   maxlength - The size of "string". Defaults to sizeof(string).
 *
 * Returns:
 *   The number of replacements that were made.
 */
stock strreplace(string[], const search[], const replacement[], bool:ignorecase = false, pos = 0, limit = -1, maxlength = sizeof(string)) {
    // No need to do anything if the limit is 0.
    if (limit == 0)
        return 0;
    
    new
             sublen = strlen(search),
             replen = strlen(replacement),
        bool:packed = ispacked(string),
             maxlen = maxlength,
             len = strlen(string),
             count = 0
    ;
    
    
    // "maxlen" holds the max string length (not to be confused with "maxlength", which holds the max. array size).
    // Since packed strings hold 4 characters per array slot, we multiply "maxlen" by 4.
    if (packed)
        maxlen *= 4;
    
    // If the length of the substring is 0, we have nothing to look for..
    if (!sublen)
        return 0;
    
    // In this line we both assign the return value from "strfind" to "pos" then check if it's -1.
    while (-1 != (pos = strfind(string, search, ignorecase, pos))) {
        // Delete the string we found
        strdel(string, pos, pos + sublen);
        
        len -= sublen;
        
        // If there's anything to put as replacement, insert it. Make sure there's enough room first.
        if (replen && len + replen < maxlen) {
            strins(string, replacement, pos, maxlength);
            
            pos += replen;
            len += replen;
        }
        
        // Is there a limit of number of replacements, if so, did we break it?
        if (limit != -1 && ++count >= limit)
            break;
    }
    
    return count;
}

/*
 * Compare two strings.
 *
 * Parameters:
 *   str1[] - The first string.
 *   str2[] - The second string.
 *   ignorecase - Whether to compare them in a case-insensitive manner.
 *
 * Returns:
 *   True if equal, otherwise false.
 */
stock bool:isequal(const str1[], const str2[], bool:ignorecase = false) {
    new
        c1 = (str1[0] > 255) ? str1{0} : str1[0],
        c2 = (str2[0] > 255) ? str2{0} : str2[0]
    ;
    
    if (!c1 != !c2)
        return false;
    
    return !strcmp(str1, str2, ignorecase);
}

/*
 * Trim whitespace or a specific group of characters from a string.
 *
 * Parameters:
 *   string[] - The string to trim.
 *   chars[] - A string with characters to trim, or all whitespace if empty. Default is all whitespace.
 *   trim_edges - Which edge to trim (trim_left/trim_right/trim_both). Default is trim_both.
 *
 * Returns:
 *   Nothing
 */
stock strtrim(string[], const chars[] = !"", trim_edges:edges = trim_both) {
	new bool:packed = ispacked(string);
	
	// If "chars" is empty, trim whitespace
	if (!strgetfirstc(chars)) {
		// Should the left side be trimmed?
		if (edges & trim_left) {
			new i = 0;
			
			if (packed)
				while (0 < string{i} <= ' ') i++;
			else
				while (0 < string[i] <= ' ') i++;
			
			if (i) {
				strdel(string, 0, i);
			}
		}
		
		// Should the right side be trimmed?
		if (edges & trim_right) {
			new i = strlen(string);
			
			if (i) {
				if (packed) {
					while (--i && 0 < string{i} <= ' ') {}
					
					string{i + 1} = '\0';
				} else {
					while (--i && 0 < string[i] <= ' ') {}
					
					string[i + 1] = '\0';
				}
			}
		}
	} else {
		// Should the left side be trimmed?
		if (edges & trim_left) {
			new i = 0, sub[2];
			
			if (packed) {
				while ((sub[0] = string{i})) {
					if (strfind(chars, sub) == -1)
						break;
					
					i++;
				}
				
				if (i) {
					strdel(string, 0, i);
				}
			} else {
				while ((sub[0] = string[i])) {
					if (strfind(chars, sub) == -1)
						break;
					
					i++;
				}
				
				if (i) strdel(string, 0, i);
			}
		}
		
		// Should the right side be trimmed?
		if (edges & trim_right) {
			new i = strlen(string), sub[2];
			
			if (i >= 0) {
				if (packed) {
					while (i--) {
						sub[0] = string{i};
						
						if (strfind(chars, sub) == -1)
							break;
					}
					
					string{i + 1} = '\0';
				} else {
					while (i--) {
						sub[0] = string[i];
						
						if (strfind(chars, sub) == -1)
							break;
					}
					
					string[i + 1] = '\0';
				}
			}
		}
	}
}

/*
 * Count substrings.
 *
 * Parameters:
 *   string[] - The string to search inside.
 *   sub[] - The string to search for.
 *   ignorecase - Whether the search should be case-insensitive.
 *
 * Returns:
 *   The number of occurrences of "sub" in "string".
 */
stock strcount(const string[], const sub[], bool:ignorecase = false) {
	new pos = 0, count = 0;
	
	while (-1 != (pos = strfind(string, sub, ignorecase, pos)))
		count++;
	
	return count;
}

/*
 * Read a string from a PAWN string literal.
 *
 * Parameters:
 *   output[] - The variable to save into.
 *   input[] - The string literal.
 *   pos - The position in "input" to start reading from. Will be modified to the end of the literal.
 *   maxlength - The size of "output". Defaults to sizeof(output).
 *
 * Returns:
 *   true on success, false on error.
 */
stock bool:strfromliteral(output[], const input[], &pos = 0, maxlength = sizeof(output)) {
	new
		bool:packed = (input[0] > 255),
		     length = strlen(input),
			 c,
			 outlen = 0
	;

	output[0] = '\0';

	// No need to do anything else.
	if (!length)
		return true;
	
	if (input[0] == '"')
		pos++;

	for (;; pos++) {
		if (outlen >= maxlength - 1)
			break;
		
		c = input[pos];
		
		switch (c) {
			// String ended
			case '"': break;
			case '\\': {}
			default: {
				output[outlen++] = c;

				continue;
			}
		}

		// String ends with a backslash - invalid.
		if (pos == length - 1)
			return false;

		// We're after a backslash now, let's see what's there.
		c = input[++pos];

		switch (c) {
			case '"',
			    '\'',
			    '\\',
			     '%': output[outlen++] = c;
			case 'a': output[outlen++] = '\a';
			case 'b': output[outlen++] = '\b';
			case 'e': output[outlen++] = '\e';
			case 'f': output[outlen++] = '\f';
			case 'r': output[outlen++] = '\r';
			case 'n': output[outlen++] = '\n';
			case 't': output[outlen++] = '\t';
			case 'v': output[outlen++] = '\v';
			
			case 'x': {
				new val = 0;

				// String ends with "\x" - invalid.
				if (c == length - 1)
					return false;

				while ((c = input[pos + 1])) {
					if ('a' <= c <= 'f' || 'A' <= c <= 'F') {
						val = (val << 4) + (tolower(c) - 'a' + 10);
					} else if ('0' <= c <= '9') {
						val = (val << 4) + (c - '0');
					} else {
						break;
					}

					pos++;
				}

				if (c == ';')
					pos++;

				output[outlen++] = val;
			}
			
			case '0' .. '9': {
				new val = 0;

				while ((c = input[pos])) {
					if ('0' <= c <= '9') {
						val = val * 10 + (c - '0');
					} else {
						break;
					}

					pos++;
				}

				if (c != ';') pos--;

				output[outlen++] = val;
			}
			default: {
				return false;
			}
		}
	}

	output[outlen] = '\0';
	
	pos++;

	return true;
}

/*
 * Build a PAWN string literal from a given string.
 *
 * Parameters:
 *   output[] - The variable to save into.
 *   substrings[] - The string to build from.
 *   maxlength - The size of "output". Defaults to sizeof(output).
 *
 * Returns:
 *   Nothing
 */
stock strtoliteral(output[], const input[], maxlength = sizeof(output)) {
	new i, c, outlen;

	output[outlen++] = '"';

	for (i = 0; (c = input[i]); i++) {
		if (maxlength - outlen <= 3) {
			outlen = min(outlen, maxlength - 2);
			
			break;
		}
		
		switch (c) {
			case ' ', '!', '#' .. '[', ']', '^' .. '~':
				output[outlen++] = c;

			case  '"': strcat(output, !"\\\"", outlen + 3), outlen += 2;
			case '\a': strcat(output, !"\\a" , outlen + 3), outlen += 2;
			case '\b': strcat(output, !"\\b" , outlen + 3), outlen += 2;
			case '\e': strcat(output, !"\\e" , outlen + 3), outlen += 2;
			case '\f': strcat(output, !"\\f" , outlen + 3), outlen += 2;
			case '\r': strcat(output, !"\\r" , outlen + 3), outlen += 2;
			case '\n': strcat(output, !"\\n" , outlen + 3), outlen += 2;
			case '\t': strcat(output, !"\\t" , outlen + 3), outlen += 2;
			case '\v': strcat(output, !"\\v" , outlen + 3), outlen += 2;

			default: {
				if (maxlength - outlen <= 8)
					break;
				
				format(output[outlen], 7, "\\x%03x;", c);

				outlen += 6;
			}
		}
	}

	output[outlen++] = '"';
	output[outlen] = '\0';
}

/*
 * Convert an array to a string.
 *
 * Example: {0x1122, 0x5566} becomes "0000112200005566".
 *
 * Parameters:
 *   output[] - The variable to save into.
 *   input[] - The array to build from.
 *   inputlength - The size of "input". Defaults to sizeof(input).
 *   maxlength - The size of "output". Defaults to sizeof(output).
 *
 * Returns:
 *   Nothing
 */
stock strfrombin(output[], const input[], inputlength = sizeof(input), maxlength = sizeof(output)) {
	static const hex_chars[] = "0123456789ABCDEF";
	new outlen = 0;
	
	for (new i = 0; i < inputlength; i++) {
		if (maxlength - outlen <= 7) {
			outlen = min(outlen, maxlength - 1);
			
			break;
		}
		
		new input_cell = input[i];
		
		output[outlen++] = hex_chars[(input_cell             ) >>> 28];
		output[outlen++] = hex_chars[(input_cell & 0x0F000000) >>> 24];
		output[outlen++] = hex_chars[(input_cell & 0x00F00000) >>> 20];
		output[outlen++] = hex_chars[(input_cell & 0x000F0000) >>> 16];
		output[outlen++] = hex_chars[(input_cell & 0x0000F000) >>> 12];
		output[outlen++] = hex_chars[(input_cell & 0x00000F00) >>>  8];
		output[outlen++] = hex_chars[(input_cell & 0x000000F0) >>>  4];
		output[outlen++] = hex_chars[(input_cell & 0x0000000F)       ];
	}
	
	output[outlen] = '\0';
}

/*
 * Convert a string to an array.
 *
 * Example: "0000112200005566" becomes {0x1122, 0x5566}.
 *
 * Parameters:
 *   output[] - The variable to save into.
 *   input[] - The array to build from.
 *   maxlength - The size of "output". Defaults to sizeof(output).
 *
 * Returns:
 *   The length of the output, in cells.
 */
stock strtobin(output[], const input[], maxlength = sizeof(output)) {
	new len = strlen(input), outlen = 0;
	
	for (new i = 0; i < len;) {
		if (outlen >= maxlength || i > len - 8) {
			break;
		}
		
		new c, out = 0;
		
		#define ADD_OUT(%1) \
			c = input[i++]; out |= (('a' <= c <= 'f' || 'A' <= c <= 'F') ? (tolower(c) - 'a' + 10) : (c - '0')) << %1
		
		ADD_OUT(28);
		ADD_OUT(24);
		ADD_OUT(20);
		ADD_OUT(16);
		ADD_OUT(12);
		ADD_OUT(8);
		ADD_OUT(4);
		ADD_OUT(0);
		
		#undef ADD_OUT
		
		output[outlen++] = out;
	}
	
	return outlen;
}
