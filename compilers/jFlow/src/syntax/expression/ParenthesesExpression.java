package syntax.expression;

import syntax.Token;

/**
 * Parentheses expression class.
 */
public final class ParenthesesExpression extends Expression {
    /**
     * The open parentheses token.
     */
    private final Token _openParenthesesToken;
    /**
     * The expression.
     */
    private final Expression _expression;
    /**
     * The close parentheses token.
     */
    private final Token _closeParenthesesToken;

    /**
     * Constructor for ParenthesesExpression class.
     *
     * @param openParenthesesToken  The open parentheses token.
     * @param expression            The expression.
     * @param closeParenthesesToken The close parentheses token.
     */
    public ParenthesesExpression(Token openParenthesesToken, Expression expression, Token closeParenthesesToken) {
        super(Expression.Type.PARENTHESES);
        _openParenthesesToken = openParenthesesToken;
        _expression = expression;
        _closeParenthesesToken = closeParenthesesToken;
    }

    /**
     * Get the open parentheses token.
     * 
     * @return The open parentheses token.
     */
    public Token getOpenParenthesesToken() {
        return _openParenthesesToken;
    }

    /**
     * Get the expression.
     * 
     * @return The expression.
     */
    public Expression getExpression() {
        return _expression;
    }

    /**
     * Get the close parentheses token.
     * 
     * @return The close parentheses token.
     */
    public Token getCloseParenthesesToken() {
        return _closeParenthesesToken;
    }
}
