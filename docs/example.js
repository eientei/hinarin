import {Window} from 'eientei/hinarin/master/base/core';

nuklear.render(new Window({
    title: "root",
    bounds: {x: '25%', y: '25%', w: '50%', h: '50%'},
    flags: Window.BORDER | Window.TITLE | Window.MOVABLE | Window.SCALABLE
}));