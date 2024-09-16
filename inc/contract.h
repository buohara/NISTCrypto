#pragma once

#include "commoninc.h"

using namespace std;

/**
 * Usage notes:
 *
 * The goal is for individual users to have a database of goods and services they can offer.
 * Items in this database can be offered publicly, or kept hidden.
 * 
 * Users will also keep a database of receipts for all contracts. There should be some
 * functionality to filer contracts in negotiation, pending, etc.
 * 
 * Database transactions will be hashed against the database's most recent transaction hash
 * along with user IDs and receipt codes from the new transation.
 * 
 * New contracts involving some item A cannot be opened if another open contract involving
 * A are already open. No double sales.
 */

enum ContractStatus
{
    OPENED,
    NEGOTIATION_PENDING,
    RECEIPT_PENDING,
    DISPUTE_PENDING,
    CLOSED
};

struct Term
{
    string item;
    uint64_t quantity;

    string senderName;
    vector<uint8_t> senderID;

    string recipientName;
    vector<uint8_t> recipientID;
};

struct Contract
{
    void Propose(vector<Term> &terms);
    void Counter(vector<Term>& terms);
    void Accept();

    void GenerateReceiptCode(vector<uint8_t> &code);
    void SendReceiptCode(vector<uint8_t>& code);
    void AcceptReceipt();
    void Close();

};