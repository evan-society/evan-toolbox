#include "ExpressionParser.h"
#include "../DataTypes/Specimens.h"
#include <QRegExp>
#include <QMessageBox>

void ExpressionParser::parseError(const QString& errMsg)
{
    m_errMsgs.push_back(errMsg);
    m_parsed = false;
}

bool ExpressionParser::tokenize(const QString& expression)
{
    QString tokenStr;
    m_symbols.clear();
    m_terms.clear();
    m_errMsgs.clear();
    for(int i=0; i<expression.length(); ++i)
    {
        switch (expression.at(i).toAscii())
        {
            case '+': m_symbols.push_back(SET_OR); break;
            case '-': m_symbols.push_back(SET_DIF); break;
            case '&': m_symbols.push_back(SET_AND); break;
            case ']': m_symbols.push_back(SET_CLOSE); break;
            case '[':
            {
                int closePos = expression.indexOf(']', i);
                if(closePos == -1)
                {
                    parseError( "] missing after expression!" );
                    return false;
                }
                tokenStr = expression.mid(i+1, closePos-(i+1)).trimmed();
                if(tokenStr == "ALL")
                {
                    m_symbols.push_back(SET_ALL);
                    i = closePos;
                }
                else if(tokenStr == "MISSING")
                {
                    m_symbols.push_back(SET_MISSING);
                    i = closePos;
                }
                else if(tokenStr == "")
                {
                    m_symbols.push_back(SET_NONE);
                    i = closePos;
                }
                else
                    m_symbols.push_back(SET_OPEN);
            }
            break;

            case '(': m_symbols.push_back(EXPR_OPEN); break;
            case ')': m_symbols.push_back(EXPR_CLOSE);break;

            case 'N':
            {
                int endPos = expression.indexOf(QRegExp("[\\s\\-()><!=&\\+\\[\\]]"), i);
                if(endPos == -1)
                {
                    parseError( "White Space not found!");
                    return false;
                }
                tokenStr = expression.mid(i, endPos-i).trimmed();
                if(tokenStr == "NOT")
                    m_symbols.push_back(LOG_NOT);
                else
                {
                    m_symbols.push_back(TERM);
                    m_terms.push_back(tokenStr);
                }
                i = endPos-1;
            }
            break;
            case 'A':
            {
                int endPos = expression.indexOf(QRegExp("[\\s\\-()><!=&\\+\\[\\]]"), i);
                if(endPos == -1)
                {
                    parseError( "White Space not found!");
                    return false;
                }
                tokenStr = expression.mid(i, endPos-i).trimmed();
                if(tokenStr == "AND")
                    m_symbols.push_back(LOG_AND);
                else
                {
                    m_symbols.push_back(TERM);
                    m_terms.push_back(tokenStr);
                }
                i = endPos-1;
            }
            break;
            case 'O':
            {
                int endPos = expression.indexOf(QRegExp("[\\s\\-()><!=&\\+\\[\\]]"), i);
                if(endPos == -1)
                {
                    parseError( "White Space not found!");
                    return false;
                }
                tokenStr = expression.mid(i, endPos-i).trimmed();
                if(tokenStr == "OR")
                    m_symbols.push_back(LOG_OR);
                else
                {
                    m_symbols.push_back(TERM);
                    m_terms.push_back(tokenStr);
                }
                i = endPos-1;
            }
            break;
            case '>':
            {
                if(i+1 < expression.length())
                {
                    if(expression.at(i+1) == '=')
                    {
                        m_symbols.push_back(REL_MORE_EQ);
                        ++i;
                    }
                    else
                        m_symbols.push_back(REL_MORE);
                }
                else
                    m_symbols.push_back(REL_MORE);
            }
            break;
            case '<':
            {
                if(i+1 < expression.length())
                {
                    if(expression.at(i+1) == '=')
                    {
                        m_symbols.push_back(REL_LESS_EQ);
                        ++i;
                    }
                    else
                        m_symbols.push_back(REL_LESS);
                }
                else
                    m_symbols.push_back(REL_LESS);
            }
            break;
            case '=': m_symbols.push_back(REL_EQUAL);break;
            case '!':
            {
                if(i+1 < expression.length())
                {
                    if(expression.at(i+1) == '=')
                    {
                        m_symbols.push_back(REL_NOT_EQUAL);
                        ++i;
                    }
                    else
                    {
                        parseError( "Incorrect Syntax !");
                        return false;
                    }
                }
                else
                {
                    parseError( "Incorrect Syntax !");
                    return false;
                }
            }
            break;
            case '"':
            {
                int closePos = expression.indexOf('"', i+1);
                if(closePos == -1)
                {
                    parseError( "Missing ' \" ' for string value!");
                    return false;
                }
                tokenStr = expression.mid(i+1, closePos-(i+1)).trimmed();
                m_symbols.push_back(TERM);
                m_terms.push_back(tokenStr);
                i = closePos;
            }
            break;
            case ' ':
            case '\t':
            case '\n':
            break;
            default:
            {
                int endPos = expression.indexOf(QRegExp("[\\s\\-()><!=&\\+\\[\\]]"), i);
                if(endPos == -1)
                {
                    parseError( "White Space not found!");
                    return false;
                }
                tokenStr = expression.mid(i, endPos-i).trimmed();
                m_symbols.push_back(TERM);
                m_terms.push_back(tokenStr);
                i = endPos-1;
            }
            break;
        }
    }
    m_symbols.push_back(EXPR_END);
    return true;
}

void ExpressionParser::expectToken(Token token, const QString& errMsg)
{
    if(peekToken() == token)
        consumeToken();
    else
        parseError( errMsg );
}

QSet<unsigned int> ExpressionParser::setExpr()
{
    QSet<unsigned int> result = setOprnd();
    if(!m_specimens)
    {
        parseError("Input Specimens is empty!");
        return result;
    }
    switch(peekToken())
    {
        case SET_OR:
            consumeToken();
            result += setExpr();
        break;
        case SET_AND:
            consumeToken();
            result &= setExpr();
        break;
        case SET_DIF:
            consumeToken();
            result -= setExpr();
        break;
        case EXPR_END: break;
        default: parseError("Unexpected symbol after set expression!"); break;
    }
    return result;
}

QSet<unsigned int> ExpressionParser::setOprnd()
{
    QSet<unsigned int> result;
    switch(peekToken())
    {
        case SET_ALL:
            consumeToken();
            for(unsigned int i=0; i<m_specimens->getSize(); ++i)
                result.insert(i);
        break;
        case SET_MISSING:
            consumeToken();
            for(unsigned int i=0; i<m_specimens->getSize(); ++i)
                if(!m_specimens->getLandmarkSet(i)->isComplete())
                    result.insert(i);
        break;
        case SET_NONE:
            consumeToken();
        break;
        case SET_OPEN:
            consumeToken();
            result = selectExpr();
            expectToken(SET_CLOSE, "] missing after expression!");
        break;
        default: parseError( "Set Expressions must be enclosed in square brackets '[]'" ); break;
    }

    return result;
}

QSet<unsigned int> ExpressionParser::selectExpr()
{
    QSet<unsigned int> result = selFactor();
    switch(peekToken())
    {
        case LOG_AND:
            consumeToken();
            result &= selectExpr();
        break;
        case LOG_OR:
            consumeToken();
            result += selectExpr();
        break;
        default: break;
    }
    return result;
}

QSet<unsigned int> ExpressionParser::selFactor()
{
    QSet<unsigned int> result;
    switch(peekToken())
    {
        case EXPR_OPEN:
            consumeToken();
            result = selectExpr();
            expectToken(EXPR_CLOSE, ") missing after expression!");
        break;
        case LOG_NOT:
            consumeToken();
            for(unsigned int i=0; i<m_specimens->getSize(); ++i)
                result.insert(i);
            result -= selFactor();
        break;
        case TERM:
            consumeToken();
            result = evalTerm();
        break;
        default: parseError( "Incomplete selection expression!" ); break;
    }
    return result;
}

QSet<unsigned int> ExpressionParser::evalTerm()
{
    QSet<unsigned int> result;
    if(!m_specimens->isValid())
    {
        parseError( "Input Specimens is empty!");
        return result;
    }

    QString termExpr = nextTerm()+"$::$";
    switch(peekToken())
    {
        case REL_EQUAL: consumeToken(); termExpr+= "=$::$"; break;
        case REL_LESS: consumeToken(); termExpr+= "<$::$"; break;
        case REL_MORE: consumeToken(); termExpr+= ">$::$"; break;
        case REL_NOT_EQUAL: consumeToken(); termExpr+= "!=$::$"; break;
        case REL_LESS_EQ: consumeToken(); termExpr+= "<=$::$"; break;
        case REL_MORE_EQ: consumeToken(); termExpr+= ">=$::$"; break;
        default: parseError("Missing operator between terms!"); break;
    }

    if(m_parsed)
    {
        if(peekToken() == TERM)
        {
            consumeToken();
            termExpr += nextTerm();
            result = m_specimens->matchExpression(termExpr);
        }
        else parseError("Missing second operand!");
    }

    return result;
}
