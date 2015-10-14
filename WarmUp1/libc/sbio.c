#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

void writeChar(char c){
	write(1, &c, 1);
}

void printPostivie(int integer){
	int digit[MAX_BUFF];
	int i=0;
	while(integer){
    	digit[i++]=integer % 10;
    	integer /= 10;
	}

	while(i-->0){
		writeChar('0'+digit[i]);
	}
}
void printInteger(int integer) {
	if(integer==0){
		writeChar('0');
		return;
	}
	if(integer<0){
		writeChar('-');
		integer=-integer; 
	}
	printPostivie(integer);	
}

int printf(const char *format, ...) {
	va_list 	val;
	va_start(val, format);
	int i = 0;
	while(*format) {
		if(*format == '%') {
			format++;
			if(*format == 'd') {
				printInteger(va_arg(val, int));
			} else if(*format == 'c') {
				writeChar((char)va_arg(val, int));

			} else if(*format == 's') {
				char* valStr = va_arg(val, char*);
				while(*valStr != '\0') {
					write(1, valStr, 1);
					valStr++;
				}

			}
		} else {
			write(1, format, 1);
		}
		i++;
		format++;
	}

	va_end(val);

	return i;
}


int scanf(const char *format, ...) {

	va_list 	val;	
	va_start(val, format);
	char* 		recvBuffer=malloc(MAX_BUFF*sizeof(char));
	int 		scannedArgs = 0;

	read(0, recvBuffer, MAX_BUFF);
	while(*format) {
		if(*format == '%') {
			format++;
			scannedArgs++;
			if(*format == 's') {
				char* valStr = va_arg(val, char*);
				while((*valStr++=*recvBuffer++)!= '\n');
			}
		}
		format++;
	}
	va_end(val);

	return scannedArgs;
}



