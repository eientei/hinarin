//
// Created by user on 3/12/18.
//

#include "util.h"

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#include <nuklear.h>
#include <stdio.h>
#include <string.h>
#include <nuklear_glfw_gl3.h>

static struct nk_rect to_rect(jerry_value_t value) {
    double x = hinarin_get_name_number(value, "x");
    double y = hinarin_get_name_number(value, "y");
    double w = hinarin_get_name_number(value, "w");
    double h = hinarin_get_name_number(value, "h");
    return nk_rect((float) x, (float) y, (float) w, (float) h);
}

static jerry_value_t from_rect(struct nk_rect rect) {
    jerry_value_t object = jerry_create_object();
    hinarin_set_name_number(object, "x", rect.x);
    hinarin_set_name_number(object, "y", rect.y);
    hinarin_set_name_number(object, "w", rect.w);
    hinarin_set_name_number(object, "h", rect.h);
    return object;
}

static struct nk_vec2 to_vec(jerry_value_t value) {
    double x = hinarin_get_name_number(value, "x");
    double y = hinarin_get_name_number(value, "y");
    return nk_vec2((float) x, (float) y);
}

static jerry_value_t from_vec(struct nk_vec2 vec) {
    jerry_value_t object = jerry_create_object();
    hinarin_set_name_number(object, "x", vec.x);
    hinarin_set_name_number(object, "y", vec.y);
    return object;
}

/*


static struct nk_vec2 get_vec2(jerry_value_t value) {
    double x = js_prop_double(value, "x");
    double y = js_prop_double(value, "y");
    return nk_vec2((float) x, (float) y);
}

static jerry_value_t from_vec2(struct nk_vec2 vec) {
    jerry_value_t object = jerry_create_object();
    js_register_double(object, "x", vec.x);
    js_register_double(object, "y", vec.y);
    return object;
}

static nk_flags obj_to_flags(jerry_value_t obj) {
    nk_flags flags = 0;

    if (js_prop_bool(obj, "border")) {
        flags |= NK_WINDOW_BORDER;
    }

    if (js_prop_bool(obj, "movable")) {
        flags |= NK_WINDOW_MOVABLE;
    }

    if (js_prop_bool(obj, "scalable")) {
        flags |= NK_WINDOW_SCALABLE;
    }

    if (js_prop_bool(obj, "closable")) {
        flags |= NK_WINDOW_CLOSABLE;
    }

    if (js_prop_bool(obj, "minimizable")) {
        flags |= NK_WINDOW_MINIMIZABLE;
    }

    if (!js_prop_bool(obj, "scrollbar")) {
        flags |= NK_WINDOW_NO_SCROLLBAR;
    }

    if (js_prop_bool(obj, "title")) {
        flags |= NK_WINDOW_TITLE;
    }

    if (js_prop_bool(obj, "scrollhide")) {
        flags |= NK_WINDOW_SCROLL_AUTO_HIDE;
    }

    if (js_prop_bool(obj, "background")) {
        flags |= NK_WINDOW_BACKGROUND;
    }

    if (js_prop_bool(obj, "scaleleft")) {
        flags |= NK_WINDOW_SCALE_LEFT;
    }

    if (js_prop_bool(obj, "noinpu")) {
        flags |= NK_WINDOW_NO_INPUT;
    }

    return flags;
}

#define define_context(expected_args, name) \
    if (args_count != (expected_args)) { \
        return js_create_abort("Invalid arg count in function " #name "()"); \
    } \
    struct nk_context *ctx; \
    jerry_get_object_native_pointer(function_obj, (void **) &ctx, NULL);

function(begin) {
    define_context(3, begin);

    js_define_string(args[0], title);
    struct nk_rect rect = get_rect(args[1]);
    nk_flags flags = obj_to_flags(args[2]);

    if (nk_begin(ctx, (const char *) title, rect, flags)) {
        return jerry_create_boolean(true);
    } else {
        return jerry_create_boolean(false);
    }
}

function(begin_titled) {
    define_context(4, begin_titled);

    js_define_string(args[0], name);
    js_define_string(args[1], title);
    struct nk_rect rect = get_rect(args[2]);

    nk_flags flags = obj_to_flags(args[3]);

    return jerry_create_boolean(nk_begin_titled(ctx, (const char *) name, (const char *) title, rect, flags) != 0);
}

function(end) {
    define_context(0, end);

    nk_end(ctx);
    return jerry_create_undefined();
}


function(window_get_bounds) {
    define_context(0, window_get_bounds);

    struct nk_rect bounds = nk_window_get_bounds(ctx);

    return from_rect(bounds);
}

function(window_get_position) {
    define_context(0, window_get_position);

    struct nk_vec2 pos = nk_window_get_position(ctx);
    
    return from_vec2(pos);
}

function(window_get_size) {
    define_context(0, window_get_size);

    struct nk_vec2 size = nk_window_get_size(ctx);

    return from_vec2(size);
}

function(window_get_width) {
    define_context(0, window_get_width);

    double width = nk_window_get_width(ctx);

    return jerry_create_number(width);
}

function(window_get_height) {
    define_context(0, window_get_height);

    double height = nk_window_get_height(ctx);

    return jerry_create_number(height);
}

function(window_get_content_region) {
    define_context(0, window_get_content_region);

    struct nk_rect region = nk_window_get_content_region(ctx);

    return from_rect(region);
}

function(window_get_content_region_min) {
    define_context(0, window_get_content_region_min);

    struct nk_vec2 region = nk_window_get_content_region_min(ctx);

    return from_vec2(region);
}

function(window_get_content_region_max) {
    define_context(0, window_get_content_region_max);

    struct nk_vec2 region = nk_window_get_content_region_max(ctx);

    return from_vec2(region);
}

function(window_get_content_region_size) {
    define_context(0, window_get_content_region_size);

    struct nk_vec2 region = nk_window_get_content_region_size(ctx);

    return from_vec2(region);
}

function(window_has_focus) {
    define_context(0, window_has_focus);

    return jerry_create_boolean(nk_window_has_focus(ctx) != 0);
}

function(window_is_collapsed) {
    define_context(1, window_is_collapsed);

    js_define_string(args[0], name);

    return jerry_create_boolean(nk_window_is_collapsed(ctx, (const char *) name) != 0);
}

function(window_is_closed) {
    define_context(1, window_is_closed);

    js_define_string(args[0], name);

    return jerry_create_boolean(nk_window_is_closed(ctx, (const char *) name) != 0);
}

function(window_is_hidden) {
    define_context(1, window_is_hidden);

    js_define_string(args[0], name);

    return jerry_create_boolean(nk_window_is_hidden(ctx, (const char *) name) != 0);
}

function(window_is_active) {
    define_context(1, window_is_active);

    js_define_string(args[0], name);

    return jerry_create_boolean(nk_window_is_active(ctx, (const char *) name) != 0);
}

function(window_is_hovered) {
    define_context(0, window_is_hovered);

    return jerry_create_boolean(nk_window_is_hovered(ctx) != 0);
}

function(window_is_any_hovered) {
    define_context(0, window_is_any_hovered);

    return jerry_create_boolean(nk_window_is_any_hovered(ctx) != 0);
}


function(item_is_any_active) {
    define_context(0, item_is_any_active);

    return jerry_create_boolean(nk_item_is_any_active(ctx) != 0);
}

function(window_set_bounds) {
    define_context(2, window_set_bounds);

    js_define_string(args[0], name);

    struct nk_rect rect = get_rect(args[1]);

    nk_window_set_bounds(ctx, (const char *) name, rect);

    return jerry_create_undefined();
}

function(window_set_position) {
    define_context(2, window_set_position);

    js_define_string(args[0], name);

    struct nk_vec2 vec = get_vec2(args[1]);

    nk_window_set_position(ctx, (const char *) name, vec);

    return jerry_create_undefined();
}

function(window_set_size) {
    define_context(2, window_set_size);

    js_define_string(args[0], name);

    struct nk_vec2 vec = get_vec2(args[1]);

    nk_window_set_size(ctx, (const char *) name, vec);

    return jerry_create_undefined();
}


function(window_set_focus) {
    define_context(1, window_set_focus);

    js_define_string(args[0], name);

    nk_window_set_focus(ctx, (const char *) name);

    return jerry_create_undefined();
}

function(window_close) {
    define_context(1, window_close);

    js_define_string(args[0], name);

    nk_window_close(ctx, (const char *) name);

    return jerry_create_undefined();
}

function(window_collapse) {
    define_context(2, window_collapse);

    js_define_string(args[0], name);

    bool collapsed = js_to_bool(args[1]);

    nk_window_collapse(ctx, (const char *) name, collapsed ? NK_MINIMIZED : NK_MAXIMIZED);

    return jerry_create_undefined();
}

function(window_collapse_if) {
    define_context(3, window_collapse_if);

    js_define_string(args[0], name);

    bool collapsed = js_to_bool(args[1]);

    bool commit = js_to_bool(args[2]);

    nk_window_collapse_if(ctx, (const char *) name, collapsed ? NK_MINIMIZED : NK_MAXIMIZED, commit);

    return jerry_create_undefined();
}

function(window_show) {
    define_context(2, window_show);

    js_define_string(args[0], name);

    bool show = js_to_bool(args[1]);

    nk_window_show(ctx, (const char *) name, show ? NK_SHOWN : NK_HIDDEN);

    return jerry_create_undefined();
}

function(window_show_if) {
    define_context(3, window_show_if);

    js_define_string(args[0], name);

    bool show = js_to_bool(args[1]);

    bool commit = js_to_bool(args[2]);

    nk_window_show_if(ctx, (const char *) name, show ? NK_SHOWN : NK_HIDDEN, commit);

    return jerry_create_undefined();
}

function(layout_set_min_row_height) {
    define_context(1, layout_set_min_row_height);

    double height = js_to_double(args[0]);

    nk_layout_set_min_row_height(ctx, (float) height);

    return jerry_create_undefined();
}

function(layout_reset_min_row_height) {
    define_context(0, layout_reset_min_row_height);
    nk_layout_reset_min_row_height(ctx);

    return jerry_create_undefined();
}

function(layout_widget_bounds) {
    define_context(0, layout_widget_bounds);

    struct nk_rect bounds = nk_layout_widget_bounds(ctx);

    return from_rect(bounds);
}

function(layout_ratio_from_pixel) {
    define_context(1, layout_ratio_from_pixel);

    double pixel_width = js_to_double(args[0]);

    double ratio = nk_layout_ratio_from_pixel(ctx, (float) pixel_width);

    return jerry_create_number(ratio);
}

function(layout_row_dynamic) {
    define_context(2, layout_row_dynamic);

    double height = js_to_double(args[0]);
    double cols = js_to_double(args[1]);

    nk_layout_row_dynamic(ctx, (float) height, (int) cols);

    return jerry_create_undefined();
}

function(layout_row_static) {
    define_context(3, layout_row_static);

    double height = js_to_double(args[0]);
    double item_width = js_to_double(args[1]);
    double cols = js_to_double(args[2]);

    nk_layout_row_static(ctx, (float) height, (int) item_width, (int) cols);

    return jerry_create_undefined();
}

function(layout_row_begin) {
    define_context(3, layout_row_begin);

    bool is_dynamic = js_to_bool(args[0]);
    double height = js_to_double(args[1]);
    double cols = js_to_double(args[2]);

    nk_layout_row_begin(ctx, is_dynamic ? NK_DYNAMIC : NK_STATIC, (float) height, (int) cols);

    return jerry_create_undefined();
}

function(layout_row_push) {
    define_context(1, layout_row_push);

    double value = js_to_double(args[0]);

    nk_layout_row_push(ctx, (float) value);

    return jerry_create_undefined();
}

function(layout_row_end) {
    define_context(0, layout_row_end);

    nk_layout_row_end(ctx);

    return jerry_create_undefined();
}

function(layout_row) {
    define_context(4, layout_row);

    bool is_dynamic = js_to_bool(args[0]);
    double height = js_to_double(args[1]);
    double cols = js_to_double(args[2]);

    jerry_value_t ratio_opt = js_prop(args[3], "ratio");
    double *ratio = NULL;
    if (!jerry_value_is_null(ratio_opt)) {
        double val = js_to_double(ratio_opt);
        ratio = &val;
    }
    jerry_release_value(ratio_opt);

    nk_layout_row(ctx, is_dynamic ? NK_DYNAMIC : NK_STATIC, (float) height, (int) cols, (const float *) ratio);

    js_register_double(args[3], "ratio", ratio == NULL ? 0 : *ratio);

    return jerry_create_undefined();
}

function(layout_row_template_begin) {
    define_context(1, layout_row_template_begin);
    
    double row_height = js_to_double(args[0]);
    
    nk_layout_row_template_begin(ctx, (float) row_height);

    return jerry_create_undefined();
}

function(layout_row_template_push_dynamic) {
    define_context(0, layout_row_template_push_dynamic);
    
    nk_layout_row_template_push_dynamic(ctx);

    return jerry_create_undefined();
}

function(layout_row_template_push_variable) {
    define_context(1, layout_row_template_push_variable);

    double min_width = js_to_double(args[0]);

    nk_layout_row_template_push_variable(ctx, (float) min_width);

    return jerry_create_undefined();
}

function(layout_row_template_push_static) {
    define_context(1, layout_row_template_push_static);
    
    double width = js_to_double(args[0]);
    
    nk_layout_row_template_push_static(ctx, (float) width);

    return jerry_create_undefined();
}

function(layout_row_template_end) {
    define_context(1, layout_row_template_end);
    
    nk_layout_row_template_end(ctx);
    
    return jerry_create_undefined();
}

function(layout_space_begin) {
    define_context(3, layout_space_begin);

    bool is_dynamic = js_to_bool(args[0]);
    double height = js_to_double(args[1]);
    double widget_count = js_to_double(args[2]);
    
    nk_layout_space_begin(ctx, is_dynamic ? NK_DYNAMIC : NK_STATIC, (float) height, (int) widget_count);

    return jerry_create_undefined();
}

function(layout_space_push) {
    define_context(1, layout_space_push);

    struct nk_rect rect = get_rect(args[0]);
    
    nk_layout_space_push(ctx, rect);

    return jerry_create_undefined();
}

function(layout_space_end) {
    define_context(0, layout_space_end);
    
    nk_layout_space_end(ctx);

    return jerry_create_undefined();
}

function(layout_space_bounds) {
    define_context(0, layout_space_bounds);
    
    struct nk_rect bounds = nk_layout_space_bounds(ctx);

    return from_rect(bounds);
}

function(layout_space_to_screen) {
    define_context(1, layout_space_to_screen);

    struct nk_vec2 vec = get_vec2(args[0]);

    struct nk_vec2 coords = nk_layout_space_to_screen(ctx, vec);

    return from_vec2(coords);
}

function(layout_space_to_local) {
    define_context(1, layout_space_to_local);

    struct nk_vec2 vec = get_vec2(args[0]);

    struct nk_vec2 coords = nk_layout_space_to_local(ctx, vec);

    return from_vec2(coords);
}

function(layout_space_rect_to_screen) {
    define_context(1, layout_space_rect_to_screen);
    
    struct nk_rect rect = get_rect(args[0]);

    struct nk_rect space = nk_layout_space_rect_to_screen(ctx, rect);

    return from_rect(space);
}

function(layout_space_rect_to_local) {
    define_context(1, layout_space_rect_to_local);

    struct nk_rect rect = get_rect(args[0]);

    struct nk_rect space = nk_layout_space_rect_to_local(ctx, rect);
    
    return from_rect(space);
}

function(group_begin) {
    define_context(2, group_begin);

    js_define_string(args[0], title);
    nk_flags flags = obj_to_flags(args[1]);

    return jerry_create_boolean(nk_group_begin(ctx, (const char *) title, flags) != 0);
}


function(group_begin_titled) {
    define_context(3, group_begin_titled);

    js_define_string(args[0], name);
    js_define_string(args[1], title);
    nk_flags flags = obj_to_flags(args[2]);

    return jerry_create_boolean(nk_group_begin_titled(ctx, (const char *) name, (const char *) title, flags) != 0);
}

function(group_end) {
    define_context(0, group_end);

    nk_group_end(ctx);

    return jerry_create_undefined();
}

function(group_scrolled_offset_begin) {
    define_context(3, group_scrolled_offset_begin);

    double *x = NULL;
    double *y = NULL;

    jerry_value_t x_opt = js_prop(args[0], "x");
    if (!jerry_value_is_null(x_opt)) {
        double val = js_to_double(x_opt);
        x = &val;
    }
    jerry_release_value(x_opt);

    jerry_value_t y_opt = js_prop(args[0], "y");
    if (!jerry_value_is_null(y_opt)) {
        double val = js_to_double(y_opt);
        y = &val;
    }
    jerry_release_value(y_opt);

    js_define_string(args[1], title);
    nk_flags flags = obj_to_flags(args[2]);

    jerry_value_t bln = jerry_create_boolean(nk_group_scrolled_offset_begin(ctx, (nk_uint *) x, (nk_uint *) y, (const char *) title, flags) != 0);

    js_register_double(args[0], "x", x == NULL ? 0 : *x);
    js_register_double(args[0], "y", y == NULL ? 0 : *y);

    return bln;
}

function(group_scrolled_begin) {
    define_context(3, group_scrolled_begin);

    jerry_value_t x_opt = js_prop(args[0], "x");
    jerry_value_t y_opt = js_prop(args[0], "y");
    js_define_string(args[1], title);
    nk_flags flags = obj_to_flags(args[2]);

    struct nk_scroll *off = NULL;

    if (!jerry_value_is_null(x_opt) && !jerry_value_is_null(y_opt)) {
        struct nk_scroll off_val;
        off_val.x = (nk_uint) js_to_double(x_opt);
        off_val.y = (nk_uint) js_to_double(y_opt);
        off = &off_val;
    }
    jerry_release_value(x_opt);
    jerry_release_value(y_opt);

    jerry_value_t bln = jerry_create_boolean(nk_group_scrolled_begin(ctx, off, (const char *) title, flags) != 0);

    js_register_double(args[0], "x", off == NULL ? 0 : off->x);
    js_register_double(args[0], "y", off == NULL ? 0 : off->y);

    return bln;
}

function(group_scrolled_end) {
    define_context(0, group_scrolled_end);

    nk_group_scrolled_end(ctx);

    return jerry_create_undefined();
}

function(tree_push) {
    define_context(3, tree_push);

    bool is_node = js_to_bool(args[0]);
    js_define_string(args[1], title);
    bool is_maximized = js_to_bool(args[2]);

    return jerry_create_boolean(nk_tree_push(ctx, is_node ? NK_TREE_NODE : NK_TREE_TAB, (const char *) title, is_maximized ? NK_MAXIMIZED : NK_MINIMIZED) != 0);
}

function(tree_push_id) {
    define_context(4, tree_push_id);

    bool is_node = js_to_bool(args[0]);
    js_define_string(args[1], title);
    bool is_maximized = js_to_bool(args[2]);
    double id = js_to_double(args[3]);

    return jerry_create_boolean(nk_tree_push_id(ctx, is_node ? NK_TREE_NODE : NK_TREE_TAB, (const char *) title, is_maximized ? NK_MAXIMIZED: NK_MINIMIZED, (int) id) != 0);
}

function(tree_push_hashed) {
    define_context(5, tree_push_hased);

    bool is_node = js_to_bool(args[0]);
    js_define_string(args[1], title);
    bool is_maximized = js_to_bool(args[2]);
    js_define_string(args[3], hash);
    double id = js_to_double(args[4]);

    return jerry_create_boolean(nk_tree_push_hashed(ctx, is_node ? NK_TREE_NODE : NK_TREE_TAB, (const char *) title, is_maximized ? NK_MAXIMIZED: NK_MINIMIZED, (const char *) hash, (int) strlen((const char *) hash), (int) id) != 0);
}

function(tree_image_push) {
    define_context(4, tree_image_push);
    
    bool is_node = js_to_bool(args[0]);

    struct nk_image *img;
    jerry_get_object_native_pointer(args[1], (void **) &img, NULL);

    js_define_string(args[2], title);
    bool is_maximized = js_to_bool(args[3]);
    
    return jerry_create_boolean(nk_tree_image_push(ctx, is_node ? NK_TREE_NODE : NK_TREE_TAB, *img, (const char *) title, is_maximized ? NK_MAXIMIZED: NK_MINIMIZED) != 0);
}

function(tree_image_push_id) {
    define_context(5, tree_image_push_id);

    bool is_node = js_to_bool(args[0]);

    struct nk_image *img;
    jerry_get_object_native_pointer(args[1], (void **) &img, NULL);

    js_define_string(args[2], title);
    bool is_maximized = js_to_bool(args[3]);
    double id = js_to_double(args[4]);

    return jerry_create_boolean(nk_tree_image_push_id(ctx, is_node ? NK_TREE_NODE : NK_TREE_TAB, *img, (const char *) title, is_maximized ? NK_MAXIMIZED: NK_MINIMIZED, (int) id) != 0);
}

function(tree_image_push_hashed) {
    define_context(6, tree_image_push_hashed);
    
    bool is_node = js_to_bool(args[0]);

    struct nk_image *img;
    jerry_get_object_native_pointer(args[1], (void **) &img, NULL);

    js_define_string(args[2], title);
    bool is_maximized = js_to_bool(args[3]);
    js_define_string(args[4], hash);
    double id = js_to_double(args[5]);

    return jerry_create_boolean(nk_tree_image_push_hashed(ctx, is_node ? NK_TREE_NODE : NK_TREE_TAB, *img, (const char *) title, is_maximized ? NK_MAXIMIZED: NK_MINIMIZED, (const char *) hash, (int) strlen((const char *) hash), (int) id) != 0);
}

function(tree_pop) {
    define_context(0, tree_pop);
    
    nk_tree_pop(ctx);
    
    return jerry_create_undefined();
}

function(tree_state_push) {
    define_context(3, tree_state_push);

    bool is_node = js_to_bool(args[0]);
    js_define_string(args[1], title);
    
    enum nk_collapse_states *collapse = NULL;
    jerry_value_t collapse_opt = js_prop(args[2], "collapse");
    if (!jerry_value_is_null(collapse_opt)) {
        bool bln = js_to_bool(collapse_opt);
        enum nk_collapse_states val = bln ? NK_MINIMIZED : NK_MAXIMIZED;
        collapse = &val;
    }
    jerry_release_value(collapse_opt);

    jerry_value_t bln = jerry_create_boolean(nk_tree_state_push(ctx, is_node ? NK_TREE_NODE : NK_TREE_TAB, (const char *) title, collapse) != 0);

    js_register_bool(args[2], "collapse", collapse != NULL ? *collapse == NK_MINIMIZED : false);

    return bln;
}

function(tree_state_image_push) {
    define_context(4, tree_state_image_push);

    bool is_node = js_to_bool(args[0]);

    struct nk_image *img;
    jerry_get_object_native_pointer(args[1], (void **) &img, NULL);

    js_define_string(args[2], title);

    enum nk_collapse_states *collapse = NULL;
    jerry_value_t collapse_opt = js_prop(args[3], "collapse");
    if (!jerry_value_is_null(collapse_opt)) {
        bool bln = js_to_bool(collapse_opt);
        enum nk_collapse_states val = bln ? NK_MINIMIZED : NK_MAXIMIZED;
        collapse = &val;
    }
    jerry_release_value(collapse_opt);

    jerry_value_t bln = jerry_create_boolean(nk_tree_state_image_push(ctx, is_node ? NK_TREE_NODE : NK_TREE_TAB, *img, (const char *) title, collapse) != 0);

    js_register_bool(args[2], "collapse", collapse != NULL ? *collapse == NK_MINIMIZED : false);

    return bln;
}

function(tree_state_pop) {
    define_context(0, tree_state_pop);

    nk_tree_state_pop(ctx);

    return jerry_create_undefined();
}

function(property_int) {
    define_context(6, property_int);
    
    js_define_string(args[0], name);
    double min = js_to_double(args[1]);

    int *value = NULL;
    jerry_value_t val_opt = js_prop(args[2], "value");
    if (!jerry_value_is_null(val_opt)) {
        double val = js_to_double(val_opt);
        value = (int *) &val;
    }
    jerry_release_value(val_opt);

    double max = js_to_double(args[3]);
    double step = js_to_double(args[4]);
    double inc_per_pixel = js_to_double(args[5]);
    
    nk_property_int(ctx, (const char *) name, (int) min, value, (int) max, (int) step, (float) inc_per_pixel);

    js_register_double(args[2], "value", value != NULL ? *value : min);

    return jerry_create_undefined();
}

function(property_double) {
    define_context(6, property_double);

    js_define_string(args[0], name);
    double min = js_to_double(args[1]);

    double *value = NULL;
    jerry_value_t val_opt = js_prop(args[2], "value");
    if (!jerry_value_is_null(val_opt)) {
        double val = js_to_double(val_opt);
        value = &val;
    }
    jerry_release_value(val_opt);

    double max = js_to_double(args[3]);
    double step = js_to_double(args[4]);
    double inc_per_pixel = js_to_double(args[5]);

    nk_property_double(ctx, (const char *) name, min, value, max, step, (float) inc_per_pixel);

    js_register_double(args[2], "value", value != NULL ? *value : min);

    return jerry_create_undefined();
}

function(propertyi) {
    define_context(6, propertyi);

    js_define_string(args[0], name);
    double val = js_to_double(args[1]);
    double min = js_to_double(args[2]);
    double max = js_to_double(args[3]);
    double step = js_to_double(args[4]);
    double inc_per_pixel = js_to_double(args[5]);

    return jerry_create_number(nk_propertyi(ctx, (const char *) name, (int) min, (int) val, (int) max, (int) step, (float) inc_per_pixel));
}


function(propertyd) {
    define_context(6, propertyd);

    js_define_string(args[0], name);
    double val = js_to_double(args[1]);
    double min = js_to_double(args[2]);
    double max = js_to_double(args[3]);
    double step = js_to_double(args[4]);
    double inc_per_pixel = js_to_double(args[5]);

    return jerry_create_number(nk_propertyd(ctx, (const char *) name, min, val, max, step, (float) inc_per_pixel));
}

static void functions(jerry_value_t nuklear, struct nk_context *ctx) {
    js_register_function(nuklear, "begin", begin, ctx);
    js_register_function(nuklear, "begin_titled", begin_titled, ctx);
    js_register_function(nuklear, "end", end, ctx);
    js_register_function(nuklear, "window_get_bounds", window_get_bounds, ctx);
    js_register_function(nuklear, "window_get_position", window_get_position, ctx);
    js_register_function(nuklear, "window_get_size", window_get_size, ctx);
    js_register_function(nuklear, "window_get_width", window_get_width, ctx);
    js_register_function(nuklear, "window_get_height", window_get_height, ctx);
    js_register_function(nuklear, "window_get_content_region", window_get_content_region, ctx);
    js_register_function(nuklear, "window_get_content_region_min", window_get_content_region_min, ctx);
    js_register_function(nuklear, "window_get_content_region_max", window_get_content_region_max, ctx);
    js_register_function(nuklear, "window_get_content_region_size", window_get_content_region_size, ctx);
    js_register_function(nuklear, "window_has_focus", window_has_focus, ctx);
    js_register_function(nuklear, "window_is_collapsed", window_is_collapsed, ctx);
    js_register_function(nuklear, "window_is_closed", window_is_closed, ctx);
    js_register_function(nuklear, "window_is_hidden", window_is_hidden, ctx);
    js_register_function(nuklear, "window_is_active", window_is_active, ctx);
    js_register_function(nuklear, "window_is_hovered", window_is_hovered, ctx);
    js_register_function(nuklear, "window_is_any_hovered", window_is_any_hovered, ctx);
    js_register_function(nuklear, "item_is_any_active", item_is_any_active, ctx);
    js_register_function(nuklear, "window_set_bounds", window_set_bounds, ctx);
    js_register_function(nuklear, "window_set_position", window_set_position, ctx);
    js_register_function(nuklear, "window_set_size", window_set_size, ctx);
    js_register_function(nuklear, "window_set_focus", window_set_focus, ctx);
    js_register_function(nuklear, "window_close", window_close, ctx);
    js_register_function(nuklear, "window_collapse", window_collapse, ctx);
    js_register_function(nuklear, "window_collapse_if", window_collapse_if, ctx);
    js_register_function(nuklear, "window_show", window_show, ctx);
    js_register_function(nuklear, "window_show_if", window_show_if, ctx);
    js_register_function(nuklear, "layout_set_min_row_height", layout_set_min_row_height, ctx);
    js_register_function(nuklear, "layout_reset_min_row_height", layout_reset_min_row_height, ctx);
    js_register_function(nuklear, "layout_widget_bounds", layout_widget_bounds, ctx);
    js_register_function(nuklear, "layout_ratio_from_pixel", layout_ratio_from_pixel, ctx);
    js_register_function(nuklear, "layout_row_dynamic", layout_row_dynamic, ctx);
    js_register_function(nuklear, "layout_row_static", layout_row_static, ctx);
    js_register_function(nuklear, "layout_row_begin", layout_row_begin, ctx);
    js_register_function(nuklear, "layout_row_push", layout_row_push, ctx);
    js_register_function(nuklear, "layout_row_end", layout_row_end, ctx);
    js_register_function(nuklear, "layout_row", layout_row, ctx);
    js_register_function(nuklear, "layout_row_template_begin", layout_row_template_begin, ctx);
    js_register_function(nuklear, "layout_row_template_push_dynamic", layout_row_template_push_dynamic, ctx);
    js_register_function(nuklear, "layout_row_template_push_variable", layout_row_template_push_variable, ctx);
    js_register_function(nuklear, "layout_row_template_push_static", layout_row_template_push_static, ctx);
    js_register_function(nuklear, "layout_row_template_end", layout_row_template_end, ctx);
    js_register_function(nuklear, "layout_space_begin", layout_space_begin, ctx);
    js_register_function(nuklear, "layout_space_push", layout_space_push, ctx);
    js_register_function(nuklear, "layout_space_end", layout_space_end, ctx);
    js_register_function(nuklear, "layout_space_bounds", layout_space_bounds, ctx);
    js_register_function(nuklear, "layout_space_to_screen", layout_space_to_screen, ctx);
    js_register_function(nuklear, "layout_space_to_local", layout_space_to_local, ctx);
    js_register_function(nuklear, "layout_space_rect_to_screen", layout_space_rect_to_screen, ctx);
    js_register_function(nuklear, "layout_space_rect_to_local", layout_space_rect_to_local, ctx);
    js_register_function(nuklear, "group_begin", group_begin, ctx);
    js_register_function(nuklear, "group_begin_titled", group_begin_titled, ctx);
    js_register_function(nuklear, "group_end", group_end, ctx);
    js_register_function(nuklear, "group_scrolled_offset_begin", group_scrolled_offset_begin, ctx);
    js_register_function(nuklear, "group_scrolled_begin", group_scrolled_begin, ctx);
    js_register_function(nuklear, "group_scrolled_end", group_scrolled_end, ctx);
    js_register_function(nuklear, "tree_push", tree_push, ctx);
    js_register_function(nuklear, "tree_push_id", tree_push_id, ctx);
    js_register_function(nuklear, "tree_push_hashed", tree_push_hashed, ctx);
    js_register_function(nuklear, "tree_image_push", tree_image_push, ctx);
    js_register_function(nuklear, "tree_image_push_id", tree_image_push_id, ctx);
    js_register_function(nuklear, "tree_image_push_hashed", tree_image_push_hashed, ctx);
    js_register_function(nuklear, "tree_pop", tree_pop, ctx);
    js_register_function(nuklear, "tree_state_push", tree_state_push, ctx);
    js_register_function(nuklear, "tree_state_image_push", tree_state_image_push, ctx);
    js_register_function(nuklear, "tree_state_pop", tree_state_pop, ctx);
    js_register_function(nuklear, "property_int", property_int, ctx);
    js_register_function(nuklear, "property_double", property_double, ctx);
    js_register_function(nuklear, "propertyi", propertyi, ctx);
    js_register_function(nuklear, "propertyd", propertyd, ctx);
}
*/


hinarin_function(hinarin_nuklear_item_is_any_active) {
    struct nk_context *ctx;
    jerry_get_object_native_pointer(function_obj, (void **) &ctx, NULL);

    return jerry_create_boolean(nk_item_is_any_active(ctx) != 0);
}

hinarin_function(hinarin_nuklear_begin) {
    if (args_count < 3) {
        return hinarin_create_error("begin must have 3 args: (title, rect, flags)");
    }

    struct nk_context *ctx;
    jerry_get_object_native_pointer(function_obj, (void **) &ctx, NULL);

    hinarin_string(args[0], title);
    struct nk_rect rect = to_rect(args[1]);
    double flags = jerry_get_number_value(args[2]);

    return jerry_create_boolean(nk_begin(ctx, title, rect, (nk_flags) flags) != 0);
}

hinarin_function(hinarin_nuklear_begin_titled) {
    if (args_count < 4) {
        return hinarin_create_error("begin must have 4 args: (name, title, rect, flags)");
    }

    struct nk_context *ctx;
    jerry_get_object_native_pointer(function_obj, (void **) &ctx, NULL);

    hinarin_string(args[0], name);
    hinarin_string(args[1], title);
    struct nk_rect rect = to_rect(args[2]);
    double flags = jerry_get_number_value(args[3]);

    return jerry_create_boolean(nk_begin_titled(ctx, name, title, rect, (nk_flags) flags) != 0);
}

hinarin_function(hinarin_nuklear_end) {
    struct nk_context *ctx;
    jerry_get_object_native_pointer(function_obj, (void **) &ctx, NULL);

    nk_end(ctx);

    return jerry_create_undefined();
}

hinarin_function(hinarin_nuklear_window_get_bounds) {
    struct nk_context *ctx;
    jerry_get_object_native_pointer(function_obj, (void **) &ctx, NULL);

    return from_rect(nk_window_get_bounds(ctx));
}

hinarin_function(hinarin_nuklear_window_get_position) {
    struct nk_context *ctx;
    jerry_get_object_native_pointer(function_obj, (void **) &ctx, NULL);

    return from_vec(nk_window_get_position(ctx));
}

hinarin_function(hinarin_nuklear_window_get_size) {
    struct nk_context *ctx;
    jerry_get_object_native_pointer(function_obj, (void **) &ctx, NULL);

    return from_vec(nk_window_get_size(ctx));
}

hinarin_function(hinarin_nuklear_window_get_width) {
    struct nk_context *ctx;
    jerry_get_object_native_pointer(function_obj, (void **) &ctx, NULL);

    return jerry_create_number(nk_window_get_width(ctx));
}

hinarin_function(hinarin_nuklear_window_get_height) {
    struct nk_context *ctx;
    jerry_get_object_native_pointer(function_obj, (void **) &ctx, NULL);

    return jerry_create_number(nk_window_get_height(ctx));
}

hinarin_function(hinarin_nuklear_window_get_content_region) {
    struct nk_context *ctx;
    jerry_get_object_native_pointer(function_obj, (void **) &ctx, NULL);

    return from_rect(nk_window_get_content_region(ctx));
}

hinarin_function(hinarin_nuklear_window_get_content_region_min) {
    struct nk_context *ctx;
    jerry_get_object_native_pointer(function_obj, (void **) &ctx, NULL);

    return from_vec(nk_window_get_content_region_min(ctx));
}

hinarin_function(hinarin_nuklear_window_get_content_region_max) {
    struct nk_context *ctx;
    jerry_get_object_native_pointer(function_obj, (void **) &ctx, NULL);

    return from_vec(nk_window_get_content_region_max(ctx));
}

hinarin_function(hinarin_nuklear_window_get_content_region_size) {
    struct nk_context *ctx;
    jerry_get_object_native_pointer(function_obj, (void **) &ctx, NULL);

    return from_vec(nk_window_get_content_region_size(ctx));
}

hinarin_function(hinarin_nuklear_window_has_focus) {
    struct nk_context *ctx;
    jerry_get_object_native_pointer(function_obj, (void **) &ctx, NULL);

    return jerry_create_boolean(nk_window_has_focus(ctx) != 0);
}

hinarin_function(hinarin_nuklear_window_is_hovered) {
    struct nk_context *ctx;
    jerry_get_object_native_pointer(function_obj, (void **) &ctx, NULL);

    return jerry_create_boolean(nk_window_is_hovered(ctx) != 0);
}

hinarin_function(hinarin_nuklear_window_is_collapsed) {
    if (args_count < 1) {
        return hinarin_create_error("window_is_collapsed must have 1 arg: (name)");
    }

    struct nk_context *ctx;
    jerry_get_object_native_pointer(function_obj, (void **) &ctx, NULL);

    hinarin_string(args[0], name);

    return jerry_create_boolean(nk_window_is_collapsed(ctx, name) != 0);
}

hinarin_function(hinarin_nuklear_window_is_closed) {
    if (args_count < 1) {
        return hinarin_create_error("window_is_closed must have 1 arg: (name)");
    }

    struct nk_context *ctx;
    jerry_get_object_native_pointer(function_obj, (void **) &ctx, NULL);

    hinarin_string(args[0], name);

    return jerry_create_boolean(nk_window_is_closed(ctx, name) != 0);
}

hinarin_function(hinarin_nuklear_window_is_hidden) {
    if (args_count < 1) {
        return hinarin_create_error("window_is_hidden must have 1 arg: (name)");
    }

    struct nk_context *ctx;
    jerry_get_object_native_pointer(function_obj, (void **) &ctx, NULL);

    hinarin_string(args[0], name);

    return jerry_create_boolean(nk_window_is_hidden(ctx, name) != 0);
}

hinarin_function(hinarin_nuklear_window_is_active) {
    if (args_count < 1) {
        return hinarin_create_error("window_is_active must have 1 arg: (name)");
    }

    struct nk_context *ctx;
    jerry_get_object_native_pointer(function_obj, (void **) &ctx, NULL);

    hinarin_string(args[0], name);

    return jerry_create_boolean(nk_window_is_active(ctx, name) != 0);
}

hinarin_function(hinarin_nuklear_window_is_any_hovered) {
    struct nk_context *ctx;
    jerry_get_object_native_pointer(function_obj, (void **) &ctx, NULL);

    return jerry_create_boolean(nk_window_is_any_hovered(ctx) != 0);
}

hinarin_function(hinarin_nuklear_window_set_bounds) {
    if (args_count < 2) {
        return hinarin_create_error("window_set_bounds must have 2 args: (name, rect)");
    }
    
    struct nk_context *ctx;
    jerry_get_object_native_pointer(function_obj, (void **) &ctx, NULL);
    
    hinarin_string(args[0], name);
    struct nk_rect rect = to_rect(args[1]);
    
    nk_window_set_bounds(ctx, name, rect);

    return jerry_create_undefined();
}

hinarin_function(hinarin_nuklear_window_set_position) {
    if (args_count < 2) {
        return hinarin_create_error("window_set_position must have 2 args: (name, vec)");
    }

    struct nk_context *ctx;
    jerry_get_object_native_pointer(function_obj, (void **) &ctx, NULL);

    hinarin_string(args[0], name);
    struct nk_vec2 vec = to_vec(args[1]);

    nk_window_set_position(ctx, name, vec);

    return jerry_create_undefined();
}

hinarin_function(hinarin_nuklear_window_set_size) {
    if (args_count < 2) {
        return hinarin_create_error("window_set_size must have 2 args: (name, vec)");
    }

    struct nk_context *ctx;
    jerry_get_object_native_pointer(function_obj, (void **) &ctx, NULL);

    hinarin_string(args[0], name);
    struct nk_vec2 vec = to_vec(args[1]);

    nk_window_set_size(ctx, name, vec);

    return jerry_create_undefined();
}

hinarin_function(hinarin_nuklear_window_set_focus) {
    if (args_count < 1) {
        return hinarin_create_error("window_set_focus must have 1 arg: (name)");
    }

    struct nk_context *ctx;
    jerry_get_object_native_pointer(function_obj, (void **) &ctx, NULL);

    hinarin_string(args[0], name);

    nk_window_set_focus(ctx, name);

    return jerry_create_undefined();
}

hinarin_function(hinarin_nuklear_window_close) {
    if (args_count < 1) {
        return hinarin_create_error("window_close must have 1 arg: (name)");
    }

    struct nk_context *ctx;
    jerry_get_object_native_pointer(function_obj, (void **) &ctx, NULL);

    hinarin_string(args[0], name);

    nk_window_close(ctx, name);

    return jerry_create_undefined();
}

hinarin_function(hinarin_nuklear_window_collapse) {
    if (args_count < 2) {
        return hinarin_create_error("window_collapse must have 2 args: (name, state)");
    }

    struct nk_context *ctx;
    jerry_get_object_native_pointer(function_obj, (void **) &ctx, NULL);

    hinarin_string(args[0], name);
    bool boolean = jerry_get_boolean_value(args[1]);

    nk_window_collapse(ctx, name, boolean ? NK_MINIMIZED : NK_MAXIMIZED);

    return jerry_create_undefined();
}

hinarin_function(hinarin_nuklear_window_collapse_if) {
    if (args_count < 3) {
        return hinarin_create_error("window_collapse_if must have 3 args: (name, state, commit)");
    }

    struct nk_context *ctx;
    jerry_get_object_native_pointer(function_obj, (void **) &ctx, NULL);

    hinarin_string(args[0], name);
    bool boolean = jerry_get_boolean_value(args[1]);
    bool commit = jerry_get_boolean_value(args[2]);

    nk_window_collapse_if(ctx, name, boolean ? NK_MINIMIZED : NK_MAXIMIZED, commit);

    return jerry_create_undefined();
}

hinarin_function(hinarin_nuklear_window_show) {
    if (args_count < 2) {
        return hinarin_create_error("window_show must have 2 args: (name, state)");
    }

    struct nk_context *ctx;
    jerry_get_object_native_pointer(function_obj, (void **) &ctx, NULL);

    hinarin_string(args[0], name);
    bool boolean = jerry_get_boolean_value(args[1]);

    nk_window_show(ctx, name, boolean ? NK_SHOWN : NK_HIDDEN);

    return jerry_create_undefined();
}

hinarin_function(hinarin_nuklear_window_show_if) {
    if (args_count < 3) {
        return hinarin_create_error("window_show_if must have 3 args: (name, state, commit)");
    }

    struct nk_context *ctx;
    jerry_get_object_native_pointer(function_obj, (void **) &ctx, NULL);

    hinarin_string(args[0], name);
    bool boolean = jerry_get_boolean_value(args[1]);
    bool commit = jerry_get_boolean_value(args[2]);

    nk_window_show_if(ctx, name, boolean ? NK_SHOWN : NK_HIDDEN, commit);

    return jerry_create_undefined();
}

jerry_value_t hinarin_nuklear_binding(GLFWwindow *window) {
    struct nk_context *ctx = nk_glfw3_init(window, NK_GLFW3_INSTALL_CALLBACKS);

    struct nk_font_atlas *atlas;
    nk_glfw3_font_stash_begin(&atlas);
    struct nk_font *font = nk_font_atlas_add_default(atlas, 16, 0);
    nk_glfw3_font_stash_end();
    nk_style_set_font(ctx, &font->handle);

    jerry_value_t nuklear_window = jerry_create_object();
    jerry_set_object_native_pointer(nuklear_window, ctx, NULL);

    hinarin_set_name_function(nuklear_window, "item_is_any_active", hinarin_nuklear_item_is_any_active, ctx);

    hinarin_set_name_function(nuklear_window, "begin", hinarin_nuklear_begin, ctx);
    hinarin_set_name_function(nuklear_window, "begin_titled", hinarin_nuklear_begin_titled, ctx);
    hinarin_set_name_function(nuklear_window, "end", hinarin_nuklear_end, ctx);

    hinarin_set_name_function(nuklear_window, "window_get_bounds", hinarin_nuklear_window_get_bounds, ctx);
    hinarin_set_name_function(nuklear_window, "window_get_position", hinarin_nuklear_window_get_position, ctx);
    hinarin_set_name_function(nuklear_window, "window_get_size", hinarin_nuklear_window_get_size, ctx);
    hinarin_set_name_function(nuklear_window, "window_get_width", hinarin_nuklear_window_get_width, ctx);
    hinarin_set_name_function(nuklear_window, "window_get_height", hinarin_nuklear_window_get_height, ctx);
    hinarin_set_name_function(nuklear_window, "window_get_content_region", hinarin_nuklear_window_get_content_region, ctx);
    hinarin_set_name_function(nuklear_window, "window_get_content_region_min", hinarin_nuklear_window_get_content_region_min, ctx);
    hinarin_set_name_function(nuklear_window, "window_get_content_region_max", hinarin_nuklear_window_get_content_region_max, ctx);
    hinarin_set_name_function(nuklear_window, "window_get_content_region_size", hinarin_nuklear_window_get_content_region_size, ctx);
    hinarin_set_name_function(nuklear_window, "window_has_focus", hinarin_nuklear_window_has_focus, ctx);
    hinarin_set_name_function(nuklear_window, "window_is_hovered", hinarin_nuklear_window_is_hovered, ctx);
    hinarin_set_name_function(nuklear_window, "window_is_collapsed", hinarin_nuklear_window_is_collapsed, ctx);
    hinarin_set_name_function(nuklear_window, "window_is_closed", hinarin_nuklear_window_is_closed, ctx);
    hinarin_set_name_function(nuklear_window, "window_is_hidden", hinarin_nuklear_window_is_hidden, ctx);
    hinarin_set_name_function(nuklear_window, "window_is_active", hinarin_nuklear_window_is_active, ctx);
    hinarin_set_name_function(nuklear_window, "window_is_any_hovered", hinarin_nuklear_window_is_any_hovered, ctx);
    hinarin_set_name_function(nuklear_window, "window_set_bounds", hinarin_nuklear_window_set_bounds, ctx);
    hinarin_set_name_function(nuklear_window, "window_set_position", hinarin_nuklear_window_set_position, ctx);
    hinarin_set_name_function(nuklear_window, "window_set_size", hinarin_nuklear_window_set_size, ctx);
    hinarin_set_name_function(nuklear_window, "window_set_focus", hinarin_nuklear_window_set_focus, ctx);
    hinarin_set_name_function(nuklear_window, "window_close", hinarin_nuklear_window_close, ctx);
    hinarin_set_name_function(nuklear_window, "window_collapse", hinarin_nuklear_window_collapse, ctx);
    hinarin_set_name_function(nuklear_window, "window_collapse_if", hinarin_nuklear_window_collapse_if, ctx);
    hinarin_set_name_function(nuklear_window, "window_show", hinarin_nuklear_window_show, ctx);
    hinarin_set_name_function(nuklear_window, "window_show_if", hinarin_nuklear_window_show_if, ctx);


    return nuklear_window;
}