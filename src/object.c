#include <object.h>
#include <stdlib.h>
 
void release(Object *obj) {
    switch (obj->objectId) {
        case STRING_ID:
            free(((StringObject *)obj)->str);
            break;
        case FUNCTION_ID:
            free(((FunctionObject *)obj)->exprs);
            break;
    }
    free(obj->value);
    free(obj);
}
