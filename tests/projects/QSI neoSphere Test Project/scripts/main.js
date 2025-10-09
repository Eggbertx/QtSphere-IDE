import { Task } from "sphere-runtime";

const windowTileBG = LoadWindowStyle("@/windowstyles/window_tile-bg.rws");
const windowStretchBG = LoadWindowStyle("@/windowstyles/window_stretch-bg.rws");
const windowGradientBG = LoadWindowStyle("@/windowstyles/window_gradient-bg.rws");
const windowGradientTiledBG = LoadWindowStyle("@/windowstyles/window_gradient-tiled-bg.rws");
const windowGradientStretchBG = LoadWindowStyle("@/windowstyles/window_gradient-stretch-bg.rws");
// const font = LoadFont("@/fonts/dailybuild.rfn");
const font = new Font("@/fonts/dailybuild.rfn");

export default class Game extends Task {
	constructor() {
		super();
	}

	async on_startUp() {
		SetRenderScript(() => this.on_render());
		SetUpdateScript(() => this.on_update());
		MapEngine("layerdrawing.rmp", 60);
	}

	on_update() {
		if(Keyboard.Default.isPressed(Key.Escape)) {
			Sphere.shutDown();
		}
	}

	on_render() {
		var x = 32;
		var y = 32;
		const w = 120;
		const h = 64;
		windowTileBG.drawWindow(x, y, w, h);
		font.drawText(Surface.Screen, x, y, "#dailybuild");
		x += w + 64;
		windowStretchBG.drawWindow(x, y, w*1.4, h);
		x += w*1.4 + 64;
		windowGradientBG.drawWindow(x, y, w, h);
		y += h + 64;
		x = 32;
		windowGradientTiledBG.drawWindow(x, y, w, h);
		x += w + 64;
		windowGradientStretchBG.drawWindow(x, y, w*1.4, h);
	}
}
