#ifndef HIGHLIGHTER_H
#define HIGHLIGHTER_H
#include <QSyntaxHighlighter>

class Highlighter : public QSyntaxHighlighter
{
		Q_OBJECT
	public:
		Highlighter(QTextDocument *parent = 0);

	protected:
		void highlightBlock(const QString &text) override;

	private:
		struct HighlightingRule {
				QRegExp pattern;
				QTextCharFormat format;
		};

		QVector<HighlightingRule> highlightingRules;

		QRegExp comment_start;
		QRegExp comment_end;
		QTextCharFormat keywordFormat;
		QTextCharFormat classFormat;
		QTextCharFormat singleLineCommentFormat;
		QTextCharFormat multiLineCommentFormat;
		QTextCharFormat quotationFormat;
		QTextCharFormat functionFormat;
};

#endif // HIGHLIGHTER_H
