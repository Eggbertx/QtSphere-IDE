#ifndef SOURCEEDIT_H
#define SOURCEEDIT_H

#include <QObject>
#include <QSyntaxHighlighter>
#include <QRegularExpression>
#include <QTextCharFormat>

class SyntaxHighlighter : public QSyntaxHighlighter {
	Q_OBJECT
    Q_ENUMS(Type)

	public:
        SyntaxHighlighter(QTextDocument *parent = 0);
		QString getSourceMode();
		void setSourceType(QString mode);
		void setSourceType(int mode);
		enum Type { Plaintext, Javascript, TypeScript, SGM, JSON, CSS };

	protected:
        void highlightBlock(const QString &text) override;

	private:
		int sourceType;
		struct HighlightingRule {
				QRegularExpression pattern;
				QTextCharFormat format;
		};
		QVector<HighlightingRule> highlightingRules;
		QRegularExpression commentStartExpression;
		QRegularExpression commentEndExpression;
		QTextCharFormat keywordFormat; // var, let, function, while, etc
		QTextCharFormat classFormat;
		QTextCharFormat singleLineCommentFormat;
		QTextCharFormat multiLineCommentFormat;
		QTextCharFormat quotationFormat;
		QTextCharFormat functionFormat;
};

#endif // SOURCEEDIT_H
