#include <stdlib.h>
#include <cstring>
#include <fstream>
#include <string>
#include <sstream>
#include <stdexcept>
#include <sqlite3.h>
#include <vector>
#include "ReportReader.h"
#include "SiteCoords.h"
#include "StaticSiteInfo.h"
#include "classify_runs.h"
#include "utils.h"

using namespace std;

/****************************************************************
 * Scheme specific information
 ***************************************************************/

class SchemeData {
public:
    int minFieldID;
    int numFields;

    SchemeData(int min_t, int num_t)
        : minFieldID(min_t), numFields(num_t)
    {}
};

SchemeData *SchemeMap = NULL;


/****************************************************************
 * ReportReader functions
 ***************************************************************/

ReportReader::ReportReader(const char* filename) {
    int rc;

    classify_runs();

    dbfile = string(filename);
    rc = sqlite3_open(filename, &db);
    if(rc)
        throwError(sqlite3_errmsg(db), __FILE__, __LINE__);

    // Collect Scheme specific information
    char query[] = "SELECT Schemes.SchemeID, min(FieldID), count(FieldID)\
                    FROM\
                        Schemes JOIN Fields\
                    ON\
                        Schemes.SchemeID=Fields.SchemeID\
                    GROUP BY\
                        Schemes.SchemeID";
    char **result, *errMsg;
    int nRows, nCols;
    rc = sqlite3_get_table(db, query, &result, &nRows, &nCols, &errMsg);

    if(rc)
        throwError(errMsg, __FILE__, __LINE__);
    if(nCols != 3)
        throwError("Unexpected number of columns", __FILE__, __LINE__);

    SchemeMap = (SchemeData *) malloc((nRows + 1) * sizeof(SchemeData));
    for(int row = 0; row < nRows; ++ row) {
        int base = nCols * (row + 1);
        int SchemeID = atoi(result[base]);
        int minFieldID = atoi(result[base + 1]);
        int numFields = atoi(result[base + 2]);

        SchemeMap[SchemeID] = SchemeData(minFieldID, numFields);
    }

    sqlite3_free_table(result);
}

void ReportReader::read(unsigned runId) {
    if (!is_srun[runId] && !is_frun[runId]) {
        ostringstream message;
        message << "Ill-formed run " << runId;
        throwError(message.str().c_str(), __FILE__, __LINE__);
    }

    /*  TODO Check this error condition using the database
    if (runId >= TotalRuns.size()) {
        ostringstream message;
        message << runId << " too large.";
        throwError(message.str().c_str(), __FILE__, __LINE__);
    }
    */

    char *query, **result, *errMsg;
    int nRows, nCols, rc;

    query = sqlite3_mprintf("SELECT SampleCounts.SiteID, SchemeID, FieldID, Count\
                             FROM\
                                 SampleCounts JOIN Sites JOIN Units ON\
                                     SampleCounts.SiteID=Sites.SiteID AND\
                                     Sites.UnitID=Units.UnitID\
                             WHERE\
                                 RunID=%d\
                             ORDER BY\
                                 SampleCounts.SiteID",
                            runId);
    if(query == NULL)
        throwError("sqlite3_mprintf failed", __FILE__, __LINE__);

    rc = sqlite3_get_table(db, query, &result, &nRows, &nCols, &errMsg);
    if(rc)
        throwError(errMsg, __FILE__, __LINE__);
    if(nCols != 4)
        throwError("Unexpected number of columns", __FILE__, __LINE__);

    vector<count_tp> counts;
    int cur_site = -1;

    for(int row = 0; row < nRows; ++ row) {
        int base = nCols * (row + 1);
        int siteID = atoi(result[base]);
        int schemeID = atoi(result[base + 1]);
        int fieldID = atoi(result[base + 2]);
        int count = atoi(result[base + 3]);

        if(siteID != cur_site) {
            // handle previous site
            if(cur_site != -1)
                handleSite(SiteCoords(cur_site), counts);

            // setup new site
            cur_site = siteID;
            counts.clear();
            counts.resize(SchemeMap[schemeID].numFields, 0);
        }

        int actualFieldID = fieldID - SchemeMap[schemeID].minFieldID;
        counts[actualFieldID] = count;
    }

    sqlite3_free_table(result);
    sqlite3_free(query);
}


// Print error message and throw runtime error
void ReportReader::throwError(const char *msg, const char *file, int line) {
    sqlite3_close(db);
    ostringstream message;
    message << "Error at " << file << ":" << line;
    message << ".  Message: " << msg;
    throw runtime_error(message.str());
}

