#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QFileInfoList>
#include <QStringList>
#include <QTextStream>

#include "qsiproject.h"
#include "util.h"

QSIProject::QSIProject(QString path, QObject *parent) : QObject(parent) {
	if(path == "") return;
	this->path = path;
	QFileInfo fileInfo(this->path);
	if(!fileInfo.exists()) {
		errorBox("Project file " + this->path + " doesn't exist!");
		return;
	}
	bool found = false;
	if(fileInfo.isDir()) {
		// get list of files in project dir, prioritizing .ssproj over game.sgm
		QStringList filters;
		filters << "*.ssproj" << "game.sgm";
		QDir projectDir = QDir(this->path);

		QFileInfoList infoList = projectDir.entryInfoList(filters,QDir::Files|QDir::NoDotAndDotDot);
		foreach (QFileInfo fi, infoList) {
			if(fi.suffix() == "ssproj" || fi.fileName() == "game.sgm") {
				this->projectPath = fi.filePath();
				fileInfo = fi;
				found = true;
				break;
			}
		}
	} else {
		this->projectPath = fileInfo.filePath();
		this->path = fileInfo.dir().path();
		found = true;
	}
	if(!found) return;
	QFile* projectFile = new QFile(this->projectPath);
	if(!projectFile->open(QFile::ReadOnly|QFile::Text)) {
		errorBox("Error reading " + fileInfo.fileName() + ": " + projectFile->errorString());
		delete projectFile;
	}
	QString suffix = fileInfo.suffix();
	QTextStream stream(projectFile);

	QString line;
	QString key;
	QString value;

	if(suffix == "ssproj") {
		while(!stream.atEnd()) {
			line = stream.readLine();
			if(line == "[.ssproj]" || line == "") continue;
			key = line.section("=", 0, 0);
			value = line.section("=", 1);

			if(key == "author") this->author = value;
			else if(key == "buildDir") this->buildDir = this->path + "/" + value;
			else if(key == "compiler") this->compler = value;
			else if(key == "description") this->summary = value;
			else if(key == "mainScript") this->script = value;
			else if(key == "name") this->name = value;
			else if(key == "screenHeight") this->height = value.toInt();
			else if(key == "screenWidth") this->width = value.toInt();
		}
	} else if(suffix == "sgm") {
		while(stream.atEnd()) {
			line = stream.readLine();
			if(line == "") continue;
			key = line.section("=", 0, 0);
			value = line.section("=", 1);
			if(key == "name") this->name = value;
			else if(key == "author") this->author = value;
			else if(key == "description") this->summary = value;
			else if(key == "screen_width") this->width = value.toInt();
			else if(key == "screen+height") this->height = value.toInt();
			else if(key == "script") this->script = value;
		}
	}
	projectFile->close();
	delete projectFile;
}

QString QSIProject::getPath(bool projectFile) {
	if(projectFile) return this->projectPath;
	return this->path;
}

QIcon* QSIProject::getIcon() {
	return nullptr;
}

QString QSIProject::getCompiler() {
	return this->compler;
}

void QSIProject::setCompiler(QString set) {
	this->compler = set;
}
