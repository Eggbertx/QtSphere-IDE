#include <QDebug>
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

#define QUOTE_REGEX "\"|'|`"

QSIProject::QSIProject(QObject *parent) : QObject(parent) {
	m_projectDir = "";
	m_projectFilePath = "";
	m_projectFileFormat = QSIProject::UnknownProjectType;
	m_buildDir = "";
}

QSIProject::~QSIProject() {

}

bool QSIProject::open(QString path) {
	m_projectDir = path;
	QFileInfo fileInfo(m_projectDir);
	if(!fileInfo.exists()) {
		errorBox("Project file " + m_projectDir + " doesn't exist!");
		return false;
	}

	if(fileInfo.isDir()) {
		QStringList filters({"*.ssproj", "Cellscript.js", "Cellscript.mjs", "Cellscript.cjs", "game.sgm"});
		QFileInfoList infoList = QDir(m_projectDir).entryInfoList(filters,QDir::Files|QDir::NoDotAndDotDot);

		// look for *.ssproj, Cellscript.js, Cellscript.mjs, Cellscript.cjs, and game.sgm, in that order
		for(int f = 0; f < infoList.length(); f++) {
			QFileInfo fi = infoList.at(f);
			m_projectFilePath = fi.filePath();
			QString fileName = fi.fileName();
			QString suffix = fi.suffix();
			if(suffix == "ssproj") {
				m_projectFileFormat = QSIProject::SSProject;
				break;
			} else if(fileName == "Cellscript.js") {
				m_compiler = "Cell";
				m_projectFileFormat = QSIProject::Cellscript_js;
				break;
			} else if(fileName == "Cellscript.mjs") {
				m_compiler = "Cell";
				m_projectFileFormat = QSIProject::Cellscript_mjs;
				break;
			} else if(fileName == "Cellscript.cjs") {
				m_compiler = "Cell";
				m_projectFileFormat = QSIProject::Cellscript_cjs;
				break;
			} else if(fileName == "game.sgm") {
				m_projectFileFormat = QSIProject::SGM;
				break;
			}
		}
	} else {
		m_projectFilePath = fileInfo.filePath();
	}
	if(m_projectFilePath == "") {
		return false;
	}
	QFile* projectFile = new QFile(m_projectFilePath);
	bool success = prepareProjectFile(projectFile);
	projectFile->close();
	delete projectFile;
	return success;
}

bool QSIProject::save() {
	m_projectFilePath = QDir(m_projectDir).absoluteFilePath(QDir(m_projectDir).dirName() + ".ssproj");
	QFile* projectFile = new QFile(m_projectFilePath);
	if(!projectFile->open(QFile::WriteOnly)) {
		errorBox("Unable to open '" + m_projectFilePath + "': " + projectFile->errorString());
		delete projectFile;
		return false;
	}
	QString ssText;
	QTextStream(&ssText) <<
		"[.ssproj]\r\n" <<
		"author=" << m_author << "\r\n" <<
		"compiler=" << getCompiler() << "\r\n" <<
		"description=" << m_summary << "\r\n" <<
		"mainScript=" << m_script << "\r\n" <<
		"name=" << getName() << "\r\n" <<
		"screenHeight=" << m_height << "\r\n" <<
		"screenWidth=" << m_width << "\r\n";


	bool success = false;
	if(projectFile->write(ssText.toLatin1()) > -1) {
		m_projectFileFormat = QSIProject::SSProject;
		success = true;
	}
	projectFile->close();
	return success;
}

QString QSIProject::getName() {
	if(m_name == "") return QFileInfo(m_projectDir).baseName();
	return m_name;
}

void QSIProject::setName(QString name) {
	m_name = name;
}

QString QSIProject::getResolutionString() {
	QString resolution = "";
	return resolution.sprintf("%dx%d", m_width, m_height);
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

QString QSIProject::getBuildDir() {
	if(m_compiler == "Vanilla") return getPath(false);
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
	if(projectFile) return m_projectFilePath;
	return m_projectDir;
}

void QSIProject::setPath(QString path, bool projectFile) {
	if(projectFile) m_projectFilePath = path;
	else m_projectDir = path;
}

QIcon QSIProject::getIcon() {
	QFileInfo icon_fi = QFileInfo(QDir(m_projectDir), "icon.png");
	if(icon_fi.exists()) {
		return QIcon(icon_fi.canonicalFilePath());
	}
	return QIcon(":/icons/sphere-icon.png");
}

QSIProject::ProjectFileFormat QSIProject::getProjectFormat() {
	return m_projectFileFormat;
}

QString QSIProject::getCompiler() {
	return m_compiler;
}

void QSIProject::setCompiler(QString compiler) {
	m_compiler = compiler;
}

bool QSIProject::prepareProjectFile(QFile* projectFile) {
	if(projectFile == nullptr) return false;
	if(!projectFile->isOpen()) {
		if(!projectFile->open(QFile::ReadOnly)) {
			errorBox("Unable to open project file in '" + m_projectFilePath + "'");
			return false;
		}
	}
	switch(m_projectFileFormat) {
	case QSIProject::SSProject:
		return readSSProj(projectFile);
	case QSIProject::Cellscript_js:
	case QSIProject::Cellscript_mjs:
	case QSIProject::Cellscript_cjs:
		return readCellscript(projectFile);
	case QSIProject::SGM:
		return readSGM(projectFile);
	default:
		// no project file in directory
		return false;
	}
}

bool QSIProject::readSSProj(QFile* projectFile) {
	QTextStream stream(projectFile);
	while(!stream.atEnd()) {
		QStringList arr = stream.readLine().split("=");
		if(arr.length() != 2) continue;
		QString key = arr.at(0);
		QString value = arr.at(1);

		if(key == "author") m_author = value;
		else if(key == "buildDir") m_buildDir = QDir(m_projectDir).absoluteFilePath(value);
		else if(key == "compiler") {
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

QString QSIProject::getCellscriptStringValue(QString cellscriptStr, QString key, QString defaultValue) {
	QString value = defaultValue;
	QString reStr = "(" + key + "):\\s*(" + QUOTE_REGEX + ")(.*)(" + QUOTE_REGEX + "),";
	QRegularExpression re(reStr, QRegularExpression::OptimizeOnFirstUsageOption);
	QStringList captureList = re.match(cellscriptStr).capturedTexts();
	if(captureList.length() > 4)
		value = captureList.at(3);

	return value
		.replace("\\'", "'")
		.replace("\\\"", "\"")
		.replace("\\`","`");
}

int QSIProject::getCellscriptIntValue(QString cellscriptStr, QString key, int defaultValue) {
	int value = defaultValue;
	QString reStr = "(" + key + "):\\s*(\\d+).*,";
	QRegularExpression re(reStr, QRegularExpression::OptimizeOnFirstUsageOption);
	QStringList captureList = re.match(cellscriptStr).capturedTexts();
	if(captureList.length() >= 3) {
		bool ok = false;
		value = captureList.at(2).toInt(&ok);
		if(!ok) value = defaultValue;
	}
	return value;
}

bool QSIProject::readCellscript(QFile* projectFile) {
	QTextStream stream(projectFile);
	QString text = stream.readAll();
	m_name = getCellscriptStringValue(text, "name");
	m_author = getCellscriptStringValue(text, "author");
	m_version = getCellscriptIntValue(text, "version", 1);
	m_apiLevel = getCellscriptIntValue(text, "apiLevel", 1);
	m_saveID = getCellscriptStringValue(text, "saveID");
	m_summary = getCellscriptStringValue(text, "summary");
	m_script = getCellscriptStringValue(text, "main");
	QStringList resolution = getCellscriptStringValue(text, "resolution").split("x");
	bool ok = false;
	if(resolution.length() != 2) {
		m_width = -1;
		m_height = -1;
		return true;
	}
	m_width = resolution.at(0).toInt(&ok);
	if(!ok) m_width = -1;
	m_height = resolution.at(1).toInt(&ok);
	if(!ok) m_height = -1;

	if(m_width < 0 || m_height < 0) {
		qDebug() << m_name << "has an invalid resolution:" << resolution.at(0) << "x" << resolution.at(1);
	}
	m_compiler = "Cell";

	/*qDebug().nospace().noquote() <<
		"Project: " << m_name << "\n" <<
		"Author: " << m_author << "\n" <<
		"Version: " << m_version << "\n" <<
		"API Level: " << m_apiLevel << "\n" <<
		"Save ID: " << m_saveID << "\n" <<
		"Summary: " << m_summary << "\n" <<
		"Resolution: " << m_width << "x" << m_height << "\n" <<
		"Main script: " << m_script << "\n";*/
	m_buildDir = QFileInfo(*projectFile).dir().absoluteFilePath("dist/");
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
		else if(key == "screen_height") m_height = value.toInt();
		else if(key == "script") m_script = value;
	}
	m_compiler = "Vanilla";
	m_buildDir = QFileInfo(*projectFile).dir().path();
	return true;
}
