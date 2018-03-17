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

function Window(title, rect, flags, body_func, name) {
    this.private = {
        title: title,
        rect: rect,
        flags: flags,
        body_func: body_func,
        name: name
    };
}

Window.prototype.render = function (ctx) {
    var result = false;
    if (this.private.name) {
        result = ctx.begin_titled(this.private.name, this.private.title, rectify(ctx, this.private.rect), this.private.flags);
    } else {
        result = ctx.begin(this.private.title, rectify(ctx, this.private.rect), this.private.flags);
    }
    if (result) {
        if (this.private.body_func) {
            this.private.ctx = ctx;
            this.private.body_func.call(this.private, ctx);
            this.private.ctx = null;
        }
    }
    ctx.end();
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
        Window: Window
    },
    render: function (ctx) {
        widgets.forEach(function (widget) {
            widget.render(ctx);
        });
    }
});