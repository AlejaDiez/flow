import java.io.File;

import exception.FileNotFoundException;
import exception.InvalidExtensionException;
import exception.SyntaxErrorException;
import syntax.Parser;
import syntax.expression.Expression;

public class Flow {
    public static void main(String[] arg) {
        final String[] args = { "test/input.flow" };
        // Print the welcome message
        System.out.print("\033[H\033[2J");
        System.out.println("    ________    ____ _       __");
        System.out.println("   / ____/ /   / __ \\ |     / /");
        System.out.println("  / /_  / /   / / / / | /| / / ");
        System.out.println(" / __/ / /___/ /_/ /| |/ |/ /  ");
        System.out.println("/_/   /_____/\\____/ |__/|__/ 0.0");
        System.out.println();
        System.out.println();

        // Start compiling the code
        try {
            if (args.length == 0) {
                // Read the input from the console
                while (true) {
                    System.out.print("\033[32m>>>\033[0m ");

                    final String input = System.console().readLine();
                }

            } else {
                // Read the input from the file
                final Parser parser = new Parser(new File(args[0]));
                final Expression ex = parser.parse();

                System.out.println(ex.evaluate());
            }
        } catch (FileNotFoundException e) {
            System.err.println(e.getMessage());
        } catch (InvalidExtensionException e) {
            System.err.println(e.getMessage());
        } catch (SyntaxErrorException e) {
            System.err.println(e.getMessage());
        }
    }
}
