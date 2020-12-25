findval: find_val.c
	gcc -o $@ $< -I.
clean:
	rm -f findval
