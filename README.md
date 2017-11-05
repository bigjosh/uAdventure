# uAdventure

An attempt to get Colossal Cave Adventure to fit into a tiny chip.  

UPDATE: I am giving up. I was hoping to get there with some quick precompling and compressing of the data structures and static strings. 
While initial work on this front was fruitful, the program code itself will not fit into 32K even getting rid of the strings entirely. I'm sure with enough effort this program could be rewritten to be space efficient, definetly not worth the effort (at least for me!).



## Starting point

This nice clean C version...

https://github.com/Quuxplusone/Advent/blob/master/ODWY0350/advent.c

## Steps

1. Starting point

```
Program Memory Usage 	:	90078 bytes   274.9 % Full	(Memory Overflow)
Data Memory Usage 		:	56272 bytes   2747.7 % Full	(Memory Overflow)
```

2. Precompile the `hash_table[]` of all words. For now, this increases RAM usage because the table has to be copied from flash to RAM at startup, but we will fit that soon!

```
Program Memory Usage 	:	93358 bytes   284.9 % Full	(Memory Overflow)
Data Memory Usage 		:	55030 bytes   2687.0 % Full	(Memory Overflow)	
```

3. Switch from a hashtable to linear lookup. This saves lots of room by getting rid of all the dead entries in the hash table. 

```
Program Memory Usage 	:	87306 bytes   266.4 % Full	(Memory Overflow)
Data Memory Usage 		:	49110 bytes   2397.9 % Full	(Memory Overflow)
```

4. Bifurcate the word database into two tables, one for `text` and one for `meaning`. Now the text can just be an array of char[5], saving the trailing `\x00` at the end of each string.
 
5. Sort the word tables by the value of meaning, and then only store the difference between sequential meaning values, which is never more than 100. The diff can now be stored as only an `unsigned char` rather than an `int` and we computer the full `int` as we scan the table. Having these simple arrays will also make it easier to switch to `PROGMEM` when the time comes (PROGMEM can't do arrays of structs). 

```
Program Memory Usage 	:	86770 bytes   264.8 % Full	(Memory Overflow)
Data Memory Usage 		:	48548 bytes   2370.5 % Full	(Memory Overflow)
```

6. Extract all static strings and convert them onto refs to `GET_CSTR()` which will pull them from flash memory and decompress them one byte at a time as they are printed.

```
Program Memory Usage 	:	33758 bytes   103.0 % Full	(Memory Overflow)
Data Memory Usage 		:	3576 bytes   174.6 % Full	(Memory Overflow)
```
  
