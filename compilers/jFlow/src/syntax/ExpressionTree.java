package syntax;

import java.util.List;

import syntax.expression.BinaryExpression;
import syntax.expression.Expression;
import syntax.expression.LiteralExpression;
import syntax.expression.ParenthesesExpression;

/**
 * Expression tree class.
 */
public final class ExpressionTree {
    /**
     * The root of the expression tree.
     */
    private final Expression _root;
    /**
     * The end of file token.
     */
    private final Token _endToken;

    /**
     * The list of diagnostics.
     */
    private final List<String> _diagnostics;

    /**
     * Constructor for TreeExpression class.
     * 
     * @param root        The root of the expression tree.
     * @param endToken    The end of file token.
     * @param diagnostics The list of diagnostics.
     */
    public ExpressionTree(Expression root, Token endToken, List<String> diagnostics) {
        _root = root;
        _endToken = endToken;
        _diagnostics = diagnostics;
    }

    /**
     * Parse the input string into an expression tree.
     * 
     * @param input The input string to be parsed.
     * @return The expression tree.
     */
    public static ExpressionTree parse(String input) {
        return new Parser(input).parse();
    }

    /**
     * Get the root of the expression tree.
     * 
     * @return The root of the expression tree.
     */
    public Expression getRoot() {
        return _root;
    }

    /**
     * Get the end of file token.
     * 
     * @return The end of file token.
     */
    public Token getEndToken() {
        return _endToken;
    }

    /**
     * Get a copy of the diagnostics.
     * 
     * @return A copy of the diagnostics.
     */
    public List<String> getDiagnostics() {
        return List.copyOf(_diagnostics);
    }

    /**
     * Evaluate one expression.
     * 
     * @param expression The expression to evaluate.
     * @return The result of the expression.
     */
    private int _evaluateExpression(Expression expression) {
        if (expression.getType() == Expression.Type.LITERAL) {
            final LiteralExpression literalExpression = (LiteralExpression) expression;

            return (Integer) literalExpression.getToken().getValue();
        } else if (expression.getType() == Expression.Type.UNARY) {
            throw new RuntimeException("Unary expressions are not yet supported.");
        } else if (expression.getType() == Expression.Type.BINARY) {
            final BinaryExpression binaryExpression = (BinaryExpression) expression;
            final int left = _evaluateExpression((binaryExpression.getLeft()));
            final int right = _evaluateExpression((binaryExpression.getRight()));

            if (binaryExpression.getOperator().getType() == Token.Type.PLUS) {
                return left + right;
            } else if (binaryExpression.getOperator().getType() == Token.Type.MINUS) {
                return left - right;
            } else if (binaryExpression.getOperator().getType() == Token.Type.MULTIPLY) {
                return left * right;
            } else if (binaryExpression.getOperator().getType() == Token.Type.DIVIDE) {
                return left / right;
            } else {
                throw new RuntimeException("Invalid binary operator.");
            }
        } else if (expression.getType() == Expression.Type.PARENTHESES) {
            final ParenthesesExpression parenthesesExpression = (ParenthesesExpression) expression;

            return _evaluateExpression(parenthesesExpression.getExpression());
        } else {
            throw new RuntimeException("Invalid expression type.");
        }
    }

    /**
     * Evaluate the expression tree.
     * 
     * @return The result of the expression.
     */
    public int evaluate() {
        return _evaluateExpression(_root);
    }

    /**
     * Convert the expression tree to a string.
     * 
     * @param expression The expression to convert.
     * @param indent     The current indentation.
     * @param isLast     Whether the expression is the last in the list.
     * @return The expression tree as a string.
     */
    private String _expressionToString(Expression expression, String indent, Boolean isLast) {
        final StringBuilder tree = new StringBuilder();
        final String marker = isLast == null ? "" : (isLast ? "└── " : "├── ");

        tree.append(indent).append(marker).append(expression.getType());

        if (expression.getType() == Expression.Type.LITERAL) {
            final LiteralExpression literalExpression = (LiteralExpression) expression;

            tree.append(" ").append(literalExpression.getToken().getValue());
        } else if (expression.getType() == Expression.Type.UNARY) {
            throw new UnsupportedOperationException("Unary expressions are not yet supported.");
        } else if (expression.getType() == Expression.Type.BINARY) {
            final BinaryExpression binaryExpression = (BinaryExpression) expression;
            final String childIndent = indent + (isLast == null ? "" : (isLast ? "    " : "│   "));

            tree.append("\n")
                    .append(_expressionToString(binaryExpression.getLeft(), childIndent, false))
                    .append("\n")
                    .append(childIndent).append("├── ").append(binaryExpression.getOperator().getType())
                    .append("\n")
                    .append(_expressionToString(binaryExpression.getRight(), childIndent, true));
        } else if (expression.getType() == Expression.Type.PARENTHESES) {
            final ParenthesesExpression parenthesesExpression = (ParenthesesExpression) expression;
            final String childIndent = indent + (isLast == null ? "" : (isLast ? "    " : "│   "));

            tree.append("\n")
                    .append(childIndent).append("├── ")
                    .append(parenthesesExpression.getOpenParenthesesToken().getType())
                    .append("\n")
                    .append(_expressionToString(parenthesesExpression.getExpression(), childIndent, false))
                    .append("\n")
                    .append(childIndent).append("└── ")
                    .append(parenthesesExpression.getCloseParenthesesToken().getType());
        } else {
            throw new IllegalArgumentException("Invalid expression type.");
        }
        return tree.toString();
    }

    @Override
    public String toString() {
        return _expressionToString(_root, "", null);
    }
}
