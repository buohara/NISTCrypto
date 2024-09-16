#include "db.h"

/**
 * Table::Select - Read values from a table given some filter.
 *
 * @param cols      [in] List of column values to retrieve.
 * @param pred      [in] List of predicates, or filters.
 * @param results   [in/out] Output results to populate.
 */

void Table::Select(const vector<string> cols, const vector<pair<string, string>>& pred, 
    vector<vector<uint8_t>>& results)
{

}

/**
 * Table::Insert - Insert a new list of values into the table.
 *
 * @param vals  [in] List of new values to add to table.
 */

void Table::Insert(const vector<uint8_t>& vals)
{

}

/**
 * Table::Delete - Remove rows from a table.
 *
 * @param pred    [in] List of predicates or filters for rows to deletes.
 */

void Table::Delete(vector<pair<string, string>>& pred)
{

}

/**
 * Table::Update - Reverse a string. Helper function since BigInts store LSBs
 * in low indices, while integer strings store them in high indices.
 *
 * @param in    [in]        String to reverse.
 */

void Table::Update(const vector<uint8_t>& vals, const vector<pair<string, string>>& pred)
{

}

/**
 * Table::ReadHeader - Read a table header from file.
 *
 * @param pFile    [in] File to parse for table header.
 */

void Table::ReadHeader(FILE *pFile)
{

}

/**
 * Table::WriteHeader - Write a table header to file.
 *
 * @param in    [in]        String to reverse.
 *
 * @return      Reversed string.
 */

void Table::WriteHeader(FILE* pFile, const bool bCompress)
{
    map<DataType, string> dataTypeStrings =
    {
        { UINT8,    "UINT8" },
        { UINT16,   "UINT16" },
        { UINT32,   "UINT32" },
        { UINT64,   "UINT64" },
        { FLOAT32,  "FLOAT32" },
        { FLOAT64,  "FLOAT64" },
        { STRING,   "STRING" }
    };

    string header = bCompress ? "1 " : "0 ";

    for (uint64_t i = 0; i < layout.size(); i++)
    {
        header = header + layout[i].first + "," + dataTypeStrings[layout[i].second] + 
            ((i == layout.size() - 1) ? "," : "");
    }

    header = header + "\n";

    fputs(header.c_str(), pFile);
}

/**
 * Table::LoadFromFile - Load a table from file.
 *
 * @param fileName  [in] Name of file to load from.
 */

void Table::LoadFromFile(const string fileName)
{
    if (fileName == "")
        throw invalid_argument("Database file path input was empty trying to load database from file.");

    FILE* pFile = fopen(fileName.c_str(), "r");

    if (!pFile)
        throw invalid_argument("Unable to open file for database load: " + fileName);
}

/**
 * Table::StoreToFile - Write a table to file.
 *
 * @param fileName  [in] Name of file to write to.
 * @param bCompress [in] True if table should be compressed before writing.
 */

void Table::StoreToFile(const string fileName, const bool bCompress = false)
{
    if (fileName == "")
        throw invalid_argument("Database file path input was empty trying to save database to file.");

    FILE* pFile = fopen(fileName.c_str(), "w");

    if (!pFile)
        throw invalid_argument("Unable to open file for database store: " + fileName);
}