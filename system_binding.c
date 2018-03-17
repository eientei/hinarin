//
// Created by user on 3/16/18.
//

#include "system_binding.h"
#include "modules_binding.h"
#include "util.h"

#include <stdio.h>

hinarin_function(hinarin_system_print) {
    for (int i = 0; i < args_count; i++) {
        printf("%d: ", i);
        hinarin_print(args[i]);
    }
    return jerry_create_undefined();
}

hinarin_function(hinarin_system_import) {
    hinarin_string(args[0], import);
    char *iter;
    for (iter = import; *iter != '\0'; iter++) {
        if (*iter == '.') {
            *iter = '\0';
            iter++;
            break;
        }
    }

    jerry_value_t result = jerry_create_null();
    jerry_value_t global = jerry_get_global_object();
    jerry_value_t modules = hinarin_get_name(global, "modules");

    modules_t *mods = NULL;
    jerry_get_object_native_pointer(modules, (void **) &mods, NULL);

    for (uint32_t i = 0; i < jerry_get_array_length(mods->modules_list); i++) {
        jerry_value_t module = jerry_get_property_by_index(mods->modules_list, i);
        hinarin_get_name_string(module, "name", name);
        if (hinarin_equals(name, import)) {
            jerry_value_t package = hinarin_get_name(module, "exports");
            int err = 0;
            while (*iter != '\0') {
                char *part = iter;
                for (;*iter != '\0'; iter++) {
                    if (*iter == '.') {
                        *iter = '\0';
                        iter++;
                        break;
                    }
                }

                if (hinarin_has_key(package, part)) {
                    jerry_value_t npackage = hinarin_get_name(package, part);
                    jerry_release_value(package);
                    package = npackage;
                } else {
                    err = 1;
                    break;
                }
            }
            if (err) {
                jerry_release_value(package);
            } else {
                jerry_release_value(result);
                result = package;
            }
            break;
        }
        jerry_release_value(module);
    }

    jerry_release_value(modules);
    jerry_release_value(global);


    return result;
}

void hinarin_system_bind() {
    jerry_value_t global = jerry_get_global_object();
    jerry_value_t system = jerry_create_object();
    hinarin_set_name_value(global, "system", system);
    hinarin_set_name_function(system, "print", hinarin_system_print, NULL);
    hinarin_set_name_function(system, "import", hinarin_system_import, NULL);
    jerry_release_value(system);
    jerry_release_value(global);
}