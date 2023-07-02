package syntax.expression;

import java.util.List;
import java.util.LinkedList;

import syntax.Token;

public final class TreeExpression {
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
     * @param root The root of the expression tree.
     */
    public TreeExpression(Expression root, Token endToken, List<String> diagnostics) {
        _root = root;
        _endToken = endToken;
        _diagnostics = diagnostics;
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
     * Get a copy of the diagnostics.
     * 
     * @return A copy of the diagnostics.
     */
    public List<String> getDiagnostics() {
        return new LinkedList<String>(_diagnostics);
    }

    /**
     * Evaluate one expression.
     * 
     * @param expression The expression to evaluate.
     * @return The result of the expression.
    */
    private int _evaluateExpression(Expression expression) {
        if (expression.getType() == Expression.Type.NUMBER) {
            return (Integer) expression.getToken().getValue();
        } else if (expression.getType() == Expression.Type.UNARY) {
            throw new RuntimeException("Unary expressions are not yet supported.");
        } else if (expression.getType() == Expression.Type.BINARY) {
            int leftNumber = _evaluateExpression((((BinaryExpression) expression).getLeft()));
            int rightNumber = _evaluateExpression((((BinaryExpression) expression).getRight()));

            if (expression.getToken().getType() == Token.Type.PLUS) {
                return leftNumber + rightNumber;
            } else if (expression.getToken().getType() == Token.Type.MINUS) {
                return leftNumber - rightNumber;
            } else if (expression.getToken().getType() == Token.Type.MULTIPLY) {
                return leftNumber * rightNumber;
            } else if (expression.getToken().getType() == Token.Type.DIVIDE) {
                return leftNumber / rightNumber;
            } else {
                throw new RuntimeException("Invalid binary operator.");
            }
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
     */
    private String _expressionToString(Expression expression, String indent, boolean isLast) {
        String tree;
        final String marker = isLast ? "└── " : "├── ";

        tree = indent + marker + expression.getType();
        if (expression.getToken().getValue() != null) {
            tree += " " + expression.getToken().getValue();
        }
        indent += isLast ? "    " : "│   ";
        if (expression.getType() == Expression.Type.BINARY) {
            final BinaryExpression binaryExpression = (BinaryExpression) expression;

            tree += "\n" + _expressionToString(binaryExpression.getLeft(), indent, false);
            tree += "\n" + _expressionToString(binaryExpression.getRight(), indent, true);
        }
        return tree;
    }

    @Override
    public String toString() {
        return _expressionToString(_root, "", true);
    }
}
