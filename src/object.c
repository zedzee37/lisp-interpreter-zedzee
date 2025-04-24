#include <object.h>
#include <stdlib.h>
 
void release(Object *obj) {
    switch (obj->objectType) {
        case STRING_ID:
            free(((StringObject *)obj->value)->str);
            break;
        case FUNCTION_ID:
            free(((FunctionObject *)obj->value)->exprs);
            break;
    }
    free(obj->value);
    free(obj);
}
