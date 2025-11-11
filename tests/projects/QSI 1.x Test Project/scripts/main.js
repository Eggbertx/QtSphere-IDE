const windowTileBG = LoadWindowStyle("window_tile-bg.rws");
const windowStretchBG = LoadWindowStyle("window_stretch-bg.rws");
const windowGradientBG = LoadWindowStyle("window_gradient-bg.rws");
const windowGradientTiledBG = LoadWindowStyle("window_gradient-tiled-bg.rws");
const windowGradientStretchBG = LoadWindowStyle("window_gradient-stretch-bg.rws");
const windowOddSizes = LoadWindowStyle("odd-sizes-test.rws");
const font = LoadFont("dailybuild.rfn");


function render() {
	var x = 32;
	var y = 32;
	const w = 120;
	const h = 64;
	windowTileBG.drawWindow(x, y, w, h);
	font.drawText(x, y, "#dailybuild");
	x += w + 64;
	windowStretchBG.drawWindow(x, y, w*1.4, h);
	x += w*1.4 + 64;
	windowGradientBG.drawWindow(x, y, w, h);
	y += h + 64;
	x = 32;
	windowGradientTiledBG.drawWindow(x, y, w, h);
	x += w + 64;
	windowGradientStretchBG.drawWindow(x, y, w*1.4, h);
	x += w*1.4 + 64+16;
	windowOddSizes.drawWindow(x, y, w, h);
}

function game() {
	SetRenderScript("render()");
	MapEngine("layerdrawing.rmp", 60);
}
