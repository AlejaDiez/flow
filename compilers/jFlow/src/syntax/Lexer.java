package syntax;

import java.util.List;
import java.util.LinkedList;

import utils.Colors;

/**
 * Lexer class.
 */
final class Lexer {
    /**
     * The input string to be tokenized.
     */
    private final String _input;
    /**
     * The current position in the input string.
     */
    private int _position;
    /**
     * The current character in the input string.
     */
    private char _character;
    /**
     * The list of diagnostics.
     */
    private List<String> _diagnostics;

    /**
     * Constructor for Lexer class.
     *
     * @param input The input string to be tokenized.
     */
    public Lexer(String input) {
        _input = input;
        _position = 0;
        _character = _input.length() > 0 ? _input.charAt(_position) : '\0';
        _diagnostics = new LinkedList<String>();

        if (input.length() == 0) {
            _diagnostics.add(Colors.red("ERROR:") + " Input string cannot be empty.");
        }
    }

    /**
     * Advance the current position in the input string.
     */
    private void _advance() {
        _position++;
        _character = _position < _input.length() ? _input.charAt(_position) : '\0';
    }

    /**
     * Analyze the number.
     * 
     * @return The token representing the number.
     */
    private Token _analyzeNumber() {
        final Token.Type type = Token.Type.NUMBER;
        String value = "";
        final int line = 0;
        final int column = _position;

        while (type.equals(_character)) {
            value += _character;
            _advance();
        }
        try {
            return new Token(type, Integer.parseInt(value), line, column);
        } catch (NumberFormatException e) {
            _diagnostics.add(Colors.red("ERROR:") + " Invalid number '" + value + "' at line " + line + ", column "
                    + column + ".");
            return new Token(Token.Type.UNKNOWN, value, line, column);
        }
    }

    /**
     * Analyze the plus operator.
     * 
     * @return The token representing the plus operator.
     */
    private Token _analyzePlusOperator() {
        final Token.Type type = Token.Type.PLUS;
        final char value = _character;
        final int line = 0;
        final int column = _position;

        _advance();
        return new Token(type, value, line, column);
    }

    /**
     * Analyze the minus operator.
     * 
     * @return The token representing the minus operator.
     */
    private Token _analyzeMinusOperator() {
        final Token.Type type = Token.Type.MINUS;
        final char value = _character;
        final int line = 0;
        final int column = _position;

        _advance();
        return new Token(type, value, line, column);
    }

    /**
     * Analyze the multiply operator.
     * 
     * @return The token representing the multiply operator.
     */
    private Token _analyzeMultiplyOperator() {
        final Token.Type type = Token.Type.MULTIPLY;
        final char value = _character;
        final int line = 0;
        final int column = _position;

        _advance();
        return new Token(type, value, line, column);
    }

    /**
     * Analyze the divide operator.
     * 
     * @return The token representing the divide operator.
     */
    private Token _analyzeDivideOperator() {
        final Token.Type type = Token.Type.DIVIDE;
        final char value = _character;
        final int line = 0;
        final int column = _position;

        _advance();
        return new Token(type, value, line, column);
    }

    /**
     * Analyze the left parenthesis.
     * 
     * @return The token representing the left parenthesis.
     */
    private Token _analyzeLeftParenthesis() {
        final Token.Type type = Token.Type.LPAREN;
        final char value = _character;
        final int line = 0;
        final int column = _position;

        _advance();
        return new Token(type, value, line, column);
    }

    /**
     * Analyze the right parenthesis.
     * 
     * @return The token representing the right parenthesis.
     */
    private Token _analyzeRightParenthesis() {
        final Token.Type type = Token.Type.RPAREN;
        final char value = _character;
        final int line = 0;
        final int column = _position;

        _advance();
        return new Token(type, value, line, column);
    }

    /**
     * Analyze the whitespace.
     * 
     * @return The token representing the whitespace.
     */
    private Token _analyzeWhitespace() {
        final Token.Type type = Token.Type.WHITESPACE;
        String value = "";
        final int line = 0;
        final int column = _position;

        while (type.equals(_character)) {
            value += _character;
            _advance();
        }
        return new Token(type, value, line, column);
    }

    /**
     * Analyze the end of file.
     * 
     * @return The token representing the end of file.
     */
    private Token _analyzeEndOfFile() {
        final Token.Type type = Token.Type.EOF;
        final char value = _character;
        final int line = 0;
        final int column = _position;

        _advance();
        return new Token(type, value, line, column);
    }

    /**
     * Analyze the unknown character.
     * 
     * @return The token representing the unknown character.
     */
    private Token _analyzeUnknownCharacter() {
        final Token.Type type = Token.Type.UNKNOWN;
        final char value = _character;
        final int line = 0;
        final int column = _position;

        _diagnostics.add(Colors.red("ERROR:") + " unknown character '" + value + "' at line " + line + ", column "
                + column + ".");
        _advance();
        return new Token(type, value, line, column);
    }

    /**
     * Get the next token in the input string.
     *
     * @return The next token in the input string.
     */
    public Token nextToken() {
        if (Token.Type.NUMBER.equals(_character)) {
            return _analyzeNumber();
        } else if (Token.Type.PLUS.equals(_character)) {
            return _analyzePlusOperator();
        } else if (Token.Type.MINUS.equals(_character)) {
            return _analyzeMinusOperator();
        } else if (Token.Type.MULTIPLY.equals(_character)) {
            return _analyzeMultiplyOperator();
        } else if (Token.Type.DIVIDE.equals(_character)) {
            return _analyzeDivideOperator();
        } else if (Token.Type.LPAREN.equals(_character)) {
            return _analyzeLeftParenthesis();
        } else if (Token.Type.RPAREN.equals(_character)) {
            return _analyzeRightParenthesis();
        } else if (Token.Type.WHITESPACE.equals(_character)) {
            return _analyzeWhitespace();
        } else if (Token.Type.EOF.equals(_character)) {
            return _analyzeEndOfFile();
        } else {
            return _analyzeUnknownCharacter();
        }
    }

    /**
     * Check if there is a next token in the input string.
     *
     * @return True if there is a next token in the input string, false otherwise.
     */
    public boolean hasNext() {
        return _position <= _input.length();
    }

    /**
     * Get a copy of the diagnostics.
     *
     * @return A copy of the diagnostics.
     */
    public List<String> getDiagnostics() {
        return List.copyOf(_diagnostics);
    }
}
