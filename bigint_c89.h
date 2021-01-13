/**/

#ifndef _BIGINT_H_
#define _BIGINT_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>



/* Definition of the bigint type */
/* Each byte holds 2 base-10 digits */
typedef struct {
    bool sign;          /* true = negative number, false = positive number */
    uint8_t *bytes;
    uint16_t digits;
} bigint;



void bigint_init(bigint *a);
void bigint_init_from_int(bigint *a, int b);
void bigint_init_from_bigint(bigint *a, bigint *b);
void bigint_free(bigint *a);
void bigint_resize(bigint *a);
uint8_t bigint_get_bit(bigint *a, uint16_t b);
void bigint_set_bit(bigint *a, uint16_t b, uint8_t c);
void bigint_print(bigint *a);
void bigint_add(bigint *a, bigint *b);
void bigint_clear(bigint *a);
void bigint_mul(bigint *a, bigint *b);
void bigint_mul_by_pow_10(bigint *a, uint16_t pow);



/* Initialize a new bigint with 16 digits */
void bigint_init(bigint *a) {
    a->digits = 16;
    a->bytes = malloc(a->digits / 2);   /* #bytes = #digits / 2 */
    a->sign = false;
}

/* Initialize a new bigint from an integer */
void bigint_init_from_int(bigint *a, int b) {
    uint16_t i;

    bigint_init(a);

    /* Format the bigint if the integer is negative */
    if (b < 0) {
        a->sign = true;
        b *= -1;
    }

    /* Copy digits ino the bigint */
    for (i = 0; b != 0; i++) {
        if (i >= a->digits) bigint_resize(a);   /* Resize the bigint if the integer is larger than 16 digits */
        bigint_set_bit(a, i, b % 10);
        b /= 10;
    }
}

/* Initialize a new bigint from a bigint */
/* Copy all of the digits from b into a */
void bigint_init_from_bigint(bigint *a, bigint *b) {
    uint16_t i;

    a->digits = b->digits;
    a->bytes = malloc(a->digits / 2);
    a->sign = b->sign;

    for (i = 0; i < (a->digits / 2); i++) a->bytes[i] = b->bytes[i];
}

/* Free the storage taken up by the bigint */
void bigint_free(bigint *a) {
    a->digits = 0;
    a->sign = false;
    free(a->bytes);
}

/* Double the number of digits that the bigint can hold */
void bigint_resize(bigint *a) {
    a->digits *= 2;
    a->bytes = realloc(a->bytes, a->digits / 2);
}

/* Get the value of a digit from the bigint */
uint8_t bigint_get_bit(bigint *a, uint16_t b) {
    return (b < a->digits) ? (
               (b % 2)
                   ? a->bytes[b / 2] & 0x0F  /* read from the right digit in the byte */
                   : a->bytes[b / 2] >> 4    /* read from the left digit in the byte */
           ) : 0;
}


/* Set the value of a digit from the bigint */
void bigint_set_bit(bigint *a, uint16_t b, uint8_t c) {
    if (b < a->digits)
        a->bytes[b / 2] = (
            (b % 2)
                ? (a->bytes[b / 2] & 0xF0) | (c & 0x0F) /* write to the right digit in the byte */
                : (a->bytes[b / 2] & 0x0F) | (c << 4)   /* write to the left digit in the byte */
        );
}

/* Print out the contents of the bigint */
void bigint_print(bigint *a) {
    int i;

    if (a->sign) printf("-");

    /* Don't print leading zeros */
    /*for (i = a->digits - 1; bigint_get_bit(a, i) == 0; i--);*/
    i = a->digits - 1;

    /* Print the other digits */
    for (; i >= 0; i--) printf("%d", bigint_get_bit(a, i));

    printf("\n");
}

/* Add the second bigint to the first bigint (a += b) */
/* a+b takes O(log(max(a, b))) time */
void bigint_add(bigint *a, bigint *b) {
    uint16_t i;
    uint8_t carry = 0, temp;

    /* If `b` couldn't fit in the space allotted for `a`, resize `a` */
    while (b->digits > a->digits) bigint_resize(a);

    /* Change every digit in `a` */
    for (i = 0; i < a->digits; i++) {
        temp = carry + bigint_get_bit(a, i) + bigint_get_bit(b, i);
        bigint_set_bit(a, i, (temp > 9) ? temp - 10 : temp);
        carry = (temp > 9) ? 1 : 0;
    }

    /* If there is another carry digit, resize the bigint and put the carry digit in */
    if (carry) {
        bigint_resize(a);
        a->bytes[i] = 1;
    }

}

/* Multiply a bigint by a power of 10 */
/* a << pow takes O(log(a)) time */
void bigint_mul_by_pow_10(bigint *a, uint16_t pow) {
    int i;

    /* Shift all digits over `pow` spaces */
    for (i = a->digits - 1; i >= pow; i--) bigint_set_bit(a, i, bigint_get_bit(a, i - pow));

    /* Fill in the remaining digits with 0 */
    for (; i >= 0; i--) bigint_set_bit(a, i, 0);
}

/* Multiply the first bigint by the second bigint (a *= b) */
/* a*b takes O(log(a) * log(max(a, b))) time */
void bigint_mul(bigint *a, bigint *b) {
    int i, j, temp, carry = 0;

    /* Initialize two local bigints */
    bigint result, tempResult;
    bigint_init(&result);
    bigint_init(&tempResult);

    for (i = a->digits - 1; i >= 0; i--) {
        /* Change every digit in `tempResult` */
        for (j = 0; j < b->digits; j++) {
            temp = carry + (bigint_get_bit(a, i) * bigint_get_bit(b, j));
            bigint_set_bit(&tempResult, j, (temp > 9) ? temp % 10 : temp);
            carry = (temp > 9) ? temp / 10 : 0;
        }

        /* If there is another carry digit, resize the bigint and put the carry digit in */
        if (carry > 0) {
            bigint_resize(&tempResult);
            bigint_resize(&result);
            tempResult.bytes[a->digits] = carry;
        }

        /* Shift the digits in tempResult */
        bigint_mul_by_pow_10(&tempResult, i);

        /* Add tempResult to the result (because the final result = the sum of all the tempResults) */
        bigint_add(&result, &tempResult);
    }

    /* Replace the digits in a with those of the result */
    a->digits = result.digits;
    a->bytes = realloc(a->bytes, a->digits * sizeof(uint8_t) / 2);
    for (i = 0; i < a->digits; i++) a->bytes[i] = result.bytes[i];

    /* Free the local bigints */
    bigint_free(&tempResult);
    bigint_free(&result);
}

/* Set all of the bigint's digits to 0 */
/* Takes O(log(a)) time */
void bigint_clear(bigint *a) {
    int i;
    for (i = 0; i < (a->digits / 2); i++) a->bytes[i] = 0;
}

#endif
