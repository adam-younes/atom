#include <atom/scene/entity.h>
#include <string.h>

static uint64_t next_entity_id = 1;
static bool entity_alive[MAX_ENTITIES] = {0};

entity_id entity_create(void) {
  if (next_entity_id >= MAX_ENTITIES) {
    return ENTITY_NULL;
  }
  entity_id id = next_entity_id++;
  entity_alive[id] = true;
  return id;
}

void entity_destroy(entity_id id) {
  if (id < MAX_ENTITIES) {
    entity_alive[id] = false;
  }
}

bool entity_is_alive(entity_id id) {
  if (id == ENTITY_NULL || id >= MAX_ENTITIES) {
    return false;
  }
  return entity_alive[id];
}
