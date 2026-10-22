// Name:        Youssef Safeh
// Course:      ITCS 2530 - C++ Programming
// Assignment:  Week 01 Programming Assignment
// Program:     Leftover Rescue Planner
// Description: Asks about the user's cooking habits and estimates how much
//              money is lost to wasted food each week and each year.

#include <iostream>
#include <iomanip>
#include <string>

using namespace std;

int main()
{
    const int WEEKS_PER_YEAR = 52;

    // Variables that store the user's answers (numbers init at 0)
    string favoriteDish;
    string spoiledIngredient;
    double weeklyGroceryBudget = 0.0;
    int wastePercent = 0;

    // Title banner
    cout << "==================================================\n";
    cout << "             LEFTOVER RESCUE PLANNER\n";
    cout << "==================================================\n\n";

    // Text questions come first and use getline, which reads the whole
    // line, so multi-word answers like "chicken alfredo, mac n cheese" are kept intact
    cout << "What is your favorite dish to cook? ";
    getline(cin, favoriteDish);

    cout << "Which ingredient usually spoils before you use it? ";
    getline(cin, spoiledIngredient);

    // Number questions use cin >>
    cout << "How much do you spend on groceries each week? $";
    cin >> weeklyGroceryBudget;

    cout << "About what percent of your groceries get thrown out (whole number ex: 15)? ";
    cin >> wastePercent;

    // wastePercent is an int, so divide by 100.0 instead of 100;
    // Since integer division would turn 15 / 100 into 0
    double weeklyWasteCost = weeklyGroceryBudget * (wastePercent / 100.0);
    double yearlyWasteCost = weeklyWasteCost * WEEKS_PER_YEAR;

    // Show money values with exactly two decimal places
    cout << fixed << setprecision(2); 

    // Summary paragraph that uses every answer and calculation
    cout << "\n==================================================\n";
    cout << "               YOUR KITCHEN REPORT\n";
    cout << "==================================================\n";
    cout << "You love cooking " << favoriteDish << ", but " << spoiledIngredient
        << " keeps spoiling before you get to it.\n";
    cout << "You spend $" << weeklyGroceryBudget << " a week on groceries, and about "
        << wastePercent << "% of that ends up in the trash.\n";
    cout << "That's $" << weeklyWasteCost << " a week, or $" << yearlyWasteCost
        << " a year, of wasted food.\n";
    cout << "Plan your meals around your " << spoiledIngredient
        << " before it spoils, and that money stays in your pocket! And the food in your belly!\n";
    cout << "==================================================\n";

    return 0;
}