#pragma once

#include "commoninc.h"
using namespace std;

enum DataType
{
    UINT8,
    UINT16,
    UINT32,
    UINT64,
    FLOAT32,
    FLOAT64,
    STRING
};

struct TableHeader
{
    bool bCompressed;
    vector<string> layout;
};

/**
 * Some example commands:
 * 
 * SELECT CustomerName, City FROM Customers;
 * 
 * SELECT * FROM Customers WHERE Country='Mexico';
 * 
 * INSERT INTO table_name (column1, column2, column3, ...) VALUES (value1, value2, value3, ...); 
 * (column names after table_name are optional if only updating specific columns)
 * 
 * DELETE FROM table_name WHERE condition;
 * 
 * UPDATE Customers SET ContactName = 'Alfred Schmidt', City= 'Frankfurt' WHERE CustomerID = 1;
 */

struct Table
{
    vector<pair<string, DataType>> layout;
    uint64_t rowSize;

    map<string, uint64_t> colOffsets;

    vector<vector<uint8_t>> rows;
    string name;

    Table() : name("") {};

    void Select(const vector<string> cols, const vector<pair<string, string>> &pred,
        vector<vector<uint8_t>> &results);
    
    void Insert(const vector<uint8_t> &vals);
    void Delete(vector<pair<string, string>>& pred);
    void Update(const vector<uint8_t>& vals, const vector<pair<string, string>>& pred);

    void WriteHeader(FILE* pFile, const bool bCompress);
    void ReadHeader(FILE* pFile);

    void LoadFromFile(string fileName);
    void StoreToFile(string fileName, bool bCompressed);
};