#ifndef EXPRESSIONPARSER_H
#define EXPRESSIONPARSER_H

#include<QVector>
#include<QSet>
#include<QString>

class Specimens;

class ExpressionParser
{
private:
    enum Token { SET_OR, SET_AND, SET_MISSING, SET_DIF, SET_NONE, SET_ALL, SET_OPEN, SET_CLOSE,
                  EXPR_OPEN, EXPR_CLOSE, LOG_NOT, LOG_AND, LOG_OR, REL_LESS, REL_MORE,
                  REL_EQUAL, REL_NOT_EQUAL, REL_LESS_EQ, REL_MORE_EQ, TERM, EXPR_END};

    QVector<Token> m_symbols;
    QVector<QString> m_terms;
    QVector<QString> m_errMsgs;
    QSet<unsigned int> m_selected;

    bool m_parsed;

    Specimens* m_specimens;

    void parseError(const QString& errMsg);
    bool tokenize(const QString&);
    Token peekToken() const {return m_symbols.size() ? m_symbols.front() : EXPR_END;}
    void consumeToken() {return m_symbols.pop_front();}
    void expectToken(Token, const QString&);
    QString nextTerm()
    {
        QString term = (m_terms.size()) ? m_terms.front() : "";
        m_terms.pop_front();
        return term;
    }
    QSet<unsigned int> setExpr();
    QSet<unsigned int> setOprnd();
    QSet<unsigned int> selectExpr();
    QSet<unsigned int> selFactor();
    QSet<unsigned int> evalTerm();

//    bool term(QString&, int&);
//    bool value(QString&, int&);
//    bool logOpr(QString&, QString&);
//    bool relOpr(QString&, QString&);

public:
	ExpressionParser(Specimens* s = 0) : m_parsed(true), m_specimens(s)
	{}
	~ExpressionParser(){}

    bool isParsed() const {return m_parsed;}
    QSet<unsigned int> getSelected() const {return m_selected;}
    void setSpecimens(Specimens* s) {m_specimens = s;}
	void parseExpression(const QString& expression)
	{
	    m_parsed = true;
	    if(tokenize(expression))
            m_selected = setExpr();
    }
    const QVector<QString>& getErrors() const {return m_errMsgs;}
};

#endif
