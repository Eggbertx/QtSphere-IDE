# QtSphere IDE
A cross-platform IDE for the [Sphere game engine](http://spheredev.org)

Porting this from C++ to Python is still a work in progress

![Screenshot](screenshots/screenshot-map.png)

## Dependencies
* Qt 6
* Python 3
* Qt Creator (optional but recommended for development)
* Qt 6 multimedia module (for the sound player)
	* The package name will vary from Linux distribution to Linux distribution. For example, it's "qt6-multimedia" in Arch Linux and "qtmultimedia6-dev" in Ubuntu.
	* It may or may not come standard for the Windows and macOS Qt package.

## Compilation
Building this in Qt Creator will make things more convenient, but if you want to run it from the command line, run `python -m venv env`.

Then, if you are in Windows, run `.\env\Scripts\activate`.

In macOS, Linux, or BSD, run `source env/bin/activate`.

Then, run the following:
```
pip install pyside6
pyside6-project build
python mainwindow.py
```
