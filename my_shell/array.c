#include <stdio.h>
#include <stdlib.h>

#include "array.h"

/* create new array */
array array_new(int size) {
    array a;
    a.ptr = malloc(sizeof(struct array));
    a.ptr->size = 0;
    a.ptr->last = size;
    a.ptr->incr = size; /* increase linearly in initial size */
    a.ptr->data = malloc(sizeof(void*)*size);
    if (a.ptr->data==NULL) a.ptr->last = 0;
    for (int i=0;i<a.ptr->last;++i)
        a.ptr->data[i] = NULL;
    return a;
}

/* realloc the array, returns old size or -1 on failure */
int array_renew(array a, int last) {
    int retval = -1;
    if (a.ptr!=NULL) { /* caveat ~ can't do anything when a.ptr==NULL */
        if (last>0) {
            retval = a.ptr->last;
            a.ptr->data = realloc(a.ptr->data, sizeof(void*)*last);
            if (a.ptr->data==NULL) {
                a.ptr->size = a.ptr->last = 0;
                retval = -2;
            } else {
                a.ptr->last = last;
                if (a.ptr->size>last) /* truncated the array */
                    a.ptr->size = last;
                for (int i=retval;i<a.ptr->size;++i)
                    a.ptr->data[i] = NULL;
            }
        } else {
            free(a.ptr->data);
            a.ptr->size = a.ptr->last = 0;
        }
    }
    return retval;
}

/* free memory used by data items in the array (useful if you allocate the items too!) */
void array_data_delete(array a) {
     if (a.ptr!=NULL && a.ptr->data!=NULL)
        for (int i=0;i<a.ptr->size;++i)
            if (a.ptr->data[i]!=NULL) {
                free(a.ptr->data[i]);
                a.ptr->data[i] = NULL;
            }
}

/* free memory used by array */
void array_delete(array a) {
    if (a.ptr!=NULL) {
        if (a.ptr->data!=NULL)
            free(a.ptr->data);
        free(a.ptr);
    }
}

/* set array property value for increment (for dynamically expanding array) */
int array_set_incr(array a, int incr) {
    int retval = a.ptr->incr;
    if (incr>0)
        a.ptr->incr=incr;
    return retval;
}

/* return array property value for size (number of items in array) */
int array_get_size(array a) {
    int i = -1;
    if (a.ptr!=NULL)
        i = a.ptr->size;
    return i;
}

/* returns pointer or NULL on error */
void *array_get(array a, int i) {
    void *p = NULL;
    if (a.ptr!=NULL && i<a.ptr->size)
        p = a.ptr->data[i];
    return p;
}

/* replaces current data[i] with p */
/* returns old pointer, for freeing if needed, or NULL on error */
void *array_set(array a, int i, void *p) {
    void *q = NULL;
    if (a.ptr!=NULL && i<a.ptr->last) {
        q = a.ptr->data[i];
        a.ptr->data[i] = p;
        if (a.ptr->size<=i)
            a.ptr->size = i+1;
    }
    return q;
}

/* increases size of array dynamically */
int array_push(array a, void *p) {
    int retval = -1;
    if (a.ptr!=NULL) {
        retval = a.ptr->size;
        if (a.ptr->size>=a.ptr->last) { /* realloc because no more room */
            a.ptr->data = realloc(a.ptr->data, sizeof(void*)*(a.ptr->last+a.ptr->incr));
            if (a.ptr->data==NULL) {
                a.ptr->size = a.ptr->last = 0;
                retval = -2;
            } else
                a.ptr->last += a.ptr->incr;
        }
        if (a.ptr->data!=NULL) { /* check in case realloc failed! */
            a.ptr->data[a.ptr->size] = p;
            a.ptr->size += 1;
        }
    }
    return retval;
}

/* print out contents of array */
void array_dump(array a) {
    int i;
    if (a.ptr!=NULL) {
        fprintf(stdout, "array:\n\tsize: %d\n\tlast: %d\n",a.ptr->size,a.ptr->last);
        for (i=0; i<a.ptr->size; ++i)
            if (a.ptr->data[i]!=NULL)
                fprintf(stdout, "\t[%d] = \"%s\"\n", i, a.ptr->data[i]);
            else
                fprintf(stdout, "\t[%d] = NULL\n", i);
    } else
        fprintf(stdout, "array: NULL\n");
}
