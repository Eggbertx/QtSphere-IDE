#!/usr/bin/env python3

from argparse import ArgumentParser
from signal import signal, SIGINT, SIG_DFL
import sys

from PySide6.QtCore import QCoreApplication
from PySide6.QtWidgets import QApplication

from mainwindow import MainWindow
from settings import Settings

__APPLICATION_NAME = "QtSphere IDE"
__ORG_NAME = "Spherical"
__VERSION = "0.11"

if __name__ == "__main__":
	parser = ArgumentParser()
	parser.add_argument("--verbose", "-v",
		action="store_true",
		default=False,
		help="If set, QtSphere IDE will print a warning when it finds a directory with no parseable game file (game.sgm, Cellscript.js, etc)")

	args, otherArgs = parser.parse_known_args()

	QCoreApplication.setApplicationName(__APPLICATION_NAME)
	QCoreApplication.setOrganizationName(__ORG_NAME)
	QCoreApplication.setApplicationVersion(__VERSION)
	app = QApplication(sys.argv)

	window = MainWindow(__VERSION, openPath=None if len(otherArgs) == 0 else otherArgs[0], verbose=args.verbose)
	settings = Settings()
	app.setStyle(settings.theme)
	if settings.maximized:
		window.showMaximized()
	else:
		window.show()
	signal(SIGINT, SIG_DFL)
	sys.exit(app.exec())
