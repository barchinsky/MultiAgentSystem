#ifndef CONSTANTS_H
#define CONSTANTS_H

// JSON keys
#define kJSON_API_KEY      QString("API_KEY")
#define kJSON_OBJECT       QString("OBJECT")
#define kJSON_MESSAGE      QString("MESSAGE")
#define kJSON_COORD        QString("COORD")
#define kJSON_COORD_ANT    QString("COORD_ANT")
#define kJSON_COORD_BASE   QString("COORD_BASE")
#define kJSON_NEXT_COORD   QString("NEXT_COORD")
#define kJSON_STEP_LENGTH  QString("STEP_LENGTH")
#define kJSON_VECTOR       QString("VECTOR")
#define kJSON_ID           QString("ID")
#define kJSON_ANTS         QString("ANTS")
#define kJSON_FOODS        QString("FOODS")
#define kJSON_BARRIERS     QString("BARRIERS")
#define kJSON_SOCKET       QString("SOCKET")
#define kJSON_GOT_FOOD     QString("GOT_FOOD")
#define kJSON_PUT_FOOD     QString("PUT_FOOD")


//  API keys
#define kAPI_register           QString("registration")
#define kAPI_nearest_objects    QString("nearest_objects")
#define kAPI_is_ant_can_move    QString("is_ant_can_move")
#define kAPI_got_food           QString("got_food")
#define kAPI_put_food           QString("put_food")
#define kAPI_error              QString("ERROR")

//enums
typedef enum {
    kObjectTypeAnt = 0,
    kObjectTypeFood,
    kObjectTypeBarrier
} ObjectType;

#define DEBUG
#ifdef DEBUG
        #define TRACE qDebug
#else
        #define TRACE(fmt,arg...) ((void)0)
#endif

#endif // CONSTANTS_H
