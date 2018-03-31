import {DynamicLayout, Window, Label} from './lib';

const rootWindow = new Window({
    title: "root",
    rect: {x: '0%', y: '0%', w: '100%', h: '100%'},
    layout: new DynamicLayout({height: '30%'})
});

const test = new Label({text: "Бака!"});

rootWindow.setChildren([[test, test, test],[test, test, test, test],[test, test, test]]);

nuklear.render(rootWindow);