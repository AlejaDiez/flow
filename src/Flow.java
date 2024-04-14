public class Flow {
    public static void main(String[] args) {
        // Print the welcome message
        System.out.print("\033[H\033[2J");
        System.out.println("    ________    ____ _       __");
        System.out.println("   / ____/ /   / __ \\ |     / /");
        System.out.println("  / /_  / /   / / / / | /| / / ");
        System.out.println(" / __/ / /___/ /_/ /| |/ |/ /  ");
        System.out.println("/_/   /_____/\\____/ |__/|__/   ");
        System.out.println();
        System.out.println();

        // Main loop
        while (true) {
            String input = "";

            // Read input from the user
            System.out.print("\033[32m>>>\033[0m ");
            input = System.console().readLine();
        }
    }
}
