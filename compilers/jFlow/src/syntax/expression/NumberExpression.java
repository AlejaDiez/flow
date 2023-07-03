package syntax.expression;

import syntax.Token;

/**
 * Number expression class.
 */
public final class NumberExpression extends Expression {
    /**
     * The token of the expression.
     */
    private final Token _token;

    /**
     * Constructor for NumberExpression class.
     *
     * @param token The token of the expression.
     */
    public NumberExpression(Token token) {
        super(Expression.Type.NUMBER);
        _token = token;
    }

    /**
     * Get the token of the expression.
     *
     * @return The token of the expression.
     */
    public Token getToken() {
        return _token;
    }
}
