import utils.Colors;

public class jFlow {
    public static void main(String[] args) {
        System.out.println("    ________    ____ _       __");
        System.out.println("   / ____/ /   / __ \\ |     / /");
        System.out.println("  / /_  / /   / / / / | /| / / ");
        System.out.println(" / __/ / /___/ /_/ /| |/ |/ /  ");
        System.out.println("/_/   /_____/\\____/ |__/|__/   ");
        System.out.println();
        System.out.println();

        while (true) {
            String input = "";

            // Read input from the user
            System.out.print(Colors.green(">>>") + " ");
            input = System.console().readLine();

            // TODO: Parse the input

            // TODO: Print syntax tree

            // TODO: Print diagnostics

            // Print the result
            System.out.println(input);
            System.out.println();
        }
    }
}
