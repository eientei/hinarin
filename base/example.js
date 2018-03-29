import {Window} from 'eientei/hinarin/master/base/core';

nuklear.render(new Window({
    title: "root",
    bounds: {x: '25%', y: '25%', w: '25%', h: '25%'},
    flags: Window.BORDER | Window.TITLE | Window.MOVABLE | Window.SCALABLE
}));