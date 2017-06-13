# QtSphere IDE (working title)

A cross-platform IDE for the [Sphere game engine](http://spheredev.org)

## Installation
### Linux/BSD/macOS (maybe)
Building this in Qt Creator will make things a whole lot easier but if you're weird and insist on using a terminal, just do:
  1. Install qscintilla-qt5 from your favorite distribution's repo
  2. qmake
  2. make
  4. ./QtSphere

### Windows
  1. Just use Qt Creator or something. Or if you have Qt/qmake installed, the above should (mostly) work.

For convenience, I have included the QScintilla library in the Qt4Qt5 directory. You should only need this in Windows (see #1 for the Unix options)