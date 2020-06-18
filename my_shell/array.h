#ifndef __ARRAY_H__
#define __ARRAY_H__

/* array is a dynamic data structure that can grow during use */
/*
 * array_new function
 *      will create a new array with the given spaces but is initially empty (size = 0)
 *      all unused spaces are always initialized to NULL until something is added
 *      array_put and array_append are ways in which you can add to the array
 * array_renew function
 *      will decrease or increase the size, you can even remove all data
 *      this will also effect the count of the items if you decrease the size
 *      any added spaces when the array increases will be initialized to NULL
 * array_push function
 *      will increase the size of the array if space is needed, the item is placed on
 *      end of the array, this will also increment the array "size" by 1
 * array_set function
 *      will cause the count to be extended but does not resize the array
 *      any index larger than the current space is ignored and the function returns NULL
 */

struct array {
    int size;
    int last;
    int incr;
    void **data;
};

typedef struct {
    struct array *ptr;
} array;

/* allocation/reallocation/deallocation of array data structure */
array array_new(int);
int array_renew(array, int);
void array_data_delete(array);
void array_delete(array);

/* get/set property methods for array */
int array_set_incr(array, int);
int array_get_size(array);

/* item get/set/insertion into the array */
void *array_get(array, int);
void *array_set(array, int, void *);
int array_push(array, void *);

/* print function for debugging purposes */
void array_dump(array);

#endif
