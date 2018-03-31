//
// Created by user on 3/23/18.
//

#include <xsAll.h>
#include "bind_root.h"
#include "hinarin.h"
#include <stdbool.h>

void hinarin_root_print(xsMachine *the) {
    for (xsIntegerValue i = 0; i < xsToInteger(xsArgc); i++) {
        if (i > 0) {
            printf(" ");
        }

        switch (xsTypeOf(xsArg(i))) {
            case xsSymbolType:
                xsResult = xsCall1(xsGlobal, xsID("String"), xsArg(i));
                printf("_'%s'", xsToString(xsResult));
                break;
            case xsStringType:
            case xsStringXType:
                printf("\"%s\"", xsToString(xsArg(i)));
                break;
            case xsReferenceType:
                if (xsIsInstanceOf(xsArg(i), xsFunctionPrototype)) {
                    char *name = "";
                    txSlot *reference = xsArg(i).value.reference;
                    if (reference->next->ID != XS_NO_ID) {
                        name = xsName(reference->next->ID);
                    }
                    printf("function %s(", name);
                    char n = 'a';
                    for (int a = 0; a < reference->next->next->next->value.integer; a++) {
                        if (a > 0) {
                            printf(", ");
                        }
                        if (a < 26) {
                            printf("%c", n++);
                        } else {
                            printf("arg%d", a);
                        }

                    }
                    printf(") { ... }");
                } else if (xsIsInstanceOf(xsArg(i), xsArrayPrototype)) {
                    printf("[");
                    bool comma = false;
                    for (xsSlot iterator = xsEnumerate(xsArg(i)), next = xsCall0(iterator, xsID("next")); !xsTest(xsGet(next, xsID("done"))); next = xsCall0(iterator, xsID("next"))) {
                        xsSlot value = xsGet(next, xsID("value"));
                        if (comma) {
                            printf(", ");
                        }

                        xsCall1(xsGlobal, xsID("print"), xsGetAt(xsArg(i), value));

                        if (!comma) {
                            comma = true;
                        }
                    }
                    printf("]");
                } else if (xsIsInstanceOf(xsArg(i), xsStringPrototype)) {
                    printf("\"%s\"", xsToString(xsArg(i)));
                } else if (xsIsInstanceOf(xsArg(i), xsSymbolPrototype)) {
                    xsResult = xsCall1(xsGlobal, xsID("String"), xsArg(i));
                    printf("_'%s'", xsToString(xsResult));
                } else if (xsIsInstanceOf(xsArg(i), xsHostPrototype)) {
                    printf("[Native Object]");
                } else if (
                        xsIsInstanceOf(xsArg(i), xsBooleanPrototype) ||
                        xsIsInstanceOf(xsArg(i), xsDatePrototype) ||
                        xsIsInstanceOf(xsArg(i), xsErrorPrototype) ||
                        xsIsInstanceOf(xsArg(i), xsMapPrototype) ||
                        xsIsInstanceOf(xsArg(i), xsNumberPrototype) ||
                        xsIsInstanceOf(xsArg(i), xsArrayBufferPrototype) ||
                        xsIsInstanceOf(xsArg(i), xsPromisePrototype) ||
                        xsIsInstanceOf(xsArg(i), xsProxyPrototype) ||
                        xsIsInstanceOf(xsArg(i), xsSetPrototype) ||
                        xsIsInstanceOf(xsArg(i), xsDataViewPrototype) ||
                        xsIsInstanceOf(xsArg(i), xsRegExpPrototype) ||
                        xsIsInstanceOf(xsArg(i), xsTypedArrayPrototype) ||
                        xsIsInstanceOf(xsArg(i), xsWeakMapPrototype) ||
                        xsIsInstanceOf(xsArg(i), xsWeakSetPrototype)
                        ) {
                    printf("%s", xsToString(xsArg(i)));
                } else {
                    printf("{");
                    bool comma = false;
                    for (xsSlot iterator = xsEnumerate(xsArg(i)), next = xsCall0(iterator, xsID("next")); !xsTest(xsGet(next, xsID("done"))); next = xsCall0(iterator, xsID("next"))) {
                        xsSlot value = xsGet(next, xsID("value"));
                        if (comma) {
                            printf(", ");
                        }

                        printf("%s: ", xsToString(value));
                        xsCall1(xsGlobal, xsID("print"), xsGetAt(xsArg(i), value));

                        if (!comma) {
                            comma = true;
                        }
                    }
                    printf("}");
                }
                break;
            default:
                printf("%s", xsToString(xsArg(i)));
                break;
        }
    }
}

void hinarin_root_println(xsMachine *the) {
    hinarin_root_print(the);
    printf("\n");
}

void hinarin_root_xpos(xsMachine *the) {
    hinarin_context *hinarin = xsGetContext(the);
    xsResult = xsNumber(hinarin->xpos);
}

void hinarin_root_ypos(xsMachine *the) {
    hinarin_context *hinarin = xsGetContext(the);
    xsResult = xsNumber(hinarin->ypos);
}

void hinarin_root_width(xsMachine *the) {
    hinarin_context *hinarin = xsGetContext(the);
    xsResult = xsNumber(hinarin->width);
}

void hinarin_root_height(xsMachine *the) {
    hinarin_context *hinarin = xsGetContext(the);
    xsResult = xsNumber(hinarin->height);
}

void hinarin_root_frame(xsMachine *the) {
    hinarin_context *hinarin = xsGetContext(the);
    xsResult = xsNumber(hinarin->frame);
}

void hinarin_root_milliseconds(xsMachine *the) {
    hinarin_context *hinarin = xsGetContext(the);
    xsResult = xsNumber(hinarin->milliseconds);
}

void hinarin_root_get_width(xsMachine *the) {
    hinarin_context *hinarin = xsGetContext(the);
    xsResult = xsNumber(hinarin->width);
}

void hinarin_root_get_height(xsMachine *the) {
    hinarin_context *hinarin = xsGetContext(the);
    xsResult = xsNumber(hinarin->height);
}

void hinarin_bind_root(xsMachine *the) {
    xsDefine(xsGlobal, xsID("print"), xsNewHostFunction(hinarin_root_print, 1), xsChangeAll);
    xsDefine(xsGlobal, xsID("println"), xsNewHostFunction(hinarin_root_println, 1), xsChangeAll);
    txSlot window_obj = xsNewObject();
    xsDefine(xsGlobal, xsID("window"), window_obj, xsChangeAll);
    xsDefine(window_obj, xsID("xpos"), xsNewHostFunction(hinarin_root_xpos, 0), xsDontSet | xsDontDelete | xsIsGetter);
    xsDefine(window_obj, xsID("ypos"), xsNewHostFunction(hinarin_root_ypos, 0), xsDontSet | xsDontDelete | xsIsGetter);
    xsDefine(window_obj, xsID("width"), xsNewHostFunction(hinarin_root_width, 0), xsDontSet | xsDontDelete | xsIsGetter);
    xsDefine(window_obj, xsID("height"), xsNewHostFunction(hinarin_root_height, 0), xsDontSet | xsDontDelete | xsIsGetter);
    xsDefine(window_obj, xsID("frame"), xsNewHostFunction(hinarin_root_frame, 0), xsDontSet | xsDontDelete | xsIsGetter);
    xsDefine(window_obj, xsID("milliseconds"), xsNewHostFunction(hinarin_root_milliseconds, 0), xsDontSet | xsDontDelete | xsIsGetter);
    xsDefine(window_obj, xsID("getWidth"), xsNewHostFunction(hinarin_root_get_width, 0), xsDontSet | xsDontDelete);
    xsDefine(window_obj, xsID("getHeight"), xsNewHostFunction(hinarin_root_get_height, 0), xsDontSet | xsDontDelete);
}