import java.io.File;

import syntax.Parser;

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
                parser.parse();
            }
        } catch (Exception e) {
            System.err.println(e.getMessage());
        }
    }
}
