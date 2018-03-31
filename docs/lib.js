export function bakePercent(percent, value) {
    return value / 100 * percent;
}

export function bakeProperty(prop, value) {
    if (typeof prop === 'string') {
        if (prop.endsWith('%')) {
            return bakePercent(parseFloat(prop), value);
        } else {
            return parseFloat(prop);
        }
    }
    return prop;
}

export class Vec {
    constructor({x, y}, parent) {
        this.state = {x, y, parent};
    }

    get x() {
        return bakeProperty(this.state.x, this.state.parent.getWidth());
    }

    get y() {
        return bakeProperty(this.state.y, this.state.parent.getHeight());
    }
}

export class Rect {
    constructor({x, y, w, h}, parent) {
        this.state = {x, y, w, h, parent};
    }

    get x() {
        return bakeProperty(this.state.x, this.state.parent.getWidth());
    }

    get y() {
        return bakeProperty(this.state.y, this.state.parent.getHeight());
    }

    get w() {
        return bakeProperty(this.state.w, this.state.parent.getWidth());
    }

    get h() {
        return bakeProperty(this.state.h, this.state.parent.getHeight());
    }
}

export class Containable {
    constructor() {
        this.state = {
            parent: null,
            children: []
        };
    }

    setParent(parent) {
        this.state.parent = parent;
    }

    getParent() {
        return this.state.parent || window;
    }

    setChildren(rows) {
        rows.forEach(row => row.forEach(element => element.setParent(this)));
        this.state.children = rows;
    }

    getChildren() {
        return this.state.children;
    }

    getBounds() {
        return this.state.bounds;
    }

    getPosition() {
        const {x,y} = this.state.bounds;
        return {x,y};
    }

    getSize() {
        const {w,h} = this.state.bounds;
        return {x: w, y: h};
    }

    getWidth() {
        return this.state.bounds.w;
    }

    getHeight() {
        return this.state.bounds.h;
    }
}

export class Window extends Containable {
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

    constructor({title = "Untitled", name, rect = {x: 0, y: 0, w: 100, h: 100}, flags = 0, layout = new DynamicLayout({height: 0})}) {
        super();
        this.state = {
            ...this.state,
            title,
            name,
            rect: new Rect(rect, this.getParent()),
            flags,
            layout,
            bounds: {x: 0, y: 0, w: 0, h: 0},
            contentRegion: {x: 0, y: 0, w: 0, h: 0},
            focus: false,
            hovered: false
        };
    }

    render() {
        const {title, name, rect, flags, layout} = this.state;
        const result = (name != null) ? nuklear.beginTitled(name, title, rect, flags) : nuklear.begin(title, rect, flags);
        try {
            if (result) {
                this.state.bounds = nuklear.windowGetBounds();
                this.state.contentRegion = nuklear.windowGetContentRegion();
                this.state.focus = nuklear.windowHasFocus();
                this.state.hovered = nuklear.windowIsHovered();
                this.getChildren().forEach(row => layout.renderRow(this.getParent(), row));
            }
        } finally {
            nuklear.end();
        }
    }

    getTitle() {
        return this.state.title;
    }
    setTitle(title) {
        this.state.title = title;
    }
    getName() {
        return this.state.name;
    }
    setName(name) {
        this.state.name = name;
    }
    getBounds() {
        return this.state.bounds;
    }
    setBounds(bounds) {
        this.state.bounds = new Rect(bounds, this.getParent());
    }
    getFlags() {
        return this.state.flags;
    }
    setFlags(flags) {
        this.state.flags = flags;
    }
    getLayout() {
        return this.state.layout;
    }
    setLayout(layout) {
        this.state.layout = layout;
    }
}

export class Mouse {
    static get LEFT() { return nuklear.BUTTON_LEFT; }
    static get MIDDLE() { return nuklear.BUTTON_MIDDLE; }
    static get RIGHT() { return nuklear.BUTTON_RIGHT; }
    static get DOUBLE() { return nuklear.BUTTON_DOUBLE; }
}

export class Widget extends Containable {
    static get INVALID() { return nuklear.WIDGET_INVALID; }
    static get VALID() { return nuklear.WIDGET_VALID; }
    static get ROM() { return nuklear.WIDGET_ROM; }

    constructor({rect, vec}) {
        super();
        this.state = {
            ...this.state,
            rect,
            vec,
            bounds: {x: 0, y: 0, w: 0, h: 0},
            hovered: false,
            clicked: {
                left: false,
                middle: false,
                right: false,
                double: false
            },
            down: {
                left: false,
                middle: false,
                right: false,
                double: false
            },
            status: Widget.INVALID
        };
    }

    render() {
        const {rect, vec, down} = this.state;
        this.state.status = (vec != null) ? nuklear.widgetFitting(rect, vec) : nuklear.widget(rect);
        if (this.state.status === Widget.VALID) {
            this.state.bounds = nuklear.widgetBounds();
            this.state.hovered = nuklear.widgetIsHovered();
            this.state.clicked.left = nuklear.widgetIsMouseClicked(Mouse.LEFT);
            this.state.clicked.middle = nuklear.widgetIsMouseClicked(Mouse.MIDDLE);
            this.state.clicked.right = nuklear.widgetIsMouseClicked(Mouse.RIGHT);
            this.state.clicked.double = nuklear.widgetIsMouseClicked(Mouse.DOUBLE);

            this.state.down.left = nuklear.widgetHasMouseClickDown(Mouse.LEFT, 1);
            this.state.down.middle = nuklear.widgetHasMouseClickDown(Mouse.MIDDLE, 1);
            this.state.down.right = nuklear.widgetHasMouseClickDown(Mouse.RIGHT, 1);
            this.state.down.double = nuklear.widgetHasMouseClickDown(Mouse.DOUBLE, 1);
        }
    }

    getStatus() {
        return this.state.status;
    }
}

export class Align {
    static get ALIGN_LEFT() { return nuklear.TEXT_ALIGN_LEFT; }
    static get ALIGN_CENTERED() { return nuklear.TEXT_ALIGN_CENTERED; }
    static get ALIGN_RIGHT() { return nuklear.TEXT_ALIGN_RIGHT; }
    static get ALIGN_TOP() { return nuklear.TEXT_ALIGN_TOP; }
    static get ALIGN_MIDDLE() { return nuklear.TEXT_ALIGN_MIDDLE; }
    static get ALIGN_BOTTOM() { return nuklear.TEXT_ALIGN_BOTTOM; }
    static get LEFT() { return nuklear.TEXT_LEFT; }
    static get CENTERED() { return nuklear.TEXT_CENTERED; }
    static get BOTTOM() { return nuklear.TEXT_BOTTOM; }
}

export class Text extends Containable {
    constructor({text, align, color, wrap}) {
        super();
        this.state = {...this.state, text, align, color, wrap};
    }

    render() {
        const {text, align = Align.CENTERED, color, wrap} = this.state;
        if (color != null && wrap === null) {
            nuklear.textWrapColored(text, color);
        } else if (color != null) {
            nuklear.textColored(text, align, color);
        } else if (wrap === true) {
            nuklear.textWrap(text);
        } else {
            nuklear.text(text, align);
        }
    }
}

export class Label extends Containable {
    constructor({text, align = Align.CENTERED, color, wrap}) {
        super();
        this.state = {...this.state, text, align, color, wrap};
    }

    render() {
        const {text, align, color, wrap} = this.state;
        if (color != null && wrap === null) {
            nuklear.labelColoredWrap(text, color);
        } else if (color != null) {
            nuklear.labelColored(text, align, color);
        } else if (wrap === true) {
            nuklear.labelWrap(text);
        } else {
            nuklear.label(text, align);
        }
    }
}

export class DynamicLayout {
    constructor({height}) {
        this.state = {height};
    }

    renderRow(parent, row) {
        const {height} = this.state;
        nuklear.layoutRowDynamic(bakeProperty(height, parent.getHeight()), row.length);
        row.forEach(element => element.render());
    }
}

export class StaticLayout {
    constructor({height, itemWidth}) {
        this.state = {height, itemWidth};
    }

    renderRow(parent, row) {
        const {height, itemWidth} = this.state;
        nuklear.layoutRowStatic(bakeProperty(height, parent.getHeight()), bakeProperty(itemWidth, parent.getWidth()), row.length);
        row.forEach(element => element.render());
    }
}