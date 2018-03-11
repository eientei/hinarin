//
// Created by user on 3/16/18.
//

#include "system_binding.h"
#include "util.h"

function(print) {
    js_print(args[0]);
    return jerry_create_undefined();
}

void bind_system() {
    jerry_value_t global = jerry_get_global_object();
    jerry_value_t system = jerry_create_object();
    js_register_prop_name(global, "system", system);
    jerry_release_value(global);

    js_register_function(system, "print", print, NULL);
    jerry_release_value(system);
}