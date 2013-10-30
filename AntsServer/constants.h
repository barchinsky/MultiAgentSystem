#ifndef CONSTANTS_H
#define CONSTANTS_H

// JSON keys
#define kJSON_API_KEY      QString("API_KEY")
#define kJSON_OBJECT       QString("OBJECT")
#define kJSON_MESSAGE      QString("MESSAGE")
#define kJSON_COORD        QString("COORD")
#define kJSON_COORD_ANT    QString("COORD_ANT")
#define kJSON_COORD_BASE   QString("COORD_BASE")
#define kJSON_VECTOR       QString("VECTOR")
#define kJSON_ID           QString("ID")
#define kJSON_ANTS         QString("ANTS")
#define kJSON_FOODS        QString("FOODS")
#define kJSON_BARRIERS     QString("BARRIERS")
#define kJSON_SOCKET       QString("SOCKET")

// API keys
#define kAPI_register           QString("registration")
#define kAPI_nearest_objects    QString("nearest_objects")
#define kAPI_is_ant_can_move    QString("is_ant_can_move")
#define kAPI_error              QString("ERROR")

//enums
typedef enum {
    kObjectTypeAnt = 0,
    kObjectTypeFood,
    kObjectTypeBarrier
} ObjectType;

#endif // CONSTANTS_H
