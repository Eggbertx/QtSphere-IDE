import os

from PySide6.QtCore import QCoreApplication, QSettings, QRect
import mainwindow

from settings import Settings

__test_ini = "settings.ini"

def __remove_ini_file():
	if os.path.exists(__test_ini):
		os.unlink(__test_ini)

def setup_module(module):
	QCoreApplication.setApplicationName("QtSphere IDE")
	QCoreApplication.setOrganizationName("Spherical")
	QCoreApplication.setApplicationVersion("0.10")

def setup_function(function):
	__remove_ini_file()

def teardown_function(function):
	__remove_ini_file()

def test_settings():
	s = Settings(__test_ini)
	qs = QSettings(__test_ini, QSettings.Format.IniFormat)

	assert s.maximized == True
	s.maximized = False
	assert s.maximized == False
	assert qs.value("maximized", None) == False

	assert len(s.projectDirs) == 0
	dirs = ["a","b","c"]
	s.projectDirs = dirs
	n = qs.beginReadArray("projectDirs")
	assert n == len(dirs)
	for i in range(n):
		qs.setArrayIndex(i)
		assert qs.value("directory") == dirs[i]
	qs.endArray()

	qs.setValue("geometry", QRect(0, 0, 800, 600))
	geom = s.geometry
	assert geom.x() == 0
	assert geom.y() == 0
	assert geom.width() == 800
	assert geom.height() == 600
