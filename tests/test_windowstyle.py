from os import path
import pytest
from PySide6.QtGui import QColor, QImage, QTransform

from formats.windowstyle import SphereWindowStyle, BackgroundMode, WindowStyleBitmap

palette = (
	QColor(0, 0, 0, 0), # transparent
	QColor(0, 0, 0), # black
	QColor(255, 255, 255), # white
	QColor(192, 192, 192), # light gray
)

ul_pixels = (
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 2,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	0, 0, 0, 0, 0, 0, 0, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	0, 0, 0, 0, 0, 0, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	0, 0, 0, 0, 0, 0, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	0, 0, 0, 0, 0, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	0, 0, 0, 0, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	0, 0, 0, 0, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	0, 0, 0, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	0, 0, 0, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	0, 0, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	0, 0, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	0, 0, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	0, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	0, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	0, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	0, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2
)

images = [QImage(32, 32, QImage.Format.Format_RGBA8888) for _ in range(9)]
for y in range(32):
	for x in range(32):
		images[WindowStyleBitmap.UpperLeft].setPixelColor(x, y, palette[ul_pixels[y * 32 + x]])
		images[WindowStyleBitmap.UpperRight].setPixelColor(31 - x, y, palette[ul_pixels[y * 32 + x]])
		images[WindowStyleBitmap.LowerLeft].setPixelColor(x, 31 - y, palette[ul_pixels[y * 32 + x]])
		images[WindowStyleBitmap.LowerRight].setPixelColor(31 - x, 31 - y, palette[ul_pixels[y * 32 + x]])

images[WindowStyleBitmap.Top].fill(palette[2])
images[WindowStyleBitmap.Bottom].fill(palette[2])
images[WindowStyleBitmap.Left].fill(palette[2])
images[WindowStyleBitmap.Right].fill(palette[2])
images[WindowStyleBitmap.Background].fill(palette[2])
images[WindowStyleBitmap.Background] = images[WindowStyleBitmap.Background].scaled(16, 16)
for i in range(32):
	images[WindowStyleBitmap.Top].setPixelColor(i, 0, palette[1])
	images[WindowStyleBitmap.Top].setPixelColor(i, 31, palette[3])
	images[WindowStyleBitmap.Bottom].setPixelColor(i, 31, palette[1])
	images[WindowStyleBitmap.Bottom].setPixelColor(i, 0, palette[3])
	images[WindowStyleBitmap.Left].setPixelColor(0, 31-i, palette[1])
	images[WindowStyleBitmap.Left].setPixelColor(31, 31-i, palette[3])
	images[WindowStyleBitmap.Right].setPixelColor(31, i, palette[1])
	images[WindowStyleBitmap.Right].setPixelColor(0, i, palette[3])
	images[WindowStyleBitmap.Background].setPixelColor(i // 2, i //2, palette[3])
	images[WindowStyleBitmap.Background].setPixelColor(i // 2, 15 - i //2, palette[3])

gradient_corner_colors = (
	QColor(255, 0, 0),
	QColor(0, 0, 255),
	QColor(0, 255, 0),
	QColor(0, 0, 0, 0),
)

rws_cases = (
	{
		"name": "Gradient Background",
		"filename": "window_gradient-bg.rws",
		"expected": {
			"background_mode": BackgroundMode.Gradient,
			"edge_offsets": (16, 24, 16, 16),
		}
	},
	{
		"name": "Gradient Stretch Background",
		"filename": "window_gradient-stretch-bg.rws",
		"expected": {
			"background_mode": BackgroundMode.StretchedWithGradient,
			"edge_offsets": (16, 5, 36, 92),
		}
	},
	{
		"name": "Gradient Tiled Background",
		"filename": "window_gradient-tiled-bg.rws",
		"expected": {
			"background_mode": BackgroundMode.TiledWithGradient,
			"edge_offsets": (16, 5, 16, 16),
		}
	},
	{
		"name": "Stretch Background",
		"filename": "window_stretch-bg.rws",
		"expected": {
			"background_mode": BackgroundMode.Stretched,
			"edge_offsets": (0, 0, 0, 0),
		}
	},
	{
		"name": "Tiled Background",
		"filename": "window_tile-bg.rws",
		"expected": {
			"background_mode": BackgroundMode.Tiled,
			"edge_offsets": (0, 0, 0, 0),
		}
	}
)


@pytest.mark.parametrize("rws_case", rws_cases, ids=[case["name"] for case in rws_cases])
def test_parse_rws(rws_case):
	basedir = path.basename(path.abspath(path.curdir))
	if basedir == "tests":
		basedir = "."
	elif basedir == "QtSphere-IDE":
		basedir = "tests"
	else:
		raise RuntimeError("Unrecognized working directory for test execution, expected 'tests' or 'QtSphere-IDE'")
	
	rws_path = path.join(basedir, "projects/QSI 1.x Test Project/windowstyles/", rws_case["filename"])
	assert path.exists(rws_path), "Test RWS file does not exist"
	rws = SphereWindowStyle(rws_path)
	rws.open()
	assert rws.version == 2
	assert rws.edgeWidth == 0
	assert rws.backgroundMode == rws_case["expected"]["background_mode"]
	if rws.backgroundMode in (BackgroundMode.Gradient, BackgroundMode.TiledWithGradient, BackgroundMode.StretchedWithGradient):
		for i in range(4):
			assert rws.cornerColors[i] == gradient_corner_colors[i]

	for i in range(4):
		assert rws.edgeOffsets[i] == rws_case["expected"]["edge_offsets"][i]

	for i in range(len(rws.bitmaps)):
		img = rws.bitmaps[i]
		assert not img.isNull()
		expected_size = 16 if i == WindowStyleBitmap.Background else 32
		assert img.width() == expected_size
		assert img.height() == expected_size
		assert img == images[i], f"Bitmap {i} does not match expected image"