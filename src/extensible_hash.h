#ifndef H_EXTENSIBLE_HASH
#define H_EXTENSIBLE_HASH

typedef void* key_type;
/* 
 * Internal record structure used to to store pointers to the data
 */
struct hash_record {
    void * data;
    key_type key;
    unsigned int hash;
};

/*
 * Structure for the buckets
 */
typedef struct {
    struct hash_record ** contents; /* array of hash_records */ 
    int bits; /* number of bits of the hash used */
    int amount; /* number of elements in the bucket */
} bucket_t;

/* y
 * The actual hash map
 */
typedef struct {
    bucket_t ** array; /* array of buckets */
    int bit_size; /* size in bits of the bucket array */
    unsigned int (*hash_f)(key_type); /* pointer to a hash function */
} hash_map;

/*
 * Fpunctions for the hashmap
 */
hash_map * hash_map_new(unsigned int (*func)(key_type));
void hash_map_add(key_type key, void * data, hash_map * map);
void * hash_map_search(void *key, hash_map *map);
void hash_map_free(hash_map * map);


#endif
