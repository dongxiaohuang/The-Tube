#include <iostream>
#include <iomanip>
#include <fstream>
#include <cassert>
#include <cstring>
#include <sstream>
#include "tube.h"
using namespace std;
int MAX_LEN = 80;

/* You are pre-supplied with the functions below. Add your own
	 function definitions to the end of this file. */

/* internal helper function which allocates a dynamic 2D array */
char **allocate_2D_array(int rows, int columns) {
    char **m = new char *[rows];
    assert(m);
    for (int r = 0; r < rows; r++) {
        m[r] = new char[columns];
        assert(m[r]);
    }
    return m;
}

/* internal helper function which deallocates a dynamic 2D array */
void deallocate_2D_array(char **m, int rows) {
    for (int r = 0; r < rows; r++)
        delete[] m[r];
    delete[] m;
}

/* internal helper function which gets the dimensions of a map */
bool get_map_dimensions(const char *filename, int &height, int &width) {
    char line[512];

    ifstream input(filename);

    height = width = 0;

    input.getline(line, 512);
    while (input) {
        if ((int) strlen(line) > width)
            width = strlen(line);
        height++;
        input.getline(line, 512);
    }

    if (height > 0)
        return true;
    return false;
}

/* pre-supplied function to load a tube map from a file*/
char **load_map(const char *filename, int &height, int &width) {

    bool success = get_map_dimensions(filename, height, width);

    if (!success) return NULL;

    char **m = allocate_2D_array(height, width);

    ifstream input(filename);

    char line[512];
    char space[] = " ";

    for (int r = 0; r < height; r++) {
        input.getline(line, 512);
        strcpy(m[r], line);
        while ((int) strlen(m[r]) < width) {
            strcat(m[r], space);
        }
    }

    return m;
}

/* pre-supplied function to print the tube map */
void print_map(char **m, int height, int width) {
    cout << setw(2) << " " << " ";
    for (int c = 0; c < width; c++) {
        if (c && (c % 10) == 0) {
            cout << c / 10;
        } else {
            cout << " ";
        }
    }

    cout << endl;
    cout << setw(2) << " " << " ";

    for (int c = 0; c < width; c++) cout << (c % 10);

    cout << endl;

    for (int r = 0; r < height; r++) {
        cout << setw(2) << r << " ";
        for (int c = 0; c < width; c++) cout << m[r][c];
        cout << endl;
    }
}

/* pre-supplied helper function to report the errors encountered in Question 3 */
const char *error_description(int code) {
    switch (code) {
        case ERROR_START_STATION_INVALID:
            return "Start station invalid";
        case ERROR_ROUTE_ENDPOINT_IS_NOT_STATION:
            return "Route endpoint is not a station";
        case ERROR_LINE_HOPPING_BETWEEN_STATIONS:
            return "Line hopping between stations not possible";
        case ERROR_BACKTRACKING_BETWEEN_STATIONS:
            return "Backtracking along line between stations not possible";
        case ERROR_INVALID_DIRECTION:
            return "Invalid direction";
        case ERROR_OFF_TRACK:
            return "Route goes off track";
        case ERROR_OUT_OF_BOUNDS:
            return "Route goes off map";
    }
    return "Unknown error";
}

/* presupplied helper function for converting string to direction enum */
Direction string_to_direction(const char *token) {
    const char *strings[] = {"N", "S", "W", "E", "NE", "NW", "SE", "SW"};
    for (int n = 0; n < 8; n++) {
        if (!strcmp(token, strings[n])) return (Direction) n;
    }
    return INVALID_DIRECTION;
}

/* self-defined function for get symbol position */
bool get_symbol_position(char **map, int height, int width, char target, int &r, int &c) {
    for (r = 0; r < height; r++) {
        for (c = 0; c < width; c++) {
            /* if found return true, and change r and c*/
            if (map[r][c] == target) return true;
        }
    }
    /* if not found. return false, r having the value -1 and c having the value -1 .*/
    r = -1;
    c = -1;
    return false;
}

/*self-defined function for get the symbol of a station or line, defined below */
char *get_symbol_for_station_or_line(const char name[]) {

    char line_station[MAX_LEN];
    char line_lines[MAX_LEN];
    /* read file from lines.txt to line_file and read file from stations.txt to station_file */
    ifstream station_file, line_file;
    station_file.open("stations.txt");
    line_file.open("lines.txt");
    if (station_file.fail()) {
        cout << " sorry we cannot open the file: stations.txt";
    }
    if (station_file.fail()) {
        cout << " sorry we cannot open the file: lines.txt";
    }
    /* defined two pointer to separate read line from input file to two part, the first part is symbol and the last part is name*/
    char *first_part;
    char *last_part;
    int isLine = 0; // check if the name is line or not, if it is line then it is no need to check the station file

    while (line_file.getline(line_lines, MAX_LEN)) // read line by line
    {
        last_part = line_lines + 2; // read line name
        /* get symbol for first part */
        first_part = line_lines;
        *(first_part + 1) = '\0'; // remain only the symbol

        if (!strcmp(name, last_part)) {
            isLine = 1;
            return first_part;
        }
    }
    if (!isLine) {
        while (station_file.getline(line_station, MAX_LEN))// read line by line
        {
            last_part = line_station + 2;// read station name
            /* get symbol for first part */
            first_part = line_station;
            *(first_part + 1) = '\0';// remain only the symbol

            if (!strcmp(name, last_part)) {
                return first_part;
            }
        }
    }
    /* if not found return space*/
    return (char *) " ";
}

/* SELF-DEFINED FUNCTION "check_destination"*/
int check_destination(char **map, int height, int width, int r, int c) {
    int bound_back, flag_not_out_front_bound = 0;
    /*CHECK BOUNDS*/
    /*CHECK IF OUT OF BACK BOUND*/
    if (r < 0 || r > height)
        return ERROR_OUT_OF_BOUNDS;
    for (int col_index = 0; col_index < width; col_index++) {
        if (map[r][col_index] != ' ') { bound_back = col_index; }
    }
    if (c > bound_back)
        return ERROR_OUT_OF_BOUNDS;
    /*CHECK IF OUT OF FONT BOUND*/
    for (int i = 0; i <= c; i++) {
        if (map[r][i] != ' ') {
            flag_not_out_front_bound = 1;
            break;
        }
    }
    if (!flag_not_out_front_bound)
        return ERROR_OUT_OF_BOUNDS;
    /*END CHECK BOUNDS*/

    /* CHECK IF OUT OF TRACK */
    if (map[r][c] == ' ') {
        return ERROR_OFF_TRACK;
    }

    return 1;
}

int validate_route(char **map, int height, int width, char start_station[], char route[], char destination[]) {

    int route_step, r, c, line_changed_count = 0;
    ifstream station_file("stations.txt");
    int right_star_station = 0;
    char station_line[MAX_LEN];
    char *station;
    char last_line = ' ';
    /* CHECK IT THE START STATION IS VALIFIED OR NOT*/
    while (station_file.getline(station_line, MAX_LEN)) {
        station = station_line + 2;
        if (!strcmp(station, start_station)) {
            right_star_station = 1;
            break;
        }
    }
    if (!right_star_station) {
        return ERROR_START_STATION_INVALID;
    }
    /* CHECK FINISHED */


    /* GET MAP COORDINATION OF START STATION */
    get_symbol_position(map, height, width, *get_symbol_for_station_or_line(start_station), r, c);

    /* MOVE AS ROUTE*/

    stringstream ss(route);
    string directions;
    Direction last_direction = INVALID_DIRECTION; // THIS IS TO CHECK IF THE ROUTE: BACKTRACKING BETWEEN STATIONS
    while (getline(ss, directions, ',')) { // READ THE ROUTE STRING AND SPLIT IT BY ',' INTO SEPARATED DIRECTION
        char *dir_ptr = &directions[0]; // POINT TO DIRECTION
        route_step = string_to_direction(dir_ptr);
        switch (route_step) {
            //N, S, W, E, NE, NW, SE, SW, INVALID_DIRECTION
            // MOVE STEP BY STEP BY ROUTE STEP
            case N: {
                int temp_r = r, temp_c = c;
                int i = check_destination(map, height, width, --temp_r, temp_c); // CHECK NEXT STEP IS VALID ROUTE OR NOT
                if (i != 1) // NEXT STEP IS NOT RIGHT
                    return i;
                // CHECK ERROR BACKTRACKING BETWEEN STATIONS; WE CAN ONLY TAKE REVERSED ROUTE WHEN WE ARE NOW IN A STATION OR WE CANNOT CHANGE TO REVERSED FIRECTION
                if (!is_station(map, r, c, destination)) {  // NOT IN A STATION NOW
                    if (last_direction == S)
                        return ERROR_BACKTRACKING_BETWEEN_STATIONS;
                }
                else
                {
                    if (last_direction == S)
                        line_changed_count++; // TAKE THE TRAIN CHANGE INTO CONSIDERATION
                }

                // CHECK END;

                r--; // STEP NEXT
                /* CHECK LINE CHANGED */
                if (is_line(map, r, c)) {
                    if (map[r][c] != last_line && last_line != ' ') {
                        line_changed_count = line_changed_count + 1; // LINE CHANGE AND LINE_CHANGED_COUNT ++
                        /* CHECK IF CHANGING LINES FROM STATION OR NOT */
                        if (!is_station(map, r + 1, c, destination))
                            return ERROR_LINE_HOPPING_BETWEEN_STATIONS;
                        /* CHECK FINISHED */
                    }

                    last_line = map[r][c];
                }

                last_direction = N;
                break;
            }
            case S: {
                int temp_r = r, temp_c = c;
                int i = check_destination(map, height, width, ++temp_r, temp_c);
                if (i != 1)
                    return i;
                if (!is_station(map, r, c, destination)) {
                    if (last_direction == N)
                        return ERROR_BACKTRACKING_BETWEEN_STATIONS;
                }
                else
                {
                    if (last_direction == N)
                        line_changed_count++; // TAKE THE TRAIN CHANGE INTO CONSIDERATION
                }
                r++;
                if (is_line(map, r, c)) {
                    if (map[r][c] != last_line && last_line != ' ') {
                        line_changed_count = line_changed_count + 1;
                        /* CHECK IF CHANGING LINES FROM STATION OR NOT */
                        if (!is_station(map, r - 1, c, destination))
                            return ERROR_LINE_HOPPING_BETWEEN_STATIONS;
                        /* CHECK FINISHED */
                    }

                    last_line = map[r][c];
                }

                last_direction = S;
                break;
            }
            case W: {
                int temp_r = r, temp_c = c;
                int i = check_destination(map, height, width, temp_r, --temp_c);
                if (i != 1)
                    return i;
                if (!is_station(map, r, c, destination)) {
                    if (last_direction == E)
                        return ERROR_BACKTRACKING_BETWEEN_STATIONS;
                }
                else
                {
                    if (last_direction == E)
                        line_changed_count++; // TAKE THE TRAIN CHANGE INTO CONSIDERATION
                }
                c--;
                if (is_line(map, r, c)) {
                    if (map[r][c] != last_line && last_line != ' ') {
                        line_changed_count = line_changed_count + 1;
                        /* CHECK IF CHANGING LINES FROM STATION OR NOT */
                        if (!is_station(map, r, c + 1, destination))
                            return ERROR_LINE_HOPPING_BETWEEN_STATIONS;
                        /* CHECK FINISHED */
                    }

                    last_line = map[r][c];
                }

                last_direction = W;
                break;
            }
            case E: {
                int temp_r = r, temp_c = c;
                int i = check_destination(map, height, width, temp_r, ++temp_c);
                if (i != 1)
                    return i;
                if (!is_station(map, r, c, destination)) {
                    if (last_direction == W)
                        return ERROR_BACKTRACKING_BETWEEN_STATIONS;
                }
                else
                {
                    if (last_direction == W)
                        line_changed_count++; // TAKE THE TRAIN CHANGE INTO CONSIDERATION
                }
                c++;
                if (is_line(map, r, c)) {
                    if (map[r][c] != last_line && last_line != ' ') {
                        line_changed_count = line_changed_count + 1;
                        /* CHECK IF CHANGING LINES FROM STATION OR NOT */
                        if (!is_station(map, r, c - 1, destination))
                            return ERROR_LINE_HOPPING_BETWEEN_STATIONS;
                        /* CHECK FINISHED */
                    }

                    last_line = map[r][c];
                }

                last_direction = E;
                break;
            }
            case NE: {   //TODO: I think we should check check_destination first
                int temp_r = r, temp_c = c;
                int i = check_destination(map, height, width, --temp_r, ++temp_c);
                if (i != 1)
                    return i;
                if (!is_station(map, r, c, destination)) {
                    if (last_direction == SW)
                        return ERROR_BACKTRACKING_BETWEEN_STATIONS;
                }
                else
                {
                    if (last_direction == SW)
                        line_changed_count++; // TAKE THE TRAIN CHANGE INTO CONSIDERATION
                }
                r--;
                c++;
                if (is_line(map, r, c)) {
                    if (map[r][c] != last_line && last_line != ' ') {
                        line_changed_count = line_changed_count + 1;
                        /* CHECK IF CHANGING LINES FROM STATION OR NOT */
                        if (!is_station(map, r + 1, c - 1, destination))
                            return ERROR_LINE_HOPPING_BETWEEN_STATIONS;
                        /* CHECK FINISHED */
                    }

                    last_line = map[r][c];
                }

                last_direction = NE;
                break;
            }
            case NW: {
                int temp_r = r, temp_c = c;
                int i = check_destination(map, height, width, --temp_r, --temp_c);
                if (i != 1)
                    return i;
                if (!is_station(map, r, c, destination)) {
                    if (last_direction == SE)
                        return ERROR_BACKTRACKING_BETWEEN_STATIONS;
                }
                else
                {
                    if (last_direction == SE)
                        line_changed_count++; // TAKE THE TRAIN CHANGE INTO CONSIDERATION
                }
                r--;
                c--;
                if (is_line(map, r, c)) {
                    if (map[r][c] != last_line && last_line != ' ') {
                        line_changed_count = line_changed_count + 1;
                        /* CHECK IF CHANGING LINES FROM STATION OR NOT */
                        if (!is_station(map, r + 1, c + 1, destination))
                            return ERROR_LINE_HOPPING_BETWEEN_STATIONS;
                        /* CHECK FINISHED */
                    }

                    last_line = map[r][c];
                }

                last_direction = NW;
                break;
            }
            case SE: {
                int temp_r = r, temp_c = c;
                int i = check_destination(map, height, width, ++temp_r, ++temp_c);
                if (i != 1)
                    return i;

                if (!is_station(map, r, c, destination)) {
                    if (last_direction == NW)
                        return ERROR_BACKTRACKING_BETWEEN_STATIONS;
                }
                else
                {
                    if (last_direction == NW)
                        line_changed_count++; // TAKE THE TRAIN CHANGE INTO CONSIDERATION
                }
                r++;
                c++;
                if (is_line(map, r, c)) {
                    if (map[r][c] != last_line && last_line != ' ') {
                        line_changed_count = line_changed_count + 1;
                        /* CHECK IF CHANGING LINES FROM STATION OR NOT */
                        if (!is_station(map, r - 1, c - 1, destination))
                            return ERROR_LINE_HOPPING_BETWEEN_STATIONS;
                        /* CHECK FINISHED */
                    }

                    last_line = map[r][c];
                }

                last_direction = SE;
                break;
            }
            case SW: {
                int temp_r = r, temp_c = c;
                int i = check_destination(map, height, width, ++temp_r, --temp_c);
                if (i != 1)
                    return i;
                last_direction = SW;
                if (!is_station(map, r, c, destination)) {
                    if (last_direction == NE)
                        return ERROR_BACKTRACKING_BETWEEN_STATIONS;
                }
                else
                {
                    if (last_direction == NE)
                        line_changed_count++; // TAKE THE TRAIN CHANGE INTO CONSIDERATION
                }
                r++;
                c--;
                if (is_line(map, r, c)) {
                    if (map[r][c] != last_line && last_line != ' ') {
                        line_changed_count = line_changed_count + 1;
                        /* CHECK IF CHANGING LINES FROM STATION OR NOT */
                        if (!is_station(map, r - 1, c + 1, destination))
                            return ERROR_LINE_HOPPING_BETWEEN_STATIONS;
                        /* CHECK FINISHED */
                    }

                    last_line = map[r][c];
                }

                break;
            }
            case INVALID_DIRECTION:
                return ERROR_INVALID_DIRECTION; //CHECK IF THE ROUTE IS VARIFIED
        }

    }
    /* ROUTE MOVED FINISHED*/

    /* CHECK IF THE DESTINATION IS A STATION OR NOT*/
    return is_station(map, r, c, destination) ? line_changed_count : ERROR_ROUTE_ENDPOINT_IS_NOT_STATION;

}

/*FUNCTION DEFINITION FOR "is_station" */
bool is_station(char **map, int r, int c, char destination[]) {
    char station_line[MAX_LEN]; // not be able to use char * without give length;
    char station_symbol;
    ifstream stations_file("stations.txt");
    while (stations_file.getline(station_line, MAX_LEN)) {
        station_symbol = station_line[0];
        if (map[r][c] == station_symbol) {
            strcpy(destination, station_line + 2);
            return true;
        }
    }
    return false;
}

/*FUNCTION DEFINITION FOR "is_line" */
bool is_line(char **map, int r, int c) {
    char lines_line[MAX_LEN]; // not be able to use char * without give length;
    char line_symbol;
    ifstream stations_file("lines.txt");
    while (stations_file.getline(lines_line, MAX_LEN)) {
        line_symbol = lines_line[0];
        if (map[r][c] == line_symbol)
            return true;
    }
    return false;
}
