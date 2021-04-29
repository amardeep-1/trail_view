/*
A3
Amardeep Singh
1079318
*/

#include <GPXParser.h>

/* deletes gpxdata */
void deleteGpxData( void* data) {
    if (data != NULL) {
        GPXData *temp;
        temp = (GPXData*)data;
        free(temp);
    }
}

char* gpxDataToString( void* data) {
    if (data != NULL) {
        char* temp;
        GPXData *other = (GPXData *) data;
        int len;
        len = strlen(other->name)+strlen(other->value) + 18;            /*dynamically allocatting memory*/
        temp = (char*)malloc(sizeof(char)*len);
        sprintf(temp, "OTHER DATA [ %s, %s ]", other->name, other->value);
        return temp;
    }
    return NULL;
}

int compareGpxData(const void *first, const void *second) {
    return 0;
}

/* deletes waypoint */
void deleteWaypoint(void* data) {
    if (data != NULL) {
        Waypoint *temp;
        temp = (Waypoint*)data;
        free(temp->name);
        freeList(temp->otherData);
        free(temp);
    }
}

char* waypointToString( void* data) {
    if (data != NULL) {
        char* temp;
        Waypoint *other = (Waypoint *) data;
        char *othrs = toString(other->otherData);
        //lon and lat can be 1077 digits each
        int len;
        len = strlen(other->name)+strlen(othrs) + 1077*2 + 27;                 /*dynamically allocatting memory*/
        temp = (char*)malloc(sizeof(char)*len);
        sprintf(temp, "WAYPOINT { name: %s, %f, %f, %s }\n", other->name, other->latitude, other->longitude, othrs);

        free(othrs);
        return temp;
    }
    return NULL;
}

int compareWaypoints(const void *first, const void *second) {
    return 0;
}

/* deletes route */
void deleteRoute(void* data) {
    if (data != NULL) {
        Route *temp;
        temp = (Route*)data;
        free(temp->name);
        freeList(temp->waypoints);
        freeList(temp->otherData);
        free(temp);
    }
}

char* routeToString(void* data) {
    if (data != NULL) {
        char* temp;
        Route *other = (Route *) data;
        char *othrs = toString(other->otherData);
        char *wpts = toString(other->waypoints);
        int len;
        len = strlen(other->name) + strlen(othrs) + strlen(wpts) + 23;               /*dynamically allocatting memory*/
        temp = (char*)malloc(sizeof(char)*len);
        sprintf(temp, "ROUTE { name: %s, \n%s, %s }\n", other->name, wpts, othrs);

        free(othrs);
        free(wpts);
        return temp;
    }
    return NULL;
}

int compareRoutes(const void *first, const void *second) {
    return 0;
}

/* deletes track segments */
void deleteTrackSegment(void* data) {
    if (data != NULL) {
        TrackSegment *temp;
        temp = (TrackSegment*)data;
        freeList(temp->waypoints);
        free(temp);
    }
}

char* trackSegmentToString(void* data) {
    if (data != NULL) {
        char *temp;
        TrackSegment *other = (TrackSegment *) data;
        char *wpts = toString(other->waypoints);
        int len;
        len = strlen(wpts) + 15;
        temp = (char*)malloc(sizeof(char)*len);               /*dynamically allocatting memory*/
        sprintf(temp, "SEGMENT [ \n%s ]\n", wpts);
        free(wpts);
        return temp;
    }
    return NULL;
}
int compareTrackSegments(const void *first, const void *second) {
    return 0;
}

/* deletes track */
void deleteTrack(void* data) {
    if (data != NULL) {
        Track *temp;
        temp = (Track*)data;
        free(temp->name);
        freeList(temp->segments);
        freeList(temp->otherData);
        free(temp);
    }
}

char* trackToString(void* data) {
    if (data != NULL) {
        char* temp;
        Track *other = (Track *) data;
        char *othrs = toString(other->otherData);
        char *segs = toString(other->segments);
        int len;
        len = strlen(other->name) + strlen(othrs) + strlen(segs) + 23;               /*dynamically allocatting memory*/
        temp = (char*)malloc(sizeof(char)*len);
        sprintf(temp, "TRACK { name: %s, \n%s, %s }\n", other->name, segs, othrs);

        free(othrs);
        free(segs);
        return temp;
    }
    return NULL;
}

int compareTracks(const void *first, const void *second) {
    return 0;
}



