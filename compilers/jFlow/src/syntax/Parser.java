package syntax;

import java.util.List;
import java.util.LinkedList;

import syntax.expression.BinaryExpression;
import syntax.expression.Expression;
import syntax.expression.NumberExpression;
import syntax.expression.TreeExpression;
import utils.Colors;

public final class Parser {
    /**
     * Array of tokens to parse.
     */
    private final Token[] _tokens;
    /**
     * The current position in the tokens array.
     */
    private int _position;
    /**
     * The list of diagnostics.
     */
    private List<String> _diagnostics;

    /**
     * Constructor for Parser class.
     *
     * @param input The input string to be parsed.
     */
    public Parser(String input) {
        final Lexer lexer = new Lexer(input);
        List<Token> tokens = new LinkedList<Token>();

        while (lexer.hasNext()) {
            Token token = lexer.nextToken();

            if (token.getType() != Token.Type.WHITESPACE && token.getType() != Token.Type.UNKNOWN) {
                tokens.add(token);
            }
        }
        _tokens = tokens.toArray(new Token[tokens.size()]);
        _position = 0;
        _diagnostics = lexer.getDiagnostics();
    }

    /**
     * Get one token ahead of the current position.
     * 
     * @param offset The offset from the current position.
     * @return The token at the offset.
     */
    private Token _peekToken(int offset) {
        int index = _position + offset;

        if (index >= _tokens.length) {
            index = _tokens.length - 1;
        } else if (index < 0) {
            index = 0;
        }
        return _tokens[index];
    }

    /**
     * Get the current token and advance the position.
     * 
     * @return The current token.
     */
    private Token _nextToken() {
        final Token current = _peekToken(0);

        _position++;
        return current;
    }

    /**
     * Match the current token with the given type.
     * 
     * @param type The type to match.
     * @return The current token if it matches, otherwise a new token with the given
     *         type.
     */
    private Token _match(Token.Type type) {
        final Token currentToken = _peekToken(0);

        if (currentToken.getType() != type) {
            _diagnostics.add(Colors.red("ERROR:") + " expected <" + type + "> but got <"
                    + currentToken.getType() + "> at line " + currentToken.getLine() + ", column "
                    + currentToken.getColumn() + ".");
            return new Token(type, null, currentToken.getLine(), currentToken.getColumn());
        }
        return _nextToken();
    }

    /**
     * Parse a number expression.
     * 
     * @return The number expression.
     */
    private NumberExpression _parseNumberExpression() {
        final Token token = _match(Token.Type.NUMBER);

        return new NumberExpression(token);
    }

    /**
     * Parse a factor expression.
     * 
     * @return The factor expression.
     */
    private Expression _parseFactorExpression() {
        Expression expression = _parseNumberExpression();

        while (_peekToken(0).getType() == Token.Type.MULTIPLY || _peekToken(0).getType() == Token.Type.DIVIDE) {
            final Token token = _nextToken();
            final Expression nextExpression = _parseNumberExpression();

            expression = new BinaryExpression(expression, token, nextExpression);
        }
        return expression;
    }

    /**
     * Parse a term expression.
     * 
     * @return The term expression.
     */
    private Expression _parseTermExpression() {
        Expression expression = _parseFactorExpression();

        while (_peekToken(0).getType() == Token.Type.PLUS || _peekToken(0).getType() == Token.Type.MINUS) {
            final Token token = _nextToken();
            final Expression nextExpression = _parseFactorExpression();

            expression = new BinaryExpression(expression, token, nextExpression);
        }
        return expression;
    }

    /**
     * Parse the input string.
     * 
     * @return The parsed expression.
     */
    public TreeExpression parse() {
        final Expression expression = _parseTermExpression();
        final Token endToken = _match(Token.Type.EOF);
        final List<String> diagnostics = new LinkedList<String>(_diagnostics);

        return new TreeExpression(expression, endToken, diagnostics);
    }
}
