import {Window} from 'eientei.github.io/hinarin/core';

nuklear.render(new Window({
    title: "root",
    bounds: {x: '25%', y: '25%', w: '50%', h: '50%'},
    flags: Window.BORDER | Window.TITLE | Window.MOVABLE | Window.SCALABLE
}));