# uAdventure

An attempt to get Colossal Cave Adventure to fit into a tiny chip.  

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


