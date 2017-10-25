enum Direction {
    N, S, W, E, NE, NW, SE, SW, INVALID_DIRECTION
};

/* error codes for Question 3 */
#define ERROR_START_STATION_INVALID -1
#define ERROR_ROUTE_ENDPOINT_IS_NOT_STATION -2
#define ERROR_LINE_HOPPING_BETWEEN_STATIONS -3
#define ERROR_BACKTRACKING_BETWEEN_STATIONS -4
#define ERROR_INVALID_DIRECTION -5
#define ERROR_OFF_TRACK -6
#define ERROR_OUT_OF_BOUNDS -7


/* pre-supplied function to load a tube map from a file*/
char **load_map(const char *filename, int &height, int &width);

/* pre-supplied function to print the tube map */
void print_map(char **m, int height, int width);

/* pre-supplied helper function to describe errors for Question 3 */
const char *error_description(int code);

/* pre-supplied helper function for converting string to Direction enum */
Direction string_to_direction(const char *token);

/*self-defined function for get symbol position */
bool get_symbol_position(char **map, int height, int width, char target, int &r, int &c);

/*self-defined function for get the symbol of a station or line */
char *get_symbol_for_station_or_line(const char name[]);

/*self-defined function for check if the route is validate or not, if not return error information else return Direction */
int validate_route(char **map, int height, int width, char start_station[], char route[],
                   char destination[]);

/*self-defined function for check if the destination is a station or not and if yes, change the variable destination to the destination station name */
bool is_station(char **map, int r, int c, char destination[]);

/*self-defined function for check if the train is currently in a line or not */
bool is_line(char **map, int r, int c);

/*self-defined function for check if the next step destination is out of bound or track */
int check_destination(char **map, int height, int width, int r, int c) ;