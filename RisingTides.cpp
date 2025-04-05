#include "RisingTides.h"
#include <queue>
#include <array>
using namespace std;

void flood(int row,
           int col,
           const Grid<double>& terrain,
           Grid<bool>& floodedRegionsGrid,
           queue<array<int, 2>>& q,
           double height)
{
    if ((terrain[row][col] <= height) && (!(floodedRegionsGrid[row][col])))
    {
        floodedRegionsGrid[row][col] = true;
        q.push({row, col});
    }
}

Grid<bool> floodedRegionsIn(const Grid<double>& terrain,
                            const Vector<GridLocation>& sources,
                            double height) {
    Grid<bool> floodedRegionsGrid(terrain.numRows(), terrain.numCols());
    queue<array<int, 2>> q;
    for (auto source : sources/*each water source at or below the water level*/) {
        if (terrain[source.row][source.col] <= height)
        {
            floodedRegionsGrid[source.row][source.col] = true;
            q.push({source.row, source.col});

        }
        // flood that square;
        // add that square to the queue;
    }

    while (! (q.empty())) {
        auto coords = q.front(); // dequeue a position from the front of the queue;
        q.pop();

        //above
        int row = coords[0] - 1;
        int col = coords[1];
        if (row >= 0)
        {
            flood(row, col, terrain, floodedRegionsGrid, q, height);
        }

        //below
        row = coords[0] + 1;
        if (row < terrain.numRows())
        {
            flood(row, col, terrain, floodedRegionsGrid, q, height);
        }

        //left
        row = coords[0];
        col = coords[1] - 1;
        if (col >= 0)
        {
            flood(row, col, terrain, floodedRegionsGrid, q, height);
        }

        //right
        row = coords[0];
        col = coords[1] + 1;
        if (col < terrain.numCols())
        {
            flood(row, col, terrain, floodedRegionsGrid, q, height);
        }
    }

    return floodedRegionsGrid;
}



/***** Test Cases Below This Point *****/
#include "GUI/SimpleTest.h"
PROVIDED_TEST("Nothing gets wet if there are no water sources.") {
    Grid<double> world = {
        { 0, 0, 0 },
        { 0, 0, 0 },
        { 0, 0, 0 }
    };

    Vector<GridLocation> sources = {
        // empty
    };

    /* There are no water sources, so nothing should be underwater. */
    Grid<bool> water = floodedRegionsIn(world, sources, 1.0);
    Grid<bool> expected = {
        { false, false, false },
        { false, false, false },
        { false, false, false }
    };

    EXPECT_EQUAL(water, expected);
}

PROVIDED_TEST("Everything gets wet if all locations are below the water level.") {
    Grid<double> world = {
        { 0, 0, 0 },
        { 0, 0, 0 },
        { 0, 0, 0 }
    };

    Vector<GridLocation> sources = {
        { 0, 0 }
    };

    /* Everything should flood; there are no barriers to stop the water. */
    Grid<bool> water = floodedRegionsIn(world, sources, 1.0);
    Grid<bool> expected = {
        {  true,  true,  true },
        {  true,  true,  true },
        {  true,  true,  true }
    };

    EXPECT_EQUAL(water, expected);
}

PROVIDED_TEST("Water can't cross a vertical levee.") {
    Grid<double> world = {
        { 0, 2, 0 },
        { 0, 2, 0 },
        { 0, 2, 0 }
    };

    Vector<GridLocation> sources = {
        { 0, 0 }
    };

    /* Only locations to the left of the barrier should be under water. */
    Grid<bool> water = floodedRegionsIn(world, sources, 1.0);
    Grid<bool> expected = {
        {  true, false, false },
        {  true, false, false },
        {  true, false, false }
    };

    EXPECT_EQUAL(water, expected);
}

PROVIDED_TEST("Water can't cross a diagonal levee.") {
    Grid<double> world = {
        { 0, 0, 2 },
        { 0, 2, 0 },
        { 2, 0, 0 }
    };

    Vector<GridLocation> sources = {
        { 0, 0 }
    };

    /* Water only flows in the four cardinal directions, so it can't
     * pass through the barrier. Only the top should be flooded.
     */
    Grid<bool> water = floodedRegionsIn(world, sources, 1.0);
    Grid<bool> expected = {
        {  true,  true, false },
        {  true, false, false },
        { false, false, false }
    };

    EXPECT_EQUAL(water, expected);
}

PROVIDED_TEST("Water can't flow diagonally.") {
    Grid<double> world = {
        { 0, 2, 0 },
        { 2, 0, 2 },
        { 0, 2, 0 }
    };

    Vector<GridLocation> sources = {
        { 1, 1 }
    };

    /* Water should be trapped in the center, since it can't move
     * diagonally.
     */
    Grid<bool> water = floodedRegionsIn(world, sources, 1.0);
    Grid<bool> expected = {
        { false, false, false },
        { false,  true, false },
        { false, false, false }
    };

    EXPECT_EQUAL(water, expected);
}

PROVIDED_TEST("Water can flow in all cardinal directions.") {
    Grid<double> world = {
        { 2, 0, 2 },
        { 0, 0, 0 },
        { 2, 0, 2 }
    };

    Vector<GridLocation> sources = {
        { 1, 1 }
    };

    /* The water in this case should flow up, down, left, and right. */
    Grid<bool> water = floodedRegionsIn(world, sources, 1.0);
    Grid<bool> expected = {
        { false,  true, false },
        {  true,  true,  true },
        { false,  true, false }
    };

    EXPECT_EQUAL(water, expected);
}

PROVIDED_TEST("Water can flow from multiple sources.") {
    Grid<double> world = {
        { 0, 0, 2 },
        { 0, 2, 0 },
        { 2, 0, 0 }
    };

    Vector<GridLocation> sources = {
        { 0, 0 },
        { 2, 2 }
    };

    /* Everything except the levee should be under water. */
    Grid<bool> water = floodedRegionsIn(world, sources, 1.0);
    Grid<bool> expected = {
        {  true,  true, false },
        {  true, false,  true },
        { false,  true,  true }
    };

    EXPECT_EQUAL(water, expected);
}

PROVIDED_TEST("Handles asymmetric worlds and non-square grids") {
    Grid<double> world = {
        { 3, 1, 4, 1 },
        { 5, 9, 2, 6 },
        { 5, 3, 5, 8 }
    };

    /* Initial test - water shouldn't leak out from the 2 if the height is 3.5. */
    Vector<GridLocation> sources = {
        { 1, 2 }
    };

    Grid<bool> expected = {
                           { false, false, false, false },
                           { false, false,  true, false },
                           { false, false, false, false },
                           };

    EXPECT_EQUAL(floodedRegionsIn(world, sources, 3.5), expected);

    /* Now, increase the water height to 4.5. */
    expected = {
                {  true,  true,  true,  true },
                { false, false,  true, false },
                { false, false, false, false },
                };

    EXPECT_EQUAL(floodedRegionsIn(world, sources, 4.5), expected);

    /* Now, increase the water height to 5.5. */
    expected = {
                {  true,  true,  true,  true },
                {  true, false,  true, false },
                {  true,  true,  true, false },
                };

    EXPECT_EQUAL(floodedRegionsIn(world, sources, 5.5), expected);

    /* Now, increase the water height to 6.5. */
    expected = {
                {  true,  true,  true,  true },
                {  true, false,  true,  true },
                {  true,  true,  true, false },
                };

    EXPECT_EQUAL(floodedRegionsIn(world, sources, 6.5), expected);

    /* Now, increase the water height to 9.5. */
    expected = {
                {  true,  true,  true,  true },
                {  true,  true,  true,  true },
                {  true,  true,  true,  true },
                };

    EXPECT_EQUAL(floodedRegionsIn(world, sources, 9.5), expected);
}

PROVIDED_TEST("Stress test: Handles a large, empty world quickly.") {
    Grid<double> world(100, 100); // Large world, everything defaults to 0 height.
    Vector<GridLocation> sources = {
        { 0, 0 }
    };

    /* This may take a long time to complete if the solution is inefficient. Look
     * for things like
     *
     * 1. passing around large objects by *value* rather than by *reference*,
     * 2. revisiting the same squares multiple times (e.g. flooding the same
     *    cell many times due to not checking if something is flooded),
     *
     * etc.
     *
     * We computed the 0.1s cutoff by taking our reference solution, running it
     * on a middle-of-the-line computer, and multiplying its runtime by 10.
     */
    Grid<bool> water;
    EXPECT_COMPLETES_IN(0.1,
                        water = floodedRegionsIn(world, sources, 1.0);
                        );
    EXPECT_EQUAL(water.numRows(), world.numRows());
    EXPECT_EQUAL(water.numCols(), world.numCols());

    /* Everything should be flooded. */
    for (int row = 0; row < world.numRows(); row++) {
        for (int col = 0; col < world.numCols(); col++) {
            EXPECT_EQUAL(water[row][col], true);
        }
    }
}

STUDENT_TEST("Water moves in all directions")
{
    Grid<double> world = {
        {0.6, 0.6, 0.7, 0.6, 0.1, 0.9, 0.4, 0.3, 0.9},
        {0.4, 66.2, 90.0, 75.7, 0.2, 29.3, 26.8, 72.3, 0.5},
        {0.7, 18.3, 0.1, 70.9, 0.1, 32.4, 0.0, 38.5, 0.4},
        {0.5, 0.0, 0.7, 88.9, 0.9, 42.6, 0.9, 0.5, 0.5},
        {45.2, 92.2, 49.7, 78.5, 0.6, 99.9, 51.5, 60.8, 80.3},
        {0.1, 0.2, 0.7, 35.8, 0.3, 86.2, 0.2, 0.9, 0.8},
        {1.0, 30.2, 0.4, 93.9, 0.4, 88.1, 0.2, 51.5, 0.9},
        {0.7, 98.3, 43.4, 7.0, 0.4, 74.0, 54.7, 1.7, 0.5},
        {0.6, 0.5, 0.3, 0.4, 0.7, 0.3, 0.4, 0.0, 0.8}
    };

    Vector<GridLocation> sources = {
        {4, 4}
    };

    Grid<bool> water = floodedRegionsIn(world, sources, 1.0);
    Grid<bool> expected = {
        {true, true, true, true, true, true, true, true, true},
        {true, false, false, false, true, false, false, false, true},
        {true, false, true, false, true, false, true, false, true},
        {true, true, true, false, true, false, true, true, true},
        {false, false, false, false, true, false, false, false, false},
        {true, true, true, false, true, false, true, true, true},
        {true, false, true, false, true, false, true, false, true},
        {true, false, false, false, true, false, false, false, true},
        {true, true, true, true, true, true, true, true, true}
    };

    EXPECT_EQUAL(water, expected);
}
