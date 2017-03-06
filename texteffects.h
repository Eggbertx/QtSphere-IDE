#ifndef TEXTEFFECTS_H
#define TEXTEFFECTS_H
#include <QSyntaxHighlighter>

class TextEffects : public QSyntaxHighlighter {
		Q_OBJECT
	public:
		TextEffects(QTextDocument *parent = 0);
		void highlightSyntax(bool yn);
		void autoComplete(bool yn);
		void apiSuggestions(bool yn);

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

#endif
