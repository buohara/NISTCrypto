#include "diff.h"

/**
 * StringToByteVec - Convert a string to a raw byte array.
 *
 * @param s     [in]        Input string to convert.
 * @param b     [in/out]    Output array with string bytes.
 */

void StringToByteVec(const string& s, vector<uint8_t>& b)
{
    if (s.size() == 0)
        return;

    b.resize(s.size());
    memcpy(&b[0], &s[0], s.size());
}

/**
 * ByteVecToString - Convert a byte array to a string.
 *
 * @param b     [in]        Input byte array to convert.
 * @param s     [in/out]    Output string.
 */

void ByteVecToString(const vector<uint8_t>& b, string& s)
{
    if (b.size() == 0)
        return;

    s.resize(b.size());
    memcpy(&s[0], &b[0], b.size());
}

/**
 * BuildLCSTable - Build an LCS table for input byte arrays. The table can then
 * be backtracked to find common subsequences and differences.
 *
 * @param s1        [in]        First input array to diff.
 * @param s2        [in]        Second input array to diff.
 * @param lcsTable  [in/out]    LCS table to populate.
 */

static void BuildLCSTable(const vector<uint8_t>& s1, const vector<uint8_t>& s2, vector<vector<LCSEntry>>& lcsTable)
{
    if (lcsTable.size() != 0)
        throw invalid_argument("Expected input LCS table to be empty when building LCS tables.");

    const uint64_t n = s1.size();
    const uint64_t m = s2.size();

    lcsTable.resize(n + 1);
    for (uint64_t i = 0; i < n + 1; i++)
        lcsTable[i].resize(m + 1);

    vector<uint8_t> s1Tmp   = s1;
    s1Tmp.insert(s1Tmp.begin(), 0);

    vector<uint8_t> s2Tmp   = s2;
    s2Tmp.insert(s2Tmp.begin(), 0);

    for (uint64_t i = 0; i < n + 1; i++)
    {
        if (i == 0)
            continue;

        for (uint64_t j = 0; j < m + 1; j++)
        {
            if (j == 0)
                continue;

            if (s1Tmp[i] == s2Tmp[j])
            {
                lcsTable[i][j] = LCSEntry(DIAG, lcsTable[i - 1][j - 1].len + 1);
                continue;
            }

            if (lcsTable[i - 1][j].len >= lcsTable[i][j - 1].len)
                lcsTable[i][j] = LCSEntry(UP, lcsTable[i - 1][j].len);
            else
                lcsTable[i][j] = LCSEntry(LEFT, lcsTable[i][j - 1].len);
        }
    }
}

/**
 * BacktraceLCSTable - Given a populated LCS table, backtrack from the end to
 * get the longest common sequence. When gaps are encountered, add them to a list
 * of diffs.
 *
 * @param s1        [in]        First array to diff.
 * @param s2        [in]        Second array to diff.
 * @param lcsTable  [in]        LCS table to traverse to extract LCS of two inputs.
 * @param lcs       [in/out]    Output lcs array. Assumed empty on input.
 * @param diffs     [in/out]    Output list of differences. Assumed empty on input.
 */

static void BacktraceLCSTable(const vector<uint8_t>& s1, const vector<uint8_t>& s2, 
    const vector<vector<LCSEntry>>& lcsTable, vector<uint8_t> &lcs, vector<Diff> &diffs)
{
    const uint64_t n = s1.size();
    const uint64_t m = s2.size();

    if (lcs.size() != 0)
        throw invalid_argument("Expected LCS array to be empty on input when backtracking LCS table.");

    if (diffs.size() != 0)
        throw invalid_argument("Expected diff list to be empty on input when backtracking LCS table.");

    if (n != lcsTable.size() - 1 || m != lcsTable[0].size() - 1)
        throw invalid_argument("Mismatch between LCS table dimensions and input array sizes when backtracking LCS table.");

    vector<uint8_t> s1Tmp = s1;
    s1Tmp.insert(s1Tmp.begin(), 0);

    vector<uint8_t> s2Tmp = s2;
    s2Tmp.insert(s2Tmp.begin(), 0);

    uint64_t i = n;
    uint64_t j = m;
    uint64_t k = lcsTable[i][j].len;

    Diff curDiff;
    lcs.resize(k--);

    while (1)
    {
        if (lcsTable[i][j].dir == EMPTY)
            break;

        if (lcsTable[i][j].dir == DIAG)
        {
            lcs[k--] = s1Tmp[i];
            i--;
            j--;
            
            if (curDiff.d1.size() != 0 || curDiff.d2.size() != 0)
            {
                curDiff.offset = k + 1;
                diffs.insert(diffs.begin(), curDiff);

                curDiff.d1.resize(0);
                curDiff.d2.resize(0);
                curDiff.offset = 0;
            }

            continue;
        }

        if (lcsTable[i][j].dir == UP)
        {
            curDiff.d1.insert(curDiff.d1.begin(), s1Tmp[i]);
            i--;
        }
        else
        {
            curDiff.d2.insert(curDiff.d2.begin(), s2Tmp[j]);
            j--;
        }
    }
}

/**
 * LCS - Get the longest common subsequence of two byte arrays and a list of diffs
 * between the two.
 *
 * @param s1      [in]        First input array to diff.
 * @param s2      [in]        Second input array to diff.
 * @param lcs     [in/out]    Longest common subsequence of the two inputs.
 * @param diffs   [in/out]    List of differences within the common subsequence.
 */

void LCS(const vector<uint8_t>& s1, const vector<uint8_t>& s2, vector<uint8_t>& lcs, vector<Diff>& diffs)
{
    if (lcs.size() != 0)
        throw invalid_argument("Expected LCS array to be empty on input when comparing input arrays.");

    if (diffs.size() != 0)
        throw invalid_argument("Expected diff list to be empty on input when comparing input arrays.");

    if (s1.size() == 0 || s2.size() == 0)
        return;

    vector<vector<LCSEntry>> lcsTable;
    BuildLCSTable(s1, s2, lcsTable);
    BacktraceLCSTable(s1, s2, lcsTable, lcs, diffs);
}