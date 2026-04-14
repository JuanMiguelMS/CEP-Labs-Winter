/*
 * departure_board.c
 * Simulates a TTC-style transit departure board with live countdown.
 * Reads route data from routes.txt and updates every second.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* Maximum number of routes and route name length */
#define MAX_ROUTES     64
#define MAX_NAME_LEN   64

/* ANSI escape codes */
#define ANSI_CLEAR     "\033[2J\033[H"   /* Clear screen and move cursor to top-left */
#define ANSI_RESET     "\033[0m"
#define ANSI_RED_BG    "\033[41m"        /* Red background (TTC style) */
#define ANSI_WHITE     "\033[97m"        /* Bright white text */
#define ANSI_BOLD      "\033[1m"
#define ANSI_RED_FG    "\033[91m"        /* Bright red foreground */
#define ANSI_YELLOW    "\033[93m"        /* Yellow for imminent arrivals */
#define ANSI_DIM       "\033[2m"

/* Struct representing a single transit route */
typedef struct {
    int  route_num;               /* Route number, e.g. 501 */
    char name[MAX_NAME_LEN];      /* Route name, e.g. "Queen" */
    int  initial_minutes;         /* Minutes until arrival when first loaded */
    int  seconds_remaining;       /* Live countdown in seconds */
} Route;

/*
 * load_routes()
 * Reads route data from the given file path into the routes array.
 * Each line in the file must be: <route_num> <name> <minutes>
 * Returns the number of routes successfully loaded.
 */
int load_routes(const char *filepath, Route routes[], int max_routes) {
    FILE *fp = fopen(filepath, "r");
    if (!fp) {
        fprintf(stderr, "Error: could not open '%s'\n", filepath);
        return 0;
    }

    int count = 0;
    while (count < max_routes) {
        int num, mins;
        char name[MAX_NAME_LEN];

        /* Parse one line: route_num name minutes */
        if (fscanf(fp, "%d %63s %d", &num, name, &mins) != 3)
            break;

        routes[count].route_num       = num;
        strncpy(routes[count].name, name, MAX_NAME_LEN - 1);
        routes[count].name[MAX_NAME_LEN - 1] = '\0';
        routes[count].initial_minutes = mins;
        routes[count].seconds_remaining = mins * 60; /* convert to seconds */

        count++;
    }

    fclose(fp);
    return count;
}

/*
 * print_header()
 * Prints the TTC-style red header banner.
 */
void print_header(void) {
    printf("%s%s%s", ANSI_RED_BG, ANSI_WHITE, ANSI_BOLD);
    printf("  %-8s %-18s %-30s%s\n",
           "ROUTE", "NAME", "STATUS", ANSI_RESET);
    printf("%s%s", ANSI_RED_BG, ANSI_WHITE);
    printf("  %-8s %-18s %-30s%s\n",
           "-----", "----", "------", ANSI_RESET);
}

/*
 * print_route()
 * Prints a single route row with its countdown.
 * Routes arriving in <= 1 minute are highlighted yellow.
 * Routes that have departed are shown dimmed.
 */
void print_route(const Route *r) {
    int mins = r->seconds_remaining / 60;
    int secs = r->seconds_remaining % 60;

    if (r->seconds_remaining <= 0) {
        /* Route has departed */
        printf("%s  %-8d %-18s %-30s%s\n",
               ANSI_DIM,
               r->route_num, r->name,
               "** DEPARTED **",
               ANSI_RESET);
    } else if (r->seconds_remaining <= 60) {
        /* Arriving very soon — highlight in yellow */
        printf("%s%s  %-8d %-18s ",
               ANSI_YELLOW, ANSI_BOLD,
               r->route_num, r->name);
        printf("Arriving in %d:%02d  <-- NOW%s\n", mins, secs, ANSI_RESET);
    } else {
        /* Normal countdown display */
        printf("%s  %-8d %-18s ",
               ANSI_RED_FG, r->route_num, r->name);
        printf("Arriving in %d min %02d sec%s\n", mins, secs, ANSI_RESET);
    }
}

/*
 * print_board()
 * Clears the terminal and redraws the entire departure board.
 */
void print_board(const Route routes[], int count) {
    /* Clear screen and move cursor to home position */
    printf("%s", ANSI_CLEAR);

    /* Title bar */
    printf("%s%s%s", ANSI_RED_BG, ANSI_WHITE, ANSI_BOLD);
    printf("         TTC TRANSIT DEPARTURE BOARD         %s\n", ANSI_RESET);
    printf("\n");

    print_header();

    /* Print each route row */
    for (int i = 0; i < count; i++) {
        print_route(&routes[i]);
    }

    printf("\n%s  Press Ctrl+C to exit.%s\n", ANSI_DIM, ANSI_RESET);
    fflush(stdout);
}

/*
 * tick()
 * Decrements the countdown for each route by one second.
 * Clamps at zero (departed).
 */
void tick(Route routes[], int count) {
    for (int i = 0; i < count; i++) {
        if (routes[i].seconds_remaining > 0)
            routes[i].seconds_remaining--;
    }
}

/* ------------------------------------------------------------------ */

int main(void) {
    Route routes[MAX_ROUTES];

    /* Load routes from file */
    int count = load_routes("routes.txt", routes, MAX_ROUTES);
    if (count == 0) {
        fprintf(stderr, "No routes loaded. Exiting.\n");
        return EXIT_FAILURE;
    }

    /* Main loop: redraw board, wait one second, tick countdown */
    while (1) {
        print_board(routes, count);
        sleep(1);
        tick(routes, count);
    }

    return EXIT_SUCCESS;
}
