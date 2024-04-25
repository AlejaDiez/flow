package syntax;

import java.io.File;
import java.util.ArrayList;
import java.util.List;

import exception.FileNotFoundException;
import exception.InvalidExtensionException;
import exception.SyntaxErrorException;
import syntax.Token.TokenType;
import syntax.expression.BinaryExpression;
import syntax.expression.Expression;
import syntax.expression.LiteralExpression;

/**
 * Represents a parser for the input code.
 */
public final class Parser {
    private final Lexer _lexer;
    private final List<Token> _tokens;
    private int _index;

    /**
     * Constructor for the Parser class.
     * 
     * @param lexer The lexer to be used.
     */
    private Parser(Lexer lexer) throws SyntaxErrorException {
        this._lexer = lexer;
        this._tokens = new ArrayList<Token>();
        this._index = 0;

        while (true) {
            final Token token = this._lexer.scan();

            this._tokens.add(token);
            if (token.type == TokenType.EOF) {
                break;
            }
        }
    }

    /**
     * Constructor for the Parser class.
     * 
     * @param command The input string to be analyzed.
     * @throws SyntaxErrorException
     */
    public Parser(String command) throws SyntaxErrorException {
        this(new Lexer(command));
    }

    /**
     * Constructor for the Parser class.
     * 
     * @param file The input file to be analyzed.
     * @throws SyntaxErrorException
     */
    public Parser(File file) throws FileNotFoundException, InvalidExtensionException, SyntaxErrorException {
        this(new Lexer(file));
    }

    /**
     * Get one token ahead of the current position.
     * 
     * @param offset The offset from the current position.
     * @return The token at the offset.
     */
    private Token _peek(int offset) {
        final int index = Math.max(0, Math.min(_tokens.size() - 1, _index + offset));

        return _tokens.get(index);
    }

    /**
     * Get the current token and advance the position.
     * 
     * @return The current token.
     */
    private Token _next() {
        final Token token = _peek(0);

        _index += 1;
        return token;
    }

    /**
     * Match the current token type with the expected token type.
     * 
     * @param types The expected token types.
     * @return The current token.
     * @throws SyntaxErrorException
     */
    private Token _match(TokenType... types) throws SyntaxErrorException {
        final Token token = _peek(0);

        for (final TokenType type : types) {
            if (token.type == type) {
                return _next();
            }
        }
        throw new SyntaxErrorException(String.format("expected %s, got %s", List.of(types), token.type), this._lexer,
                token.line,
                token.column);
    }

    /**
     * Parses the primary expression.
     * 
     * @throws SyntaxErrorException If the token is not an integer.
     * @return The parsed expression.
     */
    private Expression _parsePrimary() throws SyntaxErrorException {
        // Parse the integer expression.
        final Token token = _match(TokenType.INTEGER);

        return new LiteralExpression(token);
    }

    /**
     * Parses the binary expression.
     * 
     * @param precedence The precedence of the previous token.
     * @return The parsed expression.
     * @throws SyntaxErrorException If the token is not a binary operator.
     */
    private Expression _parseBinary(int precedence) throws SyntaxErrorException {
        Expression expression = _parsePrimary();
        Token token = _peek(0);

        while (token.type != TokenType.EOF) {
            // Check if the token is a binary operator.
            if (token.type.precedence == 0) {
                throw new SyntaxErrorException("expected binary operator", this._lexer, token.line, token.column);
            } else if (token.type.precedence <= precedence) {
                break;
            }
            // Advance the token.
            token = _next();

            // Parse the next expression.
            final Expression nextExpression = _parseBinary(token.type.precedence);

            // Create a new binary expression.
            expression = new BinaryExpression(expression, token, nextExpression);
            // Update the token.
            token = _peek(0);
        }
        return expression;
    }

    /**
     * Parses the input code.
     * 
     * @throws ExpectedTypeErrorException If the expected type does not match the
     *                                    actual type.
     * @return The parsed expression.
     * @throws IOException
     */
    public Expression parse() throws SyntaxErrorException {
        return _parseBinary(0);
    }
}
