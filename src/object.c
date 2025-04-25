#include <object.h>
#include <stdlib.h>

void reference(Object *obj) {
    obj->refCount++;
}

void dereference(Object *obj) {
    // printf("dereferencign object %p\n", obj);
    obj->refCount--;
    if (obj->refCount <= 0) {
        release(obj);
    }
}
 
void release(Object *obj) {
    // printf("Releasing object %p\n", obj);
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
