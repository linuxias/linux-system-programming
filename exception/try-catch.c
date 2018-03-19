#include <stdio.h>
#include <setjmp.h>

#define TRY \
do { \
	jmp_buf __buf; \
	switch (setjmp(__buf)) { \
		case 0: \
		{
#define CATCH(e) \
			break; \
		} \
		case e: {
#define FINALLY \
			break; \
		} \
		default: {
#define ETRY \
		} \
	} \
} while(0);
#define THROW(e) longjmp(__buf, e)

#define EXCEPTION_1 1
#define EXCEPTION_2 2

int main()
{
	TRY {
		printf("try\n");
		THROW(EXCEPTION_2);
		printf("try check\n");
	} CATCH (EXCEPTION_1) {
		printf("Exception1 !!\n");
	} CATCH (EXCEPTION_2) {
		printf("Exception2 !!\n");
	} ETRY;

	return 0;
}
