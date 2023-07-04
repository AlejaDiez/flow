package syntax.expression;

import syntax.Token;

/**
 * Literal expression class.
 */
public final class LiteralExpression extends Expression {
    /**
     * The token of the expression.
     */
    private final Token _token;

    /**
     * Constructor for LiteralExpression class.
     *
     * @param token The token of the expression.
     */
    public LiteralExpression(Token token) {
        super(Expression.Type.LITERAL);
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
