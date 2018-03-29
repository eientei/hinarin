export function bakePercent(percent, value) {
    return value / 100 * percent;
}

export function bakeProperty(prop, value) {
    if (typeof prop === 'string' && prop.endsWith('%')) {
        return bakePercent(parseFloat(prop), value);
    }
    return value;
}

export function bakeRect({x, y, w, h}) {
    return {
        x: bakeProperty(x, window.width),
        y: bakeProperty(y, window.height),
        w: bakeProperty(w, window.width),
        h: bakeProperty(h, window.height)
    };
}
export class Window {
    static get BORDER() { return nuklear.WINDOW_BORDER; }
    static get MOVABLE() { return nuklear.WINDOW_MOVABLE; }
    static get SCALABLE() { return nuklear.WINDOW_SCALABLE; }
    static get CLOSABLE() { return nuklear.WINDOW_CLOSABLE; }
    static get MINIMIZABLE() { return nuklear.WINDOW_MINIMIZABLE; }
    static get NO_SCROLLBAR() { return nuklear.WINDOW_NO_SCROLLBAR; }
    static get TITLE() { return nuklear.WINDOW_TITLE; }
    static get SCROLL_AUTO_HIDE() { return nuklear.WINDOW_SCROLL_AUTO_HIDE; }
    static get BACKGROUND() { return nuklear.WINDOW_BACKGROUND; }
    static get SCALE_LEFT() { return nuklear.WINDOW_SCALE_LEFT; }
    static get NO_INPUT() { return nuklear.WINDOW_NO_INPUT; }

    constructor({title, name, bounds, flags, render}) {
        this.state = {
            title: title != null ? title : "Untitled",
            name: name != null ? name : undefined,
            bounds: bounds != null ? bounds : {x: 0, y: 0, w: 100, h: 100},
            flags: flags != null ? flags : 0,
            render: render != null ? render : undefined
        };
    }

    render() {
        const {title, name, bounds, flags, render} = this.state;
        const bakedBounds = bakeRect(bounds);
        const result = (name != null) ? nuklear.beginTitled(name, title, bakedBounds, flags) : nuklear.begin(title, bakedBounds, flags);
        if (result && render != null) {
            render.call(this);
        }
        nuklear.end();
    }
}