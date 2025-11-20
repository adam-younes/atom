#include <components/light.h>
#include <string.h>

void light_component_init(light_component *l, entity_id id) {
  memset(l, 0, sizeof(light_component));
  l->entity = id;
  l->color = (vec3){1, 1, 1};
  l->intensity = 1.0f;
}
