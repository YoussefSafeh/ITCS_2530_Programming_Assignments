// Name:        Youssef Safeh
// Course:      ITCS 2530 - C++ Programming
// Assignment:  Week 03 Programming Assignment
// Program:     Leftover Rescue Planner
// Description: Asks about the user's cooking habits, checks every answer for
//              invalid input, and estimates how much money is lost to wasted
//              food each week, month, and year. The results are shown in a
//              formatted table, and the same report is saved to report.txt.
//              Week 03 adds selection statements: an if / else if / else chain
//              rates the user's waste level, and a switch menu lets the user
//              run a spoilage check or plan a savings goal.

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>

using namespace std;

int main()
{
    // ===================== Named constants =====================
    const int WEEKS_PER_YEAR = 52;
    const int MONTHS_PER_YEAR = 12;
    const double WASTE_CUT_FRACTION = 0.50;     // "what if I cut my waste in half?"

    // Limits that decide whether a number answer is valid
    const double MIN_WEEKLY_BUDGET = 1.00;
    const double MAX_WEEKLY_BUDGET = 2000.00;   // anything bigger is most likely a typo
    const int MIN_WASTE_PERCENT = 0;
    const int MAX_WASTE_PERCENT = 100;
    const int IGNORE_LIMIT = 1000;              // max characters to skip when clearing a bad line

    // Fallback answers used when an answer is still invalid after a second try
    const string DEFAULT_DISH = "home-cooked meals";
    const string DEFAULT_INGREDIENT = "fresh produce";
    const double DEFAULT_WEEKLY_BUDGET = 100.00;
    const int DEFAULT_WASTE_PERCENT = 10;

    // Week 03: cutoffs used by the waste level rating
    const int LOW_WASTE_PERCENT = 10;           // tossing 10% or less counts as low
    const int HIGH_WASTE_PERCENT = 25;          // tossing 25% or more counts as high
    const double HIGH_WEEKLY_WASTE = 25.00;     // $25+ tossed per week is high, even at a small %

    // Week 03: limits for the answers asked inside the menu
    const int MAX_DAYS_LEFT = 60;               // spoilage check accepts 0 to 60 days
    const int USE_SOON_DAYS = 2;                // 1 or 2 days left means "use it right away"
    const double MIN_GOAL_COST = 1.00;
    const double MAX_GOAL_COST = 10000.00;

    // Report layout: one label column plus three money columns
    const int LABEL_WIDTH = 28;
    const int MONEY_WIDTH = 12;
    const int TABLE_WIDTH = LABEL_WIDTH + 3 * MONEY_WIDTH;

    // ===================== Variables =====================
    // The user's answers (numbers start at 0)
    string favoriteDish;
    string spoiledIngredient;
    double weeklyGroceryBudget = 0.0;
    int wastePercent = 0;

    // Week 03: the rating chosen by the if / else if / else chain
    string wasteLevel;          // "LOW", "MODERATE", or "HIGH"
    string wasteAdvice;         // short recommendation that matches the level

    // Week 03: the menu choice and the answers used inside the menu
    int menuChoice = 0;
    int daysLeft = 0;           // days until the ingredient usually goes bad
    string goalItem;            // something the user wants to buy with the money saved
    double goalCost = 0.0;
    double weeksToGoal = 0.0;   // weeks of savings needed to pay for goalItem

    ofstream reportFile;    // file stream used to write report.txt

    // Show every money value with exactly two decimal places
    cout << fixed << setprecision(2);

    // ===================== Welcome banner =====================
    // setfill('=') with setw(TABLE_WIDTH) on an empty string "" prints a full row
    // of '=' characters; setfill(' ') then puts normal space padding back
    cout << setfill('=') << setw(TABLE_WIDTH) << "" << setfill(' ') << '\n';
    cout << "             WELCOME TO THE LEFTOVER RESCUE PLANNER\n";
    cout << "        Find out what wasted food is really costing you,\n";
    cout << "      and how much you could save by using it up in time.\n";
    cout << setfill('=') << setw(TABLE_WIDTH) << "" << setfill(' ') << '\n';
    cout << "Answer 4 quick questions to get your personal food waste report.\n\n";

    // ===================== Question 1: favorite dish (string) =====================
    // Text questions come first and use getline, which reads the whole line,
    // so multi-word answers like "chicken alfredo, mac n cheese" stay intact
    cout << "What is your favorite dish to cook? ";
    getline(cin, favoriteDish);

    // Invalid = blank answer (user just pressed Enter). Ask once more, then use a default.
    if (favoriteDish == "")
    {
        cout << "  Please type the name of a dish: ";
        getline(cin, favoriteDish);

        if (favoriteDish == "")
        {
            favoriteDish = DEFAULT_DISH;
            cout << "  No answer given - using \"" << DEFAULT_DISH << "\".\n";
        }
    }

    // ===================== Question 2: ingredient that spoils (string) =====================
    cout << "Which ingredient usually spoils before you use it? ";
    getline(cin, spoiledIngredient);

    if (spoiledIngredient == "")
    {
        cout << "  Please type the name of an ingredient: ";
        getline(cin, spoiledIngredient);

        if (spoiledIngredient == "")
        {
            spoiledIngredient = DEFAULT_INGREDIENT;
            cout << "  No answer given - using \"" << DEFAULT_INGREDIENT << "\".\n";
        }
    }

    // ===================== Question 3: weekly grocery spending (double) =====================
    cout << "How much do you spend on groceries each week? $";
    cin >> weeklyGroceryBudget;

    // The answer is invalid if:
    //   !cin               -> the read failed (letters or symbols, like "abc" or "$120")
    //   cin.peek() != '\n' -> extra characters were typed after the number (like "1,200").
    //                         peek looks at the next character without removing it;
    //                         after a clean answer it should be the Enter key '\n'
    //   out of range       -> less than $1.00 or more than $2000.00
    if (!cin || cin.peek() != '\n'
        || weeklyGroceryBudget < MIN_WEEKLY_BUDGET || weeklyGroceryBudget > MAX_WEEKLY_BUDGET)
    {
        cin.clear();                        // take cin out of the fail state so it works again
        cin.ignore(IGNORE_LIMIT, '\n');     // throw away the rest of the bad line

        cout << "  That wasn't valid. Enter just a number from " << MIN_WEEKLY_BUDGET
            << " to " << MAX_WEEKLY_BUDGET << ": $";
        cin >> weeklyGroceryBudget;

        // Still invalid after the second try: use the default so the report can still run
        if (!cin || cin.peek() != '\n'
            || weeklyGroceryBudget < MIN_WEEKLY_BUDGET || weeklyGroceryBudget > MAX_WEEKLY_BUDGET)
        {
            cin.clear();
            weeklyGroceryBudget = DEFAULT_WEEKLY_BUDGET;
            cout << "  Still not valid - using the default of $" << DEFAULT_WEEKLY_BUDGET << ".\n";
        }
    }
    cin.ignore(IGNORE_LIMIT, '\n');         // remove the leftover Enter key before the next question

    // ===================== Question 4: percent thrown out (int) =====================
    cout << "About what percent of your groceries get thrown out (whole number 0-100, ex: 15)? ";
    cin >> wastePercent;

    // Same checks as Question 3. Here peek also catches answers like "12.5" or "15%",
    // because cin stops reading an int at the '.' or '%'
    if (!cin || cin.peek() != '\n'
        || wastePercent < MIN_WASTE_PERCENT || wastePercent > MAX_WASTE_PERCENT)
    {
        cin.clear();
        cin.ignore(IGNORE_LIMIT, '\n');

        cout << "  That wasn't valid. Enter a whole number from " << MIN_WASTE_PERCENT
            << " to " << MAX_WASTE_PERCENT << " (no % sign): ";
        cin >> wastePercent;

        if (!cin || cin.peek() != '\n'
            || wastePercent < MIN_WASTE_PERCENT || wastePercent > MAX_WASTE_PERCENT)
        {
            cin.clear();
            wastePercent = DEFAULT_WASTE_PERCENT;
            cout << "  Still not valid - using the default of " << DEFAULT_WASTE_PERCENT << "%.\n";
        }
    }
    cin.ignore(IGNORE_LIMIT, '\n');

    // ===================== Calculations (derived values) =====================
    // wastePercent is an int, so divide by 100.0 instead of 100;
    // integer division would turn 15 / 100 into 0
    double weeklyWasteCost = weeklyGroceryBudget * (wastePercent / 100.0);
    double yearlyWasteCost = weeklyWasteCost * WEEKS_PER_YEAR;
    double monthlyWasteCost = yearlyWasteCost / MONTHS_PER_YEAR;

    double yearlyGroceryCost = weeklyGroceryBudget * WEEKS_PER_YEAR;
    double monthlyGroceryCost = yearlyGroceryCost / MONTHS_PER_YEAR;

    // Money kept each period if the user cuts their waste in half
    double weeklySavings = weeklyWasteCost * WASTE_CUT_FRACTION;
    double monthlySavings = monthlyWasteCost * WASTE_CUT_FRACTION;
    double yearlySavings = yearlyWasteCost * WASTE_CUT_FRACTION;

    // ===================== Week 03: Waste level rating (if / else if / else) =====================
    // C++ checks the conditions from top to bottom, runs only the FIRST block
    // whose condition is true, and skips the rest of the chain.
    //   LOW:      && means BOTH parts must be true (small percent AND small dollar amount)
    //   HIGH:     || means EITHER part is enough (big percent OR big dollar amount)
    //   MODERATE: the final else catches everything in between
    if (wastePercent <= LOW_WASTE_PERCENT && weeklyWasteCost < HIGH_WEEKLY_WASTE)
    {
        wasteLevel = "LOW";
        wasteAdvice = "Keep it up you use what you buy!";
    }
    else if (wastePercent >= HIGH_WASTE_PERCENT || weeklyWasteCost >= HIGH_WEEKLY_WASTE)
    {
        wasteLevel = "HIGH";
        wasteAdvice = "Buy less and shop with a list.";
    }
    else
    {
        wasteLevel = "MODERATE";
        wasteAdvice = "Cook one \"use it up\" meal a week.";
    }

    // ===================== Report on the screen =====================
    cout << '\n';
    cout << setfill('=') << setw(TABLE_WIDTH) << "" << setfill(' ') << '\n';
    cout << "                     YOUR FOOD WASTE REPORT\n";
    cout << setfill('=') << setw(TABLE_WIDTH) << "" << setfill(' ') << '\n';

    // The user's answers: each label is padded to LABEL_WIDTH, so every answer
    // starts in the same column
    cout << left;
    cout << setw(LABEL_WIDTH) << "Favorite dish to cook:" << favoriteDish << '\n';
    cout << setw(LABEL_WIDTH) << "Usually spoils first:" << spoiledIngredient << '\n';
    cout << setw(LABEL_WIDTH) << "Weekly grocery spending:" << '$' << weeklyGroceryBudget << '\n';
    cout << setw(LABEL_WIDTH) << "Share of groceries tossed:" << wastePercent << "%\n\n";

    // Money table: labels are left-aligned, numbers are right-aligned so the
    // decimal points line up in each column
    cout << "COST OF FOOD WASTE (in dollars)\n";
    cout << setfill('-') << setw(TABLE_WIDTH) << "" << setfill(' ') << '\n';
    cout << left << setw(LABEL_WIDTH) << "Category"
        << right << setw(MONEY_WIDTH) << "Per Week"
        << setw(MONEY_WIDTH) << "Per Month"
        << setw(MONEY_WIDTH) << "Per Year" << '\n';
    cout << setfill('-') << setw(TABLE_WIDTH) << "" << setfill(' ') << '\n';
    cout << left << setw(LABEL_WIDTH) << "Groceries bought"
        << right << setw(MONEY_WIDTH) << weeklyGroceryBudget
        << setw(MONEY_WIDTH) << monthlyGroceryCost
        << setw(MONEY_WIDTH) << yearlyGroceryCost << '\n';
    cout << left << setw(LABEL_WIDTH) << "Food thrown out"
        << right << setw(MONEY_WIDTH) << weeklyWasteCost
        << setw(MONEY_WIDTH) << monthlyWasteCost
        << setw(MONEY_WIDTH) << yearlyWasteCost << '\n';
    cout << left << setw(LABEL_WIDTH) << "Saved if waste is halved"
        << right << setw(MONEY_WIDTH) << weeklySavings
        << setw(MONEY_WIDTH) << monthlySavings
        << setw(MONEY_WIDTH) << yearlySavings << '\n';
    cout << setfill('-') << setw(TABLE_WIDTH) << "" << setfill(' ') << '\n';

    // Week 03: the rating from the if / else if / else chain
    // (switch back to left, because the table rows above ended on right)
    cout << left << setw(LABEL_WIDTH) << "Waste level:" << wasteLevel << '\n';
    cout << setw(LABEL_WIDTH) << "Recommendation:" << wasteAdvice << '\n';

    cout << "Tip: Plan meals around your " << spoiledIngredient << " before it spoils.\n";
    cout << "Cut your waste in half and keep $" << yearlySavings << " a year in your pocket!\n";
    cout << setfill('=') << setw(TABLE_WIDTH) << "" << setfill(' ') << '\n';

    // ===================== Same report saved to report.txt =====================
    // These lines match the screen report above, but write to reportFile instead of cout
    reportFile.open("report.txt");
    reportFile << fixed << setprecision(2);     // a file stream has its own format settings

    reportFile << setfill('=') << setw(TABLE_WIDTH) << "" << setfill(' ') << '\n';
    reportFile << "                     YOUR FOOD WASTE REPORT\n";
    reportFile << setfill('=') << setw(TABLE_WIDTH) << "" << setfill(' ') << '\n';

    reportFile << left;
    reportFile << setw(LABEL_WIDTH) << "Favorite dish to cook:" << favoriteDish << '\n';
    reportFile << setw(LABEL_WIDTH) << "Usually spoils first:" << spoiledIngredient << '\n';
    reportFile << setw(LABEL_WIDTH) << "Weekly grocery spending:" << '$' << weeklyGroceryBudget << '\n';
    reportFile << setw(LABEL_WIDTH) << "Share of groceries tossed:" << wastePercent << "%\n\n";

    reportFile << "COST OF FOOD WASTE (in dollars)\n";
    reportFile << setfill('-') << setw(TABLE_WIDTH) << "" << setfill(' ') << '\n';
    reportFile << left << setw(LABEL_WIDTH) << "Category"
        << right << setw(MONEY_WIDTH) << "Per Week"
        << setw(MONEY_WIDTH) << "Per Month"
        << setw(MONEY_WIDTH) << "Per Year" << '\n';
    reportFile << setfill('-') << setw(TABLE_WIDTH) << "" << setfill(' ') << '\n';
    reportFile << left << setw(LABEL_WIDTH) << "Groceries bought"
        << right << setw(MONEY_WIDTH) << weeklyGroceryBudget
        << setw(MONEY_WIDTH) << monthlyGroceryCost
        << setw(MONEY_WIDTH) << yearlyGroceryCost << '\n';
    reportFile << left << setw(LABEL_WIDTH) << "Food thrown out"
        << right << setw(MONEY_WIDTH) << weeklyWasteCost
        << setw(MONEY_WIDTH) << monthlyWasteCost
        << setw(MONEY_WIDTH) << yearlyWasteCost << '\n';
    reportFile << left << setw(LABEL_WIDTH) << "Saved if waste is halved"
        << right << setw(MONEY_WIDTH) << weeklySavings
        << setw(MONEY_WIDTH) << monthlySavings
        << setw(MONEY_WIDTH) << yearlySavings << '\n';
    reportFile << setfill('-') << setw(TABLE_WIDTH) << "" << setfill(' ') << '\n';

    reportFile << left << setw(LABEL_WIDTH) << "Waste level:" << wasteLevel << '\n';
    reportFile << setw(LABEL_WIDTH) << "Recommendation:" << wasteAdvice << '\n';

    reportFile << "Tip: Plan meals around your " << spoiledIngredient << " before it spoils.\n";
    reportFile << "Cut your waste in half and keep $" << yearlySavings << " a year in your pocket!\n";
    reportFile << setfill('=') << setw(TABLE_WIDTH) << "" << setfill(' ') << '\n';

    reportFile.close();

    cout << "\nYour report was also saved to report.txt\n";

    // ===================== Week 03: Options menu (switch) =====================
    cout << '\n';
    cout << setfill('=') << setw(TABLE_WIDTH) << "" << setfill(' ') << '\n';
    cout << "WHAT WOULD YOU LIKE TO DO NEXT?\n";
    cout << "  1 = Check if your " << spoiledIngredient << " is about to spoil\n";
    cout << "  2 = Plan a savings goal with the money you could save\n";
    cout << "  3 = Exit\n";
    cout << "Enter your choice (1-3): ";
    cin >> menuChoice;
    if (!cin || cin.peek() != '\n')
    {
        cin.clear();
        menuChoice = 0;
    }
    cin.ignore(IGNORE_LIMIT, '\n');     // clears the rest of the line so getline works in case 2

    // switch jumps straight to the case label that matches menuChoice.
    // Each case ends with break; without it, the code would "fall through"
    // and keep running the next case too.

    switch (menuChoice)
    {
    case 1:
        // ----- Spoilage checker -----
        cout << "\nAbout how many days until your " << spoiledIngredient
            << " goes bad? (whole number, 0 if it already has): ";
        cin >> daysLeft;

        // && means every check must be true: the read worked, nothing extra was
        // typed after the number, and the number is from 0 to MAX_DAYS_LEFT.
        // If !cin, C++ stops right there (short-circuit) and never calls peek.

        if (cin && cin.peek() == '\n' && daysLeft >= 0 && daysLeft <= MAX_DAYS_LEFT)
        {
            // Nested if / else if / else: the advice depends on the days left
            if (daysLeft == 0)
            {
                cout << "It may be past its prime - check it first, and when in doubt,\n"
                    << "throw it out.\n";
            }
            else if (daysLeft <= USE_SOON_DAYS)
            {
                cout << "Only " << daysLeft << " day(s) left - use it soon, maybe in your "
                    << favoriteDish << ".\n";
            }
            else
            {
                cout << "You have " << daysLeft << " days. Plan it into a meal this week\n"
                    << "so it doesn't end up in the trash.\n";
            }
        }
        else
        {
            cin.clear();    // take cin out of the fail state if letters were typed
            cout << "That wasn't a whole number from 0 to " << MAX_DAYS_LEFT
                << ", so the spoilage check was skipped.\n";
        }
        break;

    case 2:
        // ----- Savings goal planner -----
        cout << "\nWhat would you like to save up for (ex: a stand mixer)? ";
        getline(cin, goalItem);

        // A blank answer gets a general name so the sentences below still read well
        if (goalItem == "")
        {
            goalItem = "your goal";
        }

        cout << "How much does it cost? $";
        cin >> goalCost;

        if (cin && cin.peek() == '\n' && goalCost >= MIN_GOAL_COST && goalCost <= MAX_GOAL_COST)
        {
            // Short-circuit evaluation: if weeklySavings is 0, the left side of &&
            // is false, so C++ never runs goalCost / weeklySavings (no divide by zero error which happened 4 times.)
            if (weeklySavings > 0 && goalCost / weeklySavings <= WEEKS_PER_YEAR)
            {
                weeksToGoal = goalCost / weeklySavings;
                cout << "Cutting your waste in half pays for " << goalItem << " in about "
                    << setprecision(1) << weeksToGoal << setprecision(2) << " weeks!\n";
            }
            else if (weeklySavings > 0)
            {
                weeksToGoal = goalCost / weeklySavings;
                cout << "That would take about " << setprecision(1) << weeksToGoal
                    << setprecision(2) << " weeks of waste savings - more than a year.\n"
                    << "Try a smaller goal, or cut more waste to get there faster.\n";
            }
            else
            {
                cout << "You don't waste any food, so there are no waste savings to put\n"
                    << "toward " << goalItem << ". Nice work!\n";
            }
        }
        else
        {
            cin.clear();
            cout << "That wasn't a price from $" << MIN_GOAL_COST << " to $" << MAX_GOAL_COST
                << ", so the savings plan was skipped.\n";
        }
        break;

    case 3:
        cout << "\nNo problem - your report is saved in report.txt.\n";
        break;

    default:
        // Runs for any number that isn't 1, 2, or 3 (and for bad input, which was set to 0)
        cout << "\nThat isn't a menu choice (1-3), so no choice has been selected.\n";
        break;
    }

    cout << "\nThanks for using the Leftover Rescue Planner!!!! Hope you got great savings :)\n";

    return 0;
}
