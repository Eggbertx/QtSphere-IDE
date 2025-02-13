from PySide6.QtGui import QImage, QPixmap

def images_equal(img1: QImage|QPixmap, img2: QImage|QPixmap) -> bool:
	if img1.width() != img2.width() or img1.height() != img2.height():
		return False

	img1i = img1 if isinstance(img1, QImage) else img1.toImage()
	img2i = img2 if isinstance(img2, QImage) else img2.toImage()

	for y in range(img1i.height()):
		for x in range(img1i.width()):
			c1 = img1i.pixelColor(x, y)
			c2 = img2i.pixelColor(x, y)
			if c1 != c2:
				return False
	return True