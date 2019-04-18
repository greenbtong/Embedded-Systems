/*
 * File:   Field.c
 * Author: Brian Tong and Sam Hanawalt
 *
 * Created on June 2, 2017, 10:52 AM
 */

#include "xc.h"
#include "Field.h"
#include "FieldOled.h"
#include "Protocol.h"
#include "Oled.h"

static int i, j, PrintCount = 0, sizeTemp;
static FieldPosition fieldType, pCheck1, pCheck2;

/**
 * FieldInit() will fill the passed field array with the data specified in positionData. Also the
 * lives for each boat are filled according to the `BoatLives` enum.
 * @param f The field to initialize.
 * @param p The data to initialize the entire field to, should be a member of enum
 *                     FieldPosition.
 */
void FieldInit(Field *f, FieldPosition p)
{
    for (i = 0; i < FIELD_ROWS; i++) {
        for (j = 0; j < FIELD_COLS; j++) {
            f->field[i][j] = p;
        }
    }
    f->smallBoatLives = FIELD_BOAT_LIVES_SMALL;
    f->mediumBoatLives = FIELD_BOAT_LIVES_MEDIUM;
    f->largeBoatLives = FIELD_BOAT_LIVES_LARGE;
    f->hugeBoatLives = FIELD_BOAT_LIVES_HUGE;
}

/**
 * Retrieves the value at the specified field position.
 * @param f The Field being referenced
 * @param row The row-component of the location to retrieve
 * @param col The column-component of the location to retrieve
 * @return
 */
FieldPosition FieldAt(const Field *f, uint8_t row, uint8_t col)
{
    return f->field[row][col];
}

/**
 * This function provides an interface for setting individual locations within a Field struct. This
 * is useful when FieldAddBoat() doesn't do exactly what you need. For example, if you'd like to use
 * FIELD_POSITION_CURSOR, this is the function to use.
 * 
 * @param f The Field to modify.
 * @param row The row-component of the location to modify
 * @param col The column-component of the location to modify
 * @param p The new value of the field location
 * @return The old value at that field location
 */
FieldPosition FieldSetLocation(Field *f, uint8_t row, uint8_t col, FieldPosition p)
{
    FieldPosition oldValue = f->field[row][col];
    f->field[row][col] = p;
    return oldValue;
}

/**
 * FieldAddBoat() places a single ship on the player's field based on arguments 2-5. Arguments 2, 3
 * represent the x, y coordinates of the pivot point of the ship.  Argument 4 represents the
 * direction of the ship, and argument 5 is the length of the ship being placed. All spaces that
 * the boat would occupy are checked to be clear before the field is modified so that if the boat
 * can fit in the desired position, the field is modified as SUCCESS is returned. Otherwise the
 * field is unmodified and STANDARD_ERROR is returned. There is no hard-coded limit to how many
 * times a boat can be added to a field within this function.
 *
 * So this is valid test code:
 * {
 *   Field myField;
 *   FieldInit(&myField,FIELD_POSITION_EMPTY);
 *   FieldAddBoat(&myField, 0, 0, FIELD_BOAT_DIRECTION_EAST, FIELD_BOAT_SMALL);
 *   FieldAddBoat(&myField, 1, 0, FIELD_BOAT_DIRECTION_EAST, FIELD_BOAT_MEDIUM);
 *   FieldAddBoat(&myField, 1, 0, FIELD_BOAT_DIRECTION_EAST, FIELD_BOAT_HUGE);
 *   FieldAddBoat(&myField, 0, 6, FIELD_BOAT_DIRECTION_SOUTH, FIELD_BOAT_SMALL);
 * }
 *
 * should result in a field like:
 *  _ _ _ _ _ _ _ _
 * [ 3 3 3       3 ]
 * [ 4 4 4 4     3 ]
 * [             3 ]
 *  . . . . . . . .
 *
 * @param f The field to grab data from.
 * @param row The row that the boat will start from, valid range is from 0 and to FIELD_ROWS - 1.
 * @param col The column that the boat will start from, valid range is from 0 and to FIELD_COLS - 1.
 * @param dir The direction that the boat will face once places, from the BoatDirection enum.
 * @param boatType The type of boat to place. Relies on the FIELD_POSITION_*_BOAT values from the
 * FieldPosition enum.
 * @return TRUE for success, FALSE for failure
 */
uint8_t FieldAddBoat(Field *f, uint8_t row, uint8_t col, BoatDirection dir, BoatType type)
{

    if (type == FIELD_BOAT_SMALL) {
        sizeTemp = FIELD_BOAT_LIVES_SMALL;
        fieldType = FIELD_POSITION_SMALL_BOAT;
    } else if (type == FIELD_BOAT_MEDIUM) {
        sizeTemp = FIELD_BOAT_LIVES_MEDIUM;
        fieldType = FIELD_POSITION_MEDIUM_BOAT;
    } else if (type == FIELD_BOAT_LARGE) {
        sizeTemp = FIELD_BOAT_LIVES_LARGE;
        fieldType = FIELD_POSITION_LARGE_BOAT;
    } else if (type == FIELD_BOAT_HUGE) {
        sizeTemp = FIELD_BOAT_LIVES_HUGE;
        fieldType = FIELD_POSITION_HUGE_BOAT;
    }

    if (dir == FIELD_BOAT_DIRECTION_EAST) {
        for (PrintCount = 0; PrintCount < sizeTemp; PrintCount++) {
            if (f->field[row][col + PrintCount] != FIELD_POSITION_EMPTY) {
                return STANDARD_ERROR;
            }
        }
        for (PrintCount = 0; PrintCount < sizeTemp; PrintCount++) {
            if ((row < 6 && row > -1) && (col + sizeTemp < 10 && col + sizeTemp >-1)) {
                f->field[row][col + PrintCount] = fieldType;
            } else {
                return STANDARD_ERROR;
            }
        }

    } else if (dir == FIELD_BOAT_DIRECTION_WEST) {
        for (PrintCount = 0; PrintCount < sizeTemp; PrintCount++) {
            if (f->field[row][col - PrintCount] != FIELD_POSITION_EMPTY) {
                return STANDARD_ERROR;
            }
        }
        for (PrintCount = 0; PrintCount < sizeTemp; PrintCount++) {
            if ((row < 6 && row > -1) && (col - sizeTemp < 10 && col - sizeTemp >-1)) {
                f->field[row][col - PrintCount] = fieldType;
            } else {
                return STANDARD_ERROR;
            }
        }

    } else if (dir == FIELD_BOAT_DIRECTION_NORTH) {
        for (PrintCount = 0; PrintCount < sizeTemp; PrintCount++) {
            if (f->field[row - PrintCount][col] != FIELD_POSITION_EMPTY) {
                return STANDARD_ERROR;
            }
        }
        for (PrintCount = 0; PrintCount < sizeTemp; PrintCount++) {
            if ((row - sizeTemp < 6 && row - sizeTemp> -1) && (col < 10 && col >-1)) {
                f->field[row - PrintCount][col] = fieldType;
            } else {
                return STANDARD_ERROR;
            }
        }

    } else if (dir == FIELD_BOAT_DIRECTION_SOUTH) {
        for (PrintCount = 0; PrintCount < sizeTemp; PrintCount++) {
            if (f->field[row + PrintCount][col] != FIELD_POSITION_EMPTY) {
                return STANDARD_ERROR;
            }
        }
        for (PrintCount = 0; PrintCount < sizeTemp; PrintCount++) {
            if ((row + sizeTemp < 6 && row + sizeTemp> -1) && (col < 10 && col >-1)) {
                f->field[row + PrintCount][col] = fieldType;
            } else {
                return STANDARD_ERROR;
            }
        }

    }

    dir = 0;
    type = 0;
    return SUCCESS;
}


///**
// * This function registers an attack at the gData coordinates on the provided field. This means that
// * 'f' is updated with a FIELD_POSITION_HIT or FIELD_POSITION_MISS depending on what was at the
// * coordinates indicated in 'gData'. 'gData' is also updated with the proper HitStatus value
// * depending on what happened AND the value of that field position BEFORE it was attacked. Finally
// * this function also reduces the lives for any boat that was hit from this attack.
// * @param f The field to check against and update.
// * @param gData The coordinates that were guessed. The HIT result is stored in gData->hit as an
// *               output.
// * @return The data that was stored at the field position indicated by gData before this attack.
// */

FieldPosition FieldRegisterEnemyAttack(Field *f, GuessData *gData)
{
    pCheck1 = FieldAt(f, gData->row, gData->col);
    switch (pCheck1) {

        // Miss
    case FIELD_POSITION_EMPTY:
        gData->hit = HIT_MISS;
        FieldSetLocation(f, gData->row, gData->col, FIELD_POSITION_MISS);
        break;

        // small boat hit
    case FIELD_POSITION_SMALL_BOAT:
        if (f->smallBoatLives > 1) {
            gData->hit = HIT_HIT;
        } else {
            gData->hit = HIT_SUNK_SMALL_BOAT;
        }
        FieldSetLocation(f, gData->row, gData->col, FIELD_POSITION_HIT);
        f->smallBoatLives--;
        break;

        // medium boat hit        
    case FIELD_POSITION_MEDIUM_BOAT:
        if (f->mediumBoatLives > 1) {
            gData->hit = HIT_HIT;
        } else {
            gData->hit = HIT_SUNK_MEDIUM_BOAT;
        }
        FieldSetLocation(f, gData->row, gData->col, FIELD_POSITION_HIT);
        f->mediumBoatLives--;
        break;

        // large boat hit
    case FIELD_POSITION_LARGE_BOAT:
        if (f->largeBoatLives > 1) {
            gData->hit = HIT_HIT;
        } else {
            gData->hit = HIT_SUNK_LARGE_BOAT;
        }
        FieldSetLocation(f, gData->row, gData->col, FIELD_POSITION_HIT);
        f->largeBoatLives--;
        break;

        // huge boat hit
    case FIELD_POSITION_HUGE_BOAT:
        if (f->hugeBoatLives > 1) {
            gData->hit = HIT_HIT;
        } else {
            gData->hit = HIT_SUNK_HUGE_BOAT;
        }
        FieldSetLocation(f, gData->row, gData->col, FIELD_POSITION_HIT);
        f->hugeBoatLives--;
        break;

        // for the other case in Fieldat that was not defined 
    case FIELD_POSITION_MISS:
        break;
    case FIELD_POSITION_UNKNOWN:
        break;
    case FIELD_POSITION_HIT:
        break;
    case FIELD_POSITION_CURSOR:
        break;
    }
    return pCheck1;

}

/**
 * This function updates the FieldState representing the opponent's game board with whether the
 * guess indicated within gData was a hit or not. If it was a hit, then the field is updated with a
 * FIELD_POSITION_HIT at that position. If it was a miss, display a FIELD_POSITION_EMPTY instead, as
 * it is now known that there was no boat there. The FieldState struct also contains data on how
 * many lives each ship has. Each hit only reports if it was a hit on any boat or if a specific boat
 * was sunk, this function also clears a boats lives if it detects that the hit was a
 * HIT_SUNK_*_BOAT.
 * @param f The field to grab data from.
 * @param gData The coordinates that were guessed along with their HitStatus.
 * @return The previous value of that coordinate position in the field before the hit/miss was
 * registered.
 */
FieldPosition FieldUpdateKnowledge(Field *f, const GuessData *gData)
{
    pCheck2 = FieldAt(f, gData->row, gData->col);
    switch (gData->hit) {

        // miss
    case HIT_MISS:
        FieldSetLocation(f, gData->row, gData->col, FIELD_POSITION_EMPTY);
        break;

        // hit
    case HIT_HIT:
        FieldSetLocation(f, gData->row, gData->col, FIELD_POSITION_HIT);
        break;

        // small boat dead
    case HIT_SUNK_SMALL_BOAT:
        f->smallBoatLives = 0;
        break;

        // medium boat dead
    case HIT_SUNK_MEDIUM_BOAT:
        f->mediumBoatLives = 0;
        break;

        // large boat dead
    case HIT_SUNK_LARGE_BOAT:
        f->largeBoatLives = 0;
        break;

        // huge boat dead
    case HIT_SUNK_HUGE_BOAT:
        f->hugeBoatLives = 0;
        break;
    }
    return pCheck2;
}

/**
 * This function returns the alive states of all 4 boats as a 4-bit bitfield (stored as a uint8).
 * The boats are ordered from smallest to largest starting at the least-significant bit. So that:
 * 0b00001010 indicates that the small boat and large boat are sunk, while the medium and huge boat
 * are still alive. See the BoatStatus enum for the bit arrangement.
 * @param f The field to grab data from.
 * @return A 4-bit value with each bit corresponding to whether each ship is alive or not.
 */
uint8_t FieldGetBoatStates(const Field *f)
{
    if (f->smallBoatLives == 0 && f->mediumBoatLives == 0 && f->largeBoatLives == 0 && f->hugeBoatLives == 0) {
        return 0b00000000;
    } else if (f->smallBoatLives > 0 && f->mediumBoatLives == 0 && f->largeBoatLives == 0 && f->hugeBoatLives == 0) {
        return 0b00000001;
    } else if (f->smallBoatLives > 0 && f->mediumBoatLives > 0 && f->largeBoatLives == 0 && f->hugeBoatLives == 0) {
        return 0b00000011;
    } else if (f->smallBoatLives > 0 && f->mediumBoatLives == 0 && f->largeBoatLives > 0 && f->hugeBoatLives == 0) {
        return 0b00000101;
    } else if (f->smallBoatLives == 0 && f->mediumBoatLives > 0 && f->largeBoatLives > 0 && f->hugeBoatLives == 0) {
        return 0b00000110;
    } else if (f->smallBoatLives == 0 && f->mediumBoatLives == 0 && f->largeBoatLives > 0 && f->hugeBoatLives == 0) {
        return 0b00000100;
    } else if (f->smallBoatLives > 0 && f->mediumBoatLives > 0 && f->largeBoatLives > 0 && f->hugeBoatLives > 0) {
        return 0b00001111;
    } else if (f->smallBoatLives > 0 && f->mediumBoatLives > 0 && f->largeBoatLives == 0 && f->hugeBoatLives > 0) {
        return 0b00001011;
    } else if (f->smallBoatLives > 0 && f->mediumBoatLives == 0 && f->largeBoatLives > 0 && f->hugeBoatLives > 0) {
        return 0b00001101;
    } else if (f->smallBoatLives == 0 && f->mediumBoatLives > 0 && f->largeBoatLives > 0 && f->hugeBoatLives > 0) {
        return 0b00001110;
    } else if (f->smallBoatLives > 0 && f->mediumBoatLives == 0 && f->largeBoatLives == 0 && f->hugeBoatLives > 0) {
        return 0b00001001;
    } else if (f->smallBoatLives == 0 && f->mediumBoatLives > 0 && f->largeBoatLives == 0 && f->hugeBoatLives > 0) {
        return 0b00001010;
    } else if (f->smallBoatLives == 0 && f->mediumBoatLives == 0 && f->largeBoatLives > 0 && f->hugeBoatLives > 0) {
        return 0b00001100;
    } else if (f->smallBoatLives == 0 && f->mediumBoatLives == 0 && f->largeBoatLives == 0 && f->hugeBoatLives > 0) {
        return 0b00001000;
    } else if (f->smallBoatLives > 0 && f->mediumBoatLives > 0 && f->largeBoatLives > 0 && f->hugeBoatLives == 0) {
        return 0b00000111;
    } else if (f->smallBoatLives == 0 && f->mediumBoatLives > 0 && f->largeBoatLives == 0 && f->hugeBoatLives == 0) {
        return 0b00000010;
    }
    return 0;
}