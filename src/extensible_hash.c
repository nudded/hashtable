#include "stdio.h"
#include "stdlib.h"
#include "extensible_hash.h"

#define BUCKET_SIZE (2)
#define DEFAULT_BITS (1)


hash_map * hash_map_new(unsigned int (*func)(key_type)){
    hash_map *map = malloc(sizeof(hash_map));
    int i;

    /* set our hash_function */
    map->hash_f = func;

    /* default 2 buckets and pointer array of size 2 */
    map->bit_size = DEFAULT_BITS;

    map->array = malloc(sizeof(bucket_t *) * (1 << map->bit_size));
    
    for(i = 0; i < (1 << map->bit_size); i++) {
        /* each bucket's bits is equal to the current bit_size */
        map->array[i] = malloc(sizeof(bucket_t));
        map->array[i]->bits = map->bit_size;
        map->array[i]->amount = 0;

        map->array[i]->contents = malloc(sizeof(struct hash_record *)
                * BUCKET_SIZE);
    }

    return map;
}


unsigned int hash_key(key_type key, hash_map * map) {
    return (map->hash_f)(key);
}

/* 
 * Return the highest bits from the hash
 */
unsigned int bit_index(unsigned int hash, int bits) {
    return ((unsigned int) (hash & -(1<<(32-bits+1) - 1))) >> (32-bits); 
}

void hash_map_add(key_type key, void * data, hash_map * map) {
    int hash = hash_key(key, map);

    /* limit the hash to bit_size bits */
    int index = bit_index(hash, map->bit_size);

    /* bucket to place record in */
    bucket_t *bucket = map->array[index];

    /* keep on expanding till we can fit 'em in the bucket */
    while(bucket->amount == BUCKET_SIZE) { 
        bucket_t *new_bucket = malloc(sizeof(bucket_t));
        int i,j = 0;
        /* new array to copy the values in that should remain in bucket */
        struct hash_record ** new_arr = malloc(sizeof(struct hash_record *) * BUCKET_SIZE);
        
        /* Initialize new_bucket. bits will be set later on */
        new_bucket->contents = malloc(sizeof(struct hash_record *) * BUCKET_SIZE);
        new_bucket->amount = 0;

        /* increase pointer array by factor 2 */
        if( map->bit_size == bucket->bits) {
            /* create new bucket array */
            int i;
            bucket_t **new_arr;

            map->bit_size++;
            new_arr = malloc(sizeof(bucket_t*) * (1 << map->bit_size));

            /* make sure each entry in the new array points to a bucket */
            for(i = 0; i < (1 << map->bit_size) ; i ++) {
                new_arr[i] = map->array[i/2];
            }

            /* remove old pointer array, use the new one */
            free(map->array);
            map->array = new_arr;
        } 
        
        /* set new_bucket's bits and increment the old one too */
        new_bucket->bits = ++(bucket->bits);

        /* distribute bucket's contents over the new and old bucket */
        for(i = 0; i < bucket->amount; i ++) {
            int important_bit = bit_index(bucket->contents[i]->hash, bucket->bits) & 1;

            if (important_bit == 1) {
                new_bucket->contents[new_bucket->amount] = bucket->contents[i];
                new_bucket->amount++;
                bucket->amount--;
            } else {
                new_arr[j++] = bucket->contents[i];
            }
        }

        free(bucket->contents);
        bucket->contents = new_arr;

        map->array[index+1] = new_bucket; 

        /* this is the bucket we should add to */
        index = bit_index(hash, map->bit_size);
        bucket = map->array[index];
    }

    struct hash_record *record = malloc(sizeof(struct hash_record));
    
    record->hash = hash;
    record->key = key;
    record->data = data;

    bucket->contents[bucket->amount] = record;
    bucket->amount++;
}

/* This should be easy */
void * hash_map_search(void *key, hash_map *map);

/* Good practices something something */
void hash_map_free(hash_map * map);

/*
 * Convenience functions to see how the hash map behaves
 */
void print_bucket(bucket_t *bucket) {
    int i;
    
    printf("bucket: amount = %d bits = %d\n", bucket->amount, bucket->bits);
    for(i = 0; i < bucket->amount; i ++) {
        printf("key: %d data: %s\n", (int)bucket->contents[i]->key, (char *)bucket->contents[i]->data);
    }
}

void print_hash_map(hash_map * map) {
    int i;

    printf("bit size: %d\n", map->bit_size);

    for(i = 0; i < (1 << map->bit_size) ; i ++) {
        print_bucket(map->array[i]);
    }
}

/* 
 * since we take the high bits we shift 28 to the left (since we're adding 1,
 * 2, 3 and 4)
 */
unsigned int hashf(key_type key) {
    return (unsigned int) key << 28;
}

int main(int argc, char **argv) {
    hash_map * map = hash_map_new(hashf);
    hash_map_add((key_type)1, "1", map);
    hash_map_add((key_type)2, "2", map);
    hash_map_add((key_type)3, "3", map);
    hash_map_add((key_type)4, "4", map);
    print_hash_map(map);
}
