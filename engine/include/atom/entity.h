#ifndef ATOM_ENTITY_H
#define ATOM_ENTITY_H

#include <stdint.h>
#include <stdbool.h>

typedef uint64_t entity_id;

#define ENTITY_NULL 0
#define MAX_ENTITIES 65536

entity_id entity_create(void);
void entity_destroy(entity_id id);
bool entity_is_alive(entity_id id);

#endif
