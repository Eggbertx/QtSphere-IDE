#include <QTextDocument>
#include <QSyntaxHighlighter>
#include <QTextStream>
#include "syntaxhighlighter.h"

SyntaxHighlighter::SyntaxHighlighter(QTextDocument *parent) : QSyntaxHighlighter(parent) {
	HighlightingRule rule;
	keywordFormat.setForeground(Qt::darkBlue);
	keywordFormat.setFontWeight(QFont::Bold);
	QStringList keywordPatterns;
	keywordPatterns << "\\babstract\\b" << "\\barguments\\b" << "\\bawait*\\b" << "\\bboolean\\b"
				<< "\\bbyte\\b" << "\\bcase\\b" << "\\bcatch\\b" << "\\bbreak\\b" << "\\bchar\\b"
				<< "\\bclass*\\b" << "\\bconst\\b" << "\\bcontinue\\b" "debugger\\b"
				<< "\\bdefault\\b" << "\\bdelete\\b" << "\\bdo\\b" << "\\bdouble\\b" << "\\belse\\b"
				<< "\\benum*\\b" << "\\beval\\b" << "\\bexport*\\b" << "\\bextends\\b" << "\\bexport*\\b"
				<< "\\bextends*\\b" << "\\bfalse\\b" << "\\bfinal\\b" << "\\bfloat\\b" << "\\bfor\\b"
				<< "\\bfunction\\b" << "\\bgoto\\b" << "\\bif\\b" << "\\bimplements\\b" << "\\bimport*\\b"
				<< "\\bin\\b" << "\\binstanceof\\b" << "\\bint\\b" << "\\binterface\\b" << "\\blet*\\b"
				<< "\\blong\\b" << "\\bnative\\b" << "\\bnew\\b" << "\\bnull\\b" << "\\bpackage\\b"
				<< "\\bprivate\\b" << "\\bprotected\\b" << "\\bpublic\\b" << "\\breturn\\b"
				<< "\\bshort\\b" << "\\bstatic\\b" << "\\bsuper*\\b" << "\\bswitch\\b"
				<< "\\bsynchronized\\b" << "\\bthis\\b" << "\\bthrow\\b" << "\\bthrows\\b"
				<< "\\btransient\\b" << "\\btrue\\b" << "\\btry\\b" << "\\btypeof\\b" << "\\bvar\\b"
				<< "\\bvoid\\b" << "\\bvolatile\\b" << "\\bwhile\\b" << "\\bwith\\b" << "\\byield\\b";
	foreach(const QString &pattern, keywordPatterns) {
		rule.pattern = QRegularExpression(pattern);
		rule.format = keywordFormat;
		highlightingRules.append(rule);
	}

	QStringList legacyFunctions;
	QFile* legacyFunctions_file = new QFile(":/dictionaries/LegacyFunctions.txt");
	QTextStream textStream(legacyFunctions_file);
	while(!textStream.atEnd()) legacyFunctions.append("\\b" + textStream.readLine() + "\\b");

	QStringList sphereObjects;

	quotationFormat.setForeground(Qt::darkGreen);
	rule.pattern = QRegularExpression("\".*\"");
	rule.format = quotationFormat;
	highlightingRules.append(rule);

	singleLineCommentFormat.setForeground(Qt::red);
	rule.pattern = QRegularExpression("//[^\n]*");
	rule.format = singleLineCommentFormat;
	highlightingRules.append(rule);

	multiLineCommentFormat.setForeground(Qt::red);

	commentStartExpression = QRegularExpression("/\\*");
	commentEndExpression = QRegularExpression("\\*/");
}

void SyntaxHighlighter::highlightBlock(const QString &text) {
	foreach(const HighlightingRule &rule, highlightingRules) {
		QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
		while(matchIterator.hasNext()) {
			QRegularExpressionMatch match = matchIterator.next();
			setFormat(match.capturedStart(), match.capturedLength(), rule.format);
		}
	}
	setCurrentBlockState(0);

	int startIndex = 0;
	if(previousBlockState() != 1)
		startIndex = text.indexOf(commentStartExpression);
	while(startIndex > 0) {
		QRegularExpressionMatch match = commentEndExpression.match(text, startIndex);
		int endIndex = match.capturedStart();
		int commentLength = 0;
		if(endIndex == -1) {
			setCurrentBlockState(1);
		} else {
            commentLength = endIndex - startIndex + match.capturedLength();
		}

		setFormat(startIndex, commentLength, multiLineCommentFormat);
		startIndex = text.indexOf(commentStartExpression, startIndex + commentLength);
	}
}
