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
	m_path = path;
	QFileInfo fileInfo(m_path);
	if(!fileInfo.exists()) {
		errorBox("Project file " + m_path + " doesn't exist!");
		return;
	}
	bool found = false;
	if(fileInfo.isDir()) {
		// get list of files in project dir, prioritizing .ssproj over game.sgm
		QStringList filters({"*.ssproj", "Cellscript.mjs", "Cellscript.js", "game.sgm"});
		QDir projectDir = QDir(m_path);
		QFileInfoList infoList = projectDir.entryInfoList(filters,QDir::Files|QDir::NoDotAndDotDot);

		bool foundSSProj = false;

		// first search for a .ssproj file
		for(int f = 0; f < infoList.length(); f++) {
			QFileInfo fi = infoList.at(f);
			if(!foundSSProj && fi.suffix() == "ssproj") {
				m_projectPath = fi.filePath();
				fileInfo = fi;
				found = true;
			}
		}
		foreach(QFileInfo fi, infoList) {
			if(fi.suffix() == "ssproj") {
				foundSSProj = true;
			} else if(!foundSSProj && (
			fi.fileName() == "Cellscript.mjs"
			|| fi.fileName() == "Cellscript.js"
			|| fi.fileName() == "game.sgm")) {
				m_projectPath = fi.filePath();
				fileInfo = fi;
				found = true;
				break;
			}
		}
	} else {
		m_projectPath = fileInfo.filePath();
		m_path = fileInfo.dir().path();
		found = true;
	}
	if(!found) return;
	QFile* projectFile = new QFile(m_projectPath);
	if(!projectFile->open(QFile::ReadOnly|QFile::Text)) {
		errorBox("Error reading " + fileInfo.fileName() + ": " + projectFile->errorString());
		delete projectFile;
	}
	QString suffix = fileInfo.suffix();
	QString filename = fileInfo.fileName();
	QTextStream stream(projectFile);


	if(suffix == "ssproj") {
		readSSProj(projectFile);
	} else if(filename == "Cellscript.mjs" || filename == "Cellscript.js") {
		readCellscript(projectFile);
	} else if(suffix == "sgm") {
		readSGM(projectFile);
	}

	projectFile->close();
	delete projectFile;
}

QString QSIProject::getName() {
	return m_name;
}

void QSIProject::setName(QString name) {
	m_name = name;
}

QString QSIProject::getAuthor() {
	return m_author;
}

void QSIProject::setAuthor(QString author) {
	m_author = author;
}

int QSIProject::getWidth() {
	return m_width;
}

void QSIProject::setWidth(int width) {
	m_width = width;
}

int QSIProject::getHeight() {
	return m_height;
}

void QSIProject::setHeight(int height) {
	m_height = height;
}

void QSIProject::setSize(int width, int height) {
	m_width = width;
	m_height = height;
}

int QSIProject::getAPILevel() {
	return m_apiLevel;
}

void QSIProject::setAPILevel(int level) {
	m_apiLevel = level;
}

int QSIProject::getVersion() {
	return m_version;
}

void QSIProject::setVersion(int version) {
	m_version = version;
}

QString QSIProject::getSaveID() {
	return m_saveID;
}

void QSIProject::setSaveID(QString id) {
	m_saveID = id;
}

QString QSIProject::getSummary() {
	return m_summary;
}

void QSIProject::setSummary(QString summary) {
	m_summary = summary;
}

QString QSIProject::getBuidlDir() {
	return m_buildDir;
}

void QSIProject::setBuildDir(QString dir) {
	m_buildDir = dir;
}

QString QSIProject::getMainScript() {
	return m_script;
}

void QSIProject::setMainScript(QString path) {
	m_script = path;
}

QString QSIProject::getPath(bool projectFile) {
	if(projectFile) return m_projectPath;
	return m_path;
}

void QSIProject::setPath(QString path, bool projectFile) {
	if(projectFile) m_projectPath = path;
	else m_path = path;
}

QIcon* QSIProject::getIcon() {
	return nullptr;
}

QString QSIProject::getCompiler() {
	return m_compiler;
}

void QSIProject::setCompiler(QString compiler) {
	m_compiler = compiler;
}

bool QSIProject::readSSProj(QFile* projectFile) {
	QTextStream stream(projectFile);
	while(!stream.atEnd()) {
		QString line = stream.readLine();
		if(line == "[.ssproj]" || line == "") continue;
		QString key = line.section("=", 0, 0);
		QString value = line.section("=", 1);

		if(key == "author") m_author = value;
		else if(key == "buildDir") m_buildDir = m_path + "/" + value;
		else if(key == "compiler") {
			if(value == "Cell") m_compiler = QSIProject::Cell;
			m_compiler = value;
		}
		else if(key == "description") m_summary = value;
		else if(key == "mainScript") m_script = value;
		else if(key == "name") m_name = value;
		else if(key == "screenHeight") m_height = value.toInt();
		else if(key == "screenWidth") m_width = value.toInt();
	}
	return true;
}

bool QSIProject::readCellscript(QFile* projectFile) {
	QTextStream stream(projectFile);
	QString text = stream.readAll();
	bool ok = false;
	QRegularExpression nameRe("name[(\\s*=\\s*):\\s\"']*(.*)[\"']+", QRegularExpression::OptimizeOnFirstUsageOption);
	QStringList captureList = nameRe.match(text).capturedTexts();
	if(captureList.length() > 1) m_name = captureList.at(1);
	else m_name = "";
	m_name = m_name.replace("\\\"", "\"").replace("\\'","'");

	QRegularExpression authorRe("author[(\\s*=\\s*):\\s\"']*(.*)[\"']+", QRegularExpression::OptimizeOnFirstUsageOption);
	captureList = authorRe.match(text).capturedTexts();
	if(captureList.length() > 1) m_author = captureList.at(1);
	else m_author = "";
	m_author = m_author.replace("\\\"", "\"").replace("\\'","'");

	QRegularExpression apiRe("apiLevel[(\\s*=\\s*):\\s\"']*(\\d+)", QRegularExpression::OptimizeOnFirstUsageOption);
	captureList = apiRe.match(text).capturedTexts();
	if(captureList.length() > 1) m_apiLevel = captureList.at(1).toInt(&ok);
	if(!ok) m_apiLevel = 12;

	QRegularExpression versionRe("version[(\\s*=\\s*):\\s\"']*(\\d+)", QRegularExpression::OptimizeOnFirstUsageOption);
	captureList = versionRe.match(text).capturedTexts();
	if(captureList.length() > 1) m_version = captureList.at(1).toInt(&ok);
	if(!ok) m_version = 2;

	QRegularExpression saveIDRe("saveID[(\\s*=\\s*):\\s\"']*(.*)[\"']+", QRegularExpression::OptimizeOnFirstUsageOption);
	captureList = saveIDRe.match(text).capturedTexts();
	if(captureList.length() > 1) m_saveID = captureList.at(1);
	else m_saveID = "";

	QRegularExpression summaryRe("summary[(\\s*=\\s*):\\s\"']*(.*)[\"']+", QRegularExpression::OptimizeOnFirstUsageOption);
	captureList = summaryRe.match(text).capturedTexts();
	if(captureList.length() > 1) m_summary = captureList.at(1);
	else m_summary = "";
	m_summary = m_summary.replace("\\\"", "\"").replace("\\'","'");

	QRegularExpression resolutionRe("resolution[(\\s*=\\s*):\\s\"']*(.*)[\"']+", QRegularExpression::OptimizeOnFirstUsageOption);
	QStringList resolutionList = resolutionRe.match(text).capturedTexts();
	if(resolutionList.length() > 1) {
		m_width = resolutionList.at(1).split("x").at(0).toInt(&ok);
		if(!ok) m_width = 320;
		m_height = resolutionList.at(1).split("x").at(1).toInt(&ok);
		if(!ok) m_height = 240;
	} else {
		m_width = 320;
		m_height = 240;
	}
	return true;
}

bool QSIProject::readSGM(QFile *projectFile) {
	QTextStream stream(projectFile);
	while(!stream.atEnd()) {
		QString line = stream.readLine();
		if(line == "") continue;
		QString key = line.section("=", 0, 0);
		QString value = line.section("=", 1);
		if(key == "name") m_name = value;
		else if(key == "author") m_author = value;
		else if(key == "description") m_summary = value;
		else if(key == "screen_width") m_width = value.toInt();
		else if(key == "screen+height") m_height = value.toInt();
		else if(key == "script") m_script = value;
	}
	m_compiler = QSIProject::Vanilla;
	return true;
}
