package syntax.expression;

import syntax.Token;
import syntax.Token.TokenType;

public class LiteralExpression extends Expression {
    /**
     * The token of the expression.
     */
    public final Token literal;

    /**
     * Creates a new literal expression.
     *
     * @param literal The token of the expression.
     */
    public LiteralExpression(Token literal) {
        super(ExpressionType.LITERAL);

        if (literal.type != TokenType.INTEGER) {
            throw new IllegalArgumentException("The token must be an integer.");
        }
        this.literal = literal;
    }

    @Override
    public int evaluate() {
        return (Integer) literal.value;
    }

    @Override
    public String toString() {
        return String.format(
                "LiteralExpression(%s)",
                literal.value.toString());
    }
}
