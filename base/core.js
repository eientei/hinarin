function rectify_from(total, value) {
    if (typeof value === 'string') {
        if (value.match(/%$/)) {
            var percent = parseFloat(value);
            return total / 100 * percent;
        }
    }
    return value;
}

function rectify(ctx, rect) {
    return {
        x: rectify_from(ctx.width, rect.x),
        y: rectify_from(ctx.height, rect.y),
        w: rectify_from(ctx.width, rect.w),
        h: rectify_from(ctx.height, rect.h)
    };
}

function vectify(ctx, vec) {
    return {
        x: rectify_from(ctx.width, vec.x),
        y: rectify_from(ctx.height, vec.y)
    };
}

function Window(title, rect, flags, body_func, name) {
    this.title = title;
    this.rect = rect;
    this.flags = flags;
    this.body_func = body_func;
    this.name = name;
}

Window.prototype.render = function (ctx) {
    this.rect = rectify(ctx, this.rect);
    var result = false;
    if (this.name) {
        result = ctx.begin_titled(this.name, this.title, this.rect, this.flags);
    } else {
        result = ctx.begin(this.title, this.rect, this.flags);
    }
    if (result) {
        if (this.body_func) {
            this.ctx = ctx;
            this.body_func.call(this);
            this.ctx = null;
        }
    }
    ctx.end();
};

Window.prototype.getBounds = function () {
    return this.ctx.window_get_bounds();
};

Window.prototype.getPosition = function () {
    return this.ctx.window_get_position();
};

Window.prototype.getSize = function () {
    return this.ctx.window_get_size();
};

Window.prototype.getWidth = function () {
    return this.ctx.window_get_width();
};

Window.prototype.getHeight = function () {
    return this.ctx.window_get_height();
};

Window.prototype.getContentRegion = function () {
    return this.ctx.window_get_content_region();
};

Window.prototype.getContentRegionMin = function () {
    return this.ctx.window_get_content_region_min();
};

Window.prototype.getContentRegionMax = function () {
    return this.ctx.window_get_content_region_max();
};

Window.prototype.getContentRegionSize = function () {
    return this.ctx.window_get_content_region_size();
};

Window.prototype.hasFocus = function () {
    return this.ctx.window_has_focus();
};

Window.prototype.isHovered = function () {
    return this.ctx.window_is_hovered();
};

Window.isCollapsed = function (ctx, name) {
    return ctx.window_is_collapsed(name);
};

Window.isClosed = function (ctx, name) {
    return ctx.window_is_closed(name);
};

Window.isHidden = function (ctx, name) {
    return ctx.window_is_hidden(name);
};

Window.isActive = function (ctx, name) {
    return ctx.window_is_active(name);
};

Window.isAnyHovered = function (ctx) {
    return ctx.window_is_any_hovered();
};

Window.setBounds = function (ctx, name, rect) {
    return ctx.window_set_bounds(name, rectify(ctx, rect));
};

Window.setPosition = function (ctx, name, vec) {
    return ctx.window_set_position(name, vectify(ctx, vec));
};

Window.setSize = function (ctx, name, vec) {
    return ctx.window_set_size(name, vectify(ctx, vec));
};

Window.setFocus = function (ctx, name) {
    return ctx.window_set_focus(name);
};

Window.close = function (ctx, name) {
    return ctx.window_close(name);
};

Window.collapse = function (ctx, name, state) {
    return ctx.window_collapse(name, state);
};

Window.collapseIf = function (ctx, name, state, commit) {
    return ctx.window_collapse_if(name, state, commit);
};

Window.show = function (ctx, name, state) {
    return ctx.window_show(name, state);
};

Window.showIf = function (ctx, name, state, commit) {
    return ctx.window_show_if(name, state, commit);
};

Window.NONE             = 0;
Window.BORDER           = 1<<0;
Window.MOVABLE          = 1<<1;
Window.SCALABLE         = 1<<2;
Window.CLOSABLE         = 1<<3;
Window.MINIMIZABLE      = 1<<4;
Window.NO_SCROLLBAR     = 1<<5;
Window.TITLE            = 1<<6;
Window.SCROLL_AUTO_HIDE = 1<<7;
Window.BACKGROUND       = 1<<8;
Window.SCALE_LEFT       = 1<<9;
Window.NO_INPUT         = 1<<10;

var widgets = [];

modules.define({
    name: 'nuklear',
    exports: {
        add: function (widget) {
            if (!(typeof widget === 'object') || typeof widget.render !== 'function') {
                throw new Error('add() receives an object with render(ctx) method on it');
            }
            widgets.push(widget);
        },
        isAnyActive: function (ctx) {
            return ctx.item_is_any_active();
        },
        Window: Window
    },
    render: function (ctx) {
        widgets.forEach(function (widget) {
            widget.render(ctx);
        });
    }
});