#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QFileInfoList>
#include <QRegularExpression>
#include <QRegularExpressionMatchIterator>
#include <QRegularExpressionMatch>
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
		filters << "*.ssproj" << "Cellscript.mjs" << "Cellscript.mjs" << "game.sgm";
		QDir projectDir = QDir(this->path);

		QFileInfoList infoList = projectDir.entryInfoList(filters,QDir::Files|QDir::NoDotAndDotDot);
		foreach (QFileInfo fi, infoList) {
			if(fi.suffix() == "ssproj"
			|| fi.fileName() == "Cellscript.mjs"
			|| fi.fileName() == "Cellscript.js"
			|| fi.fileName() == "game.sgm") {
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
	QString filename = fileInfo.fileName();
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
	} else if(filename == "Cellscript.mjs" || filename == "Cellscript.mjs") {
		QString text = stream.readAll();
		bool ok = false;
		QRegularExpression nameRe("name[(\\s*=\\s*):\\s\"']*(.*)[\"']+", QRegularExpression::OptimizeOnFirstUsageOption);
		QStringList captureList = nameRe.match(text).capturedTexts();
		if(captureList.length() > 1) this->name = captureList.at(1);
		else this->name = "";
		this->name = this->name.replace("\\\"", "\"").replace("\\'","'");

		QRegularExpression authorRe("author[(\\s*=\\s*):\\s\"']*(.*)[\"']+", QRegularExpression::OptimizeOnFirstUsageOption);
		captureList = authorRe.match(text).capturedTexts();
		if(captureList.length() > 1) this->author = captureList.at(1);
		else this->author = "";
		this->author = this->author.replace("\\\"", "\"").replace("\\'","'");

		QRegularExpression apiRe("apiLevel[(\\s*=\\s*):\\s\"']*(\\d+)", QRegularExpression::OptimizeOnFirstUsageOption);
		captureList = apiRe.match(text).capturedTexts();
		if(captureList.length() > 1) this->apiLevel = captureList.at(1).toInt(&ok);
		if(!ok) this->apiLevel = 12;

		QRegularExpression versionRe("version[(\\s*=\\s*):\\s\"']*(\\d+)", QRegularExpression::OptimizeOnFirstUsageOption);
		captureList = versionRe.match(text).capturedTexts();
		if(captureList.length() > 1) this->version = captureList.at(1).toInt(&ok);
		if(!ok) this->version = 2;

		QRegularExpression saveIDRe("saveID[(\\s*=\\s*):\\s\"']*(.*)[\"']+", QRegularExpression::OptimizeOnFirstUsageOption);
		captureList = saveIDRe.match(text).capturedTexts();
		if(captureList.length() > 1) this->saveID = captureList.at(1);
		else this->saveID = "";

		QRegularExpression summaryRe("summary[(\\s*=\\s*):\\s\"']*(.*)[\"']+", QRegularExpression::OptimizeOnFirstUsageOption);
		captureList = summaryRe.match(text).capturedTexts();
		if(captureList.length() > 1) this->summary = captureList.at(1);
		else this->summary = "";
		this->summary = this->summary.replace("\\\"", "\"").replace("\\'","'");

		QRegularExpression resolutionRe("resolution[(\\s*=\\s*):\\s\"']*(.*)[\"']+", QRegularExpression::OptimizeOnFirstUsageOption);
		QStringList resolutionList = resolutionRe.match(text).capturedTexts();
		if(resolutionList.length() > 1) {
			this->width = resolutionList.at(1).split("x").at(0).toInt(&ok);
			if(!ok) this->width = 320;
			this->height = resolutionList.at(1).split("x").at(1).toInt(&ok);
			if(!ok) this->height = 240;
		} else {
			this->width = 320;
			this->height = 240;
		}

		qDebug().nospace() <<
			"Name: " << this->name << "\n" <<
			"Author: " << this->author << "\n" <<
			"API Level: " << this->apiLevel << "\n" <<
			"Version: " << this->version << "\n" <<
			"Summary: " << this->summary << "\n" <<
			"Save ID: " << this->saveID << "\n" <<
			"Resolution: (" << this->width << "," << this->height << ")";
		qDebug() << "\n";
	} else if(suffix == "sgm") {
		while(!stream.atEnd()) {
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
