
#include <GPXParser.h>

/* function that takes a XML doc and validates it against a xsd file */
bool validateXMLDoc(xmlDoc* doc, char* fileName) {
    xmlSchemaPtr schema = NULL;
    xmlSchemaParserCtxtPtr ctxt;

    xmlLineNumbersDefault(1);
    ctxt = xmlSchemaNewParserCtxt(fileName);
    schema = xmlSchemaParse(ctxt);
    xmlSchemaFreeParserCtxt(ctxt);

    if (doc != NULL) {
        int ret;
        xmlSchemaValidCtxtPtr ctxt;
        ctxt = xmlSchemaNewValidCtxt(schema);        /* i used the resources in the assignment discription for this part*/
        ret = xmlSchemaValidateDoc(ctxt, doc);
        xmlSchemaFreeValidCtxt(ctxt);
        xmlFreeDoc(doc);

        if(schema != NULL)
            xmlSchemaFree(schema);

        xmlSchemaCleanupTypes();
        xmlCleanupParser();
        xmlMemoryDump();

        if (ret == 0) {
            return true;
        } else {
            return false;
        }
    }    
    return false;
}

/* converts other data struct to xml */
void otherDataXML(xmlNode* node, ListIterator itr) {
    GPXData* tempj = nextElement(&itr);
    while(tempj != NULL) {
        xmlNewChild(node, NULL, BAD_CAST tempj->name, BAD_CAST tempj->value);
        tempj = nextElement(&itr);
    }
}

/* converts other data struct to xml */
void waypointXML(xmlNode* root_element, char* name, ListIterator itr) {
    xmlNode* node = NULL;
    char tempStr[1077];
    Waypoint *temp = nextElement(&itr);
    while(temp != NULL) {
        node = xmlNewChild(root_element, NULL, BAD_CAST name, NULL);
        if(strcmp(temp->name, "\0") != 0) 
            xmlNewChild(node, NULL, BAD_CAST "name", BAD_CAST temp->name);
        
        sprintf(tempStr, "%f", temp->latitude);
        xmlNewProp(node, BAD_CAST "lat", BAD_CAST tempStr);
        sprintf(tempStr, "%f", temp->longitude);
        xmlNewProp(node, BAD_CAST "lon", BAD_CAST tempStr);
    
        if(getLength(temp->otherData) != 0) {	
            ListIterator jtr = createIterator(temp->otherData);
            otherDataXML(node, jtr);
        }
        temp = nextElement(&itr);      
    }
}

/* converts gpxdoc struct to xml */
xmlDoc* createXMLdoc(GPXdoc* gdoc) {
    if (gdoc == NULL)
        return NULL;
        
    xmlDoc* doc = NULL;
    xmlNode* root_element = NULL;
    
    LIBXML_TEST_VERSION;
    
    doc = xmlNewDoc(BAD_CAST "1.0");
    root_element = xmlNewNode(NULL, BAD_CAST "gpx");
    xmlDocSetRootElement(doc, root_element); 

    int docIsEmpty = 1;

    xmlNode* node = NULL;
    xmlNode* nodeIn = NULL;

    char tempStr[1077];
    sprintf(tempStr, "%0.1f", gdoc->version);
    xmlNewProp(root_element, BAD_CAST "version", BAD_CAST tempStr);
    xmlNewProp(root_element, BAD_CAST "creator", BAD_CAST gdoc->creator);
    xmlNs* ns = xmlNewNs(root_element, BAD_CAST gdoc->namespace, NULL);
    xmlSetNs(root_element, ns);

    if (getNumWaypoints(gdoc) != 0) {
        docIsEmpty = 0;
        ListIterator itr = createIterator(gdoc->waypoints);              /* converts waypoints to xml */
        waypointXML(root_element, "wpt\0", itr);
    }

    if (getNumRoutes(gdoc) != 0) {
        docIsEmpty = 0;
        ListIterator itr = createIterator(gdoc->routes);                /* converts routes to xml */
        Route * temp  = nextElement(&itr);
        while(temp != NULL) {
            node = xmlNewChild(root_element, NULL, BAD_CAST "rte", NULL);
            if(strcmp(temp->name, "\0") != 0) 
                xmlNewChild(node, NULL, BAD_CAST "name", BAD_CAST temp->name);
        
            if(getLength(temp->otherData) != 0) {	
                ListIterator jtr = createIterator(temp->otherData);
                otherDataXML(node, jtr);
            }
            if(getLength(temp->waypoints) != 0) {
                ListIterator jtr = createIterator(temp->waypoints);
                waypointXML(node, "rtept\0", jtr);
            } 
            temp = nextElement(&itr);     
        }
    }
    if (getNumTracks(gdoc) != 0) {
        docIsEmpty = 0;
        ListIterator itr = createIterator(gdoc->tracks);           /* converts tracks to xml */
        Track * temp  = nextElement(&itr);
        
        while(temp != NULL) {
            node = xmlNewChild(root_element, NULL, BAD_CAST "trk", NULL);
            if(strcmp(temp->name, "\0") != 0) 
                xmlNewChild(node, NULL, BAD_CAST "name", BAD_CAST temp->name);
            if(getLength(temp->otherData) != 0) {	
                ListIterator jtr = createIterator(temp->otherData);
                otherDataXML(node, jtr);
            }

            if(getLength(temp->segments) != 0) {	
                ListIterator jtr = createIterator(temp->segments);
                TrackSegment * tempSeg  = nextElement(&jtr);
                while(tempSeg != NULL) {
                    nodeIn = xmlNewChild(node, NULL, BAD_CAST "trkseg", NULL);
                    if(getLength(tempSeg->waypoints) != 0) {
                        ListIterator ktr = createIterator(tempSeg->waypoints);
                        waypointXML(nodeIn, "trkpt\0", ktr);
                    } 
                    tempSeg = nextElement(&jtr);     
                }
            }
            temp = nextElement(&itr); 
        }
    }

    if (docIsEmpty) {
        xmlNodePtr com = xmlNewDocComment(doc, BAD_CAST "empty"); 
        xmlAddChild(root_element, com);
    }
    return doc;
}

/*bonus functions*/
void addWaypoint(Route *rt, Waypoint *pt){
}
void addRoute(GPXdoc* doc, Route* rt){
}
GPXdoc* JSONtoGPX(const char* gpxString){
    return NULL;
}
Waypoint* JSONtoWaypoint(const char* gpxString){
    return NULL;
}
Route* JSONtoRoute(const char* gpxString){
    return NULL;
}

/*Module 1*/
/*validates xml then makes a gpx doc struct*/
GPXdoc* createValidGPXdoc(char* fileName, char* gpxSchemaFile){
    
    if (fileName != NULL && gpxSchemaFile != NULL) {
        xmlDoc* doc = NULL;

        FILE *testFile;
        if (!(testFile = fopen(fileName, "r"))) {
            xmlCleanupParser();
            xmlMemoryDump();
            return NULL;
        }
        fclose(testFile);
        if (!(testFile = fopen(gpxSchemaFile, "r"))){
            xmlCleanupParser();
            xmlMemoryDump();
            return NULL;
        }
        fclose(testFile);

        doc = xmlReadFile(fileName, NULL, 0);
        if (doc == NULL) {
            xmlCleanupParser();
            xmlMemoryDump();

            return (NULL);  
        }
        if (validateXMLDoc(doc, gpxSchemaFile)) {
            GPXdoc * gdoc = createGPXdoc(fileName);
            return gdoc;
        }        
    }
    return NULL;
}

/*takes gpx doc struct converts it to an xml doc and validates it*/
bool validateGPXDoc(GPXdoc* doc, char* gpxSchemaFile) {
    if (doc != NULL && gpxSchemaFile != NULL) {
        xmlDoc* xdoc = createXMLdoc(doc);
        if (validateXMLDoc(xdoc, gpxSchemaFile))
            return true;
        
    }
    return false;
}


/*writes a valid gpx doc struct to a file*/
bool writeGPXdoc(GPXdoc* doc, char* fileName) {
    if (doc != NULL && fileName != NULL) {
        xmlDoc* xdoc = createXMLdoc(doc);
        if (xmlSaveFormatFileEnc(fileName, xdoc, "UTF-8", 4)) {
            xmlFreeDoc(xdoc);
            xmlCleanupParser();
            xmlMemoryDump();
            return true;
        }

    }
    return false;
}

/*Module 2*/
/*rounds to nearest 10*/
float round10(float len) {
    int n = (len+5)/10;
    return n*10;
}

/*uses haversine formula to calculate distance between 2 points*/
float haversine(float latOne, float lonOne, float latTwo, float lonTwo) {
    float lons = (lonTwo - lonOne) * (M_PI/180);
    float lats = (latTwo - latOne) * (M_PI/180);
    float a = (sinf(lats/2)*sinf(lats/2)) + cosf(latOne * (M_PI/180)) * cosf(latTwo * (M_PI/180)) * (sinf(lons/2)*sinf(lons/2));
    float b = 2 * atan2f(sqrt(a), sqrt( 1 - a));
    return((6371 * b)*1000);
}

/*gets length between list of waypoints*/
float getWaypointLens(List* points) {
    int count = 0;
    float latOne, lonOne, dist = 0;

    if (getLength(points) > 1) {
        ListIterator i = createIterator(points);
        Waypoint *temp = nextElement(&i);
        while(temp != NULL) {
            if (count == 0) {
                latOne = temp->latitude;
                lonOne = temp->longitude;
                count++;
            } else {
                dist += haversine(latOne, lonOne, temp->latitude, temp->longitude);  
                latOne = temp->latitude;
                lonOne = temp->longitude;
            }
            temp = nextElement(&i);      
        }
    }
    return dist;
}

/*gets length of a route*/
float getRouteLen(const Route *rt) {
    if (rt != NULL) {
        return getWaypointLens(rt->waypoints);
    }
    return 0;
}

/*gets length of a track*/
float getTrackLen(const Track *tr){
    if (tr != NULL){
        float latOne, lonOne, latTwo, lonTwo, dist = 0;
        if (getLength(tr->segments) != 0) {
            int count = 0;
            ListIterator j = createIterator(tr->segments);
            TrackSegment *tempTS = nextElement(&j);
            while(tempTS != NULL) {
                if (getLength(tempTS->waypoints) != 0) {            
                    dist += getWaypointLens(tempTS->waypoints);
                    if (count == 0) {
                        Waypoint * lastWp = getFromBack(tempTS->waypoints);  /*adding distance between segments*/
                        latOne = lastWp->latitude;
                        lonOne = lastWp->longitude;
                        count++;
                    } else {
                        Waypoint * firstWp = getFromFront(tempTS->waypoints);
                        latTwo = firstWp->latitude;
                        lonTwo = firstWp->longitude;
                        dist += haversine(latOne, lonOne, latTwo, lonTwo);
                        Waypoint * lastWp = getFromBack(tempTS->waypoints);
                        latOne = lastWp->latitude;
                        lonOne = lastWp->longitude;
                    }
                }
                tempTS = nextElement(&j);
            }
        }
        return dist;
    }
    return 0;
}

/*gets number of routes with the same length that was given*/
int numRoutesWithLength(const GPXdoc* doc, float len, float delta){
    if (doc != NULL && len >= 0 && delta >= 0) {
        if (getNumRoutes(doc) != 0) {
            int count = 0;
            ListIterator i = createIterator(doc->routes);
            Route *temp = nextElement(&i); 
            while(temp != NULL) {
                if (fabs((getRouteLen(temp)) - len) <= delta) {
                    count++;
                }
                temp = nextElement(&i);
            }
            return count;
        }
    }
    return 0;
}

/*gets number of track with the same length that was given*/
int numTracksWithLength(const GPXdoc* doc, float len, float delta){
    if (doc != NULL && len >= 0 && delta >= 0) {
        if (getNumTracks(doc) != 0) {
            int count = 0;
            ListIterator i = createIterator(doc->tracks);
            Track *temp = nextElement(&i);
            while(temp != NULL) {
                if (fabs((getTrackLen(temp)) - len) <= delta) {
                    count++;
                }
                temp = nextElement(&i);
            }
            return count;
        }
    }
    return 0;
}

/*finds if a route end on its first point*/
bool isLoopRoute(const Route* route, float delta){
    if (route != NULL && delta >= 0) {
        if (getLength(route->waypoints) > 3) {
            Waypoint * start = getFromFront(route->waypoints);
            Waypoint * end = getFromBack(route->waypoints);
            if (haversine(start->latitude, start->longitude, end->latitude, end->longitude) <= delta){
                return true;
            }
        }
    }
    return false;
}

/*finds if a track end on its first point*/
bool isLoopTrack(const Track *tr, float delta){
    if (tr != NULL && delta >= 0) {
        if (getLength(tr->segments) != 0) {
            int points = 0;
            ListIterator i = createIterator(tr->segments);
            TrackSegment *temp = nextElement(&i); 
            while(temp != NULL) {
                points += getLength(temp->waypoints);
                temp = nextElement(&i);
            }

            if (points > 3) {
                TrackSegment* startSeg = getFromFront(tr->segments);
                TrackSegment* endSeg = getFromBack(tr->segments);
                Waypoint* start = getFromFront(startSeg->waypoints);
                Waypoint* end = getFromBack(endSeg->waypoints);
                if (haversine(start->latitude, start->longitude, end->latitude, end->longitude) <= delta){
                    return true;
                }
            }
        }
    }
    return false;
}

void dummyDelete(){     /*delete func for RoutesBetween lists*/
}

/*gets a list of routes with the same start and end point*/
List* getRoutesBetween(const GPXdoc* doc, float sourceLat, float sourceLong, float destLat, float destLong, float delta){

    if (doc != NULL) {
        if (getNumRoutes(doc) != 0) {
            List* routesList = initializeList( &routeToString, &dummyDelete, &compareRoutes);
            ListIterator i = createIterator(doc->routes);
            Route *temp = nextElement(&i);
            while(temp != NULL) {
                if (getLength(temp->waypoints) != 0) {
                    Waypoint * start = getFromFront(temp->waypoints);
                    Waypoint * end = getFromBack(temp->waypoints);
                    if (haversine(start->latitude, start->longitude, sourceLat, sourceLong) <= delta &&
                        haversine(end->latitude, end->longitude, destLat, destLong) <= delta ) {
                        insertBack(routesList, temp);                    
                    }
                }
                temp = nextElement(&i);
            }
            if (getLength(routesList) != 0)
                return routesList;
            
        }
    }
    return NULL;
}

/*gets a list of tracks with the same start and end point*/
List* getTracksBetween(const GPXdoc* doc, float sourceLat, float sourceLong, float destLat, float destLong, float delta) {

    if (doc != NULL) {
        if (getNumTracks(doc) != 0) {
            if (getNumSegments(doc) != 0) {
                List* trackList = initializeList( &routeToString, &dummyDelete, &compareRoutes);
                ListIterator i = createIterator(doc->tracks);
                Track *temp = nextElement(&i);
                while(temp != NULL) {
                    if (getLength(temp->segments) != 0) {                    
                        TrackSegment* startSeg = getFromFront(temp->segments);
                        TrackSegment* endSeg = getFromBack(temp->segments);
                        if (getLength(startSeg->waypoints) != 0 && getLength(endSeg->waypoints) != 0 ) {
                            Waypoint* start = getFromFront(startSeg->waypoints);
                            Waypoint* end = getFromBack(endSeg->waypoints);
                            if (haversine(start->latitude, start->longitude, sourceLat, sourceLong) <= delta &&
                                haversine(end->latitude, end->longitude, destLat, destLong) <= delta ) {
                                insertBack(trackList, temp);                    
                            }
                        }
                    }
                    temp = nextElement(&i);
                }
                if (getLength(trackList) != 0)
                    return trackList;
                else
                    freeList(trackList);
            }
        }
    }
    return NULL;
}

/*Module 3*/
/*converts route struct to json string*/
char* routeToJSON(const Route *rt){

    if (rt != NULL) {
        char* temp;
        int len;
        char name[strlen(rt->name) + 5];
        char isLoop[6];

        if (!strcmp(rt->name,"")) {
            strcpy(name, "None");
        } else {
            strcpy(name, rt->name);
        }

        if (isLoopRoute(rt, 10)) {                        /*assuming 10 for delta value becuase it is not specified in the description*/
            strcpy(isLoop, "true");
        } else {
            strcpy(isLoop, "false");
        } 

        len = strlen(name) + strlen(isLoop) + 2*1077 + 40 ;               /*dynamically allocatting memory*/
        temp = (char*)malloc(sizeof(char)*len);
        sprintf(temp, "{\"name\":\"%s\",\"numPoints\":%d,\"len\":%0.1f,\"loop\":%s}", name, getLength(rt->waypoints), round10(getRouteLen(rt)), isLoop);

        return temp;
    }
    return "{}";
}

/*converts track struct to json string*/
char* trackToJSON(const Track *tr){
    
    if (tr != NULL) {
        char* temp;
        int len;

        char name[strlen(tr->name) + 5];
        char isLoop[6];

        if (!strcmp(tr->name,"")) {
            strcpy(name, "None");
        } else {
            strcpy(name, tr->name);
        }

        if (isLoopTrack(tr, 10)) {                    /*assuming 10 for delta value becuase it is not specified in the description*/
            strcpy(isLoop, "true");
        } else {
            strcpy(isLoop, "false");
        } 

        len = strlen(name) + strlen(isLoop) + 1077 + 27 ;               /*dynamically allocatting memory*/
        temp = (char*)malloc(sizeof(char)*len);
        sprintf(temp, "{\"name\":\"%s\",\"len\":%0.1f,\"loop\":%s}", name, round10(getTrackLen(tr)), isLoop);

        return temp;
    }
    return "{}";
}

/*converts a list of route structs to a json string*/
char* routeListToJSON(const List *list){

    if (list != NULL && getLength((List*) list) != 0) {

        char* temp;
        int len = 0;

        ListIterator i = createIterator((List*) list);
        Route* tempR = nextElement(&i);
        while(tempR != NULL) {
            char* routeJSON = routeToJSON(tempR);
            len += strlen(routeJSON);
            free(routeJSON);

            if ((tempR = nextElement(&i)) != NULL)
                len++;
        }
        
        char routesTemp[len + 3];
        strcpy(routesTemp, "[");

        ListIterator j = createIterator((List*) list);
        tempR = nextElement(&j);
        while(tempR != NULL) {
            char* routeJSON = routeToJSON(tempR);
            strcat(routesTemp, routeJSON);
            free(routeJSON);

            if ((tempR = nextElement(&j)) != NULL)
                strcat(routesTemp, ",");   
        }
        strcat(routesTemp, "]");
        
        len = strlen(routesTemp) + 2;               /*dynamically allocatting memory*/
        temp = (char*)malloc(sizeof(char)*len);
        strcpy(temp, routesTemp);

        return(temp);
    }
    return "[]";
}

/*converts a list of track structs to a json string*/
char* trackListToJSON(const List *list){

    if (list != NULL && getLength((List*) list) != 0) {
        char* temp;
        int len = 0;
        ListIterator i = createIterator((List*) list);
        Track* tempR = nextElement(&i);
        while(tempR != NULL) {
            char* routeJSON = trackToJSON(tempR);
            len += strlen(routeJSON);
            free(routeJSON);
            if ((tempR = nextElement(&i)) != NULL)
                len++;
        }
        
        char routesTemp[len + 3];
        strcpy(routesTemp, "[");

        ListIterator j = createIterator((List*) list);
        tempR = nextElement(&j);
        while(tempR != NULL) {
            char* routeJSON = trackToJSON(tempR);
            strcat(routesTemp, routeJSON);
            free(routeJSON);
            if ((tempR = nextElement(&j)) != NULL)
                strcat(routesTemp, ",");   
        }
        strcat(routesTemp, "]");
        len = strlen(routesTemp) + 2;
        temp = (char*)malloc(sizeof(char)*len);
        strcpy(temp, routesTemp);
        return(temp);
    }
    return "[]";
}

/*converts a gpxdoc structs to a json string*/
char* GPXtoJSON(const GPXdoc* gpx){
    if (gpx != NULL) {
        char* temp;
        int len;
        len = strlen(gpx->creator) + 4 * 1077 + 68;               /*dynamically allocatting memory*/
        temp = (char*)malloc(sizeof(char)*len);
        sprintf(temp, "{\"version\":%0.1f,\"creator\":\"%s\",\"numWaypoints\":%d,\"numRoutes\":%d,\"numTracks\":%d}", gpx->version, gpx->creator, getNumWaypoints(gpx), getNumRoutes(gpx), getNumTracks(gpx));
        return temp;
    }
    return "{}";
}

/* returns number of waypoints */
int getNumWaypoints(const GPXdoc* doc) {
    if (doc != NULL) { 
        return getLength(doc->waypoints);    /* using list LinkedListAPI function to get number */
    }
    return 0; 
}

/* returns number of routes */
int getNumRoutes(const GPXdoc* doc) {
    if (doc != NULL) {
        return getLength(doc->routes);    /* using list LinkedListAPI function to get number */
    }
    return 0; 
}

/* returns number of tracks */
int getNumTracks(const GPXdoc* doc) {
    if (doc != NULL) {
        return getLength(doc->tracks);    /* using list LinkedListAPI function to get number */
    }
    return 0; 
}

/* returns number of track segments */
int getNumSegments(const GPXdoc* doc) {
    if (doc != NULL) {
        int count = 0;
        ListIterator i = createIterator(doc->tracks);
        Track *temp = NULL; 
        while((temp = nextElement(&i)) != NULL) {
            count += getLength(temp->segments);           /* using list LinkedListAPI function to get number */
        }

        return count;
    }
    return 0;
}

/* returns number of track other data from waypoints */
int getNumOtherData(List * wptsList, int count) {
    ListIterator i = createIterator(wptsList);
    Waypoint *temp = NULL; 
    while((temp = nextElement(&i)) != NULL) {
        if(strcmp(temp->name,"") != 0) {           /* need to check if element name is not empty */
            count++;
        }
        count += getLength(temp->otherData);   /* using list LinkedListAPI function to get number */
    }
    return count;
}

/* returns number of track other data */
int getNumGPXData(const GPXdoc* doc) {
    if (doc != NULL) {
        int count = 0, wpts = getNumWaypoints(doc), rtes = getNumRoutes(doc), trks = getNumTracks(doc);

        if (wpts != 0) {
            count = getNumOtherData(doc->waypoints, count);
        }
        if (rtes != 0) {
            ListIterator i = createIterator(doc->routes);
            Route *temp = NULL; 
            while((temp = nextElement(&i)) != NULL) {
                if(strcmp(temp->name,"") != 0) {           /* need to check if element name is not empty */
                    count++;
                }
                count += getLength(temp->otherData);       /* using list LinkedListAPI function to get number */

                int rtwpts = getLength(temp->waypoints);
                if (rtwpts != 0) {
                    count = getNumOtherData(temp->waypoints, count);         /* using list LinkedListAPI function to get number */
                }
            }
        }
        if (trks != 0) {
            ListIterator i = createIterator(doc->tracks);
            Track *temp = NULL; 
            while((temp = nextElement(&i)) != NULL) {
                if(strcmp(temp->name,"") != 0) {             /* need to check if element name is not empty */
                    count++;
                }
                count += getLength(temp->otherData);           /* using list LinkedListAPI function to get number */

                int segs = getNumSegments(doc);
                if (segs != 0) {
                    ListIterator j = createIterator(temp->segments);
                    TrackSegment *tempTS = NULL;
                    while((tempTS = nextElement(&j)) != NULL) {

                        int trkwpts = getLength(tempTS->waypoints);
                        if (trkwpts != 0) {
                            count = getNumOtherData(tempTS->waypoints, count);    /* using list LinkedListAPI function to get number */
                        }
                    }
                }
            }
        }
        return count;
    }
    return 0;
}

/* returns a specific waypoint */
Waypoint* getWaypoint(const GPXdoc* doc, char* name) {
    if(name != NULL && doc != NULL) {
        ListIterator i = createIterator(doc->waypoints);
        Waypoint *temp = nextElement(&i);
        while(temp != NULL) {
            if(strcmp(temp->name, name) == 0) {
                return temp;
            } 
            temp = nextElement(&i);      
        }
    }
    return NULL;
}

/* returns a specific route */
Route* getRoute(const GPXdoc* doc, char* name) {
    if(name != NULL && doc != NULL) {
        ListIterator i = createIterator(doc->routes);
        Route *temp = nextElement(&i);
        while(temp != NULL) {
            if(strcmp(temp->name, name) == 0) {
                return temp;
            } 
            temp = nextElement(&i);      
        }
    }
    return NULL;
}

/* returns a specific track */
Track* getTrack(const GPXdoc* doc, char* name) {
    if(name != NULL && doc != NULL) {
        ListIterator i = createIterator(doc->tracks);
        Track *temp = nextElement(&i);
        while(temp != NULL) {
            if(strcmp(temp->name, name) == 0) {
                return temp;
            } 
            temp = nextElement(&i);      
        }
    }
    return NULL;
}

/* creates a GPXData struct for the OtherData for the GPXdoc */
GPXData * setOtherData(xmlNode *elements) {
    char *temp = (char *)xmlNodeGetContent(elements);
    GPXData *other = (GPXData*) malloc(sizeof(GPXData) + sizeof(char) * strlen(temp) + 1); /*dynamically allocatting memory*/
    memcpy(other->name,(char *)elements->name, sizeof(other->name));
    strcpy(other->value, temp);
    free(temp);
    return other;
}

/* creates a Waypoint struct for the different waypoints in the GPXdoc */
Waypoint * setWaypoints(xmlNode *curNode) {
    Waypoint* wpt = (Waypoint*)calloc(1, sizeof(Waypoint));
    wpt->name = (char*)calloc(1000, sizeof(char));

    for (xmlAttr *prop = curNode->properties; prop != NULL; prop = prop->next) {     /* checks for the attributes */
        if(xmlStrcmp(prop->name, (const xmlChar *)"lat") == 0) {
            wpt->latitude = atof((const char *)(prop->children->content));
        } else if(xmlStrcmp(prop->name, (const xmlChar *)"lon") == 0) {
            wpt->longitude = atof((const char *)(prop->children->content));
        }
    }

    wpt->otherData = initializeList(&gpxDataToString, &deleteGpxData, &compareGpxData);
    
    if (curNode->children != NULL) {
        for(xmlNode *elements = curNode->children->next; elements != NULL; elements = elements->next->next) {    /* checks for the elements */
            if(elements->type == XML_ELEMENT_NODE) {
                if(xmlStrcmp(elements->name, (const xmlChar *)"name") == 0) {
                    char * name = (char *)xmlNodeGetContent(elements);
                    strcpy(wpt->name, name);
                    free(name);
                } else {
                    GPXData *other = setOtherData(elements);
                    insertBack(wpt->otherData, (void*)other);
                }    
            }
        }
    }
    return wpt;
}


/* creates a GPXdoc struct from an xml file */
GPXdoc *createGPXdoc(char *fileName) {
    xmlDoc *doc = NULL;
    xmlNode *root_element = NULL;

    if (fileName == NULL)
        return NULL;

    FILE *testFile;
    if (!(testFile = fopen(fileName, "r")))
        return NULL;
    fclose(testFile);

    LIBXML_TEST_VERSION
    doc = xmlReadFile(fileName, NULL, 0);

    if (doc == NULL) 
        return (NULL);

    root_element = xmlDocGetRootElement(doc);
    GPXdoc *gdoc = (GPXdoc*)calloc(1, sizeof(GPXdoc));
    strcpy(gdoc->namespace, (char *)root_element->ns->href);


    while (root_element != NULL) {
        if (xmlStrcmp(root_element->name, (const xmlChar *)"gpx") == 0) {
            char *version = (char *)xmlGetProp(root_element, (const xmlChar *)"version");     /* used http://xmlsoft.org/html/libxml-tree.html#xmlGetProp */
            gdoc->version = atof(version);
            free(version);
            char *creator = (char *)xmlGetProp(root_element, (const xmlChar *)"creator");
            gdoc->creator = (char*)malloc(sizeof(char)*strlen(creator)+1);
            strcpy(gdoc->creator, creator);
            free(creator);                           
        }
        root_element = root_element->next;
    }
    gdoc->waypoints = initializeList(&waypointToString, &deleteWaypoint, &compareWaypoints);
    gdoc->routes = initializeList( &routeToString, &deleteRoute, &compareRoutes);
    gdoc->tracks = initializeList(&trackToString, &deleteTrack, &compareTracks);

    xmlNode *root = xmlDocGetRootElement(doc);

    
    xmlNode *node = root->children->next;
    xmlNode *curNode = NULL;

    if (node == NULL) {
        // xmlFreeDoc(doc);
        // deleteGPXdoc(gdoc);
        // xmlCleanupParser();
        // return NULL;
        xmlFreeDoc(doc);
        xmlCleanupParser();
        return gdoc;
    }

    if (node->children != NULL) {
        for(curNode = node; curNode != NULL; curNode = curNode->next->next) {
            
            if(curNode->type == XML_ELEMENT_NODE) {
                if (xmlStrcmp(curNode->name, (const xmlChar *)"wpt") == 0) {                /* used http://www.xmlsoft.org/html/libxml-xmlstring.html#xmlStrcmp */
                    Waypoint *wpt = setWaypoints(curNode);
                    insertBack(gdoc->waypoints, (void*)wpt);

                } else if(xmlStrcmp(curNode->name, (const xmlChar *)"rte") == 0) {
                    Route *rte = (Route*)calloc(1, sizeof(Route));
                    rte->name = (char*)calloc(1000, sizeof(char));
                    rte->waypoints = initializeList(&waypointToString, &deleteWaypoint, &compareWaypoints);
                    rte->otherData = initializeList(&gpxDataToString, &deleteGpxData, &compareGpxData);
                    
                    if (curNode->children != NULL) {
                        for(xmlNode *elements = curNode->children->next; elements != NULL; elements = elements->next->next) {
                            if(elements->type == XML_ELEMENT_NODE) {
                                if (xmlStrcmp(elements->name, (const xmlChar *)"name") == 0) {      
                                    char * name = (char *)xmlNodeGetContent(elements);
                                    strcpy(rte->name, name);
                                    free(name);
                                } else if (xmlStrcmp(elements->name, (const xmlChar *)"rtept") == 0) {
                                    Waypoint* routeP = setWaypoints(elements);
                                    insertBack(rte->waypoints, (void*)routeP);
                                } else {
                                    GPXData *other = setOtherData(elements);
                                    insertBack(rte->otherData, (void*)other);
                                }
                            }
                        }
                    }
                    insertBack(gdoc->routes, (void*)rte);

                } else if (xmlStrcmp(curNode->name, (const xmlChar *)"trk") == 0) {
                    Track *trk = (Track*)calloc(1, sizeof(Track));
                    trk->name = (char*)calloc(1000, sizeof(char));
                    trk->segments = initializeList(&trackSegmentToString, &deleteTrackSegment, &compareTrackSegments);
                    trk->otherData = initializeList(&gpxDataToString, &deleteGpxData, &compareGpxData);
                    
                    if (curNode->children != NULL) {
                        for(xmlNode *elements = curNode->children->next; elements != NULL; elements = elements->next->next) {
                            if(elements->type == XML_ELEMENT_NODE) {
                                if (xmlStrcmp(elements->name, (const xmlChar *)"name") == 0) {
                                    char * name = (char *)xmlNodeGetContent(elements);
                                    strcpy(trk->name, name);
                                    free(name);
                                } else if (xmlStrcmp(elements->name, (const xmlChar *)"trkseg") == 0) {

                                    TrackSegment *trkseg = (TrackSegment*)calloc(1, sizeof(TrackSegment));
                                    trkseg->waypoints = initializeList(&waypointToString, &deleteWaypoint, &compareWaypoints);

                                    if (elements->children != NULL) {
                                        for(xmlNode *trkelement = elements->children->next; trkelement != NULL; trkelement = trkelement->next->next) {
                                            if(trkelement->type == XML_ELEMENT_NODE) {
                                                if (xmlStrcmp(trkelement->name, (const xmlChar *)"trkpt") == 0) {
                                                    Waypoint* trkpt = setWaypoints(trkelement);
                                                    insertBack(trkseg->waypoints, (void*)trkpt);
                                                }
                                            }
                                        }
                                    }
                                    insertBack(trk->segments, (void*)trkseg);

                                } else {
                                    GPXData *other = setOtherData(elements);
                                    insertBack(trk->otherData, (void*)other);
                                }
                            }
                        }
                    }
                    insertBack(gdoc->tracks, (void*)trk);                    
                } else {
                    xmlFreeDoc(doc);
                    deleteGPXdoc(gdoc);
                    xmlCleanupParser();
                    return NULL;
                }
            }
        }
    }
    xmlFreeDoc(doc);
    xmlCleanupParser();
    return gdoc;
}

/* toString for entire GPXdoc */
char* GPXdocToString(GPXdoc* doc) {
    if (doc != NULL) {
        char* temp;
        char *wpts = toString(doc->waypoints);
        char *rtes = toString(doc->routes);
        char *trks = toString(doc->tracks);
        //double can be 1077 digits each
        int len;
        len = strlen(doc->namespace) + strlen(doc->creator) + strlen(wpts) + strlen(rtes) + strlen(trks) + 1077 + 49;
        temp = (char*)malloc(sizeof(char)*len);
        sprintf(temp, "GPXdoc { namespace: %s, version: %f, creator: %s\n%s\n%s\n%s }\n", doc->namespace, doc->version, doc->creator, wpts, rtes, trks);
        free(wpts);
        free(rtes);
        free(trks);
        return temp;
    }
    return NULL;
}

/* deletes and frees memory for gpx doc using helper functions */
void deleteGPXdoc(GPXdoc* doc) {
    if(doc != NULL) {
        free(doc->creator);
        freeList(doc->waypoints);
        freeList(doc->routes);
        freeList(doc->tracks);
        free(doc);
    }
}

char* otherToJSON(const GPXData *rt){

    if (rt != NULL) {
        char* temp;
        int len;
        char name[strlen(rt->name) + 5];
        char value[strlen(rt->value) + 5];

        if (!strcmp(rt->name,"")) {
            strcpy(name, "None");
        } else {
            strcpy(name, rt->name);
        }
        if (!strcmp(rt->value,"")) {
            strcpy(value, "None");
        } else {
            strcpy(value, rt->value);
        }

        len = strlen(name) + strlen(value) + 40 ;               /*dynamically allocatting memory*/
        temp = (char*)malloc(sizeof(char)*len);
        sprintf(temp, "{\"name\":\"%s\",\"value\":\"%s\"}", name, value);

        return temp;
    }
    return "{}";
}

char* otherListToJSON(const List *list){

    if (list != NULL && getLength((List*) list) != 0) {

        char* temp;
        int len = 0;

        ListIterator i = createIterator((List*) list);
        GPXData* tempR = nextElement(&i);
        while(tempR != NULL) {
            char* routeJSON = otherToJSON(tempR);
            len += strlen(routeJSON);
            free(routeJSON);

            if ((tempR = nextElement(&i)) != NULL)
                len++;
        }
        
        char routesTemp[len + 3];
        strcpy(routesTemp, "[");

        ListIterator j = createIterator((List*) list);
        tempR = nextElement(&j);
        while(tempR != NULL) {
            char* routeJSON = otherToJSON(tempR);
            strcat(routesTemp, routeJSON);
            free(routeJSON);

            if ((tempR = nextElement(&j)) != NULL)
                strcat(routesTemp, ",");   
        }
        strcat(routesTemp, "]");
        
        len = strlen(routesTemp) + 2;               /*dynamically allocatting memory*/
        temp = (char*)malloc(sizeof(char)*len);
        strcpy(temp, routesTemp);

        return(temp);
    }
    return "[]";
}

char* waypointToJSON(const Waypoint *rt){

    if (rt != NULL) {
        char* temp;
        int len;
        char name[strlen(rt->name) + 5];

        if (!strcmp(rt->name,"")) {
            strcpy(name, "None");
        } else {
            strcpy(name, rt->name);
        }

        len = strlen(name) + 40 + 1077*2;               /*dynamically allocatting memory*/
        temp = (char*)malloc(sizeof(char)*len);
        sprintf(temp, ",\"name\":\"%s\",\"lat\":%f,\"lon\":%f}", name, rt->latitude, rt->longitude);

        return temp;
    }
    return "{}";
}

char* waypointListToJSON(const List *list) {
    char pointIndexValue[50];
    if (list != NULL && getLength((List*) list) != 0) {

        char* temp;
        int len = 0;

        ListIterator i = createIterator((List*) list);
        Route* tempR = nextElement(&i);

        while(tempR != NULL) {
            char routeJSON[10000];
            strcpy(routeJSON, "{");
            strcat(routeJSON, "\"routename\"");
            strcat(routeJSON, ":\"");
            if (!strcmp(tempR->name,"")) {
                strcat(routeJSON, "None ");
            } else {
                strcat(routeJSON, tempR->name);
            }
            strcat(routeJSON, "\",");
            strcat(routeJSON, "\"waypoint\"");
            strcat(routeJSON, ":[");

            ListIterator ii = createIterator((List*) tempR->waypoints);
            Waypoint * tempW = nextElement(&ii);
            int countW = 0;
            while(tempW != NULL) {

                strcat(routeJSON, "{");
                strcat(routeJSON, "\"waypointindex\"");
                strcat(routeJSON, ":");
                sprintf(pointIndexValue, "%d", countW);
                strcat(routeJSON, pointIndexValue);
                
                strcat(routeJSON, waypointToJSON(tempW));

                if ((tempW = nextElement(&ii)) != NULL)
                    len++;
                countW++;
            }
            strcat(routeJSON, "]}");
            len += strlen(routeJSON);

            if ((tempR = nextElement(&i)) != NULL)
                len++;
        }
        
        char routesTemp[len + 3];
        strcpy(routesTemp, "[");
        
        ListIterator j = createIterator((List*) list);
        tempR = nextElement(&j);
        while(tempR != NULL) {
            strcat(routesTemp, "{");
            strcat(routesTemp, "\"routename\"");
            strcat(routesTemp, ":\"");
            if (!strcmp(tempR->name,"")) {
                strcat(routesTemp, "None");
            } else {
                strcat(routesTemp, tempR->name);
            }
            strcat(routesTemp, "\",");
            strcat(routesTemp, "\"waypoint\"");
            strcat(routesTemp, ":[");

            ListIterator ii = createIterator((List*) tempR->waypoints);
            Waypoint * tempW = nextElement(&ii);
            int countW = 0;
            while(tempW != NULL) {

                strcat(routesTemp, "{");
                strcat(routesTemp, "\"waypointindex\"");
                strcat(routesTemp, ":");
                sprintf(pointIndexValue, "%d", countW);
                strcat(routesTemp, pointIndexValue);
                
                strcat(routesTemp, waypointToJSON(tempW));

                if ((tempW = nextElement(&ii)) != NULL)
                    strcat(routesTemp, ",");

                countW++;
            }
            strcat(routesTemp, "]}");
            
            if ((tempR = nextElement(&j)) != NULL)
                strcat(routesTemp, ",");   
        }
        strcat(routesTemp, "]");
        
        len = strlen(routesTemp) + 2;               /*dynamically allocatting memory*/
        temp = (char*)malloc(sizeof(char)*len);
        strcpy(temp, routesTemp);

        return(temp);
    }
    return "[]";
}


char * validateGPXtoJSON (char* filename) { 
    GPXdoc * gdoc = createGPXdoc(filename);
    if (validateGPXDoc(gdoc, "gpx.xsd")) {
        char * str = GPXtoJSON(gdoc);
        deleteGPXdoc(gdoc);	
        return str;
    } else {
        return "Invalid";
    } 
}


char * makeOtherJsonRoute(char* filename, char* routename) {
    GPXdoc * gdoc = createGPXdoc(filename);
    Route * route = getRoute(gdoc, routename);
    char * jsonOther = otherListToJSON(route->otherData);
    deleteGPXdoc(gdoc);
    return jsonOther;
}


char * makeOtherJsonTrack(char* filename, char* routename) {
    GPXdoc * gdoc = createGPXdoc(filename);
    Track * route = getTrack(gdoc, routename);
    char * jsonOther = otherListToJSON(route->otherData);
    deleteGPXdoc(gdoc);
    return jsonOther;
}

char * waypointToComponentsJson(char* filename) {
    GPXdoc * gdoc = createGPXdoc(filename);
    char * componentJson = waypointListToJSON(gdoc->routes);
    deleteGPXdoc(gdoc);
    return componentJson;
}


char * routeToComponentsJson(char* filename) {
    GPXdoc * gdoc = createGPXdoc(filename);
    char * componentJson = routeListToJSON(gdoc->routes);
    deleteGPXdoc(gdoc);
    return componentJson;
}


char * trackToComponentsJson(char* filename) {
    GPXdoc * gdoc = createGPXdoc(filename);
    char * componentJson = trackListToJSON(gdoc->tracks);
    deleteGPXdoc(gdoc);
    return componentJson;
}


char * renameRoute(char* filename, char* newroutename, int idx) {

    GPXdoc * gdoc = createGPXdoc(filename);    
    ListIterator i = createIterator(gdoc->routes);
    Route *temp = nextElement(&i); 
    int count = 0;
    int startCount = 1;
    while(temp != NULL) {
        if (count + (startCount - 1) == idx) {
            strcpy(temp->name, newroutename);
        } else {
            count = count*startCount;
        }
        count++;
        temp = nextElement(&i);
    }
    
    if (writeGPXdoc(gdoc, filename)) {
        deleteGPXdoc(gdoc);
        return " ";
    } else {
        deleteGPXdoc(gdoc);
        return " ";
    }
}


char * renameTrack(char* filename, char* newroutename, int idx) {

    GPXdoc * gdoc = createGPXdoc(filename);    
    ListIterator i = createIterator(gdoc->tracks);
    Track *temp = nextElement(&i); 
    int count = 0;
    int startCount = 1;
    while(temp != NULL) {
        if (count + (startCount - 1) == idx) {
            strcpy(temp->name, newroutename);
        } else {
            count = count*startCount;
        }
        count++;
        temp = nextElement(&i);
    }
    
    if (writeGPXdoc(gdoc, filename)) {
        deleteGPXdoc(gdoc);
        return " ";
    } else {
        deleteGPXdoc(gdoc);
        return " ";
    }
}


char * makeGPXjson (char* filename, char* creator, float version) {
    
    GPXdoc *gdoc = (GPXdoc*)calloc(1, sizeof(GPXdoc));
    gdoc->creator = (char*)malloc(sizeof(char)*strlen(creator)+1);
    strcpy(gdoc->creator, creator);
    gdoc->version = 1.1; 
    strcpy(gdoc->namespace,  "http://www.topografix.com/GPX/1/1");
    gdoc->waypoints = initializeList(&waypointToString, &deleteWaypoint, &compareWaypoints);
    gdoc->routes = initializeList( &routeToString, &deleteRoute, &compareRoutes);
    gdoc->tracks = initializeList(&trackToString, &deleteTrack, &compareTracks);
    gdoc->waypoints->length = 0;
    gdoc->routes->length = 0;
    gdoc->tracks->length = 0;

    if (writeGPXdoc(gdoc, filename)) {
        deleteGPXdoc(gdoc);
        return filename;
    } else {
        deleteGPXdoc(gdoc);
        return "Invalid";
    }
}


char * makeRouteJson (char* filename, char* routename, int addWpt, char* waypointname, float lon, float lat) {

    GPXdoc * gdoc = createGPXdoc(filename);
    Route *rte = getRoute(gdoc, routename);

    if (rte == NULL) {
        Route * newrte = (Route*)calloc(1, sizeof(Route));
        newrte->name = (char*)calloc(1000, sizeof(char));
        strcpy(newrte->name, routename);
        newrte->waypoints = initializeList(&waypointToString, &deleteWaypoint, &compareWaypoints);
        newrte->otherData = initializeList(&gpxDataToString, &deleteGpxData, &compareGpxData);
        insertBack(gdoc->routes, (void*)newrte);
    }
    
    if (addWpt) {
        rte = getRoute(gdoc, routename);
        Waypoint* wpt = (Waypoint*)calloc(1, sizeof(Waypoint));
        wpt->name = (char*)calloc(1000, sizeof(char));
        strcpy(wpt->name, waypointname);
        wpt->latitude = lon;
        wpt->longitude = lat;
        wpt->otherData = initializeList(&gpxDataToString, &deleteGpxData, &compareGpxData);
        insertBack(rte->waypoints, (void*)wpt);
    }
    
    if (writeGPXdoc(gdoc, filename)) {
        deleteGPXdoc(gdoc);
        return " ";
    } else {
        deleteGPXdoc(gdoc);
        return " ";
    }
}


char * RoutesBetweentoJson(char* filename, float startLon, float startlat, float endLon, float endLat, float delta) {
    GPXdoc * gdoc = createGPXdoc(filename);
    List * paths = getRoutesBetween(gdoc, startlat, startLon, endLat, endLon, delta);
    char * pathJson = routeListToJSON(paths);
    deleteGPXdoc(gdoc);
    return pathJson;
}


char * TracksBetweentoJson(char* filename, float startLon, float startlat, float endLon, float endLat, float delta) {
    GPXdoc * gdoc = createGPXdoc(filename);
    List * paths = getTracksBetween(gdoc, startlat, startLon, endLat, endLon, delta);
    char * pathJson = trackListToJSON(paths);
    deleteGPXdoc(gdoc);
    return pathJson;
}
