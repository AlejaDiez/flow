package syntax.expression;

import syntax.Token;

/**
 * Number expression class.
 */
public final class NumberExpression extends Expression {
    /**
     * Constructor for NumberExpression class.
     *
     * @param token The token of the expression.
     */
    public NumberExpression(Token token) {
        super(Expression.Type.NUMBER, token);
    }
}
