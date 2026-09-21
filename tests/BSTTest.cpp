#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <vector>

#include "../src/dsa_core/repositories/LoanRepository.h"
#include "../src/dsa_core/structures/BST.h"

using std::string;
using std::vector;

std::size_t checkCount = 0;

void expect(bool condition, const string& message)
{
    ++checkCount;
    if (!condition)
    {
        std::cerr << "FAIL: " << message << '\n';
        std::exit(1);
    }
}

Loan makeLoan(const string& loanId, int dueDate)
{
    return Loan(loanId, "B01", "M01", 1, dueDate);
}

string describeLoans(const vector<Loan>& loans)
{
    std::ostringstream output;
    output << '[';
    for (std::size_t i = 0; i < loans.size(); ++i)
    {
        if (i != 0) output << ", ";
        output << loans[i].getLoanId() << '@' << loans[i].getDueDate();
    }
    output << ']';
    return output.str();
}

void expectLoans(const vector<Loan>& actual, const vector<Loan>& expected, const string& label)
{
    bool equal = actual.size() == expected.size();
    if (equal)
    {
        for (std::size_t i = 0; i < actual.size(); ++i)
        {
            if (actual[i].getLoanId() != expected[i].getLoanId() ||
                actual[i].getDueDate() != expected[i].getDueDate())
            {
                equal = false;
                break;
            }
        }
    }
    expect(equal, label + ": expected " + describeLoans(expected) +
                      ", actual " + describeLoans(actual));
}

void expectLoanFields(const Loan& actual, const Loan& expected, const string& label)
{
    expect(actual.getLoanId() == expected.getLoanId(), label + ": loanId");
    expect(actual.getBookId() == expected.getBookId(), label + ": bookId");
    expect(actual.getMemberId() == expected.getMemberId(), label + ": memberId");
    expect(actual.getBorrowDate() == expected.getBorrowDate(), label + ": borrowDate");
    expect(actual.getDueDate() == expected.getDueDate(), label + ": dueDate");
    expect(actual.getReturnDate() == expected.getReturnDate(), label + ": returnDate");
    expect(actual.isReturned() == expected.isReturned(), label + ": returned");
}

vector<Loan> expectedRange(const vector<Loan>& loans, int startDate, int endDate)
{
    vector<Loan> result;
    for (const Loan& loan : loans)
    {
        if (startDate <= loan.getDueDate() && loan.getDueDate() <= endDate)
            result.push_back(loan);
    }
    std::stable_sort(result.begin(), result.end(), [](const Loan& a, const Loan& b) {
        return a.getDueDate() < b.getDueDate();
    });
    return result;
}

void expectTreeMatchesLoans(BST& tree, const vector<Loan>& loans)
{
    const int low = std::numeric_limits<int>::min();
    const int high = std::numeric_limits<int>::max();
    vector<Loan> sorted = expectedRange(loans, low, high);
    std::size_t distinctDates = 0;
    for (std::size_t i = 0; i < sorted.size(); ++i)
    {
        if (i == 0 || sorted[i].getDueDate() != sorted[i - 1].getDueDate())
            ++distinctDates;
    }

    expect(tree.isEmpty() == loans.empty(), "tree emptiness matches reference loans");
    expect(tree.getLoanCount() == loans.size(), "loan count matches reference loans");
    expect(tree.getKeyCount() == distinctDates, "key count matches distinct dates");
    expectLoans(tree.getAllSortedByDueDate(), sorted, "all loans and BST ordering");

    for (std::size_t i = 0; i < sorted.size(); ++i)
    {
        if (i != 0 && sorted[i].getDueDate() == sorted[i - 1].getDueDate()) continue;
        int date = sorted[i].getDueDate();
        expectLoans(tree.findByDueDate(date), expectedRange(loans, date, date),
                    "find due date " + std::to_string(date));
    }

    const int ranges[][2] = {
        {low, high}, {10, 10}, {15, 25}, {20, 60}, {40, 70},
        {60, 100}, {100, 200}, {30, 29}
    };
    for (const auto& range : ranges)
    {
        expectLoans(tree.getLoanInTimeRange(range[0], range[1]),
                    expectedRange(loans, range[0], range[1]),
                    "range [" + std::to_string(range[0]) + ", " +
                        std::to_string(range[1]) + "]");
    }
}

void testEmptyAndClear()
{
    BST tree;
    expect(tree.isEmpty(), "new tree is empty");
    expect(tree.getLoanCount() == 0 && tree.getKeyCount() == 0, "new tree counts are zero");
    expect(tree.findByDueDate(20).empty(), "find in empty tree");
    expect(tree.getLoanInTimeRange(10, 30).empty(), "range in empty tree");

    tree.insert(makeLoan("L1", 20));
    expect(!tree.isEmpty(), "tree is not empty after insert");
    tree.clear();
    expect(tree.isEmpty(), "clear makes tree empty");
    expect(tree.getLoanCount() == 0 && tree.getKeyCount() == 0, "clear resets counts");
    tree.insert(makeLoan("L2", 30));
    expect(tree.findByDueDate(30).size() == 1, "tree can be reused after clear");
    std::cout << "PASS: empty and clear\n";
}

void testInsertFindAndRange()
{
    BST tree;
    tree.insert(makeLoan("L20a", 20));
    tree.insert(makeLoan("L10", 10));
    tree.insert(makeLoan("L30", 30));
    tree.insert(makeLoan("L20b", 20));

    expect(tree.getLoanCount() == 4, "four loans are stored");
    expect(tree.getKeyCount() == 3, "three distinct due dates are stored");
    expect(tree.findByDueDate(10).size() == 1, "find date in left subtree");
    expect(tree.findByDueDate(30).size() == 1, "find date in right subtree");
    expect(tree.findByDueDate(20).size() == 2, "find both loans on the same date");
    expect(tree.findByDueDate(25).empty(), "missing date returns empty result");

    vector<Loan> range = tree.getLoanInTimeRange(20, 30);
    expect(range.size() == 3, "range includes both boundaries");
    expect(range[0].getLoanId() == "L20a" && range[1].getLoanId() == "L20b" &&
               range[2].getLoanId() == "L30", "range is sorted by due date");
    expect(tree.getLoanInTimeRange(20, 20).size() == 2, "single-date range includes duplicates");
    expect(tree.getLoanInTimeRange(21, 29).empty(), "range with no matching date is empty");
    expect(tree.getLoanInTimeRange(30, 20).empty(), "reversed range is empty");

    vector<Loan> all = tree.getAllSortedByDueDate();
    expect(all.size() == 4 && all[0].getDueDate() == 10 &&
               all[3].getDueDate() == 30, "inorder traversal returns all loans in order");
    std::cout << "PASS: insert, find, duplicates, and range\n";
}

void testRemoveFromBucketAndLeaf()
{
    BST tree;
    tree.insert(makeLoan("L20a", 20));
    tree.insert(makeLoan("L20b", 20));
    tree.insert(makeLoan("L10", 10));

    expect(!tree.remove(20, "missing"), "unknown loan ID is not removed");
    expect(!tree.remove(99, "L20a"), "unknown due date is not removed");
    expect(tree.getLoanCount() == 3 && tree.getKeyCount() == 2, "failed remove keeps counts");
    expect(tree.remove(20, "L20a"), "remove one loan from a shared date");
    expect(tree.findByDueDate(20).size() == 1, "other loan on that date remains");
    expect(tree.getLoanCount() == 2 && tree.getKeyCount() == 2, "date node remains in tree");
    expect(tree.remove(10, "L10"), "remove a leaf node");
    expect(tree.findByDueDate(10).empty(), "removed leaf date is absent");
    expect(tree.getLoanCount() == 1 && tree.getKeyCount() == 1, "leaf removal updates counts");
    std::cout << "PASS: remove from shared date and leaf\n";
}

void testRemoveOneChild()
{
    BST leftTree;
    leftTree.insert(makeLoan("L20", 20));
    leftTree.insert(makeLoan("L10", 10));
    expect(leftTree.remove(20, "L20"), "remove node with left child");
    expect(leftTree.findByDueDate(10).size() == 1, "left child remains");

    BST rightTree;
    rightTree.insert(makeLoan("L20", 20));
    rightTree.insert(makeLoan("L30", 30));
    expect(rightTree.remove(20, "L20"), "remove node with right child");
    expect(rightTree.findByDueDate(30).size() == 1, "right child remains");
    expect(rightTree.remove(30, "L30"), "remove last node");
    expect(rightTree.isEmpty(), "tree is empty after removing last node");
    std::cout << "PASS: remove node with one child or no child\n";
}

void testRemoveTwoChildren()
{
    BST tree;
    tree.insert(makeLoan("L20", 20));
    tree.insert(makeLoan("L10", 10));
    tree.insert(makeLoan("L40", 40));
    tree.insert(makeLoan("L30a", 30));
    tree.insert(makeLoan("L30b", 30));
    tree.insert(makeLoan("L50", 50));

    expect(tree.remove(20, "L20"), "remove node with two children");
    expect(tree.findByDueDate(20).empty(), "removed date is absent");
    expect(tree.findByDueDate(30).size() == 2, "both successor loans remain");
    expect(tree.getLoanCount() == 5 && tree.getKeyCount() == 4, "two-child removal updates counts");
    vector<Loan> all = tree.getAllSortedByDueDate();
    expect(all.size() == 5 && all[0].getDueDate() == 10 &&
               all[1].getDueDate() == 30 && all[2].getDueDate() == 30 &&
               all[3].getDueDate() == 40 && all[4].getDueDate() == 50,
           "two-child removal preserves sorted order");
    std::cout << "PASS: remove node with two children\n";
}

void testRemoveTwoChildrenWithSuccessorRightChild()
{
    BST tree;
    tree.insert(makeLoan("root", 20));
    tree.insert(makeLoan("left", 10));
    tree.insert(makeLoan("right", 40));
    tree.insert(makeLoan("successor-a", 30));
    tree.insert(makeLoan("successor-b", 30));
    tree.insert(makeLoan("successor-right", 35));
    tree.insert(makeLoan("far-right", 50));

    expect(tree.remove(20, "root"), "remove root whose successor has a right child");
    const vector<Loan> remaining = {
        makeLoan("left", 10), makeLoan("right", 40),
        makeLoan("successor-a", 30), makeLoan("successor-b", 30),
        makeLoan("successor-right", 35), makeLoan("far-right", 50)
    };
    expectTreeMatchesLoans(tree, remaining);
    expectLoans(tree.getLoanInTimeRange(30, 35),
                {makeLoan("successor-a", 30), makeLoan("successor-b", 30),
                 makeLoan("successor-right", 35)},
                "successor bucket and its right child remain after root removal");
    expect(tree.findByDueDate(20).empty(), "removed root date is absent");
    std::cout << "PASS: remove two-child node when successor has a right child\n";
}

void testDeepNodesAndSkewedInsertion()
{
    BST multiLevel;
    const int dates[] = {50, 25, 75, 10, 30, 60, 90, 28, 65, 85, 95};
    for (int date : dates)
        multiLevel.insert(makeLoan("L" + std::to_string(date), date));
    expect(multiLevel.getHeight() >= 3, "insert builds multiple tree levels");
    expectLoans(multiLevel.findByDueDate(50), {makeLoan("L50", 50)}, "find root");
    expectLoans(multiLevel.findByDueDate(28), {makeLoan("L28", 28)}, "find deep left leaf");
    expectLoans(multiLevel.findByDueDate(65), {makeLoan("L65", 65)}, "find deep right leaf");
    expect(multiLevel.findByDueDate(29).empty(), "missing date between deep nodes");

    const int count = 128;
    BST increasing;
    BST decreasing;
    for (int date = 1; date <= count; ++date)
        increasing.insert(makeLoan("I" + std::to_string(date), date));
    for (int date = count; date >= 1; --date)
        decreasing.insert(makeLoan("D" + std::to_string(date), date));
    // Record current heights without requiring a particular tree shape.
    // MC2 requires correct results even when insertion order is monotonic.
    int increasingHeight = increasing.getHeight();
    int decreasingHeight = decreasing.getHeight();
    expectLoans(increasing.findByDueDate(count), {makeLoan("I128", count)},
                "find bottom of right-skewed tree");
    expectLoans(decreasing.findByDueDate(1), {makeLoan("D1", 1)},
                "find bottom of left-skewed tree");
    vector<Loan> forward = increasing.getLoanInTimeRange(1, count);
    vector<Loan> backward = decreasing.getLoanInTimeRange(1, count);
    expect(forward.size() == count && backward.size() == count, "full range covers both skewed trees");
    for (int i = 0; i < count; ++i)
    {
        expect(forward[i].getDueDate() == i + 1, "right-skewed range is ordered");
        expect(backward[i].getDueDate() == i + 1, "left-skewed range is ordered");
    }
    std::cout << "PASS: deep nodes and increasing/decreasing insertion"
              << " (heights " << increasingHeight << ", " << decreasingHeight << ")\n";
}

void testRangeBoundariesAndSingleNode()
{
    BST single;
    single.insert(makeLoan("only", 20));
    expectLoans(single.getLoanInTimeRange(20, 20), {makeLoan("only", 20)},
                "single node with equal boundaries");
    expect(single.getLoanInTimeRange(21, 30).empty(), "range above single node");
    expect(single.getLoanInTimeRange(1, 19).empty(), "range below single node");

    BST tree;
    vector<Loan> loans = {makeLoan("A", 20), makeLoan("B", 10),
                          makeLoan("C", 30), makeLoan("D", 10), makeLoan("E", 30)};
    for (const Loan& loan : loans) tree.insert(loan);
    expectLoans(tree.getLoanInTimeRange(10, 30), expectedRange(loans, 10, 30),
                "duplicates on both range boundaries");
    expectLoans(tree.getLoanInTimeRange(11, 30), expectedRange(loans, 11, 30),
                "exclude lower boundary");
    expectLoans(tree.getLoanInTimeRange(10, 29), expectedRange(loans, 10, 29),
                "exclude upper boundary");
    expectLoans(tree.getLoanInTimeRange(20, 20), {makeLoan("A", 20)},
                "narrow range with exactly one loan");
    expectLoans(tree.getLoanInTimeRange(0, 40), expectedRange(loans, 0, 40),
                "range covers entire tree");
    expect(tree.getLoanInTimeRange(31, 40).empty(), "range entirely above tree");
    expect(tree.getLoanInTimeRange(0, 9).empty(), "range entirely below tree");
    // [start, end] with start > end is empty in the current BST behavior.
    expect(tree.getLoanInTimeRange(30, 10).empty(), "reversed range is empty");
    std::cout << "PASS: range boundaries and single-node tree\n";
}

void testDuplicateRemovalPositions()
{
    BST tree;
    tree.insert(makeLoan("A", 20));
    tree.insert(makeLoan("left", 10));
    tree.insert(makeLoan("right", 30));
    tree.insert(makeLoan("B", 20));
    tree.insert(makeLoan("C", 20));
    tree.insert(makeLoan("D", 20));
    tree.insert(makeLoan("E", 20));
    expectLoans(tree.findByDueDate(20), {makeLoan("A", 20), makeLoan("B", 20),
                                         makeLoan("C", 20), makeLoan("D", 20),
                                         makeLoan("E", 20)}, "five loans share a date");
    expect(tree.remove(20, "A"), "remove first loan in bucket");
    expectLoans(tree.findByDueDate(20), {makeLoan("B", 20), makeLoan("C", 20),
                                         makeLoan("D", 20), makeLoan("E", 20)},
                "remaining loans after first removal");
    expect(tree.remove(20, "C"), "remove middle loan in bucket");
    expectLoans(tree.findByDueDate(20), {makeLoan("B", 20), makeLoan("D", 20),
                                         makeLoan("E", 20)}, "remaining loans after middle removal");
    expect(tree.remove(20, "E"), "remove last loan in bucket");
    expectLoans(tree.findByDueDate(20), {makeLoan("B", 20), makeLoan("D", 20)},
                "remaining loans after last-position removal");
    expect(tree.remove(20, "B") && tree.remove(20, "D"), "remove final two loans on date");
    expect(tree.findByDueDate(20).empty(), "date node disappears after final duplicate");
    expectLoans(tree.getLoanInTimeRange(10, 30),
                {makeLoan("left", 10), makeLoan("right", 30)},
                "neighboring nodes survive removal of duplicate date");
    expect(tree.getLoanCount() == 2 && tree.getKeyCount() == 2,
           "counts after removing all duplicates");
    std::cout << "PASS: remove first, middle, last, and final duplicate\n";
}

void testDynamicSequenceAndIntegrity()
{
    BST tree;
    vector<Loan> reference;
    auto add = [&](const string& id, int date) {
        Loan loan = makeLoan(id, date);
        tree.insert(loan);
        reference.push_back(loan);
        expectTreeMatchesLoans(tree, reference);
    };
    auto remove = [&](const string& id, int date) {
        auto found = std::find_if(reference.begin(), reference.end(), [&](const Loan& loan) {
            return loan.getLoanId() == id && loan.getDueDate() == date;
        });
        bool shouldRemove = found != reference.end();
        expect(tree.remove(date, id) == shouldRemove, "remove result matches reference");
        if (shouldRemove) reference.erase(found);
        expectTreeMatchesLoans(tree, reference);
    };

    add("A", 40);
    add("B", 20);
    expectLoans(tree.getLoanInTimeRange(20, 40), expectedRange(reference, 20, 40),
                "range between dynamic inserts");
    add("C", 60);
    add("D", 10);
    add("E", 30);
    add("F", 25);
    add("G", 70);
    add("H", 50);
    add("I", 20);
    remove("B", 20);
    expectLoans(tree.findByDueDate(20), {makeLoan("I", 20)},
                "find after removing one shared-date loan");
    add("J", 15);
    remove("I", 20); // Non-root node with two children and a deeper successor.
    add("K", 65);
    remove("C", 60); // Another non-root node with two children.
    remove("D", 10);
    remove("missing", 99);
    std::cout << "PASS: dynamic insert/remove sequence and reference comparison\n";
}

void testRepositoryIntegrationAndReturnedBehavior()
{
    LoanRepository repository;
    repository.addLoan(makeLoan("A", 10));
    repository.addLoan(makeLoan("B", 20));
    repository.addLoan(makeLoan("C", 20));
    Loan returned = makeLoan("R", 30);
    returned.markAsReturned(25);
    repository.addLoan(returned);

    expectLoans(repository.getLoansDueOn(20), {makeLoan("B", 20), makeLoan("C", 20)},
                "repository exact-date lookup includes both loans");
    vector<Loan> range = repository.getLoansDueInRange(20, 30);
    expectLoans(range, {makeLoan("B", 20), makeLoan("C", 20), makeLoan("R", 30)},
                "repository range uses BST index");
    // Current repository indexes every added Loan; it has no active-only filter.
    expect(range[2].isReturned(), "current repository includes an inserted returned Loan");
    expect(repository.removeLoan(20, "B"), "repository removes one shared-date loan");
    expectLoans(repository.getLoansDueOn(20), {makeLoan("C", 20)},
                "repository index updates after removal");
    expect(!repository.removeLoan(99, "C"), "repository rejects wrong due date");
    expect(repository.removeLoan(30, "R"), "repository removes returned Loan by key and ID");
    expect(repository.getLoansDueInRange(30, 30).empty(),
           "repository range updates after returned Loan removal");
    std::cout << "PASS: LoanRepository integration and current returned behavior\n";
}

void testCompleteLoanPayload()
{
    Loan active("active", "B17", "M23", 20260901, 20260930);
    Loan returned("returned", "B42", "M08", 20260902, 20260930);
    returned.markAsReturned(20260920);

    BST tree;
    tree.insert(active);
    tree.insert(returned);
    vector<Loan> exact = tree.findByDueDate(20260930);
    expect(exact.size() == 2, "exact lookup returns both full Loan records");
    expectLoanFields(exact[0], active, "BST exact lookup active Loan");
    expectLoanFields(exact[1], returned, "BST exact lookup returned Loan");

    LoanRepository repository;
    repository.addLoan(active);
    repository.addLoan(returned);
    vector<Loan> range = repository.getLoansDueInRange(20260930, 20260930);
    expect(range.size() == 2, "repository range returns both full Loan records");
    expectLoanFields(range[0], active, "repository range active Loan");
    expectLoanFields(range[1], returned, "repository range returned Loan");
    std::cout << "PASS: complete Loan fields survive BST and repository retrieval\n";
}

void testIntegerBoundsAndDuplicateHeavyData()
{
    const int low = std::numeric_limits<int>::min();
    const int high = std::numeric_limits<int>::max();
    BST tree;
    tree.insert(makeLoan("min", low));
    tree.insert(makeLoan("max", high));
    const int duplicateCount = 256;
    for (int i = 0; i < duplicateCount; ++i)
        tree.insert(makeLoan("D" + std::to_string(i), 0));
    expect(tree.getLoanCount() == duplicateCount + 2 && tree.getKeyCount() == 3,
           "large duplicate bucket keeps correct counts");
    vector<Loan> sameDate = tree.findByDueDate(0);
    expect(sameDate.size() == duplicateCount, "find returns all 256 same-date loans");
    expect(sameDate.front().getLoanId() == "D0" &&
               sameDate.back().getLoanId() == "D255", "duplicate bucket preserves insertion order");
    expect(tree.getLoanInTimeRange(0, 0).size() == duplicateCount,
           "single-key range returns large duplicate bucket");
    vector<Loan> all = tree.getLoanInTimeRange(low, high);
    expect(all.size() == duplicateCount + 2 && all.front().getLoanId() == "min" &&
               all.back().getLoanId() == "max", "int endpoints include entire tree in order");
    expect(tree.remove(0, "D0") && tree.remove(0, "D128") && tree.remove(0, "D255"),
           "remove first, middle, and last from large duplicate bucket");
    expect(tree.findByDueDate(0).size() == duplicateCount - 3,
           "large duplicate bucket keeps all other loans");
    expect(tree.getLoanCount() == duplicateCount - 1 && tree.getKeyCount() == 3,
           "large bucket removals update counts without removing date node");
    std::cout << "PASS: integer endpoints and duplicate-heavy data\n";
}

int main()
{
    testEmptyAndClear();
    testInsertFindAndRange();
    testRemoveFromBucketAndLeaf();
    testRemoveOneChild();
    testRemoveTwoChildren();
    testRemoveTwoChildrenWithSuccessorRightChild();
    testDeepNodesAndSkewedInsertion();
    testRangeBoundariesAndSingleNode();
    testDuplicateRemovalPositions();
    testDynamicSequenceAndIntegrity();
    testRepositoryIntegrationAndReturnedBehavior();
    testCompleteLoanPayload();
    testIntegerBoundsAndDuplicateHeavyData();
    std::cout << "All 13 BST/MC2 test groups passed (" << checkCount << " checks).\n";
    return 0;
}
