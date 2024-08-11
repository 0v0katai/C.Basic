#ifndef _STDLIB
#define _STDLIB


extern "C" {

extern int abs(int __j);
#define abs(j) ({ \
	int __j = (j); \
	(__j >= 0) ? __j : -(__j); \
})
}

#endif
