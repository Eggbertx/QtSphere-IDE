import pytest

from PySide6.QtCore import QPoint

from widgets.sphereeditor import SphereEditor

test_data = (
	(QPoint(0, 0), QPoint(4,4), [QPoint(0,0), QPoint(1,1), QPoint(2,2), QPoint(3,3), QPoint(4,4)]),
	(QPoint(5, 3), QPoint(5,3), [QPoint(5,3)]),
	(QPoint(5, 3), QPoint(11,2), [QPoint(5,3),QPoint(6,3),QPoint(7,3),QPoint(8,3),QPoint(9,2),QPoint(10,2),QPoint(11,2)]),
	(QPoint(11, 2), QPoint(5,3), [QPoint(11,2),QPoint(10,2),QPoint(9,2),QPoint(8,2),QPoint(7,3),QPoint(6,3),QPoint(5,3)])
)

test_ids = ("0,0 to 0,4", "5,3 to 5,3 (single point)", "5,3 to 11,2", "11,2 to 5,3")

@pytest.mark.parametrize("p0,p1,expectedPixels", test_data, ids=test_ids)
def test_bresenhaum(p0:QPoint, p1:QPoint, expectedPixels:list[QPoint]):
	pixels = SphereEditor.pixelsInLine(p0, p1)
	assert pixels == expectedPixels
