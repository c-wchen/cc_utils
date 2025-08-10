#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "json_helper.h"

int json_object_helper(void *struct_ptr, cJSON *obj, json2struct_t *jstruct_arr, uint32_t jstruct_size)
{
    assert(obj->type == cJSON_Object);
    for (uint32_t i = 0; i < jstruct_size; i++) {
        cJSON *item = cJSON_GetObjectItem(obj, jstruct_arr[i].name);
        if (item == NULL && jstruct_arr[i].must) {
            return -2;
        }

        void *ptr = struct_ptr + jstruct_arr[i].offset;
        switch (jstruct_arr[i].type) {
            case CH_TYPE_INT: {
                switch (jstruct_arr[i].length) {
                    case 1: {
                        *(int8_t *)ptr = (int8_t)item->valueint;
                        break;
                    }
                    case 2: {
                        *(int16_t *)ptr = (int16_t)item->valueint;
                        break;
                    }
                    case 4: {
                        *(int32_t *)ptr = (int32_t)item->valueint;
                        break;
                    }
                    default: { /* case 8 */
                        *(int64_t *)ptr = (int64_t)item->valueint;
                        break;
                    }
                }
                break;
            }
            case CH_TYPE_FLOAT: {
                switch (jstruct_arr[i].length) {
                    case 4: {
                        *(float *)ptr = (float)item->valuedouble;
                        break;
                    }
                    default: { /* case 8 */
                        *(double *)ptr = (double)item->valuedouble;
                        break;
                    }
                }
                break;
            }
            case CH_TYPE_STRING: {
                strncpy(ptr, item->valuestring, jstruct_arr[i].length);
                break;
            }
            default: {
                break;
            }
        }
    }
    return 0;
}